/*
JuffEd - An advanced text editor
Copyright 2007-2010 Mikhail Murzin

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
#include <qscilexertex.h>

#include "lexers/qscilexerada.h"
#include "lexers/qscilexerasm.h"
#include "lexers/qscilexerhaskell.h"
#include "lexers/qscilexerlisp.h"
#include "lexers/qscilexernsis.h"
#include "lexers/my/QsciLexerMatlab.h"

#ifdef JUFF_TCL_LEXER
#include <qscilexertcl.h>
#endif // JUFF_TCL_LEXER

#ifdef JUFF_PASCAL_LEXER
#include <qscilexerpascal.h>
#endif // JUFF_PASCAL_LEXER

#ifdef JUFF_FORTRAN_LEXER
#include <qscilexerfortran.h>
#endif // JUFF_FORTRAN_LEXER

// Qt headers
#include <QtCore/QFileInfo>
#include <QtCore/QMap>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>

// local headers
#include "AppInfo.h"
#include "FileTypeSettings.h"
#include "EditorSettings.h"
#include "QSciSettings.h"

#include "Log.h"
#include <stdio.h>

	struct Style {
		Style(const QColor& c = QColor(), const QColor& bgc = QColor(), bool b = false, bool i = false) {
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
	typedef QMap<QString, Scheme*> SchemeMap;

class LSInterior {
public:
	LSInterior() {
		curFont_ = EditorSettings::font();
	}
	~LSInterior() {
	}
	
	QsciLexer* lexer(const QString& name/*, const QFont&*/);
	void readCustomStyle(const QString& name);
	void applyCustomStyle(const QString& name, const QFont& font);
	
	QMap<QString, QsciLexer*> lexers_;
	SchemeMap schemes_;
	QMap<QString, QColor> curLineColors_;
	QMap<QString, QColor> selectionBgColors_;
	QFont curFont_;
};

bool stringToBool(const QString& str) {
	return (str.compare("true") == 0 || str.compare("yes") == 0 || str.compare("1") == 0);
}

QColor stringToColor(const QString& str) {
	return QVariant(str).value<QColor>();
}

void parseScheme(const QDomElement& schEl, StyleMap& styles) {
	QDomNode styleNode = schEl.firstChild();

	QString defColorStr = schEl.attribute("defaultColor", "");
	QString defBgColorStr = schEl.attribute("defaultBgColor", "");
	QString defBoldStr = schEl.attribute("defaultBold", "false");
	QString defItalicStr = schEl.attribute("defaultItalic", "false");
	
	Style defaultStyle(stringToColor(defColorStr), stringToColor(defBgColorStr), 
			stringToBool(defBoldStr), stringToBool(defItalicStr));
	styles["default"] = defaultStyle;

	while ( !styleNode.isNull() ) {
		QDomElement styleEl = styleNode.toElement();
		if ( styleEl.tagName().toLower().compare("style") == 0 ) {
			QString name = styleEl.attribute("name", "");
			if ( !name.isEmpty() ) {
				Style style;
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
//	LOGGER;
	
	QDomDocument doc("JuffScheme");
	QString nm = name;
	nm = nm.replace(QString("+"), "plus").replace(QString("#"), "sharp").toLower();
	QString fileName = QString("%1.xml").arg(nm);
	fileName = AppInfo::configDirPath() + "/hlschemes/" + fileName;

//	Log::debug(QString("Reading custom style from file '%1'...").arg(fileName), true);

	QFile file(fileName);
	if ( !file.open(QIODevice::ReadOnly) ) {
		Log::warning(QString("Can't open file '%1'").arg(fileName));
		return;
	}
	else {
//		Log::debug(QString("File '%1' opened successfully").arg(fileName), true);
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
//		Log::debug(QString("File '%1' was parsed successfully").arg(fileName), true);
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

				if ( schEl.hasAttribute("curLineColor") && schEl.hasAttribute("name") ) {
					QColor curLineColor = stringToColor(schEl.attribute("curLineColor"));
					QString schName = schEl.attribute("name");
					curLineColors_[schName] = curLineColor;
				}
				if ( schEl.hasAttribute("selectionBgColor") && schEl.hasAttribute("name") ) {
					QColor selectionBgColor = stringToColor(schEl.attribute("selectionBgColor"));
					QString schName = schEl.attribute("name");
					selectionBgColors_[schName] = selectionBgColor;
				}
			}
		}
		schNode = schNode.nextSibling();
	}


//	Log::debug(QString("Preparing to create a lexer '%1'").arg(name));

	Scheme* scheme = NULL;
	
	if ( name.compare("C++") == 0 ) {
		scheme = new Scheme();
		scheme->defaultStyle = styles["default"];
		scheme->rules << Rule(styles["preprocessor"], QList<int>() << QsciLexerCPP::PreProcessor)
				<< Rule(styles["comment"], QList<int>() << QsciLexerCPP::Comment << QsciLexerCPP::CommentLine)
				<< Rule(styles["commentdoc"], QList<int>() << QsciLexerCPP::CommentDoc << QsciLexerCPP::CommentLineDoc)
				<< Rule(styles["commentdockeyword"], QList<int>() << QsciLexerCPP::CommentDocKeyword)
				<< Rule(styles["number"], QList<int>() << QsciLexerCPP::Number)
				<< Rule(styles["keyword"], QList<int>() << QsciLexerCPP::Keyword)
				<< Rule(styles["operator"], QList<int>() << QsciLexerCPP::Operator)
				<< Rule(styles["string"], QList<int>() << QsciLexerCPP::DoubleQuotedString)
				<< Rule(styles["unclosedString"], QList<int>() << QsciLexerCPP::UnclosedString);
		schemes_[name] = scheme;
	}
	else if ( name.compare("CMake") == 0 ) {
		scheme = new Scheme();
		scheme->defaultStyle = styles["default"];
		scheme->rules << Rule(styles["comment"], QList<int>() << QsciLexerCMake::Comment)
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
		schemes_[name] = scheme;
	}
	else if ( name.compare("Makefile") == 0 ) {
		scheme = new Scheme();
		scheme->defaultStyle = styles["default"];
		scheme->rules << Rule(styles["variable"], QList<int>() << QsciLexerMakefile::Variable)
				<< Rule(styles["target"], QList<int>() << QsciLexerMakefile::Target)
				<< Rule(styles["comment"], QList<int>() << QsciLexerMakefile::Comment)
				<< Rule(styles["error"], QList<int>() << QsciLexerMakefile::Error);
		schemes_[name] = scheme;
	}
	else if ( name.compare("Diff") == 0 ) {
		scheme = new Scheme();
		scheme->defaultStyle = styles["default"];
		scheme->rules << Rule(styles["lineadded"], QList<int>() << QsciLexerDiff::LineAdded)
				<< Rule(styles["lineremoved"], QList<int>() << QsciLexerDiff::LineRemoved)
				<< Rule(styles["command"], QList<int>() << QsciLexerDiff::Command)
				<< Rule(styles["position"], QList<int>() << QsciLexerDiff::Position)
				<< Rule(styles["header"], QList<int>() << QsciLexerDiff::Header)
				<< Rule(styles["comment"], QList<int>() << QsciLexerDiff::Comment);
		schemes_[name] = scheme;
	}
	else if ( name.compare("Java") == 0 ) {
		scheme = new Scheme();
		scheme->defaultStyle = styles["default"];
		scheme->rules << Rule(styles["preprocessor"], QList<int>() << QsciLexerCPP::PreProcessor)
				<< Rule(styles["comment"], QList<int>() << QsciLexerCPP::Comment << QsciLexerCPP::CommentLine)
				<< Rule(styles["commentdoc"], QList<int>() << QsciLexerCPP::CommentDoc)
				<< Rule(styles["commentdockeyword"], QList<int>() << QsciLexerCPP::CommentDocKeyword)
				<< Rule(styles["number"], QList<int>() << QsciLexerCPP::Number)
				<< Rule(styles["keyword"], QList<int>() << QsciLexerCPP::Keyword)
				<< Rule(styles["operator"], QList<int>() << QsciLexerCPP::Operator)
				<< Rule(styles["string"], QList<int>() << QsciLexerCPP::DoubleQuotedString)
				<< Rule(styles["unclosedString"], QList<int>() << QsciLexerCPP::UnclosedString);
		schemes_[name] = scheme;
	}
	else if ( name.compare("Python") == 0 ) {
		scheme = new Scheme();
		scheme->defaultStyle = styles["default"];
		scheme->rules << Rule(styles["keyword"], QList<int>() << QsciLexerPython::Keyword)
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
		schemes_[name] = scheme;
	}
	else if ( name.compare("XML") == 0 ) {
		scheme = new Scheme();
		scheme->defaultStyle = styles["default"];
		scheme->rules << Rule(styles["tag"], QList<int>() << QsciLexerHTML::Tag << QsciLexerHTML::UnknownTag << QsciLexerHTML::XMLTagEnd)
				<< Rule(styles["attribute"], QList<int>() << QsciLexerHTML::Attribute << QsciLexerHTML::UnknownAttribute)
				<< Rule(styles["comment"], QList<int>() << QsciLexerHTML::HTMLComment)
				<< Rule(styles["value"], QList<int>() << QsciLexerHTML::HTMLSingleQuotedString << QsciLexerHTML::HTMLDoubleQuotedString)
				<< Rule(styles["entity"], QList<int>() << QsciLexerHTML::Entity);
		schemes_[name] = scheme;
	}
	else if ( name.compare("Bash") == 0 ) {
		scheme = new Scheme();
		scheme->defaultStyle = styles["default"];
		scheme->rules << Rule(styles["identifier"], QList<int>() << QsciLexerBash::Identifier)
				<< Rule(styles["singleString"], QList<int>() << QsciLexerBash::SingleQuotedString)
				<< Rule(styles["doubleString"], QList<int>() << QsciLexerBash::DoubleQuotedString)
				<< Rule(styles["keyword"], QList<int>() << QsciLexerBash::Keyword)
				<< Rule(styles["operator"], QList<int>() << QsciLexerBash::Operator)
				<< Rule(styles["backticks"], QList<int>() << QsciLexerBash::Backticks)
				<< Rule(styles["comment"], QList<int>() << QsciLexerBash::Comment)
				<< Rule(styles["scalar"], QList<int>() << QsciLexerBash::Scalar << QsciLexerBash::ParameterExpansion)
				<< Rule(styles["error"], QList<int>() << QsciLexerBash::Error);
		schemes_[name] = scheme;
	}
	else if ( name.compare("Batch") == 0 ) {
		scheme = new Scheme();
		scheme->defaultStyle = styles["default"];
		scheme->rules << Rule(styles["variable"], QList<int>() << QsciLexerBatch::Variable)
				<<	Rule(styles["comment"], QList<int>() << QsciLexerBatch::Comment)
				<<	Rule(styles["operator"], QList<int>() << QsciLexerBatch::Operator)
				<<	Rule(styles["keyword"], QList<int>() << QsciLexerBatch::Keyword)
				<<	Rule(styles["label"], QList<int>() << QsciLexerBatch::Label);
		schemes_[name] = scheme;
	}
	else if ( name.compare("HTML") == 0 ) {
		scheme = new Scheme();
		scheme->defaultStyle = styles["default"];
		scheme->rules 
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
		schemes_[name] = scheme;
	}
	else if ( name.compare("PHP") == 0 ) {
		scheme = new Scheme();
		scheme->defaultStyle = styles["default"];
		scheme->rules 
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
		schemes_[name] = scheme;
	}
	else if ( name.compare("JavaScript") == 0 ) {
		scheme = new Scheme();
		scheme->defaultStyle = styles["default"];
		scheme->rules << Rule(styles["keyword"], QList<int>() << QsciLexerCPP::Keyword)
				<< Rule(styles["singleString"], QList<int>() << QsciLexerCPP::SingleQuotedString)
				<< Rule(styles["doubleString"], QList<int>() << QsciLexerCPP::DoubleQuotedString)
				<< Rule(styles["comment"], QList<int>() << QsciLexerCPP::Comment << QsciLexerCPP::CommentLine)
				<< Rule(styles["commentdoc"], QList<int>() << QsciLexerCPP::CommentDoc << QsciLexerCPP::CommentLineDoc)
				<< Rule(styles["commentdockeyword"], QList<int>() << QsciLexerCPP::CommentDocKeyword)
				<< Rule(styles["number"], QList<int>() << QsciLexerCPP::Number)
				<< Rule(styles["identifier"], QList<int>() << QsciLexerCPP::Identifier);
		schemes_[name] = scheme;
	}
	else if ( name.compare("CSS") == 0 ) {
		scheme = new Scheme();
		scheme->defaultStyle = styles["default"];
		scheme->rules 
				<< Rule(styles["tag"], QList<int>() << QsciLexerCSS::Tag)
#ifdef CSS3_FOUND		
				<< Rule(styles["property"], QList<int>() << QsciLexerCSS::CSS1Property << QsciLexerCSS::CSS2Property << QsciLexerCSS::CSS3Property)
#else
				<< Rule(styles["property"], QList<int>() << QsciLexerCSS::CSS1Property << QsciLexerCSS::CSS2Property)
#endif
				<< Rule(styles["value"], QList<int>() << QsciLexerCSS::Value)
				<< Rule(styles["operator"], QList<int>() << QsciLexerCSS::Operator)
				<< Rule(styles["id"], QList<int>() << QsciLexerCSS::IDSelector)
				<< Rule(styles["important"], QList<int>() << QsciLexerCSS::Important)
				<< Rule(styles["comment"], QList<int>() << QsciLexerCSS::Comment)
				<< Rule(styles["singleString"], QList<int>() << QsciLexerCSS::SingleQuotedString)
				<< Rule(styles["doubleString"], QList<int>() << QsciLexerCSS::DoubleQuotedString)
				<< Rule(styles["pseudoClass"], QList<int>() << QsciLexerCSS::PseudoClass)
				<< Rule(styles["classSelector"], QList<int>() << QsciLexerCSS::ClassSelector)
//				<< Rule(styles["attribute"], QList<int>() << QsciLexerCSS::Attribute)
		;
		schemes_[name] = scheme;
	}
	else if ( name.compare("Perl") == 0 ) {
		scheme = new Scheme();
		scheme->defaultStyle = styles["default"];
		scheme->rules << Rule(styles["comment"], QList<int>() << QsciLexerPerl::Comment)
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
		schemes_[name] = scheme;
	}
	else if ( name.compare("Ruby") == 0 ) {
		scheme = new Scheme();
		scheme->defaultStyle = styles["default"];
		scheme->rules << Rule(styles["comment"], QList<int>() << QsciLexerRuby::Comment)
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
		schemes_[name] = scheme;
	}
	else if ( name.compare("TeX") == 0 ) {
		scheme = new Scheme();
		scheme->defaultStyle = styles["default"];
		scheme->rules << Rule(styles["special"], QList<int>() << QsciLexerTeX::Special)
				<< Rule(styles["group"], QList<int>() << QsciLexerTeX::Group)
				<< Rule(styles["symbol"], QList<int>() << QsciLexerTeX::Symbol)
				<< Rule(styles["command"], QList<int>() << QsciLexerTeX::Command)
				<< Rule(styles["text"], QList<int>() << QsciLexerTeX::Text);
		schemes_[name] = scheme;
	}
#ifdef JUFF_TCL_LEXER
	else if ( name.compare("TCL") == 0 ) {
		scheme = new Scheme();
		scheme->defaultStyle = styles["default"];
		scheme->rules 
				<< Rule(styles["number"], QList<int>() << QsciLexerTCL::Number)
				<< Rule(styles["keyword"], QList<int>() << QsciLexerTCL::QuotedKeyword << QsciLexerTCL::ExpandKeyword << QsciLexerTCL::KeywordSet6 << QsciLexerTCL::KeywordSet7 << QsciLexerTCL::KeywordSet8 << QsciLexerTCL::KeywordSet9)
				<< Rule(styles["string"], QList<int>() << QsciLexerTCL::QuotedString)
				<< Rule(styles["operator"], QList<int>() << QsciLexerTCL::Operator)
				<< Rule(styles["identifier"], QList<int>() << QsciLexerTCL::Identifier)
				<< Rule(styles["substitution"], QList<int>() << QsciLexerTCL::Substitution)
				<< Rule(styles["substitutionbrace"], QList<int>() << QsciLexerTCL::SubstitutionBrace)
				<< Rule(styles["modifier"], QList<int>() << QsciLexerTCL::Modifier)
				<< Rule(styles["tclkeyword"], QList<int>() << QsciLexerTCL::TCLKeyword)
				<< Rule(styles["tkkeyword"], QList<int>() << QsciLexerTCL::TkKeyword)
				<< Rule(styles["itclkeyword"], QList<int>() << QsciLexerTCL::ITCLKeyword)
				<< Rule(styles["tkcommand"], QList<int>() << QsciLexerTCL::TkCommand)
				<< Rule(styles["comment"], QList<int>() << QsciLexerTCL::CommentBox << QsciLexerTCL::CommentBlock << QsciLexerTCL::Comment << QsciLexerTCL::CommentLine);
		schemes_[name] = scheme;
	}
#endif	//	JUFF_TCL_LEXER

#ifdef JUFF_PASCAL_LEXER
	else if ( name.compare("Pascal") == 0 ) {
		scheme = new Scheme();
		scheme->defaultStyle = styles["default"];
		scheme->rules 
				<< Rule(styles["comment"], QList<int>() << QsciLexerPascal::Comment << QsciLexerPascal::CommentLine << QsciLexerPascal::CommentParenthesis)
				<< Rule(styles["number"], QList<int>() << QsciLexerPascal::Number)
				<< Rule(styles["keyword"], QList<int>() << QsciLexerPascal::Keyword)
				<< Rule(styles["string"], QList<int>() << QsciLexerPascal::SingleQuotedString)
				<< Rule(styles["preprocessor"], QList<int>() << QsciLexerPascal::PreProcessor << QsciLexerPascal::PreProcessorParenthesis)
				<< Rule(styles["operator"], QList<int>() << QsciLexerPascal::Operator)
				<< Rule(styles["identifier"], QList<int>() << QsciLexerPascal::Identifier)
				<< Rule(styles["asm"], QList<int>() << QsciLexerPascal::Asm);
		schemes_[name] = scheme;
	}
#endif	//	JUFF_PASCAL_LEXER

#ifdef JUFF_FORTRAN_LEXER
	else if ( name.compare("Fortran") == 0 ) {
		scheme = new Scheme();
		scheme->defaultStyle = styles["default"];
		scheme->rules 
				<< Rule(styles["comment"], QList<int>() << QsciLexerFortran77::Comment)
				<< Rule(styles["number"], QList<int>() << QsciLexerFortran77::Number)
				<< Rule(styles["keyword"], QList<int>() << QsciLexerFortran77::Keyword)
				<< Rule(styles["operator"], QList<int>() << QsciLexerFortran77::Operator)
				<< Rule(styles["identifier"], QList<int>() << QsciLexerFortran77::Identifier)
				<< Rule(styles["preprocessor"], QList<int>() << QsciLexerFortran77::PreProcessor)
				<< Rule(styles["string"], QList<int>() << QsciLexerFortran77::SingleQuotedString << QsciLexerFortran77::DoubleQuotedString)
				<< Rule(styles["label"], QList<int>() << QsciLexerFortran77::Label)
				<< Rule(styles["dottedoperator"], QList<int>() << QsciLexerFortran77::DottedOperator)
				<< Rule(styles["intrinsicfunction"], QList<int>() << QsciLexerFortran77::IntrinsicFunction)
				<< Rule(styles["extendedfunction"], QList<int>() << QsciLexerFortran77::ExtendedFunction)
				<< Rule(styles["continuation"], QList<int>() << QsciLexerFortran77::Continuation)
				<< Rule(styles["unclosedstring"], QList<int>() << QsciLexerFortran77::UnclosedString);
		schemes_[name] = scheme;
	}
#endif	//	JUFF_FORTRAN_LEXER


//	Log::debug("Exiting readCustomStyle()");
}

void LSInterior::applyCustomStyle(const QString& name, const QFont& font) {
//	LOGGER;
	
	QsciLexer* lex = lexers_.value(name, 0);
	if ( lex != 0 ) {
//		Log::debug("Have a lexer");
		
		lex->setFont(font, -1);

		if ( name.compare("none") == 0 ) {
			lex->setDefaultPaper(EditorSettings::get(EditorSettings::DefaultBgColor));
			lex->setDefaultColor(EditorSettings::get(EditorSettings::DefaultFontColor));
			lex->setColor(EditorSettings::get(EditorSettings::DefaultFontColor), -1);
			lex->setPaper(EditorSettings::get(EditorSettings::DefaultBgColor), -1);
		}
		else {
//			Log::debug(QString("Lexer is not 'none'"));
			
			if ( schemes_.contains(name) ) {
//				Log::debug(QString("Found scheme %1").arg(name));
				Scheme* scheme = schemes_[name];
			
				QFont f(font);
				f.setStyle(scheme->defaultStyle.italic ? QFont::StyleItalic : QFont::StyleNormal);
				f.setWeight(scheme->defaultStyle.bold ? QFont::Bold : QFont::Normal);
				lex->setFont(f, -1);
				if ( scheme->defaultStyle.color.isValid() ) {
					lex->setColor(scheme->defaultStyle.color, -1);
					lex->setDefaultColor(scheme->defaultStyle.color);
				}
				else {
					lex->setColor(EditorSettings::get(EditorSettings::DefaultFontColor), -1);
					lex->setDefaultColor(EditorSettings::get(EditorSettings::DefaultFontColor));
				}
				if ( scheme->defaultStyle.bgColor.isValid() ) {
					lex->setPaper(scheme->defaultStyle.bgColor, -1);
					lex->setDefaultPaper(scheme->defaultStyle.bgColor);
				}
				else {
					lex->setPaper(EditorSettings::get(EditorSettings::DefaultBgColor), -1);
					lex->setDefaultPaper(EditorSettings::get(EditorSettings::DefaultBgColor));
				}

				foreach (Rule const& rule, scheme->rules) {
					foreach (int element, rule.hlElements) {
						QFont f(font);
						f.setStyle(rule.style.italic ? QFont::StyleItalic : QFont::StyleNormal);
						f.setWeight(rule.style.bold ? QFont::Bold : QFont::Normal);
						if ( rule.style.color.isValid() )
							lex->setColor(rule.style.color, element);
						if ( rule.style.bgColor.isValid() )
							lex->setPaper(rule.style.bgColor, element);
						lex->setFont(f, element);
					}
				}
			}
			else {
				lex->setDefaultPaper(EditorSettings::get(EditorSettings::DefaultBgColor));
				lex->setDefaultColor(EditorSettings::get(EditorSettings::DefaultFontColor));
				lex->setPaper(EditorSettings::get(EditorSettings::DefaultBgColor), -1);
			}
		}
		lex->refreshProperties();
	}
//	Log::debug("Exiting applyCustomStyle()");
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
		else if ( name.compare("TeX") == 0 ) {
			newLexer = new QsciLexerTeX();
		}
		else if ( name.compare("none") == 0 ) {
			newLexer = new QsciLexerPython();
		}
#ifdef JUFF_TCL_LEXER
		else if ( name.compare("TCL") == 0 ) {
			newLexer = new QsciLexerTCL();
		}
#endif // JUFF_TCL_LEXER

#ifdef JUFF_PASCAL_LEXER
		else if ( name.compare("Pascal") == 0 ) {
			newLexer = new QsciLexerPascal();
		}
#endif // JUFF_PASCAL_LEXER

#ifdef JUFF_FORTRAN_LEXER
		else if ( name.compare("Fortran") == 0 ) {
			newLexer = new QsciLexerFortran();
		}
#endif // JUFF_FORTRAN_LEXER
		else if ( name.compare("Ada") == 0 ) {
			newLexer = new QsciLexerAda();
		}
		else if ( name.compare("Asm") == 0 ) {
			newLexer = new QsciLexerASM();
		}
		else if ( name.compare("Haskell") == 0 ) {
			newLexer = new QsciLexerHaskell();
		}
		else if ( name.compare("Lisp") == 0 ) {
			newLexer = new QsciLexerLisp();
		}
		else if ( name.compare("NSIS") == 0 ) {
			newLexer = new QsciLexerNSIS();
		}
		else if ( name.compare("NBC") == 0 ) {
			newLexer = new QsciLexerASM();
		}
		else if ( name.compare("NXC") == 0 ) {
			newLexer = new QsciLexerCPP();
		}
		else if ( name.compare("Matlab") == 0 ) {
			newLexer = new QsciLexerMatlab();
		}
		else if ( name.compare("Qore") == 0 ) {
			// TODO/FIXME: special lexer
			newLexer = new QsciLexerPerl();
		}
		else if ( name.compare("Qorus") == 0) {
			// TODO/FIXME: special lexer. Qorus has to inherit Qore
			newLexer = new QsciLexerPerl();
		}
		
		if ( newLexer != 0 ) {
			lexers_[name] = newLexer;
			if ( !name.isEmpty() && name.compare("none") != 0 ) {
				readCustomStyle(name);
			}
			applyCustomStyle(name, curFont_);
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
	delete lsInt_;
}

QString LexerStorage::lexerName(const QString& fName) {
	QFileInfo fi(fName);
	QString fileName = fi.fileName();
	QString ext = fi.suffix().toLower();
	QString name = "none";

	//	try to guess lexer using file name
	QStringList types = lexersList();
	types.removeAll("none");
	
	foreach(QString type, types ) {
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

QsciLexer* LexerStorage::lexer(const QString& lexerName) {
	return lsInt_->lexer(lexerName);
}

QColor LexerStorage::curLineColor(const QString& name) const {
	return lsInt_->curLineColors_.value(name, QSciSettings::get(QSciSettings::CurLineColor));
}

QColor LexerStorage::selectionBgColor(const QString& name) const {
	return lsInt_->selectionBgColors_.value(name, EditorSettings::get(EditorSettings::SelectionBgColor));
}

QsciLexer* LexerStorage::lexerByFileName(const QString& fileName) {
	return lexer(lexerName(fileName));
}

QStringList LexerStorage::lexersList() const {
	QStringList list;
	list << "none" << "Ada" << "Asm" << "Bash" << "Batch" << "C++" << "C#" 
			<< "CMake" << "CSS" << "D" << "Diff" 
			
#ifdef JUFF_FORTRAN_LEXER
			<< "Fortran"
#endif	//	JUFF_FORTRAN_LEXER

			<< "Haskell" << "HTML" << "IDL" << "Java" << "JavaScript" << "Lisp" 
			<< "Lua" << "Makefile" << "Matlab" << "NBC" << "NSIS" << "NXC"

#ifdef JUFF_PASCAL_LEXER
			<< "Pascal"
#endif	//	JUFF_PASCAL_LEXER

			<< "Perl" << "Python" << "PHP" << "Ruby" << "SQL"

#ifdef JUFF_TCL_LEXER
			<< "TCL"
#endif	//	JUFF_TCL_LEXER

			<< "Qore" << "Qorus"

			<< "TeX" << "XML";
	return list;
}

void LexerStorage::updateLexers(const QFont& font) {
//	if ( font == lsInt_->curFont_ )
//		return;
	
	QMap<QString, QsciLexer*>::iterator it = lsInt_->lexers_.begin();
	while (it != lsInt_->lexers_.end()) {
		lsInt_->applyCustomStyle(it.key(), font);
		it++;
	}
	lsInt_->curFont_ = font;
}


LexerStorage* LexerStorage::instance() {
	if ( instance_ == 0 )
		instance_ = new LexerStorage();
	return instance_;
}

