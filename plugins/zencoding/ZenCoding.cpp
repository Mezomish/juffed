#include <QDebug>

#include "ZenCoding.h"

#include <QtCore>
//#include <QtGui>
#include "Document.h"
#include "EditorSettings.h"
#include "DocWrapper.h"

void readFile(QScriptEngine& engine, const QString& fileName) {
	QString scriptFileName(QString("/home/mrz/zen/javascript/") + fileName);
	QFile scriptFile(scriptFileName);
	if ( scriptFile.open(QIODevice::ReadOnly) ) {
		QScriptValue result = engine.evaluate(scriptFile.readAll());
		scriptFile.close();
		if ( engine.hasUncaughtException() ) {
			qDebug() << "Error in file " << fileName;
			qDebug() << "Line: " << engine.uncaughtExceptionLineNumber();
			qDebug() << engine.uncaughtExceptionBacktrace();
		}
	}
	else {
		qDebug() << "Failed to open file " << fileName;
	}
}

ZenCoding::ZenCoding() : QObject(), JuffPlugin() {
	expandAct_ = new QAction(tr("Zen Coding expand"), this);
	expandAct_->setShortcut(QKeySequence("Ctrl+,"));
	connect(expandAct_, SIGNAL(triggered()), this, SLOT(expandAbbr()));
	
	docWrapper_ = new DocWrapper();
	QScriptValue wrapperObj = engine_.newQObject(docWrapper_);
	engine_.globalObject().setProperty("docFacade", wrapperObj);
	
	readFile(engine_, "zen_settings.js");
	readFile(engine_, "zen_resources.js");
	readFile(engine_, "base64.js");
	readFile(engine_, "html_matcher.js");
	readFile(engine_, "zen_file.js");
	readFile(engine_, "zen_coding.js");
	readFile(engine_, "zen_actions.js");
	readFile(engine_, "zen_parser.js");
	readFile(engine_, "ZenScript.js");
/*	QString scriptFileName("/home/mrz/zen/javascript/ZenScript.js");
	QFile scriptFile(scriptFileName);
	scriptFile.open(QIODevice::ReadOnly);
	QString scriptText = scriptFile.readAll();
	scriptFile.close();
	
	QScriptValue result = engine_.evaluate(scriptText);
	if ( engine_.hasUncaughtException() ) {
		qDebug() << "Error!!!!";
	}*/
}

void ZenCoding::init() {
	connect(api(), SIGNAL(docActivated(Juff::Document*)), SLOT(onDocActivated(Juff::Document*)));
}

ZenCoding::~ZenCoding() {
	delete docWrapper_;
}

QString ZenCoding::name() const {
	return "Zen Coding";
}

QString ZenCoding::targetEngine() const {
	return "all";
}

QString ZenCoding::description() const {
	return "Zen Coding plugin";
}

Juff::ActionList ZenCoding::mainMenuActions(Juff::MenuID id) const {
	Juff::ActionList list;
	if ( Juff::MenuTools == id ) {
		list << expandAct_;
	}
	return list;
}



void ZenCoding::expandAbbr() {
	Juff::Document* doc = api()->currentDocument();
	if ( doc->isNull() ) {
		return;
	}
	
	engine_.evaluate("expand()");
	if ( engine_.hasUncaughtException() ) {
		qDebug() << "Error!!!!";
//		qDebug() << engine_.uncaughtException();
		qDebug() << engine_.uncaughtExceptionLineNumber();
		qDebug() << engine_.uncaughtExceptionBacktrace();
	}
}


void ZenCoding::onDocActivated(Juff::Document* doc) {
	docWrapper_->setDoc(doc);
	engine_.evaluate("docChanged();");
}

Q_EXPORT_PLUGIN2(zencoding, ZenCoding)
