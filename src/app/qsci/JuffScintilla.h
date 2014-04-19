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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef _JUFF_SCINTILLA_H_
#define _JUFF_SCINTILLA_H_

#include "QSciLibConfig.h"

#include "Types.h"

#include <Qsci/qsciscintilla.h>

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMenu>

class QSCI_ENGINE_EXPORT JuffScintilla : public QsciScintilla {
Q_OBJECT
public:
	enum HLMode {
		HLCurrentWord,
		HLSearch
	};
	JuffScintilla();
	virtual ~JuffScintilla();
	
	virtual void cut();
	virtual void paste();
	
	void showLineNumbers(bool);
	bool lineNumbersVisible() const;

	QString wordUnderCursor();
	
	// TODO : refactor this method
	void highlightText(HLMode, const Juff::SearchParams&);
	void highlight(HLMode, int, int, int, int);

signals:
	void contextMenuCalled(int, int);
	void focusReceived();
	void markersMenuRequested(const QPoint&);
	void escapePressed();

protected slots:
	void updateLineNumbers();

protected:
	virtual void dragEnterEvent(QDragEnterEvent* e);
	virtual void dropEvent(QDropEvent* e);
	virtual void contextMenuEvent(QContextMenuEvent* e);
	virtual void focusInEvent(QFocusEvent* e);
	virtual void focusOutEvent(QFocusEvent* e);
	virtual void keyPressEvent(QKeyEvent*);
	virtual void wheelEvent(QWheelEvent*);

private:
	void cancelRectInput();
	void getOrderedSelection(int&, int&, int&, int&);
	void deleteRectSelection();
	void deleteRectSelection(int, int, int, int);
	long curPos() const;
	void highlight(int start, int end, int ind);
	void clearHighlighting();
	void initHighlightingStyle(int id, const QColor &color);
//	bool findML(const QString& s, const DocFindFlags& flags);

	QMenu* contextMenu_;
	bool showLineNumbers_;
};

#endif
