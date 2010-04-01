#ifndef __JUFFED_ICON_MANAGER_H__
#define __JUFFED_ICON_MANAGER_H__

#include <QIcon>

#include "Enums.h"

class IconManager {
public:
	static IconManager* instance();
	
	QIcon icon(Juff::ActionID) const;
	int iconSize() const;
	void setIconSize(int);
	QString iconTheme() const;
	void setIconTheme(const QString&);
	
private:
	IconManager();
	QIcon defaultIcon(Juff::ActionID) const;

	class Interior;
	Interior* int_;
	
	static IconManager* instance_;
};

#endif // __JUFFED_ICON_MANAGER_H__
