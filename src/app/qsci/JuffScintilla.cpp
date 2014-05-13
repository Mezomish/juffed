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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "JuffScintilla.h"

#include "QSciSettings.h"

#include <QApplication>
#include <QClipboard>
#include <QScrollBar>
#include <QMimeData>

#include <Qsci/qscicommandset.h>

#include "Constants.h"
#include "Log.h"
#include "Utils.h"

#define WORD_HIGHLIGHT     1
#define SEARCH_HIGHLIGHT   2

JuffScintilla::JuffScintilla() : QsciScintilla() {
	initHighlightingStyle(WORD_HIGHLIGHT, QSciSettings::get(QSciSettings::WordHLColor));
	initHighlightingStyle(SEARCH_HIGHLIGHT, QSciSettings::get(QSciSettings::SearchHLColor));
	
	contextMenu_ = new QMenu();
	CommandStorageInt* st = Juff::Utils::commandStorage();
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
*/

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
	
	int line, col;
	long position = SendScintilla(SCI_POSITIONFROMPOINTCLOSE, point.x(), point.y());
	lineIndexFromPosition(position, &line, &col);
	
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
			zoomOut();
		}
		else if ( e->delta() > 0 ) {
			zoomIn();
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
		getOrderedSelection(line1, col1, line2, col2);
		
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
				if ( col1 == col2 ) {
					beginUndoAction();
					deleteRectSelection(line1, col1 - 1, line2, col1);
					endUndoAction();
					setSelection(line1, col1 - 1, line2, col1 - 1);
					SendScintilla(SCI_SETSELECTIONMODE, 1);
				}
				else {
					beginUndoAction();
					deleteRectSelection(line1, col1, line2, col2);
					endUndoAction();
//					cancelRectInput();
					
					
//					setSelection(rLine1, rCol1, rLine2, rCol1);
//					SendScintilla(SCI_SETSELECTIONMODE, 1);
				}
				break;
				
			case Qt::Key_Delete :
				if ( col1 == col2 ) {
					beginUndoAction();
					deleteRectSelection(line1, col1, line2, col1 + 1);
					endUndoAction();
					setSelection(line1, col1, line2, col1);
					SendScintilla(SCI_SETSELECTIONMODE, 1);
				}
				else {
					beginUndoAction();
					deleteRectSelection(line1, col1, line2, col2);
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
				if ( e->modifiers() & Qt::ControlModifier ) {
					break;
				}
				
				QString t = e->text();
				if ( t.length() < 0 ) {
					break;
				}
				
				beginUndoAction();
				if ( col1 != col2 && t.length() > 0 ) {
					deleteRectSelection(line1, col1, line2, col2);
				}
				for ( int i = line2; i >= line1; --i ) {
					insertAt(t, i, col1);
				}
				if ( e->key() != Qt::Key_Enter && e->key() != Qt::Key_Return ) {
					setSelection(line1, col1 + t.length(), line2, col1 + t.length());
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
					clearHighlighting();
					emit escapePressed();
					QsciScintilla::keyPressEvent(e);
					break;
				
				default:
					QsciScintilla::keyPressEvent(e);
			}
		}
	}
}

void JuffScintilla::cut() {
	if ( SendScintilla(SCI_SELECTIONISRECTANGLE) ) {
		beginUndoAction();
		copy();
		deleteRectSelection();
		endUndoAction();
	}
	else {
		QsciScintilla::cut();
	}
}

void JuffScintilla::paste() {
	QString originalText = QApplication::clipboard()->text();
	QString convertedText;
	if ( originalText.contains(LineSeparatorRx) ) {
		QStringList lines = originalText.split(LineSeparatorRx);
		switch ( eolMode() ) {
			case EolWindows : convertedText = lines.join("\r\n"); break;
			case EolUnix    : convertedText = lines.join("\n"); break;
			case EolMac     : convertedText = lines.join("\r"); break;
		}
		QApplication::clipboard()->setText(convertedText);
	}
	
	if ( SendScintilla(SCI_SELECTIONISRECTANGLE) ) {
		QString text = QApplication::clipboard()->text();
		int line1, col1, line2, col2;
		getOrderedSelection(line1, col1, line2, col2);
		
		beginUndoAction();
		deleteRectSelection(line1, col1, line2, col2);
		for ( int line = line2; line >= line1; --line ) {
			insertAt(text, line, col1);
		}
		endUndoAction();
	}
	else {
		QsciScintilla::paste();
	}
	
	// restore the original clipboard content
	QApplication::clipboard()->setText(originalText);
}

void JuffScintilla::getOrderedSelection(int& rLine1, int& rCol1, int& rLine2, int& rCol2) {
	int line1, col1, line2, col2;
	getSelection(&line1, &col1, &line2, &col2);
	
	rLine1 = qMin(line1, line2);
	rCol1 = qMin(col1, col2);
	rLine2 = qMax(line1, line2);
	rCol2 = qMax(col1, col2);
}

void JuffScintilla::deleteRectSelection() {
	int line1, col1, line2, col2;
	getOrderedSelection(line1, col1, line2, col2);
	deleteRectSelection(line1, col1, line2, col2);
}

void JuffScintilla::deleteRectSelection(int line1, int col1, int line2, int col2) {
	for ( int line = line1; line <= line2; ++line ) {
		QString lineStr = text(line).section(LineSeparatorRx, 0, 0);
		int length = lineStr.length();
		if ( col1 >= length ) {
			continue;
		}
		setSelection(line, col1, line, qMin(col2, length));
		removeSelectedText();
	}
}

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

/*
 * It changes document's current selection (if any), so you'd better keep it before 
 * calling this function and restore it later.
 */
void JuffScintilla::highlight(HLMode, int row1, int col1, int row2, int col2) {
	int pos1 = positionFromLineIndex(row1, col1);
	int pos2 = positionFromLineIndex(row2, col2);
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
			lineIndexFromPosition(end, &line, &col);
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
