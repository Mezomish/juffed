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

#ifndef _JUFF_VIEWER_H_
#define _JUFF_VIEWER_H_

#include <QtGui/QWidget>

namespace Juff {

class Document;

namespace GUI {

class Viewer : public QObject {
Q_OBJECT
public:
	Viewer();
	virtual ~Viewer();
	
	QWidget* widget();

	void addDoc(Document*, int panel = 1);
	void setDocModified(Document*, bool modified);
	void updateDocTitle(Document*);
	void removeDoc(Document*);
	void activateDoc(Document*);
	int curPanel() const;
	void applySettings();

	/**
	*	Returns list of doc views that are opened at panel \par panel.
	*/
	void getViewsList(int panel, QWidgetList& list) const;

	QWidget* curDoc() const;

signals:
	void curDocChanged(QWidget*);
	void requestDocName(QWidget*, QString&);
	void requestDocClose(QWidget*);

public slots:
	void nextDoc();
	void prevDoc();

private slots:
	void curIndexChanged(int);
	void onFileNameRequested(int, QString&);
	void onTabCloseRequested(int);

private:
	class Interior;
	Interior* vInt_;
};

}	//	namespace GUI
}	//	namespace Juff

#endif
