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

#include "TextDocView.h"

//	Qt headers
#include <QtGui/QMenu>
#include <QtGui/QMessageBox>
#include <QtGui/QPainter>
#include <QtGui/QResizeEvent>
#include <QtGui/QScrollBar>
#include <QtGui/QTextBlock>
#include <QtGui/QTextEdit>

//	local headers
#include "HL.h"
#include "Log.h"
#include "TextDoc.h"
#include "TextDocSettings.h"
#include "TextProcessor.h"
#include "types.h"

#include <math.h>

class LineNumWidget : public QWidget {
public:
	LineNumWidget(QWidget* parent) : QWidget(parent), ed_(0), doc_(0), view_(0) {
		indent_ = 15;
		visible_ = true;
	}
	virtual ~LineNumWidget() {
	}

	void applySettings() {
		setFont(TextDocSettings::font());
		repaint();
	}

	void update() {
		int rowCount = ed_->document()->blockCount();
		int digits = int(log10(float(rowCount))) + 1;

		int w = 0;
		if (visible_)
			w = QFontMetrics(font()).width("0") * digits + indent_;
		resize(w, height());

		repaint();
	}
	
	virtual void paintEvent(QPaintEvent*) {
		if (doc_ == 0 || ed_ == 0 || view_ == 0)
			return;

		QPainter p(this);
		p.setFont(font());
		p.setPen(Qt::black);
		p.setBrush(QColor(250, 250, 250));
		p.drawRect(rect());
		QTextBlock bl = ed_->document()->begin();
		int w = width() - indent_;
		int h = height();
		int ind_2 = indent_ / 2;

		IntList markers = view_->markers();
	
		//	draw line numbers
		int blockNum = 1;
		p.setBrush(QColor("#9999cc"));
		while (bl.isValid()) {
			QTextCursor cur(bl);
			QRect rc(ed_->cursorRect(cur));
			int y = rc.y();
			int rc_h = rc.height();
			if (y > h)
				break;

			if (y >= -rc_h) {
				//	draw marker
				if (markers.contains(blockNum)) {
					p.setPen(QPen("#888899"));
					p.drawRect(2, y, width() - 4, rc_h);
				}

				p.setPen(Qt::black);
				p.drawText(ind_2, y, w, rc_h, Qt::AlignRight | Qt::AlignBottom, QString::number(blockNum));
			}
			bl = bl.next();
			++blockNum;
		}
	}
	
	QTextEdit* ed_;
	TextDoc* doc_;
	TextDocView* view_;
	int indent_;
	bool visible_;
};

QTextDocument::FindFlags textDocFlags(DocFindFlags f) {
	QTextDocument::FindFlags flags(0);
	if (f.matchCase)
		flags |= QTextDocument::FindCaseSensitively;
	if (f.backward)
		flags |= QTextDocument::FindBackward;
	return flags;
}

class TextEdit : public QTextEdit {
public:
	TextEdit(QWidget* parent) : QTextEdit(parent) {
		setAcceptRichText(false);
		contextMenu_ = new QMenu();
		contextMenu_->addAction(CommandStorage::instance()->action(ID_EDIT_CUT));
		contextMenu_->addAction(CommandStorage::instance()->action(ID_EDIT_COPY));
		contextMenu_->addAction(CommandStorage::instance()->action(ID_EDIT_PASTE));
		contextMenu_->addSeparator();
		contextMenu_->addAction(CommandStorage::instance()->action(ID_EDIT_UNDO));
		contextMenu_->addAction(CommandStorage::instance()->action(ID_EDIT_REDO));
		contextMenu_->addSeparator();
		contextMenu_->addAction(CommandStorage::instance()->action(ID_FIND));
		contextMenu_->addAction(CommandStorage::instance()->action(ID_FIND_NEXT));
		contextMenu_->addAction(CommandStorage::instance()->action(ID_FIND_PREV));
		contextMenu_->addSeparator();
		contextMenu_->addAction(CommandStorage::instance()->action(ID_GOTO_LINE));
	}
	virtual ~TextEdit() {
		delete contextMenu_;
	}

	QTextCursor getCursor(const QString& text, DocFindFlags flags) {
		QTextCursor cursor = textCursor();
		if (cursor.selectedText().compare(text) == 0 && flags.backward)
			cursor.setPosition(cursor.selectionStart());

		QTextCursor cur = document()->find(text, cursor.position(), textDocFlags(flags));
		return cur;
	}
	QTextCursor getCursor(const QRegExp& regexp, DocFindFlags flags) {
		QTextCursor cursor = textCursor();
		if (regexp.exactMatch(cursor.selectedText()) && flags.backward)
			cursor.setPosition(cursor.selectionStart());

		QTextCursor cur = document()->find(regexp, cursor.position(), textDocFlags(flags));
		return cur;
	}

protected:
	virtual void dragEnterEvent(QDragEnterEvent* e) {
		if (!e->mimeData()->hasUrls())
			QTextEdit::dragEnterEvent(e);
	}

	virtual void dropEvent(QDropEvent* e) {
		if (!e->mimeData()->hasUrls())
			QTextEdit::dropEvent(e);
	}
	virtual void contextMenuEvent(QContextMenuEvent* e) {
		contextMenu_->exec(e->globalPos());
	}

	virtual void paintEvent(QPaintEvent* e) {
		QTextEdit::paintEvent(e);

		int x = TextDocSettings::lineLengthIndicator() * QFontMetrics(font()).width("0");
		if (x > 0) {
			x -= horizontalScrollBar()->value();
			QPainter p(this->viewport());
			p.setPen(QPen(Qt::gray, 1, Qt::DotLine));
			p.drawLine(x + 1, 0, x + 1, height());
		}
	}

private:
	QMenu* contextMenu_;
};

class TDViewInterior {
public:
	TDViewInterior(QWidget* parent) {
		ed_ = new TextEdit(parent);
		parent->setFocusProxy(ed_);
		parent->connect(ed_, SIGNAL(cursorPositionChanged()), parent, SLOT(cursorPositionChangeEvent()));
		
		lineNumWidget_ = new LineNumWidget(parent);
		
		lineNumVisible_ = true;
		adjustedByWidth_ = false;
		
		hl_ = 0;
		proc_ = new TextProcessor(ed_);
	}
	~TDViewInterior() {
		if (hl_ != 0)
			delete hl_;
		delete proc_;
		delete lineNumWidget_;
		delete ed_;
	}
	
	TextEdit* ed_;
	LineNumWidget* lineNumWidget_;
	HL* hl_;
	TextProcessor* proc_;
	
	bool lineNumVisible_;
	bool adjustedByWidth_;
	IntList markers_;
};



TextDocView::TextDocView(QWidget* parent) : DocView(parent) {
	vInt_ = new TDViewInterior(this);

	connect(vInt_->ed_, SIGNAL(textChanged()), SLOT(updateLayout()));
	QScrollBar* scr = vInt_->ed_->verticalScrollBar();
	connect(scr, SIGNAL(valueChanged(int)), this, SLOT(scrolled(int)));
}

TextDocView::~TextDocView() {
	delete vInt_;
}

void TextDocView::setDocument(Document* doc) {
	DocView::setDocument(doc);
	
	TextDoc* tDoc = qobject_cast<TextDoc*>(doc);
	if (tDoc == 0) {
		Log::debug("Wrong document");
		Log::print("Wrong document");
		return;
	}
	
	vInt_->lineNumWidget_->doc_ = tDoc;
	vInt_->lineNumWidget_->ed_= vInt_->ed_;
	vInt_->lineNumWidget_->view_ = this;
	
	if (vInt_->hl_ != 0) {
		delete vInt_->hl_;
		vInt_->hl_ = 0;
	}
	vInt_->hl_ = new HL(vInt_->ed_->document());
	
	connect(vInt_->ed_->document(), SIGNAL(modificationChanged(bool)), doc, SLOT(setModified(bool)));
	connect(vInt_->ed_->document(), SIGNAL(modificationChanged(bool)), this, SIGNAL(modified(bool)));
	connect(doc, SIGNAL(fileNameChanged()), SLOT(rehighlight()));

	rehighlight();
}

void TextDocView::rehighlight() {
	if (vInt_->hl_ != 0)
		vInt_->hl_->changeFileName(document()->fileName());
}

void TextDocView::setModified(bool mod) {
	vInt_->ed_->document()->setModified(mod);
}

void TextDocView::updateLayout() {
	int h = height();
	int w = width();

//	TextDoc* doc = qobject_cast<TextDoc*>(document());
	vInt_->lineNumWidget_->update();

	int w1 = vInt_->lineNumWidget_->width();
	
	vInt_->lineNumWidget_->setGeometry(0, 0, w1, h);
	vInt_->ed_->setGeometry(w1, 0, w - w1, h);
}

void TextDocView::scrolled(int) {
	updateLayout();
}
	
void TextDocView::resizeEvent(QResizeEvent*) {
	updateLayout();
}

void TextDocView::setText(const QString& text) {
	if (vInt_->ed_->document() == 0) {
	}
	else {
		int scrollPos = vInt_->ed_->verticalScrollBar()->value();
		QTextCursor c = vInt_->ed_->textCursor();
		int pos = c.position();
		
		vInt_->ed_->document()->setPlainText(text);
		updateLayout();
		
		vInt_->ed_->verticalScrollBar()->setValue(scrollPos);
		c = vInt_->ed_->textCursor();
		c.setPosition(pos);
		vInt_->ed_->setTextCursor(c);
	}
}

void TextDocView::getText(QString& text) const {
	text = vInt_->ed_->document()->toPlainText();
}



bool TextDocView::lineNumIsVisible() const {
	return vInt_->lineNumWidget_->visible_;
}

void TextDocView::setLineNumVisible(bool visible) {
	vInt_->lineNumWidget_->visible_ = visible;
	updateLayout();
}

bool TextDocView::isAdjustedByWidth() const {
	return vInt_->adjustedByWidth_;
}

void TextDocView::setAdjustedByWidth(bool adjust) {
	vInt_->adjustedByWidth_ = adjust;
	if (adjust)
		vInt_->ed_->setLineWrapMode(QTextEdit::WidgetWidth);
	else
		vInt_->ed_->setLineWrapMode(QTextEdit::NoWrap);
		
	updateLayout();
}

void TextDocView::cursorPositionChangeEvent() {
	int row(-1), col(-1);
	getCursorPos(row, col);
	emit cursorPositionChanged(row, col);
}

void TextDocView::getCursorPos(int& row, int& col) const {
	QTextCursor c = vInt_->ed_->textCursor();
	row = c.blockNumber();
	col = c.position() - c.block().position();
}

void TextDocView::gotoLine(int line) const {
	QTextEdit* edit = vInt_->ed_;
	QTextBlock block = edit->document()->begin();
	QTextCursor cur = edit->textCursor();
	cur.setPosition(block.position());
	while (block.isValid() && cur.blockNumber() + 1 < line) {
		cur.setPosition(block.position());
		block = block.next();
	}
	edit->setTextCursor(cur);
}

int TextDocView::lineCount() const {
	return vInt_->ed_->document()->blockCount();
}

void TextDocView::applySettings() {
	QFont font = TextDocSettings::font();
	vInt_->ed_->setFont(font);

	int tabIndent = TextDocSettings::tabStopWidth() * QFontMetrics(font).width(" ");
	vInt_->ed_->setTabStopWidth(tabIndent);
	vInt_->ed_->setPlainText(vInt_->ed_->toPlainText());

	vInt_->lineNumWidget_->applySettings();
}



////////////////////////////////////////////////////////////
//	EDIT
void TextDocView::cut() {
	vInt_->ed_->cut();
}

void TextDocView::copy() {
	vInt_->ed_->copy();
}

void TextDocView::paste() {
	vInt_->ed_->paste();
}

void TextDocView::undo() {
	vInt_->ed_->undo();
}

void TextDocView::redo() {
	vInt_->ed_->redo();
}
//	EDIT
////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
//	FIND
void TextDocView::find(const QString& str, DocFindFlags flags) {
	TextEdit* edit = vInt_->ed_;
	
	if (flags.backward) {
		//	move cursor to the beginning of the selection
		QTextCursor c = edit->textCursor();
		c.setPosition(c.selectionStart());
		edit->setTextCursor(c);
	}

	QTextCursor cur = edit->getCursor(str, flags);
	if (cur.isNull()) {
		//	not found
		QString msg;
		QTextCursor::MoveOperation moveOp;
		if (flags.backward) {
			msg = tr("The search has reached the beginning of file.\nContinue from the end?");
			moveOp = QTextCursor::End;
		}
		else {
			msg = tr("The search has reached the end of file.\nContinue from the beginning?");
			moveOp = QTextCursor::Start;
		}
		QMessageBox::StandardButton choice = QMessageBox::question(this, tr("Find"),
						msg, QMessageBox::Ok | QMessageBox::Cancel);
		if (choice == QMessageBox::Ok) {
			QTextCursor c = vInt_->ed_->textCursor();
			c.movePosition(moveOp);
			vInt_->ed_->setTextCursor(c);
			find(str, flags);
		}
	}
	else {
		//	found
		edit->setTextCursor(cur);
	}
}

void TextDocView::find(const QRegExp& regexp, DocFindFlags flags) {
	TextEdit* edit = vInt_->ed_;

	if (flags.backward) {
		//	move cursor to the beginning of the selection
		QTextCursor c = edit->textCursor();
		c.setPosition(c.selectionStart());
		edit->setTextCursor(c);
	}

	QTextCursor cur = edit->getCursor(regexp, flags);
	if (cur.isNull()) {
		//	not found
		QString msg;
		QTextCursor::MoveOperation moveOp;
		if (flags.backward) {
			msg = tr("The search has reached the beginning of file.\nContinue from the end?");
			moveOp = QTextCursor::End;
		}
		else {
			msg = tr("The search has reached the end of file.\nContinue from the beginning?");
			moveOp = QTextCursor::Start;
		}
		QMessageBox::StandardButton choice = QMessageBox::question(this, tr("Find"),
						msg, QMessageBox::Ok | QMessageBox::Cancel);
		if (choice == QMessageBox::Ok) {
			QTextCursor c = vInt_->ed_->textCursor();
			c.movePosition(moveOp);
			vInt_->ed_->setTextCursor(c);
			find(regexp, flags);
		}
	}
	else {
		//	found
		edit->setTextCursor(cur);
	}
}

enum Answer { Yes, No, All, Cancel };

Answer confirm(QWidget* w) {
	Answer answer = No;
	QMessageBox::StandardButton btn = QMessageBox::question(w, QObject::tr("Confirmation"), 
					QObject::tr("Replace this text?"), 
					QMessageBox::Yes | QMessageBox::No | QMessageBox::YesToAll | QMessageBox::Cancel, 
					QMessageBox::Yes);
		switch (btn) {
		case QMessageBox::Yes:
			answer = Yes;
				break;
		case QMessageBox::No:
			answer = No;
			break;
		case QMessageBox::Cancel:
			answer = Cancel;
			break;
		case QMessageBox::YesToAll:
			answer = All;
			break;
		default: ;
	}

	return answer;
}

bool TextDocView::doReplace(QTextCursor& cur, const QString& text, bool& replaceAll) {
	if (!replaceAll) {
		//	ask confirmation if replace all hasn't been chosen yet
		Answer conf = confirm(this);
		if (conf == Cancel)
			return false;
		else if (conf == Yes)
			cur.insertText(text);
		else if (conf == All) {
			replaceAll = true;
			cur.insertText(text);
		}
	}
	else {
		//	just replace, because there has been chosen "select all"
		cur.insertText(text);
	}
	return true;
}

void TextDocView::replace(const QString& from, const QString& to, DocFindFlags flags) {
	QTextCursor cur = vInt_->ed_->getCursor(from, flags);

	bool replaceAll = false;
	while (cur.position() >= 0) {
		vInt_->ed_->setTextCursor(cur);

		if (!doReplace(cur, to, replaceAll))
			break;

		cur = vInt_->ed_->getCursor(from, flags);
	}


	if (cur.position() < 0) {
		//	not found
		QString msg;
		QTextCursor::MoveOperation moveOp;
		if (flags.backward) {
			msg = tr("The search has reached the beginning of file.\nContinue from the end?");
			moveOp = QTextCursor::End;
		}
		else {
			msg = tr("The search has reached the end of file.\nContinue from the beginning?");
			moveOp = QTextCursor::Start;
		}
		QMessageBox::StandardButton choice = QMessageBox::question(this, tr("Find"),
						msg, QMessageBox::Ok | QMessageBox::Cancel);
		if (choice == QMessageBox::Ok) {
			QTextCursor c = vInt_->ed_->textCursor();
			c.movePosition(moveOp);
			vInt_->ed_->setTextCursor(c);
			replace(from, to, flags);
		}
	}
}

void TextDocView::replace(const QRegExp& regexp, const QString& to, DocFindFlags flags) {
	QTextCursor cur = vInt_->ed_->getCursor(regexp, flags);

	bool replaceAll = false;
	while (cur.position() >= 0) {
		vInt_->ed_->setTextCursor(cur);

		if (!doReplace(cur, to, replaceAll))
			break;

		cur = vInt_->ed_->getCursor(regexp, flags);
	}


	if (cur.position() < 0) {
		//	not found
		QString msg;
		QTextCursor::MoveOperation moveOp;
		if (flags.backward) {
			msg = tr("The search has reached the beginning of file.\nContinue from the end?");
			moveOp = QTextCursor::End;
		}
		else {
			msg = tr("The search has reached the end of file.\nContinue from the beginning?");
			moveOp = QTextCursor::Start;
		}
		QMessageBox::StandardButton choice = QMessageBox::question(this, tr("Find"),
						msg, QMessageBox::Ok | QMessageBox::Cancel);
		if (choice == QMessageBox::Ok) {
			QTextCursor c = vInt_->ed_->textCursor();
			c.movePosition(moveOp);
			vInt_->ed_->setTextCursor(c);
			replace(regexp, to, flags);
		}
	}
}
//	FIND & REPLACE
////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
//	MARKERS
void TextDocView::toggleMarker() {
	QTextCursor c = vInt_->ed_->textCursor();
	int line = c.blockNumber() + 1;
	if (vInt_->markers_.contains(line)) {
		vInt_->markers_.removeAll(line);
	}
	else {
		vInt_->markers_.push_back(line);
		qSort(vInt_->markers_.begin(), vInt_->markers_.end());
	}
	updateLayout();
}

void TextDocView::gotoNextMarker() {
	if (vInt_->markers_.count() == 0)
		return;

	QTextCursor c = vInt_->ed_->textCursor();
	int line = c.blockNumber() + 1;
	foreach (int marker, vInt_->markers_) {
		if (marker > line) {
			//	As soon as markers are sorted,
			//	the condition "marker" > "line" will be 
			//	true for the first marker after the 
			//	current line
			gotoLine(marker);
			return;
		}
	}
	
	//	next marker is not found, go to the first one
	gotoLine( vInt_->markers_.first() );
}

void TextDocView::gotoPrevMarker() {
	if (vInt_->markers_.count() == 0)
		return;

	QTextCursor c = vInt_->ed_->textCursor();
	int line = c.blockNumber() + 1;
	int targetLine = -1;
	foreach (int marker, vInt_->markers_) {
		if (marker < line) {
			targetLine = marker;
		}
		else {
			//	As soon as markers are sorted,
			//	if "marker" becomes > than "line"
			//	then the previous marker was the closest
			//	one that precedes current line

			//	If there was no preceding markers,
			//	we should go to the last one

			if (targetLine >= 0) {
				gotoLine(targetLine);
			}
			else {
				gotoLine( vInt_->markers_.last() );
			}
			return;
		}
	}
}

void TextDocView::removeAllMarkers() {
	vInt_->markers_.clear();
	updateLayout();
}

IntList TextDocView::markers() const {
	return vInt_->markers_;
}

QString TextDocView::markedLine(int line) const {
	QTextCursor c = vInt_->ed_->textCursor();
	c.movePosition(QTextCursor::Start);
	c.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, line - 1);
	return c.block().text();
}
//	MARKERS
////////////////////////////////////////////////////////////
