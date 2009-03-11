/*
JuffEd - A simple text editor
Copyright 2007-2009 Mikhail Murzin

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

#ifndef _JUFF_GUI_H_
#define _JUFF_GUI_H_

class AboutDlg;
class QMenu;
class QMainWindow;
	
#include <QtCore/QMap>
#include <QtCore/QString>

#include "Juff.h"
#include "SettingsDlg.h"

namespace Juff {
namespace GUI {

class GUI : public QObject {
Q_OBJECT
public:
	GUI();
	virtual ~GUI();

	QStringList getOpenFileNames(const QString& startDir, const QString& filters);
	QString getSaveFileName(const QString& curFileName, const QString& filters, bool& asCopy);
	QString getOpenSessionName(bool&);
	QString getSaveSessionName(const QString& session);
	bool getFindParams(QString&, QString&, DocFindFlags&);
	QString lastFindText() const;
	DocFindFlags lastFlags() const;

	void show();
	void setCentralWidget(QWidget*);
	void addToolBar(QToolBar*);
	void addToolBars(const ToolBarList&);
	void addDocks(const QWidgetList&);
	void setMainMenus(const MenuList&);

	QMenu* toolsMenu() const;

	void setToolBarIconSize(int);
	void setToolButtonStyle(Qt::ToolButtonStyle);
	void updateTitle(const QString& fileName, const QString& session, bool modified);
	void displayError(const QString&);
	void addStatusWidget(QWidget*);
	void setAdditionalStatusWidgets(const QWidgetList&);
	
	void saveState();
	void restoreState();

signals:
	void settingsApplied();
	void closeRequested(bool&);
	void docOpenRequested(const QString&);

private slots:
	void settings();
	void applySettings();
	void about();
	void aboutQt();

private:

	QMenu* fileMenu_;
	QMenu* toolsMenu_;
	QMenu* helpMenu_;

	ToolBarList toolBars_;
	QMainWindow* mw_;

	SettingsDlg* settDlg_;
	DocFindFlags lastFlags_;
	AboutDlg* aboutDlg_;
	QWidgetList statusWidgets_;
};

}	//	namespace GUI
}	//	namespace Juff

#endif
