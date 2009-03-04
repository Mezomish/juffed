#ifndef _JUFF_ICON_MANAGER_H_
#define _JUFF_ICON_MANAGER_H_

class IMInterior;

#include "Juff.h"
#include <QtGui/QIcon>

/**
*	class IconManager
*
*	Manages icon theme, reads icon theme from disk and provides 
*	main window icons.
*/

class IconManager {
public:
	/**
	*	instance()
	*
	*	Returns an instance of IconManager
	*/
	static IconManager* instance();

	/**
	*	~IconManager()
	*
	*	Destructor
	*/
	virtual ~IconManager();

	/**
	*	TODO
	*/
	void setCurrentIconTheme(const QString&, int size);
	
	/**
	*	iconThemes()
	*
	*	Returns the list of all available icon themes.
	*/
	QStringList themeList() const;

	/**
	*	icon()
	*
	*	Returns an icon by action name.
	*/
	QIcon getIcon(Juff::CommandID);
	
protected:
	IconManager();

private:
	QIcon getDefaultIcon(Juff::CommandID);

	static IconManager* instance_;
	IMInterior* imInt_;
};

#endif // _ICON_MANAGER_H_
