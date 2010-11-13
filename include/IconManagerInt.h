#ifndef __JUFFED_ICON_MANAGER_INTERFACE_H__
#define __JUFFED_ICON_MANAGER_INTERFACE_H__

#include <QIcon>

class IconManagerInt {
public:
	virtual ~IconManagerInt() {}
	/**
	* Returns an icon of the current icon theme and the current size 
	* for the specified \param key. If the current theme doesn't contain
	* an appropriate icon then returns a built-in icon (from "<default>" theme).
	* If there is no default icon the returns an empty icon QIcon().
	*/
	virtual QIcon icon(const QString& key) const = 0;

	/**
	* Returns the current icon size. The default size is 16.
	*/
	virtual int size() const = 0;

	/**
	* Sets the icon size to \param size.
	*/
	virtual void setSize(int) = 0;
};

#endif // __JUFFED_ICON_MANAGER_INTERFACE_H__
