#ifndef _KEYS_PLUGIN_H_
#define _KEYS_PLUGIN_H_

class KeysPage;

#include <QtCore>
#include <QtCore/QObject>

#include "JuffPlugin.h"

class KeysPlugin : public QObject, public JuffPlugin {
Q_OBJECT
Q_INTERFACES(JuffPlugin)
public:
	KeysPlugin();
	virtual ~KeysPlugin();

	//	info
	virtual QString name() const;
	virtual QString description() const;
	virtual QString targetEngine() const;

	virtual void init();
	virtual void allPluginsLoaded();

	//	controls
	virtual QWidget* settingsPage() const;
	virtual void applySettings();

private:
	KeysPage* w_;
};

#endif
