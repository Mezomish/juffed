#ifndef _KEYS_PLUGIN_H_
#define _KEYS_PLUGIN_H_

class KeysPage;

#include <QtCore/QObject>

#include "JuffPlugin.h"

class KeysPlugin : public QObject, public JuffPlugin {
Q_OBJECT
#if QT_VERSION >= 0x050000
Q_PLUGIN_METADATA(IID JuffPlugin_iid)
#endif
Q_INTERFACES(JuffPlugin)
public:
	KeysPlugin();
	virtual ~KeysPlugin();

	//	info
	virtual QString name() const;
	virtual QString title() const;
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
