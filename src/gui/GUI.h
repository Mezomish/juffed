/*
JuffEd - An advanced text editor
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

#include "GUIManager.h"
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
	QString getSaveFileName(const QString& curFileName, const QString& filters, bool& asCopy, QString& charset);
	QString getOpenSessionName(bool&);
	QString getSaveSessionName(const QString& session);
	bool getFindParams(QString&, QString&, DocFindFlags&);
	QString lastFindText() const;
	DocFindFlags lastFlags() const;

	void show();
	void activateMW();
	void setCentralWidget(QWidget*);

	void addMenu(const QString& type, QMenu* menu);
	void addMenus(const QString& type, const Juff::MenuList menus);
	void addToolBar(const QString& type, QToolBar* tb);
	void addToolBars(const QString& type, const Juff::ToolBarList toolBars);
	void addDocks(const QString&, const QWidgetList&);
	void addAction(const QString& type, QAction* act);
	void addActions(const QString& type, const Juff::ActionList& list);
	void setCurType(const QString&);

	QMenu* toolsMenu() const;

	void setToolBarIconSize(int);
	void setToolButtonStyle(Qt::ToolButtonStyle);
	void updateTitle(const QString& fileName, const QString& session, bool modified);
	void displayError(const QString&);
	void addStatusWidget(QWidget*);
	void setAdditionalStatusWidgets(const QWidgetList&);
	void addPluginSettingsPage(const QString&, QWidget*);
	
	void saveState();
	void restoreState();

signals:
	void settingsApplied();
	void closeRequested(bool&);

private slots:
	void settings();
	void applySettings();
	void about();
	void aboutQt();

private:

	QMenu* fileMenu_;
	QMenu* toolsMenu_;
	QMenu* toolBarsMenu_;
	QMenu* docksMenu_;
	QMenu* helpMenu_;

	QMainWindow* mw_;
	GUIManager guiManager_;

	SettingsDlg* settDlg_;
	DocFindFlags lastFlags_;
	AboutDlg* aboutDlg_;
	QWidgetList statusWidgets_;
};

}	//	namespace GUI
}	//	namespace Juff

#endif
