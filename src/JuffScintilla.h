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

#ifndef _JUFF_SCINTILLA_H_
#define _JUFF_SCINTILLA_H_

#include "Juff.h"

#include <Qsci/qsciscintilla.h>

#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QMenu>

namespace Juff {

class JuffScintilla : public QsciScintilla {
Q_OBJECT
public:
	JuffScintilla();
	virtual ~JuffScintilla();

	bool find(const QString& s, const DocFindFlags& flags);
	void replaceSelected(const QString& targetText, bool backwards);
	void addContextMenuActions(const ActionList&);
	void showLineNumbers(bool);

signals:
	void contextMenuCalled(int, int);

protected slots:
	void updateLineNumbers();

protected:
	virtual void dragEnterEvent(QDragEnterEvent* e);
	virtual void dropEvent(QDropEvent* e);
	virtual void contextMenuEvent(QContextMenuEvent* e);
	virtual void focusInEvent(QFocusEvent* e);

private:
	QMenu* contextMenu_;
	bool showLineNumbers_;
};

}	//	namespace Juff

#endif
