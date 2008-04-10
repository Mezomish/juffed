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
		Rule(Style st) {
			style = st;
		}
		
		QList<int> hlElements;
		Style style;
	};
	
	typedef QList<Rule> Scheme;
	typedef QMap<QString, Style> StyleMap;
	typedef QMap<QString, Scheme> SchemeMap;

class LSInterior {
public:
	LSInterior() {
		cppExtList_ << "cpp" << "cxx" << "c++" << "c" << "cc" << "h" << "hpp" << "hxx" << "h++";
		bashExtList_ << "sh" << "run";
		htmlExtList_ << "html" << "htm" << "php";
	}
	~LSInterior() {
	}
	
	QsciLexer* lexer(const QString& name/*, const QFont&*/);
	void readCustomStyle(const QString& name);
	void applyCustomStyle(const QString& name, const QFont& font);
	
	QStringList cppExtList_;
	QStringList bashExtList_;
	QStringList htmlExtList_;
	
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
	
	QString defColorStr = schEl.attribute("defaultColor", "#000000");
	QString defBgColorStr = schEl.attribute("defaultBgColor", "#ffffff");
	QString defBoldStr = schEl.attribute("defaultBold", "false");
	QString defItalicStr = schEl.attribute("defaultItalic", "false");
	
	while (!styleNode.isNull()) {
		QDomElement styleEl = styleNode.toElement();
		if (styleEl.tagName().toLower().compare("style") == 0) {
			Style style;
			QString name = styleEl.attribute("name", "");
			if (!name.isEmpty()) {
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
	nm = nm.replace(QString("+"), "plus").replace(QString("#"), "csharp").toLower();
	QString fileName = QString("%1.xml").arg(nm);
//	Log::debug(fileName);
	fileName = AppInfo::configDir() + "/hlschemes/" + fileName;
	QFile file(fileName);
	if (QFileInfo(fileName).suffix().toLower().compare("xml") != 0 || !file.open(QIODevice::ReadOnly)) {
		return;
	}
	QString err;
	if (!doc.setContent(&file, &err)) {
		Log::print(err);
		file.close();
		return;
	}
	file.close();

	QDomElement docElem = doc.documentElement();
	QDomNode schNode = docElem.firstChild();
	StyleMap styles;
	while(!schNode.isNull()) {
		QDomElement schEl = schNode.toElement();
		if(!schEl.isNull()) {
			if (schEl.tagName().toLower().compare("scheme") == 0) {
				parseScheme(schEl, styles);
			}
		}
		schNode = schNode.nextSibling();
	}


	if (name.compare("C++") == 0) {
		Rule preprRule(styles["preprocessor"]), commRule(styles["comment"]), 
				numbRule(styles["numbers"]), keywRule(styles["keywords"]), strRule(styles["strings"]);

		preprRule.hlElements << QsciLexerCPP::PreProcessor;
		commRule.hlElements << QsciLexerCPP::CommentLine << QsciLexerCPP::Comment;
		numbRule.hlElements << QsciLexerCPP::Number;
		keywRule.hlElements << QsciLexerCPP::Keyword;
		strRule.hlElements << QsciLexerCPP::DoubleQuotedString;

		Scheme cppSch;
		cppSch << preprRule << commRule << numbRule << keywRule << strRule;
		schemes_[name] = cppSch;
	}
	else if(name.compare("Makefile") == 0) {
		Rule variables(styles["variables"]), targets(styles["targets"]), comment(styles["comment"]);

		variables.hlElements << QsciLexerMakefile::Variable;
		targets.hlElements << QsciLexerMakefile::Target;
		comment.hlElements << QsciLexerMakefile::Comment;
			
		Scheme mkSch;
		mkSch << variables << targets << comment;			
		schemes_[name] = mkSch;
	}
	else if (name.compare("Python") == 0) {
		Rule keywords(styles["keywords"]), comment(styles["comment"]), numbers(styles["numbers"]), 
				operators(styles["operators"]), identifiers(styles["identifiers"]), 
				functions(styles["functions"]),	stringSingle(styles["stringSingle"]), 
				stringDouble(styles["stringDouble"]), tripleSingle(styles["tripleSingle"]), 
				tripleDouble(styles["tripleDouble"]), decorators(styles["decorators"]);
		
		keywords.hlElements << QsciLexerPython::Keyword;
		comment.hlElements << QsciLexerPython::Comment << QsciLexerPython::CommentBlock;
		numbers.hlElements << QsciLexerPython::Number;
		operators.hlElements << QsciLexerPython::Operator;
		identifiers.hlElements << QsciLexerPython::Identifier;
		functions.hlElements << QsciLexerPython::FunctionMethodName;
		stringSingle.hlElements << QsciLexerPython::SingleQuotedString;
		stringDouble.hlElements << QsciLexerPython::DoubleQuotedString;
		tripleSingle.hlElements << QsciLexerPython::TripleSingleQuotedString;
		tripleDouble.hlElements << QsciLexerPython::TripleDoubleQuotedString;
		decorators.hlElements << QsciLexerPython::Decorator;
		
		Scheme pySch;
		pySch << keywords << comment << numbers << operators 
				<< identifiers << functions << stringSingle << stringDouble 
				<< tripleSingle << tripleDouble << decorators;
		schemes_[name] = pySch;
	}
	else if (name.compare("XML") == 0) {
		Rule tags(styles["tags"]), attributes(styles["attributes"]), comment(styles["comment"]), 
				values(styles["values"]), entities(styles["entities"]);
			
		tags.hlElements << QsciLexerHTML::Tag << QsciLexerHTML::UnknownTag << QsciLexerHTML::XMLTagEnd;
		attributes.hlElements << QsciLexerHTML::Attribute << QsciLexerHTML::UnknownAttribute;
		comment.hlElements << QsciLexerHTML::HTMLComment;
		values.hlElements << QsciLexerHTML::HTMLSingleQuotedString << QsciLexerHTML::HTMLDoubleQuotedString;
		entities.hlElements << QsciLexerHTML::Entity;
		
		Scheme xmlSch;
		xmlSch << tags << attributes << comment << values << entities;
		schemes_[name] = xmlSch;
	}
	else if (name.compare("Bash") == 0) {
		Rule variables(styles["variables"]), strings(styles["strings"]), 
				keywords(styles["keywords"]), comment(styles["comment"]), backticks(styles["backticks"]);
			
		variables.hlElements << QsciLexerBash::Identifier;
		strings.hlElements << QsciLexerBash::DoubleQuotedString << QsciLexerBash::SingleQuotedString;
		keywords.hlElements << QsciLexerBash::Keyword << QsciLexerBash::Operator;
		comment.hlElements << QsciLexerBash::Comment;
		backticks.hlElements << QsciLexerBash::Backticks;

		Scheme bashSch;
		bashSch << variables << strings << keywords << comment << backticks;
		schemes_[name] = bashSch;
	}
	else if (name.compare("HTML") == 0) {
		Rule tags(styles["tags"]), attributes(styles["attributes"]), comment(styles["comment"]), 
				values(styles["values"]), entities(styles["entities"]);
			
		tags.hlElements << QsciLexerHTML::Tag;
		attributes.hlElements << QsciLexerHTML::Attribute;
		comment.hlElements << QsciLexerHTML::HTMLComment;
		values.hlElements << QsciLexerHTML::HTMLSingleQuotedString << QsciLexerHTML::HTMLDoubleQuotedString 
				<< QsciLexerHTML::HTMLValue << QsciLexerHTML::HTMLNumber << QsciLexerHTML::OtherInTag;
		entities.hlElements << QsciLexerHTML::Entity;

		Scheme htmlSch;
		htmlSch << tags << attributes << comment << values << entities;
		schemes_[name] = htmlSch;
	}
}

void LSInterior::applyCustomStyle(const QString& name, const QFont& font) {
	QsciLexer* lex = lexers_.value(name, 0);
	if (lex != 0) {
		lex->setFont(font, -1);

		if (name.compare("none") == 0) {
			lex->setColor(Qt::black, -1);
			lex->setPaper(Qt::white, -1);
		}
		else {
			if (schemes_.contains(name)) {
				Scheme& scheme = schemes_[name];
				
				foreach (Rule const& rule, scheme) {
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
	if (lexers_.contains(name)) {
		//	return lexer with this name 
		//	if it already exists
		return lexers_[name];
	}
	else {
		//	if doesn't exist yet, create lexer
		//	if it's type is known. If unknown,
		//	return 0
		QsciLexer* newLexer = 0;
		if (name.compare("C++") == 0) {
			newLexer = new QsciLexerCPP();
		}
		else if (name.compare("Java") == 0) {
			newLexer = new QsciLexerJava();
		}
		else if (name.compare("C#") == 0) {
			newLexer = new QsciLexerCSharp();
		}
		else if (name.compare("Makefile") == 0) {
			newLexer = new QsciLexerMakefile();
		}
		else if (name.compare("Python") == 0) {
			newLexer = new QsciLexerPython();
		}
		else if (name.compare("Ruby") == 0) {
			newLexer = new QsciLexerRuby();
		}
		else if (name.compare("Perl") == 0) {
			newLexer = new QsciLexerPerl();
		}
		else if (name.compare("Bash") == 0) {
			newLexer = new QsciLexerBash();
		}
		else if (name.compare("HTML") == 0) {
			newLexer = new QsciLexerHTML();
		}
		else if (name.compare("CSS") == 0) {
			newLexer = new QsciLexerCSS();
		}
		else if (name.compare("XML") == 0) {
			newLexer = new QsciLexerHTML();
		}
		else if (name.compare("SQL") == 0) {
			newLexer = new QsciLexerSQL();
		}
		else if (name.compare("JavaScript") == 0) {
			newLexer = new QsciLexerJavaScript();
		}
		else if (name.compare("IDL") == 0) {
			newLexer = new QsciLexerIDL();
		}
		else if (name.compare("D") == 0) {
			newLexer = new QsciLexerD();
		}
		else if (name.compare("Lua") == 0) {
			newLexer = new QsciLexerLua();
		}
		else if (name.compare("none") == 0) {
			newLexer = new QsciLexerPython();
		}
	
		if (newLexer != 0) {
			lexers_[name] = newLexer;
			if (!name.isEmpty() && name.compare("none") != 0) {
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
	delete lsInt_;
}

QString LexerStorage::lexerName(const QString& fileName) const {
	QString ext = QFileInfo(fileName).suffix().toLower();
	QString name = "none";

	if (ext.isEmpty()) {
		if (fileName.contains("Makefile")) {
			name = "Makefile";
		}
	}
	else if (lsInt_->cppExtList_.contains(ext)) {
		name = "C++";
	}
	else if (ext.compare("java") == 0) {
		name = "Java";
	}
	else if (ext.compare("cs") == 0) {
		name = "C#";
	}
	else if (ext.compare("py") == 0) {
		name = "Python";
	}
	else if (ext.compare("rb") == 0) {
		name = "Ruby";
	}
	else if (ext.compare("pl") == 0) {
		name = "Perl";
	}
	else if (lsInt_->bashExtList_.contains(ext)) {
		name = "Bash";
	}
	else if (lsInt_->htmlExtList_.contains(ext)) {
		name = "HTML";
	}
	else if (ext.compare("css") == 0) {
		name = "CSS";
	}
	else if (ext.compare("xml") == 0) {
		name = "XML";
	}
	else if (ext.compare("sql") == 0) {
		name = "SQL";
	}
	else if (ext.compare("js") == 0) {
		name = "JavaScript";
	}
	else if (ext.compare("idl") == 0) {
		name = "IDL";
	}
	else if (ext.compare("d") == 0 || ext.compare("di") == 0) {
		name = "D";
	}
	else if (ext.compare("lua") == 0 || ext.compare("tasklua") == 0) {
		name = "Lua";
	}

	return name;
}

QsciLexer* LexerStorage::lexer(const QString& lexerName, const QFont& font) {
	QsciLexer* lex = lsInt_->lexer(lexerName);

	if (lex != 0) {
		lsInt_->applyCustomStyle(lexerName, font);
	}

	return lex;
}

void LexerStorage::getLexersList(QStringList& list) {
	list.clear();
	list << "none" << "Bash" << "C++" << "C#" << "CSS" << "D" << "HTML" << "Java" 
				<< "JavaScript" << "IDL" << "Lua" << "Makefile" << "Perl" << "Python" 
				<< "Ruby" << "SQL" << "XML";
}

void LexerStorage::updateLexer(const QString& name, const QFont& font) {
	if (lsInt_->lexers_.contains(name)) {
		QsciLexer* lex = lsInt_->lexers_[name];
		lex->setFont(font);
		lex->refreshProperties();

		lsInt_->applyCustomStyle(name, font);
	}
}
	
LexerStorage* LexerStorage::instance() {
	if (instance_ == 0)
		instance_ = new LexerStorage();
	return instance_;
}
