/*
JuffEd - A simple text editor
Copyright 2007-2008 Murzin Mikhail

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License 
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef _ABOUT_DLG_H_
#define _ABOUT_DLG_H_

class QIcon;
class QUrl;
class QString;
class DialogInterior;

#include <QtGui/QDialog>

class AboutDlg : public QDialog {
Q_OBJECT
public:
	AboutDlg(QWidget* parent = 0, Qt::WindowFlags f = 0);
	virtual ~AboutDlg();
	
	/**
	*/
	void setProgramName(const QString& name);
	
	/**
	*/
	void setIcon(const QIcon&);
	
	/** setText
	* Sets the main text of the dialog
	*
	* @param text Main dialog text
	*/
	void setText(const QString& text);
	
	/**
	*/
	void setAuthors(const QString&);
	
	/**
	*/
	void setThanks(const QString&);
	
	/** setLicense
	* Sets the text of the program license
	*
	* @param str License file name if isFileName is true,
	*            license text otherwise
	*/
	void setLicense(const QString& str, bool isFileName = true);

private slots:
	void gotoUrl(const QUrl&);
	
private:
	DialogInterior* dlgInt_;
};

#endif
