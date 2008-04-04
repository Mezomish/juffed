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

#include "TextProcessor.h"

//	Qt headers
#include <QtGui/QTextCursor>
#include <QtGui/QTextBlock>
#include <QtGui/QTextEdit>

TextProcessor::TextProcessor(QTextEdit* ed) : QObject(), edit_(ed) {
	autoInsertion_ = false;
	lastRow_ = -1;
	preLastRow_ = -1;
	connect(edit_, SIGNAL(textChanged()), SLOT(textChangeEvent()));
	connect(edit_, SIGNAL(cursorPositionChanged()), SLOT(cursorMoveEvent()));
}

void TextProcessor::textChangeEvent() {
	if (autoInsertion_)
		return;

	QTextCursor c = edit_->textCursor();
	if (c.atBlockStart()) {
		QTextBlock bl = c.block();
		QTextBlock prevBl = bl.previous();
		if (bl.isValid() && prevBl.isValid()) {

			//	ensure that cursor was moved from the previous row
			if (lastRow_ - 1 != preLastRow_)
				return;

			QString text = bl.text();
			QString prevText = prevBl.text();
			if (/*text.isEmpty() &&*/ !prevText.isEmpty()) {
				int lineBeginIndex = prevText.indexOf(QRegExp("[^ \t]"));
				QString lineBegin = prevText.left(lineBeginIndex);

				autoInsertion_ = true;
				while (bl.text().startsWith('\t') || bl.text().startsWith(' ')) {
					c.deleteChar();
				}
				c.insertText(lineBegin);
				autoInsertion_ = false;
			}
		}
	}
}

void TextProcessor::cursorMoveEvent() {
	preLastRow_ = lastRow_;
	lastRow_ = edit_->textCursor().blockNumber();
}
