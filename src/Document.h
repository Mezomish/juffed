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

#ifndef _JUFF_DOCUMENT_H_
#define _JUFF_DOCUMENT_H_

class QWidget;

#include "Juff.h"

#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/QMutex>
#include <QtCore/QTimer>

namespace Juff {
	class DocHandler;

class Document : public QObject {
Q_OBJECT
friend class Juff::DocHandler;
public:
	Document(const QString&);
	virtual ~Document();

	QString fileName() const;
	QString type() const;

	virtual bool isModified() const = 0;
	virtual void setModified(bool) = 0;
	virtual QWidget* widget() = 0;

	virtual void setFileName(const QString& fileName);
	virtual bool save(const QString&, QString& err);
	virtual void print() = 0;
	virtual void reload() = 0;

	//	TODO : make the following methods pure virtual
	virtual void undo() {}
	virtual void redo() {}
	virtual void cut() {}
	virtual void copy() {}
	virtual void paste() {}

	virtual void find(const QString&, const DocFindFlags&) {}
	virtual void replace(const QString&, const QString&, const DocFindFlags&) {}

	virtual QString text() const { return ""; }
	virtual QString text(int) const { return ""; }
	virtual QString selectedText() const { return ""; }
	virtual void getCursorPos(int&, int&) const {}
	virtual void setCursorPos(int, int) {}
	virtual void getSelection(int&, int&, int&, int&) const {}
	virtual void setSelection(int, int, int, int) {}
	virtual void insertText(const QString&) {}
	virtual void removeSelectedText() {}
	virtual int curLine() const { return 0; }
	virtual void gotoLine(int) {}
	virtual int curScrollPos() const { return 0; }
	virtual void setScrollPos(int) {}
	virtual int lineCount() const { return 0; }
	virtual QString charset() const { return ""; }
	virtual void setCharset(const QString&) {}
	
	virtual void applySettings() {}

	/**
	*	Do not reimplement this method anywhere but at NullDoc class!
	*/
	virtual bool isNull() const;
	
	virtual void updateActivated() {}
	virtual void addContextMenuActions(const ActionList&) {}

signals:
	void modified(bool);
	void fileNameChanged(const QString& oldFileName);
	void cursorPositionChanged(int, int);
	void contextMenuCalled(int, int);

protected slots:
	void checkLastModified();

protected:
	QString type_;

private:
	QString fileName_;
	QDateTime lastModified_;
	QTimer* modCheckTimer_;
	QMutex checkingMutex_;
};

}	//	namespace Juff

#endif
