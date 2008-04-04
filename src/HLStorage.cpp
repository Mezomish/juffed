/*
JuffEd - A simple text editor
Copyright 2007-2008 Mikhail Murzin

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License 
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "HLStorage.h"

//	Qt headers
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>

//	Local headers
#include "AppInfo.h"
#include "HL.h"

bool stringToBool(const QString& str) {
	return (str.compare("true") == 0 || str.compare("yes") == 0 || str.compare("1") == 0);
}

void parseStyle(const QDomElement& el, QTextCharFormat& fmt) {
	QString boldStr = el.attribute("bold", "false").toLower();
	QString italicStr = el.attribute("italic", "false").toLower();
	QString colorStr = el.attribute("color", "#000000");
	fmt.setForeground(QVariant(colorStr).value<QColor>());
	if (stringToBool(boldStr)) {
		fmt.setFontWeight(QFont::Bold);
	}
	if (stringToBool(italicStr)) {
		fmt.setFontItalic(true);
	}
}

void parseRule(const QDomElement& el, HLScheme& sch) {
	QList<QRegExp> list;
	QMap<int, QTextCharFormat> formats;

	QDomNode itemNode = el.firstChild();
	while (!itemNode.isNull()) {
		QDomElement itemEl = itemNode.toElement();
		if (!itemEl.isNull()) {
			QString tagName = itemEl.tagName().toLower();
			if (tagName.compare("expr") == 0) {
				QRegExp expr = QRegExp(itemEl.text().simplified());
				expr.setCaseSensitivity(stringToBool(itemEl.attribute("caseSensitive", "true")) ? Qt::CaseSensitive : Qt::CaseInsensitive);
				if (stringToBool(itemEl.attribute("minimal", "false"))) {
					expr.setMinimal(true);
				}
				list << expr;
			}
			else if (tagName.compare("style") == 0) {
				QTextCharFormat fmt;
				parseStyle(itemEl, fmt);
				int entry = itemEl.attribute("entry", "0").toInt();
				formats[entry] = fmt;
			}
		}
		itemNode = itemNode.nextSibling();
	}
	foreach (QRegExp regExp, list) {
		HLRule rule;
		rule.regExp = regExp;
		rule.formats = formats;
		sch.rules.append(rule);
	}
}

void parseScheme(const QDomElement& schEl, HLScheme& sch) {
	sch.name = schEl.attribute("name", "");
	QString extStr = schEl.attribute("extensions", "");
	if (!extStr.isEmpty())
		sch.extensions = extStr.split(';');
	else {
		sch.fileMask = schEl.attribute("fileMask", "");
	}
	QDomNode ruleNode = schEl.firstChild();
	
	while (!ruleNode.isNull()) {
		QStringList exprList;
		QDomElement ruleEl = ruleNode.toElement();
		QString ruleTagName = ruleEl.tagName().toLower();
		if (ruleTagName.compare("rule") == 0) {
			parseRule(ruleEl, sch);
		}
		else if (ruleTagName.compare("comment") == 0) {
			QDomNode commNode = ruleEl.firstChild();
			while (!commNode.isNull()) {
				QDomElement commEl = commNode.toElement();
				QString tagName = commEl.tagName().toLower();
				if (tagName.compare("begin") == 0)
					sch.comment = commEl.text();
				else if (tagName.compare("style") == 0)
					parseStyle(commEl, sch.commentFmt);
				commNode = commNode.nextSibling();
			}
		}
		else if (ruleTagName.compare("block") == 0) {
			QDomNode blNode = ruleEl.firstChild();
			HLBlock blockRule;
			bool regExp = QVariant(ruleEl.attribute("regExp", "false")).toBool();
			while (!blNode.isNull()) {
				QDomElement blEl = blNode.toElement();
				QString tagName = blEl.tagName().toLower();
				if (tagName.compare("begin") == 0) {
					blockRule.begin = QRegExp(blEl.text());
					if (!regExp)
						blockRule.begin.setPatternSyntax(QRegExp::FixedString);
				}
				else if (tagName.compare("end") == 0) {
					blockRule.end = QRegExp(blEl.text());
					if (!regExp)
						blockRule.end.setPatternSyntax(QRegExp::FixedString);
				}
				else if (tagName.compare("style") == 0)
					parseStyle(blEl, blockRule.fmt);
				blNode = blNode.nextSibling();
			}
			sch.blocks.append(blockRule);
		}
		ruleNode = ruleNode.nextSibling();
	}
}

///////////////////////////////////////////////////////////////////////
//	HLStorage
///////////////////////////////////////////////////////////////////////

HLStorage* HLStorage::st_ = 0;

HLStorage* HLStorage::instance() {
	if (st_ == 0) {
		st_ = new HLStorage();
		st_->readSchemes();
	}
	return st_;
}

bool HLStorage::readScheme(const QString& fileName, HLScheme& scheme) {
	QDomDocument doc("HLScheme");
	QFile file(fileName);
	if (QFileInfo(fileName).suffix().toLower().compare("xml") != 0 || !file.open(QIODevice::ReadOnly))
		return false;
	if (!doc.setContent(&file)) {
		file.close();
		return false;
	}
	file.close();

	QDomElement docElem = doc.documentElement();
	QDomNode schNode = docElem.firstChild();
	while(!schNode.isNull()) {
		QDomElement schEl = schNode.toElement();
		if(!schEl.isNull()) {
			if (schEl.tagName().toLower().compare("scheme") == 0) {
				parseScheme(schEl, scheme);
			}
		}
		schNode = schNode.nextSibling();
	}
	return true;
}

void HLStorage::readSchemes() {
	//	read schemes
	QString schemePath = AppInfo::configDir() + "/hlschemes";
	foreach (QString schemeFile, QDir(schemePath).entryList(QDir::Files)) {
		HLScheme scheme;
		if (readScheme(schemePath + "/" + schemeFile, scheme)) {
			schemes_.append(scheme);
		}
	}
}

bool HLStorage::getScheme(const QString& name, HLScheme& sch) {
	bool res = false;
	QString ext = QFileInfo(name).suffix();
	foreach (HLScheme scheme, schemes_) {
		if (!scheme.extensions.isEmpty()) {
			if (scheme.extensions.contains(ext)) {
				sch = scheme;
				res = true;
				break;
			}
		}
		else if (!scheme.fileMask.isEmpty()) {
			if (QRegExp(scheme.fileMask, Qt::CaseSensitive, QRegExp::Wildcard).exactMatch(QFileInfo(name).fileName())) {
				sch = scheme;
				res = true;
				break;
			}
		}
	}
	return res;
}
