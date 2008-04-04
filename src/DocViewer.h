/*
JuffEd - A simple text editor
Copyright 2007-2008 Mikhail Murzin

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

#ifndef _DOC_VIEWER_H_
#define _DOC_VIEWER_H_

class DocViewerInterior;
class QString;

#include <QtCore/QObject>
#include <QtGui/QWidgetList>

class DocViewer : public QObject {
Q_OBJECT
public:
	DocViewer(QWidget*);
	virtual ~DocViewer();
	
	QWidget* widget();
	void addDocView(const QString&, QWidget*);
	QWidget* currentView();
	
	void setCurrentView(QWidget*);
	void setDocViewTitle(QWidget*, const QString&);
	void nextView();
	void prevView();
	
	void updateCurrentViewInfo();
	void applySettings();

	void getViewsOrder(QWidgetList&);
	
signals:
	void docViewChanged(QWidget*);
	void docViewCloseRequested(QWidget*);
	
private slots:
	void onChangeCurrent(int);
	void onTabCloseRequested(int);
	
private:
	DocViewerInterior* vInt_;
};

#endif
