#include "PluginsSettings.h"

bool PluginsSettings::pluginEnabled(const QString& name) {
	return Settings::boolValue(name, "enabled", true);
}

QString PluginsSettings::pluginPath(const QString& name) {
	return Settings::stringValue(name, "path", "");
}

//void PluginsSettings::setPluginEnabled(const QString& name, bool enable) {
//	Settings::setValue(name, "enabled", enable);
//}

void PluginsSettings::setPluginPath(const QString& name, const QString& path) {
	Settings::setValue(name, "path", path);
}
