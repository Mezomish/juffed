/*
JuffEd - An advanced text editor
Copyright 2007-2009 Mikhail Murzin

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

#include "LexerStorage.h"

//	Lexers
#include <qscilexerbash.h>
#include <qscilexerbatch.h>
#include <qscilexercmake.h>
#include <qscilexercpp.h>
#include <qscilexercsharp.h>
#include <qscilexercss.h>
#include <qscilexerd.h>
#include <qscilexerdiff.h>
#include <qscilexerhtml.h>
#include <qscilexeridl.h>
#include <qscilexerjava.h>
#include <qscilexerjavascript.h>
#include <qscilexerlua.h>
#include <qscilexermakefile.h>
#include <qscilexerperl.h>
#include <qscilexerpython.h>
#include <qscilexerruby.h>
#include <qscilexersql.h>

//	Qt headers
#include <QtCore/QFileInfo>
#include <QtCore/QMap>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>

//	local headers
#include "AppInfo.h"
#include "FileTypeSettings.h"
#include "TextDocSettings.h"

#include "Log.h"

	struct Style {
		Style(const QColor& c = Qt::black, const QColor& bgc = Qt::white, bool b = false, bool i = false) {
			color = c;
			bgColor = bgc;
			bold = b;
			italic = i;
		}

		bool bold;
		bool italic;
		QColor color;
		QColor bgColor;
	};

	struct Rule {
		Rule(Style st, const QList<int>& elements) {
			style = st;
			hlElements = elements;
		}
		
		QList<int> hlElements;
		Style style;
	};

	struct Scheme {
		Style defaultStyle;
		QList<Rule> rules;
	};
	typedef QMap<QString, Style> StyleMap;
	typedef QMap<QString, Scheme> SchemeMap;

class LSInterior {
public:
	LSInterior() {
	}
	~LSInterior() {
	}
	
	QsciLexer* lexer(const QString& name/*, const QFont&*/);
	void readCustomStyle(const QString& name);
	void applyCustomStyle(const QString& name, const QFont& font);
	
	QMap<QString, QsciLexer*> lexers_;
	SchemeMap schemes_;
};

bool stringToBool(const QString& str) {
	return (str.compare("true") == 0 || str.compare("yes") == 0 || str.compare("1") == 0);
}

QColor stringToColor(const QString& str) {
	return QVariant(str).value<QColor>();
}

void parseScheme(const QDomElement& schEl, StyleMap& styles) {
	QDomNode styleNode = schEl.firstChild();
	
//	QString defColorStr = schEl.attribute("defaultColor", "#ff00ff");
//	QString defBgColorStr = schEl.attribute("defaultBgColor", "#ffff00");
	QString defColorStr = schEl.attribute("defaultColor", TextDocSettings::defaultFontColor().name());
	QString defBgColorStr = schEl.attribute("defaultBgColor", TextDocSettings::defaultBgColor().name());
	QString defBoldStr = schEl.attribute("defaultBold", "false");
	QString defItalicStr = schEl.attribute("defaultItalic", "false");
	
	Style defaultStyle(stringToColor(defColorStr), stringToColor(defBgColorStr), 
			stringToBool(defBoldStr), stringToBool(defItalicStr));
	styles["default"] = defaultStyle;
	
	while ( !styleNode.isNull() ) {
		QDomElement styleEl = styleNode.toElement();
		if ( styleEl.tagName().toLower().compare("style") == 0 ) {
			Style style;
			QString name = styleEl.attribute("name", "");
			if ( !name.isEmpty() ) {
				style.color = stringToColor(styleEl.attribute("color", defColorStr));
				style.bgColor = stringToColor(styleEl.attribute("bgColor", defBgColorStr));
				style.bold = stringToBool(styleEl.attribute("bold", defBoldStr));
				style.italic = stringToBool(styleEl.attribute("italic", defItalicStr));
				styles[name] = style;
			}
		}

		styleNode = styleNode.nextSibling();
	}
}

void LSInterior::readCustomStyle(const QString& name) {
	QDomDocument doc("JuffScheme");
	QString nm = name;
	nm = nm.replace(QString("+"), "plus").replace(QString("#"), "sharp").toLower();
	QString fileName = QString("%1.xml").arg(nm);
	fileName = AppInfo::configDirPath() + "/hlschemes/" + fileName;

	Log::debug(QString("Reading custom style from file '%1'...").arg(fileName), true);

	QFile file(fileName);
	if ( !file.open(QIODevice::ReadOnly) ) {
		Log::debug(QString("Can't open file '%1'").arg(fileName));
		return;
	}
	else {
		Log::debug(QString("File '%1' opened successfully").arg(fileName), true);
	}

	QString err;
	int errLine, errCol;
	if ( !doc.setContent(&file, &err, &errLine, &errCol) ) {
		Log::debug(QString("File %1: XML reading error: '%2', line %3, column %4")
				.arg(fileName).arg(err).arg(errLine).arg(errCol));
		file.close();
		return;
	}
	else {
		Log::debug(QString("File '%1' was parsed successfully").arg(fileName), true);
	}
	
	file.close();

	QDomElement docElem = doc.documentElement();
	QDomNode schNode = docElem.firstChild();
	StyleMap styles;
	while( !schNode.isNull() ) {
		QDomElement schEl = schNode.toElement();
		if( !schEl.isNull() ) {
			if ( schEl.tagName().toLower().compare("scheme") == 0 ) {
				parseScheme(schEl, styles);
			}
		}
		schNode = schNode.nextSibling();
	}


	if ( name.compare("C++") == 0 ) {
		Scheme cppSch;
		cppSch.defaultStyle = styles["default"];
		cppSch.rules << Rule(styles["preprocessor"], QList<int>() << QsciLexerCPP::PreProcessor)
				<< Rule(styles["comment"], QList<int>() << QsciLexerCPP::Comment << QsciLexerCPP::CommentLine)
				<< Rule(styles["commentdoc"], QList<int>() << QsciLexerCPP::CommentDoc << QsciLexerCPP::CommentLineDoc)
				<< Rule(styles["commentdockeyword"], QList<int>() << QsciLexerCPP::CommentDocKeyword)
				<< Rule(styles["number"], QList<int>() << QsciLexerCPP::Number)
				<< Rule(styles["keyword"], QList<int>() << QsciLexerCPP::Keyword)
				<< Rule(styles["operator"], QList<int>() << QsciLexerCPP::Operator)
				<< Rule(styles["string"], QList<int>() << QsciLexerCPP::DoubleQuotedString)
				<< Rule(styles["unclosedString"], QList<int>() << QsciLexerCPP::UnclosedString);
		schemes_[name] = cppSch;
	}
	else if ( name.compare("CMake") == 0 ) {
		Scheme cmakeSch;
		cmakeSch.defaultStyle = styles["default"];
		cmakeSch.rules << Rule(styles["comment"], QList<int>() << QsciLexerCMake::Comment)
				<< Rule(styles["string"], QList<int>() << QsciLexerCMake::String)
				<< Rule(styles["variable"], QList<int>() << QsciLexerCMake::Variable)
				<< Rule(styles["stringvariable"], QList<int>() << QsciLexerCMake::StringVariable)
				<< Rule(styles["function"], QList<int>() << QsciLexerCMake::Function)
				<< Rule(styles["number"], QList<int>() << QsciLexerCMake::Number)
				<< Rule(styles["label"], QList<int>() << QsciLexerCMake::Label)
				<< Rule(styles["blockif"], QList<int>() << QsciLexerCMake::BlockIf)
				<< Rule(styles["blockwhile"], QList<int>() << QsciLexerCMake::BlockWhile)
				<< Rule(styles["blockforeach"], QList<int>() << QsciLexerCMake::BlockForeach)
				<< Rule(styles["blockmacro"], QList<int>() << QsciLexerCMake::BlockMacro)
				<< Rule(styles["keywordset3"], QList<int>() << QsciLexerCMake::KeywordSet3);
		schemes_[name] = cmakeSch;
	}
	else if ( name.compare("Makefile") == 0 ) {
		Scheme mkSch;
		mkSch.defaultStyle = styles["default"];
		mkSch.rules << Rule(styles["variable"], QList<int>() << QsciLexerMakefile::Variable)
				<< Rule(styles["target"], QList<int>() << QsciLexerMakefile::Target)
				<< Rule(styles["comment"], QList<int>() << QsciLexerMakefile::Comment)
				<< Rule(styles["error"], QList<int>() << QsciLexerMakefile::Error);
		schemes_[name] = mkSch;
	}
	else if ( name.compare("Diff") == 0 ) {
		Scheme diffSch;
		diffSch.defaultStyle = styles["default"];
		diffSch.rules << Rule(styles["lineadded"], QList<int>() << QsciLexerDiff::LineAdded)
				<< Rule(styles["lineremoved"], QList<int>() << QsciLexerDiff::LineRemoved)
				<< Rule(styles["command"], QList<int>() << QsciLexerDiff::Command)
				<< Rule(styles["position"], QList<int>() << QsciLexerDiff::Position)
				<< Rule(styles["header"], QList<int>() << QsciLexerDiff::Header)
				<< Rule(styles["comment"], QList<int>() << QsciLexerDiff::Comment);
		schemes_[name] = diffSch;
	}
	else if ( name.compare("Java") == 0 ) {
		Scheme javaSch;
		javaSch.defaultStyle = styles["default"];
		javaSch.rules << Rule(styles["preprocessor"], QList<int>() << QsciLexerCPP::PreProcessor)
				<< Rule(styles["comment"], QList<int>() << QsciLexerCPP::Comment << QsciLexerCPP::CommentLine)
				<< Rule(styles["commentdoc"], QList<int>() << QsciLexerCPP::CommentDoc)
				<< Rule(styles["commentdockeyword"], QList<int>() << QsciLexerCPP::CommentDocKeyword)
				<< Rule(styles["number"], QList<int>() << QsciLexerCPP::Number)
				<< Rule(styles["keyword"], QList<int>() << QsciLexerCPP::Keyword)
				<< Rule(styles["operator"], QList<int>() << QsciLexerCPP::Operator)
				<< Rule(styles["string"], QList<int>() << QsciLexerCPP::DoubleQuotedString)
				<< Rule(styles["unclosedString"], QList<int>() << QsciLexerCPP::UnclosedString);
		schemes_[name] = javaSch;
	}
	else if ( name.compare("Python") == 0 ) {
		Scheme pySch;
		pySch.defaultStyle = styles["default"];
		pySch.rules << Rule(styles["keyword"], QList<int>() << QsciLexerPython::Keyword)
				<< Rule(styles["comment"], QList<int>() << QsciLexerPython::Comment << QsciLexerPython::CommentBlock)
				<< Rule(styles["number"], QList<int>() << QsciLexerPython::Number)
				<< Rule(styles["operator"], QList<int>() << QsciLexerPython::Operator)
				<< Rule(styles["identifier"], QList<int>() << QsciLexerPython::Identifier)
				<< Rule(styles["function"], QList<int>() << QsciLexerPython::FunctionMethodName)
				<< Rule(styles["stringSingle"], QList<int>() << QsciLexerPython::SingleQuotedString)
				<< Rule(styles["stringDouble"], QList<int>() << QsciLexerPython::DoubleQuotedString)
				<< Rule(styles["tripleSingle"], QList<int>() << QsciLexerPython::TripleSingleQuotedString)
				<< Rule(styles["tripleDouble"], QList<int>() << QsciLexerPython::TripleDoubleQuotedString)
				<< Rule(styles["decorator"], QList<int>() << QsciLexerPython::Decorator)
				<< Rule(styles["unclosedString"], QList<int>() << QsciLexerPython::UnclosedString);
		schemes_[name] = pySch;
	}
	else if ( name.compare("XML") == 0 ) {
		Scheme xmlSch;
		xmlSch.defaultStyle = styles["default"];
		xmlSch.rules << Rule(styles["tag"], QList<int>() << QsciLexerHTML::Tag << QsciLexerHTML::UnknownTag << QsciLexerHTML::XMLTagEnd)
				<< Rule(styles["attribute"], QList<int>() << QsciLexerHTML::Attribute << QsciLexerHTML::UnknownAttribute)
				<< Rule(styles["comment"], QList<int>() << QsciLexerHTML::HTMLComment)
				<< Rule(styles["value"], QList<int>() << QsciLexerHTML::HTMLSingleQuotedString << QsciLexerHTML::HTMLDoubleQuotedString)
				<< Rule(styles["entity"], QList<int>() << QsciLexerHTML::Entity);
		schemes_[name] = xmlSch;
	}
	else if ( name.compare("Bash") == 0 ) {
		Scheme bashSch;
		bashSch.defaultStyle = styles["default"];
		bashSch.rules << Rule(styles["identifier"], QList<int>() << QsciLexerBash::Identifier)
				<< Rule(styles["singleString"], QList<int>() << QsciLexerBash::SingleQuotedString)
				<< Rule(styles["doubleString"], QList<int>() << QsciLexerBash::DoubleQuotedString)
				<< Rule(styles["keyword"], QList<int>() << QsciLexerBash::Keyword)
				<< Rule(styles["operator"], QList<int>() << QsciLexerBash::Operator)
				<< Rule(styles["backticks"], QList<int>() << QsciLexerBash::Backticks)
				<< Rule(styles["comment"], QList<int>() << QsciLexerBash::Comment)
				<< Rule(styles["scalar"], QList<int>() << QsciLexerBash::Scalar << QsciLexerBash::ParameterExpansion)
				<< Rule(styles["error"], QList<int>() << QsciLexerBash::Error);
		schemes_[name] = bashSch;
	}
	else if ( name.compare("Batch") == 0 ) {
		Scheme batSch;
		batSch.defaultStyle = styles["default"];
		batSch.rules << Rule(styles["variable"], QList<int>() << QsciLexerBatch::Variable)
				<<	Rule(styles["comment"], QList<int>() << QsciLexerBatch::Comment)
				<<	Rule(styles["operator"], QList<int>() << QsciLexerBatch::Operator)
				<<	Rule(styles["keyword"], QList<int>() << QsciLexerBatch::Keyword)
				<<	Rule(styles["label"], QList<int>() << QsciLexerBatch::Label);
		schemes_[name] = batSch;
	}
	else if ( name.compare("HTML") == 0 ) {
		Scheme htmlSch;
		htmlSch.defaultStyle = styles["default"];
		htmlSch.rules 
				<< Rule(styles["tag"], QList<int>() << QsciLexerHTML::Tag)
				<< Rule(styles["attribute"], QList<int>() << QsciLexerHTML::Attribute)
				<< Rule(styles["comment"], QList<int>() << QsciLexerHTML::HTMLComment)
				<< Rule(styles["value"], QList<int>() << QsciLexerHTML::HTMLSingleQuotedString << QsciLexerHTML::HTMLDoubleQuotedString << QsciLexerHTML::HTMLValue << QsciLexerHTML::HTMLNumber << QsciLexerHTML::OtherInTag)
				<< Rule(styles["entity"], QList<int>() << QsciLexerHTML::Entity)
				<< Rule(styles["singleString"], QList<int>() << QsciLexerHTML::HTMLSingleQuotedString)
				<< Rule(styles["doubleString"], QList<int>() << QsciLexerHTML::HTMLDoubleQuotedString)
				<< Rule(styles["jsKeyword"], QList<int>() << QsciLexerHTML::JavaScriptKeyword)
				<< Rule(styles["jsSingleString"], QList<int>() << QsciLexerHTML::JavaScriptSingleQuotedString)
				<< Rule(styles["jsDoubleString"], QList<int>() << QsciLexerHTML::JavaScriptDoubleQuotedString)
				<< Rule(styles["jsComment"], QList<int>() << QsciLexerHTML::JavaScriptComment << QsciLexerHTML::JavaScriptCommentLine)
				<< Rule(styles["jsNumber"], QList<int>() << QsciLexerHTML::JavaScriptNumber)
				<< Rule(styles["jsSymbol"], QList<int>() << QsciLexerHTML::JavaScriptSymbol);
		schemes_[name] = htmlSch;
	}
	else if ( name.compare("PHP") == 0 ) {
		Scheme phpSch;
		phpSch.defaultStyle = styles["default"];
		phpSch.rules 
				<< Rule(styles["phpKeyword"], QList<int>() << QsciLexerHTML::PHPKeyword)
				<< Rule(styles["phpOperator"], QList<int>() << QsciLexerHTML::PHPOperator)
				<< Rule(styles["phpVariable"], QList<int>() << QsciLexerHTML::PHPVariable)
				<< Rule(styles["phpSingleString"], QList<int>() << QsciLexerHTML::PHPSingleQuotedString)
				<< Rule(styles["phpDoubleString"], QList<int>() << QsciLexerHTML::PHPDoubleQuotedString)
				<< Rule(styles["phpComment"], QList<int>() << QsciLexerHTML::PHPComment << QsciLexerHTML::PHPCommentLine)
				<< Rule(styles["phpNumber"], QList<int>() << QsciLexerHTML::PHPNumber)
				<< Rule(styles["tag"], QList<int>() << QsciLexerHTML::Tag)
				<< Rule(styles["attribute"], QList<int>() << QsciLexerHTML::Attribute)
				<< Rule(styles["comment"], QList<int>() << QsciLexerHTML::HTMLComment)
				<< Rule(styles["value"], QList<int>() << QsciLexerHTML::HTMLSingleQuotedString << QsciLexerHTML::HTMLDoubleQuotedString << QsciLexerHTML::HTMLValue << QsciLexerHTML::HTMLNumber << QsciLexerHTML::OtherInTag)
				<< Rule(styles["entity"], QList<int>() << QsciLexerHTML::Entity)
				<< Rule(styles["singleString"], QList<int>() << QsciLexerHTML::HTMLSingleQuotedString)
				<< Rule(styles["doubleString"], QList<int>() << QsciLexerHTML::HTMLDoubleQuotedString)
				<< Rule(styles["jsKeyword"], QList<int>() << QsciLexerHTML::JavaScriptKeyword)
				<< Rule(styles["jsSingleString"], QList<int>() << QsciLexerHTML::JavaScriptSingleQuotedString)
				<< Rule(styles["jsDoubleString"], QList<int>() << QsciLexerHTML::JavaScriptDoubleQuotedString)
				<< Rule(styles["jsComment"], QList<int>() << QsciLexerHTML::JavaScriptComment << QsciLexerHTML::JavaScriptCommentLine)
				<< Rule(styles["jsNumber"], QList<int>() << QsciLexerHTML::JavaScriptNumber)
				<< Rule(styles["jsSymbol"], QList<int>() << QsciLexerHTML::JavaScriptSymbol);
		schemes_[name] = phpSch;
	}
	else if ( name.compare("JavaScript") == 0 ) {
		Scheme jsSch;
		jsSch.defaultStyle = styles["default"];
		jsSch.rules << Rule(styles["keyword"], QList<int>() << QsciLexerCPP::Keyword)
				<< Rule(styles["singleString"], QList<int>() << QsciLexerCPP::SingleQuotedString)
				<< Rule(styles["doubleString"], QList<int>() << QsciLexerCPP::DoubleQuotedString)
				<< Rule(styles["comment"], QList<int>() << QsciLexerCPP::Comment << QsciLexerCPP::CommentLine)
				<< Rule(styles["commentdoc"], QList<int>() << QsciLexerCPP::CommentDoc << QsciLexerCPP::CommentLineDoc)
				<< Rule(styles["commentdockeyword"], QList<int>() << QsciLexerCPP::CommentDocKeyword)
				<< Rule(styles["number"], QList<int>() << QsciLexerCPP::Number)
				<< Rule(styles["identifier"], QList<int>() << QsciLexerCPP::Identifier);
		schemes_[name] = jsSch;
	}
	else if ( name.compare("Perl") == 0 ) {
		Scheme perlSch;
		perlSch.defaultStyle = styles["default"];
		perlSch.rules << Rule(styles["comment"], QList<int>() << QsciLexerPerl::Comment)
				<< Rule(styles["number"], QList<int>() << QsciLexerPerl::Number)
				<< Rule(styles["keyword"], QList<int>() << QsciLexerPerl::Keyword)
				<< Rule(styles["operator"], QList<int>() << QsciLexerPerl::Operator)
				<< Rule(styles["identifier"], QList<int>() << QsciLexerPerl::Identifier)
				<< Rule(styles["regexp"], QList<int>() << QsciLexerPerl::Regex)
				<< Rule(styles["backticks"], QList<int>() << QsciLexerPerl::Backticks)
				<< Rule(styles["scalar"], QList<int>() << QsciLexerPerl::Scalar)
				<< Rule(styles["array"], QList<int>() << QsciLexerPerl::Array)
				<< Rule(styles["hash"], QList<int>() << QsciLexerPerl::Hash)
				<< Rule(styles["singleString"], QList<int>() << QsciLexerPerl::SingleQuotedString)
				<< Rule(styles["doubleString"], QList<int>() << QsciLexerPerl::DoubleQuotedString)
				<< Rule(styles["substitution"], QList<int>() << QsciLexerPerl::Substitution)
				<< Rule(styles["hereDocument"], QList<int>() << QsciLexerPerl::HereDocumentDelimiter << QsciLexerPerl::SingleQuotedHereDocument << QsciLexerPerl::DoubleQuotedHereDocument)
				<< Rule(styles["error"], QList<int>() << QsciLexerPerl::Error);
		schemes_[name] = perlSch;
	}
	else if ( name.compare("Ruby") == 0 ) {
		Scheme rbSch;
		rbSch.defaultStyle = styles["default"];
		rbSch.rules << Rule(styles["comment"], QList<int>() << QsciLexerRuby::Comment)
				<< Rule(styles["number"], QList<int>() << QsciLexerRuby::Number)
				<< Rule(styles["keyword"], QList<int>() << QsciLexerRuby::Keyword)
				<< Rule(styles["operator"], QList<int>() << QsciLexerRuby::Operator)
				<< Rule(styles["identifier"], QList<int>() << QsciLexerRuby::Identifier)
				<< Rule(styles["regexp"], QList<int>() << QsciLexerRuby::Regex)
				<< Rule(styles["backticks"], QList<int>() << QsciLexerRuby::Backticks)
				<< Rule(styles["singleString"], QList<int>() << QsciLexerRuby::SingleQuotedString)
				<< Rule(styles["doubleString"], QList<int>() << QsciLexerRuby::DoubleQuotedString)
				<< Rule(styles["hereDocument"], QList<int>() << QsciLexerRuby::HereDocumentDelimiter << QsciLexerRuby::HereDocument)
				<< Rule(styles["pod"], QList<int>() << QsciLexerRuby::POD)
				<< Rule(styles["symbol"], QList<int>() << QsciLexerRuby::Symbol)
				<< Rule(styles["className"], QList<int>() << QsciLexerRuby::ClassName)
				<< Rule(styles["classVariable"], QList<int>() << QsciLexerRuby::ClassVariable)
				<< Rule(styles["instanceVariable"], QList<int>() << QsciLexerRuby::InstanceVariable)
				<< Rule(styles["functionMethod"], QList<int>() << QsciLexerRuby::FunctionMethodName)
				<< Rule(styles["global"], QList<int>() << QsciLexerRuby::Global)
				<< Rule(styles["error"], QList<int>() << QsciLexerRuby::Error);
		schemes_[name] = rbSch;
	}
}

void LSInterior::applyCustomStyle(const QString& name, const QFont& font) {
	QsciLexer* lex = lexers_.value(name, 0);
	if ( lex != 0 ) {
		lex->setFont(font, -1);

		if ( name.compare("none") == 0 ) {
			lex->setColor(Qt::black, -1);
			lex->setPaper(Qt::white, -1);
		}
		else {
			if ( schemes_.contains(name) ) {
				Scheme& scheme = schemes_[name];
			
				QFont f(font);
				f.setStyle(scheme.defaultStyle.italic ? QFont::StyleItalic : QFont::StyleNormal);
				f.setWeight(scheme.defaultStyle.bold ? QFont::Bold : QFont::Normal);
				lex->setFont(f, -1);
				lex->setColor(scheme.defaultStyle.color, -1);
				lex->setPaper(scheme.defaultStyle.bgColor, -1);

				foreach (Rule const& rule, scheme.rules) {
					foreach (int element, rule.hlElements) {
						QFont f(font);
						f.setStyle(rule.style.italic ? QFont::StyleItalic : QFont::StyleNormal);
						f.setWeight(rule.style.bold ? QFont::Bold : QFont::Normal);
						lex->setColor(rule.style.color, element);
						lex->setPaper(rule.style.bgColor, element);
						lex->setFont(f, element);
					}
				}
			}
		}
		lex->refreshProperties();
	}
}

QsciLexer* LSInterior::lexer(const QString& name) {
	if ( lexers_.contains(name) ) {
		//	return lexer with this name 
		//	if it already exists
		return lexers_[name];
	}
	else {
		//	if doesn't exist yet, create lexer
		//	if it's type is known. If unknown,
		//	return 0
		QsciLexer* newLexer = 0;
		if ( name.compare("C++") == 0 ) {
			newLexer = new QsciLexerCPP();
			((QsciLexerCPP*)newLexer)->setFoldComments(true);
		}
		else if ( name.compare("CMake") == 0 ) {
			newLexer = new QsciLexerCMake();
		}
		else if ( name.compare("Java") == 0 ) {
			newLexer = new QsciLexerJava();
		}
		else if ( name.compare("C#") == 0 ) {
			newLexer = new QsciLexerCSharp();
		}
		else if ( name.compare("Makefile") == 0 ) {
			newLexer = new QsciLexerMakefile();
		}
		else if ( name.compare("Diff") == 0 ) {
			newLexer = new QsciLexerDiff();
		}
		else if ( name.compare("Python") == 0 ) {
			newLexer = new QsciLexerPython();
		}
		else if ( name.compare("PHP") == 0 ) {
			newLexer = new QsciLexerHTML();
		}
		else if ( name.compare("Ruby") == 0 ) {
			newLexer = new QsciLexerRuby();
		}
		else if ( name.compare("Perl") == 0 ) {
			newLexer = new QsciLexerPerl();
		}
		else if ( name.compare("Bash") == 0 ) {
			newLexer = new QsciLexerBash();
		}
		else if ( name.compare("Batch") == 0 ) {
			newLexer = new QsciLexerBatch();
		}
		else if ( name.compare("HTML") == 0 ) {
			newLexer = new QsciLexerHTML();
		}
		else if ( name.compare("CSS") == 0 ) {
			newLexer = new QsciLexerCSS();
		}
		else if ( name.compare("XML") == 0 ) {
			newLexer = new QsciLexerHTML();
		}
		else if ( name.compare("SQL") == 0 ) {
			newLexer = new QsciLexerSQL();
		}
		else if ( name.compare("JavaScript") == 0 ) {
			newLexer = new QsciLexerJavaScript();
		}
		else if ( name.compare("IDL") == 0 ) {
			newLexer = new QsciLexerIDL();
		}
		else if ( name.compare("D") == 0 ) {
			newLexer = new QsciLexerD();
		}
		else if ( name.compare("Lua") == 0 ) {
			newLexer = new QsciLexerLua();
		}
		else if ( name.compare("none") == 0 ) {
			newLexer = new QsciLexerPython();
		}
	
		if ( newLexer != 0 ) {
			lexers_[name] = newLexer;
			if ( !name.isEmpty() && name.compare("none") != 0 ) {
				readCustomStyle(name);
			}
		}

		return newLexer;
	}
}


//////////////////////////////////////////////////////////////////////
//	LexerStorage implementation
//////////////////////////////////////////////////////////////////////

LexerStorage* LexerStorage::instance_ = 0;

LexerStorage::LexerStorage() {
	lsInt_ = new LSInterior();
}

LexerStorage::~LexerStorage() {
	JUFFDTOR;

	delete lsInt_;
}

QString LexerStorage::lexerName(const QString& fName) const {
	QFileInfo fi(fName);
	QString fileName = fi.fileName();
	QString ext = fi.suffix().toLower();
	QString name = "none";

	//	try to guess lexer using file name
	QStringList types = FileTypeSettings::getTypeList();
	if ( types.isEmpty() ) {
		getLexersList(types);
		types.removeAll("none");
	}
	foreach(QString type, types ) {
		Log::debug(type);
		QStringList patterns = FileTypeSettings::getFileNamePatterns(type);
		foreach (QString pattern, patterns) {
			QRegExp rx(pattern);
			rx.setPatternSyntax(QRegExp::Wildcard);
			rx.setCaseSensitivity(Qt::CaseInsensitive);
			if ( rx.exactMatch(fileName) ) {
				return type;
			}
		}
	}

	//	file name didn't match to any pattern - try to 
	//	analize file's 1st line

	QFile file(fName);
	if ( file.open(QIODevice::ReadOnly) ) {
		QString line = QString::fromLatin1(file.readLine().constData()).simplified();

		foreach(QString type, types) {
			QStringList patterns = FileTypeSettings::getFirstLinePatterns(type);
			foreach (QString pattern, patterns) {
				QRegExp rx(pattern);
				rx.setPatternSyntax(QRegExp::Wildcard);
				rx.setCaseSensitivity(Qt::CaseInsensitive);
				if ( rx.exactMatch(line) ) {
					return type;
				}
			}
		}
		file.close();
	}

	return name;
}

QsciLexer* LexerStorage::lexer(const QString& lexerName, const QFont& font) {
	QsciLexer* lex = lsInt_->lexer(lexerName);

	if ( lex != 0 ) {
		lsInt_->applyCustomStyle(lexerName, font);
	}

	return lex;
}

void LexerStorage::getLexersList(QStringList& list) const {
	list.clear();
	list << "none" << "Bash" << "Batch" << "C++" << "C#" << "CMake" << "CSS" 
			<< "D" << "Diff" << "HTML" << "IDL" << "Java" << "JavaScript" 
			<< "Lua" << "Makefile" << /*"Pascal" <<*/ "Perl" << "Python" << "PHP" 
			<< "Ruby" << "SQL" << /*"TCL" <<*/ "XML";
}

void LexerStorage::updateLexer(const QString& name, const QFont& font) {
	if ( lsInt_->lexers_.contains(name) ) {
		QsciLexer* lex = lsInt_->lexers_[name];
		lex->setFont(font);
		lex->refreshProperties();

		lsInt_->applyCustomStyle(name, font);
	}
}

LexerStorage* LexerStorage::instance() {
	if ( instance_ == 0 )
		instance_ = new LexerStorage();
	return instance_;
}
