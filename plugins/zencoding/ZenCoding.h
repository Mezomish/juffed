#ifndef __ZEN_CODING_PLUGIN_H__
#define __ZEN_CODING_PLUGIN_H__

#include <QObject>
#include <QScriptEngine>

#include "JuffPlugin.h"

namespace Juff {
	class Document;
}

class DocWrapper;

class ZenCoding : public QObject, public JuffPlugin {
Q_OBJECT
Q_INTERFACES(JuffPlugin)

public:
	ZenCoding();
	virtual ~ZenCoding();
	void init();

	QString name() const;
	QString description() const;
	QString targetEngine() const;

	Juff::ActionList mainMenuActions(Juff::MenuID) const;

public slots:
	void expandAbbr();
	void onDocActivated(Juff::Document*);

private:
	QAction* expandAct_;
	QScriptEngine engine_;
	DocWrapper* docWrapper_;
};

#endif // __ZEN_CODING_PLUGIN_H__
