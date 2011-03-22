#ifndef __JUFFED_SETTINGS_PAGE_H__
#define __JUFFED_SETTINGS_PAGE_H__

#include "LibConfig.h"

class SettingsItem;

#include <QWidget>

class LIBJUFF_EXPORT SettingsPage : public QWidget {
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
