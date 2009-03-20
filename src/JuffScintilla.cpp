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

#include "JuffScintilla.h"

#include "CommandStorage.h"

namespace Juff {

JuffScintilla::JuffScintilla() : QsciScintilla() {
	contextMenu_ = new QMenu();
	CommandStorage* st = CommandStorage::instance();
	contextMenu_->addAction(st->action(ID_EDIT_CUT));
	contextMenu_->addAction(st->action(ID_EDIT_COPY));
	contextMenu_->addAction(st->action(ID_EDIT_PASTE));
	contextMenu_->addSeparator();
	contextMenu_->addAction(st->action(ID_EDIT_UNDO));
	contextMenu_->addAction(st->action(ID_EDIT_REDO));
	contextMenu_->addSeparator();
	contextMenu_->addAction(st->action(ID_FIND));
	contextMenu_->addAction(st->action(ID_FIND_NEXT));
	contextMenu_->addAction(st->action(ID_FIND_PREV));
	contextMenu_->addAction(st->action(ID_REPLACE));
	contextMenu_->addSeparator();
	contextMenu_->addAction(st->action(ID_GOTO_LINE));
	
	connect(this, SIGNAL(textChanged()), this, SLOT(updateLineNumbers()));
}

JuffScintilla::~JuffScintilla() {
	delete contextMenu_;
}

bool JuffScintilla::find(const QString& s, const DocFindFlags& flags) {
	QString str(s);
	QString text = this->text();
	QStringList lines = text.split(QRegExp("\r?\n"));
	int row(-1), col(-1);
	this->getCursorPosition(&row, &col);
	int lineIndex(0);
	if (row < 0 || col < 0)
		return false;

	if ( !flags.backwards ) {
		foreach (QString line, lines) {
			if ( lineIndex < row ) {
			}
			else {
				int indent(0);
				if ( lineIndex == row ) {
					line = line.right(line.length() - col);
					indent = col;
				}
				int index(-1);
				QRegExp regExp(str);
				regExp.setCaseSensitivity(flags.matchCase ? Qt::CaseSensitive : Qt::CaseInsensitive);
				if ( flags.isRegExp ) {
					index = line.indexOf(regExp);
				}
				else {
					if ( !flags.matchCase ) {
						str = str.toLower();
						line = line.toLower();
					}
					index = line.indexOf(str);
				}

				if ( index >= 0 ) {
					if ( flags.isRegExp ) {
						this->setSelection(lineIndex, index + indent, lineIndex, index + indent + regExp.matchedLength());
						this->ensureCursorVisible();
					}
					else {
						this->setSelection(lineIndex, index + indent, lineIndex, index + indent + str.length());
						this->ensureCursorVisible();
					}
					return true;
				}
			}
			++lineIndex;
		}
	}
	else {
		QStringList::iterator it = lines.end();
		it--;
		int lineIndex = lines.count() - 1;
		while ( lineIndex >= 0 ) {
			if ( lineIndex > row ) {
			}
			else {
				QString line = *it;
				if ( lineIndex == row ) {
					line = line.left(col);
				}

				int index(-1);
				QRegExp regExp(str);
				regExp.setCaseSensitivity(flags.matchCase ? Qt::CaseSensitive : Qt::CaseInsensitive);
				if ( flags.isRegExp )
					index = line.lastIndexOf(regExp);
				else {
					if ( !flags.matchCase ) {
						str = str.toLower();
						line = line.toLower();
					}
					index = line.lastIndexOf(str);
				}

				if ( index >= 0 ) {
					if ( flags.isRegExp ) {
						this->setSelection(lineIndex, index, lineIndex, index + regExp.matchedLength());
						this->ensureCursorVisible();
					}
					else {
						this->setSelection(lineIndex, index, lineIndex, index + str.length());
						this->ensureCursorVisible();
					}
					return true;
				}
			}
			lineIndex--;
			it--;
		}
	}

	return false;
}

void JuffScintilla::replaceSelected(const QString& targetText, bool backwards) {
	beginUndoAction();
	removeSelectedText();
	int r, c;
	getCursorPosition(&r, &c);
	insert(targetText);
	if ( !backwards ) {
		setCursorPosition(r, c + targetText.length());
	}
	endUndoAction();
}

void JuffScintilla::dragEnterEvent(QDragEnterEvent* e) {
	if ( !e->mimeData()->hasUrls() )
		QsciScintilla::dragEnterEvent(e);
}

void JuffScintilla::dropEvent(QDropEvent* e) {
	if ( !e->mimeData()->hasUrls() )
		QsciScintilla::dropEvent(e);
}

void JuffScintilla::contextMenuEvent(QContextMenuEvent* e) {
	QPoint point = e->pos();
	
	/*
	* The following piece of code has been taken from 
	* QScintilla 2.3.2 source code in order to have this 
	* functionality in lower QScintilla versions 
	*/
	long position = SendScintilla(SCI_POSITIONFROMPOINTCLOSE, point.x(), point.y());

	int line = SendScintilla(SCI_LINEFROMPOSITION, position);
	int linpos = SendScintilla(SCI_POSITIONFROMLINE, line);
	int col = 0;

	// Allow for multi-byte characters.
	while ( linpos < position ) {
		int new_linpos = SendScintilla(SCI_POSITIONAFTER, linpos);

		// If the position hasn't moved then we must be at the end of the text
		// (which implies that the position passed was beyond the end of the
		// text).
		if ( new_linpos == linpos )
			break;

		linpos = new_linpos;
		++col;
	}
	/*
	* End of QScintilla code
	*/

	emit contextMenuCalled(line, col);
	contextMenu_->exec(e->globalPos());
}

void JuffScintilla::focusInEvent(QFocusEvent* e) {
	parentWidget()->setFocusProxy(this);
	QsciScintilla::focusInEvent(e);
}

void JuffScintilla::addContextMenuActions(const ActionList& list) {
	foreach (QAction* a, list) {
		contextMenu_->addAction(a);
	}
}

void JuffScintilla::showLineNumbers(bool show) {
	showLineNumbers_ = show;
	updateLineNumbers();
}

void JuffScintilla::updateLineNumbers() {
	if ( showLineNumbers_ ) {
		QString str = QString("00%1").arg(lines());
		setMarginWidth(1, str);
	}
	else {
		setMarginWidth(1, 0);
	}
}


}	//	namespace Juff
