#ifndef _ICON_MANAGER_H_
#define _ICON_MANAGER_H_

class IMInterior;
	
#include <QtGui/QIcon>

class IconManager {
public:
	static IconManager* instance();
	virtual ~IconManager();

	void loadTheme(const QString&);
	QIcon icon(const QString& actionStr) const;
	QStringList iconThemes() const;

protected:
	IconManager();

private:
	static IconManager* instance_;
	IMInterior* imInt_;
};

#endif // _ICON_MANAGER_H_
