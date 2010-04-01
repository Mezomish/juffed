#ifndef __JUFFED_SETTINGS_H__
#define __JUFFED_SETTINGS_H__

class QString;

#include <QtCore/QStringList>
#include <QtCore/QVariant>

class Settings {
public:
	static void read();
	static void write();

//	static int count();

	static QVariant defaultValue(const QString& section, const QString& key);

protected:
	static bool valueExists(const QString& section, const QString& key);
	static int intValue(const QString& section, const QString& key);
	static bool boolValue(const QString& section, const QString& key);
	static QString stringValue(const QString& section, const QString& key);
	static QVariant value(const QString& section, const QString& key, const QVariant& defValue = QVariant());
//
	static void setValue(const QString& section, const QString& key, const QVariant& value);
//
//	static QStringList sectionList();
//	static QStringList keyList(const QString& section);

private:
	class SettingsData;
	static SettingsData* settData_;
};

#endif // __JUFFED_SETTINGS_H__
