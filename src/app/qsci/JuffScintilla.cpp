#include <QDebug>

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

#include "JuffScintilla.h"

#include "QSciSettings.h"

#include <QScrollBar>

#include <Qsci/qscicommandset.h>

#include "CommandStorage.h"
#include "Constants.h"

#define WORD_HIGHLIGHT     1
#define SEARCH_HIGHLIGHT   2

JuffScintilla::JuffScintilla() : QsciScintilla() {
	initHighlightingStyle(WORD_HIGHLIGHT, QSciSettings::get(QSciSettings::WordHLColor));
	initHighlightingStyle(SEARCH_HIGHLIGHT, QSciSettings::get(QSciSettings::SearchHLColor));
	
	contextMenu_ = new QMenu();
	CommandStorage* st = CommandStorage::instance();
	contextMenu_->addAction(st->action(EDIT_UNDO));
	contextMenu_->addAction(st->action(EDIT_REDO));
	contextMenu_->addSeparator();
	contextMenu_->addAction(st->action(EDIT_CUT));
	contextMenu_->addAction(st->action(EDIT_COPY));
	contextMenu_->addAction(st->action(EDIT_PASTE));
	contextMenu_->addSeparator();
/*	contextMenu_->addAction(st->action(SEARCH_FIND));
	contextMenu_->addAction(st->action(SEARCH_FIND_NEXT));
	contextMenu_->addAction(st->action(SEARCH_FIND_PREV));
	contextMenu_->addAction(st->action(SEARCH_REPLACE));
	contextMenu_->addSeparator();*/
	contextMenu_->addAction(st->action(SEARCH_GOTO_LINE));
	
	connect(this, SIGNAL(linesChanged()), this, SLOT(updateLineNumbers()));
	
	// list of commands we want to disable initially
	QList<int> cmdsToRemove;
	cmdsToRemove << (Qt::Key_D | Qt::CTRL) << (Qt::Key_L | Qt::CTRL) 
	             << (Qt::Key_T | Qt::CTRL) << (Qt::Key_U | Qt::CTRL) 
	             << (Qt::Key_U | Qt::CTRL | Qt::SHIFT);
	
	QsciCommandSet* set = standardCommands();
	QList< QsciCommand*> cmds = set->commands();
	foreach (QsciCommand* cmd, cmds) {
		if ( cmdsToRemove.contains(cmd->key()) )
			cmd->setKey(0);
		if ( cmdsToRemove.contains(cmd->alternateKey()) )
			cmd->setAlternateKey(0);
	}
}

JuffScintilla::~JuffScintilla() {
	delete contextMenu_;
}

//bool JuffScintilla::find(const Juff::SearchParams&) {
//}

/*bool JuffScintilla::find(const QString& s, const DocFindFlags& flags) {
	if ( flags.multiLine )
		return findML(s, flags);

	QString str(s);
	QString text = this->text();
	QStringList lines = text.split(QRegExp("\r\n|\n|\r"));
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
				QRegExp regExp;
				if ( flags.wholeWords ) {
					regExp = QRegExp(QString("\\b%1\\b").arg(QRegExp::escape(str)));
				}
				else
					regExp = QRegExp(str);
				regExp.setCaseSensitivity(flags.matchCase ? Qt::CaseSensitive : Qt::CaseInsensitive);
				if ( flags.isRegExp || flags.wholeWords ) {
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
				QRegExp regExp;
				if ( flags.wholeWords )
					regExp = QRegExp(QString("\\b%1\\b").arg(QRegExp::escape(str)));
				else
					regExp = QRegExp(str);
				regExp.setCaseSensitivity(flags.matchCase ? Qt::CaseSensitive : Qt::CaseInsensitive);
				if ( flags.isRegExp || flags.wholeWords )
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
}*/

void JuffScintilla::posToLineCol(long pos, int& line, int& col) const {
	line = SendScintilla(SCI_LINEFROMPOSITION, pos);
	long linpos = SendScintilla(SCI_POSITIONFROMLINE, line);
	col = (int)(pos - linpos);
}

long JuffScintilla::lineColToPos(int line, int col) const {
	long linpos = SendScintilla(SCI_POSITIONFROMLINE, line);
	return linpos + col;
}
/*
long JuffScintilla::curPos() const {
	int line, col;
	getCursorPosition(&line, &col);
	return lineColToPos(line, col);
}*/

QString JuffScintilla::wordUnderCursor() {
	int line, col;
	getCursorPosition(&line, &col);
	QString str = text(line);
	int startPos = str.left(col).lastIndexOf(QRegExp("\\b"));
	int endPos = str.indexOf(QRegExp("\\b"), col);
	if ( startPos >= 0 && endPos >= 0 && endPos > startPos )
		return str.mid(startPos, endPos - startPos);
	else
		return "";
}

/*bool JuffScintilla::findML(const QString& s, const DocFindFlags& flags) {
	JUFFENTRY;
	QString text = this->text();
	QRegExp rx(s);
	
	long pos = -1;
	if ( flags.backwards ) {
		long cPos;
		if ( hasSelectedText() ) {
			int line1, col1, line2, col2;
			getSelection(&line1, &col1, &line2, &col2);
			cPos = lineColToPos(line1, col1);
		}
		else {
			cPos = curPos();
		}

		pos = text.left(cPos).lastIndexOf(rx);
	}
	else {
		long cPos;
		if ( hasSelectedText() ) {
			int line1, col1, line2, col2;
			getSelection(&line1, &col1, &line2, &col2);
			cPos = lineColToPos(line2, col2);
		}
		else {
			cPos = curPos();
		}

		pos = text.indexOf(rx, cPos);
	}

	if ( pos >= 0 ) {
		int line1, col1, line2, col2;
		posToLineCol(pos, line1, col1);
		posToLineCol(pos + rx.matchedLength(), line2, col2);
		setSelection(line1, col1, line2, col2);
		ensureCursorVisible();
		return true;
	}
	else {
		return false;
	}
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
}*/

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
	
	int mWidth = marginWidth(0) + marginWidth(1); // width of two margins: markers' and line numbers'
	if ( point.x() <= mWidth ) {
		mWidth += marginWidth(2) + 5; // just in case :)
		long pos = SendScintilla(SCI_POSITIONFROMPOINTCLOSE, point.x() + mWidth, point.y());
		int line = SendScintilla(SCI_LINEFROMPOSITION, pos);
		
		setCursorPosition(line, 0);
		emit markersMenuRequested(mapToGlobal(point));
		return;
	}
	
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
	emit focusReceived();
}

void JuffScintilla::focusOutEvent(QFocusEvent* e) {
	cancelList();
	QsciScintilla::focusOutEvent(e);
}

void JuffScintilla::wheelEvent(QWheelEvent* e) {
	if ( e->modifiers() & Qt::ControlModifier ) {
		if ( e->delta() < 0 ) {
			zoomIn();
		}
		else if ( e->delta() > 0 ) {
			zoomOut();
		}
	}
	else {
		QsciScintilla::wheelEvent(e);
	}
}

void JuffScintilla::cancelRectInput() {
	SendScintilla(SCI_CANCEL);
}

void JuffScintilla::keyPressEvent(QKeyEvent* e) {
	if ( SendScintilla(SCI_SELECTIONISRECTANGLE) ) {
		int line, col;
		getCursorPosition(&line, &col);

		int line1, col1, line2, col2;
		getSelection(&line1, &col1, &line2, &col2);
		
		int rLine1 = qMin(line1, line2);
		int rCol1 = qMin(col1, col2);
		int rLine2 = qMax(line1, line2);
		int rCol2 = qMax(col1, col2);

		switch ( e->key() ) {
			case Qt::Key_Escape :
				setSelection(line, col, line, col);
				cancelRectInput();
				break;

			case Qt::Key_Left:
			case Qt::Key_Right:
			case Qt::Key_Up:
			case Qt::Key_Down:
				if ( !(e->modifiers() & Qt::AltModifier) ) {
					setSelection(line, col, line, col);
					cancelRectInput();
				}
				else {
					QsciScintilla::keyPressEvent(e);
				}
				break;

			case Qt::Key_Backspace:
				if ( rCol1 == rCol2 ) {
					beginUndoAction();
					deleteRectSelection(rLine1, rCol1 - 1, rLine2, rCol1);
					endUndoAction();
					setSelection(rLine1, rCol1 - 1, rLine2, rCol1 - 1);
					SendScintilla(SCI_SETSELECTIONMODE, 1);
				}
				else {
					beginUndoAction();
					deleteRectSelection(rLine1, rCol1, rLine2, rCol2);
					endUndoAction();
//					cancelRectInput();
					
					
//					setSelection(rLine1, rCol1, rLine2, rCol1);
//					SendScintilla(SCI_SETSELECTIONMODE, 1);
				}
				break;
				
			case Qt::Key_Delete :
				if ( rCol1 == rCol2 ) {
					beginUndoAction();
					deleteRectSelection(rLine1, rCol1, rLine2, rCol1 + 1);
					endUndoAction();
					setSelection(rLine1, rCol1, rLine2, rCol1);
					SendScintilla(SCI_SETSELECTIONMODE, 1);
				}
				else {
					beginUndoAction();
					deleteRectSelection(rLine1, rCol1, rLine2, rCol2);
					endUndoAction();
					
//					cancelRectInput();
//					setSelection(rLine1, rCol1, rLine1, rCol1);
//					SendScintilla(SCI_SETSELECTIONMODE, 0);
//					setSelection(rLine1, rCol1, rLine2, rCol1);
//					SendScintilla(SCI_SETSELECTIONMODE, 1);
				}
//				SendScintilla(SCI_SETSELECTIONMODE, 1);
//				cancelRectInput();
				break;

			default:
				QString t = e->text();
				if ( t.length() < 0 ) {
					break;
				}
				
				beginUndoAction();
				if ( rCol1 != rCol2 ) {
					deleteRectSelection(rLine1, rCol1, rLine2, rCol2);
//					for ( int line = rLine1; line <= rLine2; ++line ) {
//						setSelection(line, rCol1, line, rCol2);
//						removeSelectedText();
//					}
				}
				for ( int i = line2; i >= line1; --i ) {
					insertAt(t, i, rCol1 );
				}
				if ( e->key() != Qt::Key_Enter && e->key() != Qt::Key_Return ) {
					setSelection(rLine1, rCol1 + t.length(), rLine2, rCol1 + t.length());
					SendScintilla(SCI_SETSELECTIONMODE, 1);
				}
				endUndoAction();
		}
	}
	else {
		// not rectangular selection
		bool eclipseStyle = QSciSettings::get(QSciSettings::JumpOverWordParts);
		
		int key = e->key();
		if ( e->modifiers() & Qt::ControlModifier ) {
			// control hotkeys
			switch ( key ) {
				case Qt::Key_Left :
					if ( eclipseStyle ) {
						if ( e->modifiers() & Qt::ShiftModifier )
							SendScintilla(SCI_WORDPARTLEFTEXTEND);
						else
							SendScintilla(SCI_WORDPARTLEFT);
					}
					else {
						QsciScintilla::keyPressEvent(e);
					}
					break;
				
				case Qt::Key_Right :
					if ( eclipseStyle ) {
						if ( e->modifiers() & Qt::ShiftModifier )
							SendScintilla(SCI_WORDPARTRIGHTEXTEND);
						else
							SendScintilla(SCI_WORDPARTRIGHT);
					}
					else {
						QsciScintilla::keyPressEvent(e);
					}
					break;
				
				case Qt::Key_Backspace :
					if ( eclipseStyle ) {
						beginUndoAction();
						SendScintilla(SCI_WORDPARTLEFTEXTEND);
						removeSelectedText();
						endUndoAction();
					}
					else {
						QsciScintilla::keyPressEvent(e);
					}
					break;
				
				case Qt::Key_Delete :
					if ( eclipseStyle ) {
						beginUndoAction();
						SendScintilla(SCI_WORDPARTRIGHTEXTEND);
						removeSelectedText();
						endUndoAction();
					}
					else {
						QsciScintilla::keyPressEvent(e);
					}
					break;
				
					case Qt::Key_Space :
					{
						// Dirty hack but looks like it works :)
						// To display auto-completion box we "imitate"
						// entering the previous char. We do not enter it,
						// we just emit the notifying signal that triggers
						// displaying the auto-completion box.
						int pos = SendScintilla(SCI_GETCURRENTPOS);
						if ( pos > 0 ) {
							int ch = SendScintilla(SCI_GETCHARAT, pos - 1);
							emit SCN_CHARADDED(ch);
						}
					}
						break;
					
				default:
					if ( e->modifiers() & Qt::AltModifier ) {
						return;
					}
					else {
						QsciScintilla::keyPressEvent(e);
					}
			}
		}
		else {
			// hotkeys without Control
			switch ( key ) {
				case Qt::Key_Enter :
				case Qt::Key_Return :
					beginUndoAction();
					QsciScintilla::keyPressEvent(e);
					endUndoAction();
					break;
				
				case Qt::Key_Escape :
					emit escapePressed();
					QsciScintilla::keyPressEvent(e);
					break;
				
				default:
					QsciScintilla::keyPressEvent(e);
			}
		}
	}
}

void JuffScintilla::deleteRectSelection(int line1, int col1, int line2, int col2) {
	for ( int line = line1; line <= line2; ++line ) {
		setSelection(line, col1, line, col2);
		removeSelectedText();
	}
}

/*void JuffScintilla::addContextMenuActions(const ActionList& list) {
	foreach (QAction* a, list) {
		contextMenu_->addAction(a);
	}
}
*/
void JuffScintilla::showLineNumbers(bool show) {
	showLineNumbers_ = show;
	updateLineNumbers();
}

bool JuffScintilla::lineNumbersVisible() const {
	return showLineNumbers_;
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

void JuffScintilla::clearHighlighting() {
	SendScintilla(SCI_SETINDICATORCURRENT, WORD_HIGHLIGHT);
	SendScintilla(SCI_INDICATORCLEARRANGE, 0, length());
	SendScintilla(SCI_SETINDICATORCURRENT, SEARCH_HIGHLIGHT);
	SendScintilla(SCI_INDICATORCLEARRANGE, 0, length());
}

void JuffScintilla::highlight(HLMode, int row1, int col1, int row2, int col2) {
	int pos1 = lineColToPos(row1, col1);
	int pos2 = lineColToPos(row2, col2);
	highlight(pos1, pos2, SEARCH_HIGHLIGHT);
}

// TODO : refactor this method
void JuffScintilla::highlightText(HLMode mode, const Juff::SearchParams& params) {
	clearHighlighting();
	QString text = params.findWhat;
	
	if ( text.length() < 1 )
		return;
	
	int initialLine, initialCol;
	getCursorPosition(&initialLine, &initialCol);
	int scrollPos = verticalScrollBar()->value();
	
	if ( mode == HLCurrentWord ) {
		int line = 0, col = 0;
		while ( findFirst(text, false, false, true, false, true, line, col) ) {
			int start = SendScintilla(SCI_GETSELECTIONSTART);
			int end = SendScintilla(SCI_GETSELECTIONEND);
			highlight(start, end, WORD_HIGHLIGHT);
			posToLineCol(end, line, col);
		}
	}
	
	setCursorPosition(initialLine, initialCol);
	verticalScrollBar()->setValue(scrollPos);
}

void JuffScintilla::highlight(int start, int end, int ind) {
	SendScintilla(SCI_SETINDICATORCURRENT, ind);
	SendScintilla(SCI_INDICATORFILLRANGE, start, end - start);
}

void JuffScintilla::initHighlightingStyle(int id, const QColor &color) {
	SendScintilla(SCI_INDICSETSTYLE, id, INDIC_ROUNDBOX);
	SendScintilla(SCI_INDICSETUNDER, id, true);
	SendScintilla(SCI_INDICSETFORE, id, color);
	SendScintilla(SCI_INDICSETALPHA, id, 50);
}
