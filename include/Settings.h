#ifndef _SETTINGS_H_
#define _SETTINGS_H_

class QString;
class SettingsData;

#include <QtCore/QStringList>
#include <QtCore/QVariant>

class Settings {
public:
	static void read();
	static void write();

	static int count();

protected:
	static bool valueExists(const QString& section, const QString& key);
	static QString stringValue(const QString& section, const QString& key, const QString& def = "");
	static int intValue(const QString& section, const QString& key, int def = 0);
	static bool boolValue(const QString& section, const QString& key, bool def = false);
	static QVariant value(const QString& section, const QString& key);
	
	static void setValue(const QString& section, const QString& key, const QVariant& value);

	static QStringList sectionList();
	static QStringList keyList(const QString& section);

private:
	static SettingsData* settData_;
};

#endif
