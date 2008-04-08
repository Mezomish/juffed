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

#include "LexerStorage.h"

//	Lexers
#include <Qsci/qscilexerbash.h>
#include <Qsci/qscilexercpp.h>
#include <Qsci/qscilexercsharp.h>
#include <Qsci/qscilexercss.h>
#include <Qsci/qscilexerd.h>
#include <Qsci/qscilexerhtml.h>
#include <Qsci/qscilexeridl.h>
#include <Qsci/qscilexerjava.h>
#include <Qsci/qscilexerjavascript.h>
#include <Qsci/qscilexerlua.h>
#include <Qsci/qscilexermakefile.h>
#include <Qsci/qscilexerperl.h>
#include <Qsci/qscilexerpython.h>
#include <Qsci/qscilexerruby.h>
#include <Qsci/qscilexersql.h>

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

class LSInterior {
	struct Style {
		Style() {
			bold = false;
			italic = false;
			color = Qt::black;
		}
		
		Style(const QColor& c, bool b, bool i) {
			color = c;
			bold = b;
			italic = i;
		}
				
		bool bold;
		bool italic;
		QColor color;
	};
		
	struct StyleMaper {
		StyleMaper(const QString& style) {
			configStyle = style;
		}
		QString configStyle;
		QList<int> lexerStyles;
	};

	typedef QList<StyleMaper> StyleMaperList;
	typedef QMap<QString, Style> StyleMap;

public:
	LSInterior() {
		cppExtList_ << "cpp" << "cxx" << "c++" << "c" << "cc" << "h" << "hpp" << "hxx" << "h++";
		bashExtList_ << "sh" << "run";
		htmlExtList_ << "html" << "htm" << "php";
		xmlExtList_ << "xml";
	}
	~LSInterior() {
	}
	
	bool stringToBool(const QString& str) {
		return (str.compare("true") == 0 || str.compare("yes") == 0 || str.compare("1") == 0);
	}

	void parseScheme(const QDomElement& schEl, StyleMap& styles) {
//		sch.name = schEl.attribute("name", "");

		QDomNode styleNode = schEl.firstChild();
	
		while (!styleNode.isNull()) {
			QDomElement styleEl = styleNode.toElement();
			if (styleEl.tagName().toLower().compare("style") == 0) {
				Style style;
				QString name = styleEl.attribute("name", "");
				if (styleEl.hasAttribute("color")) {
					QString colorStr = styleEl.attribute("color");
					style.color = QVariant(colorStr).value<QColor>();
				}
				style.bold = stringToBool(styleEl.attribute("bold", "false"));
				style.italic = stringToBool(styleEl.attribute("italic", "false"));
				styles[name] = style;
			}

			styleNode = styleNode.nextSibling();
		}
	}

	bool readScheme(const QString& name, StyleMap& styles) {
		QDomDocument doc("HLScheme2");
		QString fileName = QString("%1.xml").arg(name);
		fileName = AppInfo::configDir() + "/hlschemes/" + fileName;
		QFile file(fileName);
		if (QFileInfo(fileName).suffix().toLower().compare("xml") != 0 || !file.open(QIODevice::ReadOnly)) {
			return false;
		}
		QString err;
		if (!doc.setContent(&file, &err)) {
			qDebug(qPrintable(err));
			file.close();
			return false;
		}
		file.close();

		styles.clear();
		QDomElement docElem = doc.documentElement();
		QDomNode schNode = docElem.firstChild();
		while(!schNode.isNull()) {
			QDomElement schEl = schNode.toElement();
			if(!schEl.isNull()) {
				if (schEl.tagName().toLower().compare("scheme") == 0) {
					parseScheme(schEl, styles);
				}
			}
			schNode = schNode.nextSibling();
		}
		return true;
	}

	QsciLexer* lexer(const QString& name);
	void setCustomLexerStyle(QsciLexer* lexer, const QString& lexerName, const QFont& font);
	void applyLexerStyle(QsciLexer* lexer, int arg, const Style& style, const QFont& font);
	
	QStringList cppExtList_;
	QStringList bashExtList_;
	QStringList htmlExtList_;
	QStringList xmlExtList_;
	
	QMap<QString, QsciLexer*> lexers_;
};


QsciLexer* LSInterior::lexer(const QString& name) {
	if (lexers_.contains(name)) {
		//	return lexer with this name 
		//	if it already exists
		return lexers_[name];
	}
	else {
		//	if doesn't exist yes, create lexer
		//	if it's type is known. If unknown,
		//	return 0
		QsciLexer* newLexer = 0;
		if (name.compare("cpp") == 0) {
			newLexer = new QsciLexerCPP();
		}
		else if (name.compare("java") == 0) {
			newLexer = new QsciLexerJava();
		}
		else if (name.compare("csharp") == 0) {
			newLexer = new QsciLexerCSharp();
		}
		else if (name.compare("makefile") == 0) {
			newLexer = new QsciLexerMakefile();
		}
		else if (name.compare("python") == 0) {
			newLexer = new QsciLexerPython();
		}
		else if (name.compare("ruby") == 0) {
			newLexer = new QsciLexerRuby();
		}
		else if (name.compare("perl") == 0) {
			newLexer = new QsciLexerPerl();
		}
		else if (name.compare("bash") == 0) {
			newLexer = new QsciLexerBash();
		}
		else if (name.compare("html") == 0) {
			newLexer = new QsciLexerHTML();
		}
		else if (name.compare("css") == 0) {
			newLexer = new QsciLexerCSS();
		}
		else if (name.compare("xml") == 0) {
			newLexer = new QsciLexerHTML();
		}
		else if (name.compare("sql") == 0) {
			newLexer = new QsciLexerSQL();
		}
		else if (name.compare("jscript") == 0) {
			newLexer = new QsciLexerJavaScript();
		}
		else if (name.compare("idl") == 0) {
			newLexer = new QsciLexerIDL();
		}
		else if (name.compare("d") == 0) {
			newLexer = new QsciLexerD();
		}
		else if (name.compare("lua") == 0) {
			newLexer = new QsciLexerLua();
		}
	
		if (newLexer != 0) {
			lexers_[name] = newLexer;
		}
	
		return newLexer;
	}
}

void LSInterior::applyLexerStyle(QsciLexer* lexer, int arg, const Style& style, const QFont& font) {
	QFont f(font);
	f.setStyle(style.italic ? QFont::StyleItalic : QFont::StyleNormal);
	f.setWeight(style.bold ? QFont::Bold : QFont::Normal);
	lexer->setColor(style.color, arg);
	lexer->setFont(f, arg);
}

void LSInterior::setCustomLexerStyle(QsciLexer* lexer, const QString& lexerName, const QFont& font) {
/*	StyleMap styles;
	if (readScheme(lexerName, styles)) {

		StyleMaperList styleMaperList;

		if (lexerName.compare("cpp") == 0) {
			StyleMaper prepr("preprocessor"), comment("comment"), 
					numbers("numbers"), keywords("keywords"), strings("strings");

			prepr.lexerStyles << QsciLexerCPP::PreProcessor;
			comment.lexerStyles << QsciLexerCPP::CommentLine << QsciLexerCPP::Comment;
			numbers.lexerStyles << QsciLexerCPP::Number;
			keywords.lexerStyles << QsciLexerCPP::Keyword;
			strings.lexerStyles << QsciLexerCPP::DoubleQuotedString;

			styleMaperList << prepr << comment << numbers << keywords << strings;
		}
		else if(lexerName.compare("makefile") == 0) {
			StyleMaper variables("variables"), targets("targets"), comment("comment");

			variables.lexerStyles << QsciLexerMakefile::Variable;
			targets.lexerStyles << QsciLexerMakefile::Target;
			comment.lexerStyles << QsciLexerMakefile::Comment;
			
			styleMaperList << variables << targets << comment;			
		}
		else if (lexerName.compare("python") == 0) {
			StyleMaper keywords("keywords"), comment("comment"), numbers("numbers"), 
					operators("operators"), identifiers("identifiers"), 
					functions("functions"),	stringSingle("stringSingle"), 
					stringDouble("stringDouble"), tripleSingle("tripleSingle"), 
					tripleDouble("tripleDouble"), decorators("decorators");

			keywords.lexerStyles << QsciLexerPython::Keyword;
			comment.lexerStyles << QsciLexerPython::Comment << QsciLexerPython::CommentBlock;
			numbers.lexerStyles << QsciLexerPython::Number;
			operators.lexerStyles << QsciLexerPython::Operator;
			identifiers.lexerStyles << QsciLexerPython::Identifier;
			functions.lexerStyles << QsciLexerPython::FunctionMethodName;
			stringSingle.lexerStyles << QsciLexerPython::SingleQuotedString;
			stringDouble.lexerStyles << QsciLexerPython::DoubleQuotedString;
			tripleSingle.lexerStyles << QsciLexerPython::TripleSingleQuotedString;
			tripleDouble.lexerStyles << QsciLexerPython::TripleDoubleQuotedString;
			decorators.lexerStyles << QsciLexerPython::Decorator;
			
			styleMaperList << keywords << comment << numbers << operators 
					<< identifiers << functions << stringSingle << stringDouble 
					<< tripleSingle << tripleDouble << decorators;
		}
		else if (lexerName.compare("bash") == 0) {
			StyleMaper variables("variables"), strings("strings"), 
					keywords("keywords"), comment("comment"), backticks("backticks");
			
			variables.lexerStyles << QsciLexerBash::Identifier;
			strings.lexerStyles << QsciLexerBash::DoubleQuotedString << QsciLexerBash::SingleQuotedString;
			keywords.lexerStyles << QsciLexerBash::Keyword << QsciLexerBash::Operator;
			comment.lexerStyles << QsciLexerBash::Comment;
			backticks.lexerStyles << QsciLexerBash::Backticks;
			
			styleMaperList << variables << strings << keywords << comment << backticks;
		}
		else if (lexerName.compare("html") == 0) {
			StyleMaper tags("tags"), attributes("attributes"), comment("comment"), 
					values("values"), entities("entities");
			
			tags.lexerStyles << QsciLexerHTML::Tag;
			attributes.lexerStyles << QsciLexerHTML::Attribute;
			comment.lexerStyles << QsciLexerHTML::HTMLComment;
			values.lexerStyles << QsciLexerHTML::HTMLSingleQuotedString << QsciLexerHTML::HTMLDoubleQuotedString 
					<< QsciLexerHTML::HTMLValue << QsciLexerHTML::HTMLNumber << QsciLexerHTML::OtherInTag;
			entities.lexerStyles << QsciLexerHTML::Entity;
			
			styleMaperList << tags << attributes << comment << values << entities;
		}
		else if (lexerName.compare("xml") == 0) {
			StyleMaper tags("tags"), attributes("attributes"), comment("comment"), 
					values("values"), entities("entities");
			
			tags.lexerStyles << QsciLexerHTML::Tag << QsciLexerHTML::UnknownTag << QsciLexerHTML::XMLTagEnd;
			attributes.lexerStyles << QsciLexerHTML::Attribute << QsciLexerHTML::UnknownAttribute;
			comment.lexerStyles << QsciLexerHTML::HTMLComment;
			values.lexerStyles << QsciLexerHTML::HTMLSingleQuotedString << QsciLexerHTML::HTMLDoubleQuotedString;
			entities.lexerStyles << QsciLexerHTML::Entity;
		
			styleMaperList << tags << attributes << comment << values << entities;
		}

		//	apply style
		StyleMaperList::const_iterator mIt = styleMaperList.begin();
		while (mIt != styleMaperList.end()) {
			if (styles.contains((*mIt).configStyle)) {
				const Style& style = styles[(*mIt).configStyle];
				QList<int>::const_iterator it = (*mIt).lexerStyles.begin();
				while (it != (*mIt).lexerStyles.end()) {
					applyLexerStyle(lexer, *it, style, font);
					it++;
				}
			}
			mIt++;
		}
	}*/
}

LexerStorage* LexerStorage::instance_ = 0;

LexerStorage::LexerStorage() {
	lsInt_ = new LSInterior();
}

LexerStorage::~LexerStorage() {
	delete lsInt_;
}

QsciLexer* LexerStorage::getLexerByFileName(const QString& fileName, const QFont& font) {
	QsciLexer* lexer = 0;

	QString ext = QFileInfo(fileName).suffix().toLower();
	QString lexerName;
	if (lsInt_->cppExtList_.contains(ext)) {
		lexerName = "cpp";
	}
	else if (ext.compare("java") == 0) {
		lexerName = "java";
	}
	else if (ext.compare("cs") == 0) {
		lexerName = "csharp";
	}
	else if (ext.isEmpty()) {
		if (fileName.contains("Makefile")) {
			lexerName = "makefile";
		}
	}
	else if (ext.compare("py") == 0) {
		lexerName = "python";
	}
	else if (ext.compare("rb") == 0) {
		lexerName = "ruby";
	}
	else if (ext.compare("pl") == 0) {
		lexerName = "perl";
	}
	else if (lsInt_->bashExtList_.contains(ext)) {
		lexerName = "bash";
	}
	else if (lsInt_->htmlExtList_.contains(ext)) {
		lexerName = "html";
	}
	else if (ext.compare("css") == 0) {
		lexerName = "css";
	}
	else if (lsInt_->xmlExtList_.contains(ext)) {
		lexerName = "xml";
	}
	else if (ext.compare("sql") == 0) {
		lexerName = "sql";
	}
	else if (ext.compare("js") == 0) {
		lexerName = "jscript";
	}
	else if (ext.compare("idl") == 0) {
		lexerName = "idl";
	}
	else if (ext.compare("d") == 0 || ext.compare("di") == 0) {
		lexerName = "d";
	}
	else if (ext.compare("lua") == 0 || ext.compare("tasklua") == 0) {
		lexerName = "lua";
	}

	if (!lexerName.isEmpty())
		lexer = getLexer(lexerName, font);

	return lexer;
}

QsciLexer* LexerStorage::getLexer(const QString& lexerName, const QFont& font) {
	QsciLexer* lexer = lsInt_->lexer(lexerName);

	if (lexer != 0) {
		lexer->setFont(font, -1);
		lsInt_->setCustomLexerStyle(lexer, lexerName, font);
	}

	return lexer;
}

LexerStorage* LexerStorage::instance() {
	if (instance_ == 0)
		instance_ = new LexerStorage();
	return instance_;
}
