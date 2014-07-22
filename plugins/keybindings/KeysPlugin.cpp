#include "KeysPlugin.h"

#include "KeysPage.h"
#include "Log.h"
#include "CommandStorageInt.h"

KeysPlugin::KeysPlugin() : QObject(), JuffPlugin() {
	w_ = 0;
}

KeysPlugin::~KeysPlugin() {
	delete w_;
}

void KeysPlugin::init() {
	w_ = new KeysPage(api()->commandStorage());
}

void KeysPlugin::allPluginsLoaded() {
//	LOGGER;
	w_->init();
}

QString KeysPlugin::name() const {
	return "Keybindings";
}

QString KeysPlugin::title() const {
	return tr("Key bindings");
}

QString KeysPlugin::targetEngine() const {
	return "all";
}

QString KeysPlugin::description() const {
	return "Allows you to configure shortcuts";
}

QWidget* KeysPlugin::settingsPage() const {
	return w_;
}

void KeysPlugin::applySettings() {
//	LOGGER;
	w_->apply();
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(keybindings, KeysPlugin)
#endif
