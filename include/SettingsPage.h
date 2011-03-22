#ifndef __JUFFED_SETTINGS_PAGE_H__
#define __JUFFED_SETTINGS_PAGE_H__

class SettingsItem;

#include <QWidget>

class SettingsPage : public QWidget {
Q_OBJECT
public:
	SettingsPage(QWidget*);
	virtual ~SettingsPage() {}
	
	virtual void init() = 0;
	virtual void apply();

protected:
	QList<SettingsItem*> items_;
};

#endif // __JUFFED_SETTINGS_PAGE_H__
