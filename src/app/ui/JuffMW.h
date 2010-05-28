/*
JuffEd - An advanced text editor
Copyright 2007-2010 Mikhail Murzin

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

#ifndef __JUFFED_MAIN_WINDOW_H__
#define __JUFFED_MAIN_WINDOW_H__

class AboutDlg;
class FindDlg;
class QHBoxLayout;
class QVBoxLayout;
class Popup;

#include "Types.h"

#include <QMainWindow>

namespace Juff {
	class Document;
}

class JuffMW : public QMainWindow {
Q_OBJECT
public:
	JuffMW();

	void setViewer(QWidget*);
	QString getOpenFileName(const QString& dir, const QString& filters);
	QStringList getOpenFileNames(const QString& dir, const QString& filters);
	QString getSaveFileName(const QString& curFileName, const QString& filters);
	QString getSavePrjName(const QString& title);
	int getGotoLineNumber(int);
	QString getJumpToFileName(const QStringList&);
	int askForSave(const QString& fileName);
	bool askForSave(const QStringList&, QStringList&);

	// information display
	void addStatusWidget(QWidget*, int);
	void addToolBar(QToolBar*);
	void message(const QIcon& icon, const QString& title, const QString& message, int timeout = 10);

	void showFindDialog(const QString&, bool replace = false);
	void hideFindDialog();
	void getSearchParams(Juff::SearchParams&);

	bool isFullScreen() const;
	void toggleFullscreen();

	void applySettings();
	void saveState();
	void restoreState();

signals:
	void closeRequested(bool&);
	void searchRequested(const Juff::SearchParams&);

public slots:
	void about();
	void aboutQt();

protected:
	virtual void closeEvent(QCloseEvent*);
	virtual void resizeEvent(QResizeEvent*);
	virtual void moveEvent(QMoveEvent*);
	virtual bool eventFilter(QObject*, QEvent*);

private:
	void resizePopup(int);

	QWidget* viewer_;
	QWidget* mainWidget_;
	AboutDlg* aboutDlg_;
	FindDlg* findDlg_;
	QWidget* statusWidget_;
	QHBoxLayout* statusLayout_;
	QList<QToolBar*> allToolBars_;
	QList<QToolBar*> hiddenToolBars_;

	Popup* popup_;
};

#endif // __JUFFED_MAIN_WINDOW_H__
