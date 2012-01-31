#ifndef __JUFFED_PYTHON_PLUGIN_H__
#define __JUFFED_PYTHON_PLUGIN_H__

#include <Python.h>

#include <QObject>

#include "JuffPlugin.h"

class PythonPlugin : public QObject, public JuffPlugin {
Q_OBJECT
Q_INTERFACES(JuffPlugin)

public:
	PythonPlugin();
	virtual ~PythonPlugin();
	void init();

	QString name() const;
	QString title() const;
	QString description() const;
	QString targetEngine() const;

	Juff::ActionList mainMenuActions(Juff::MenuID) const;

public slots:
	void onDocOpened(Juff::Document* doc, Juff::PanelIndex);
	void onDocActivated(Juff::Document*);
	void onDocClosed(Juff::Document* doc);
	void onDocRenamed(Juff::Document* doc, const QString& oldName);
	void onDocModified(Juff::Document* doc);

//	void shoot();
	void onAction();
//	void sortDocument();
//	void openSettings();

private:
	PyObject* getFunction(const char*) const;
	Juff::ActionList getActions() const;
	
	PyObject* globalDict_;
};

#endif
