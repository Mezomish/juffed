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

#include <Qsci/qscilexer.h>
#include <Qsci/qsciscintilla.h>

//	local headers
#include "LexerStorage.h"
#include "Log.h"
#include "TextDoc.h"
#include "TextDocSettings.h"
#include "types.h"

#include <math.h>

class MyQScintilla : public QsciScintilla {
public:
	MyQScintilla(QWidget* parent) : QsciScintilla(parent) {
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
	virtual ~MyQScintilla() {
		delete contextMenu_;
	}
	
protected:
	virtual void dragEnterEvent(QDragEnterEvent* e) {
		if (!e->mimeData()->hasUrls())
			QsciScintilla::dragEnterEvent(e);
	}

	virtual void dropEvent(QDropEvent* e) {
		if (!e->mimeData()->hasUrls())
			QsciScintilla::dropEvent(e);
	}

	virtual void contextMenuEvent(QContextMenuEvent* e) {
		contextMenu_->exec(e->globalPos());
	}

private:
	QMenu* contextMenu_;
};

class TDViewInterior {
public:
	TDViewInterior(QWidget* parent) {
		lineNumVisible_ = true;
		adjustedByWidth_ = false;
		syntax_ = "none";
		
		edit_ = new MyQScintilla(parent);
		edit_->setUtf8(true);
		edit_->setCaretLineBackgroundColor(TextDocSettings::curLineColor());
		edit_->setIndentationGuidesForegroundColor(QColor(200, 200, 200));
		edit_->setFolding(QsciScintilla::BoxedTreeFoldStyle);
		edit_->setAutoIndent(true);
		edit_->setBraceMatching(QsciScintilla::SloppyBraceMatch);
		edit_->setMatchedBraceBackgroundColor(QColor(255, 255, 120));
		
		edit_->setMarginLineNumbers(1, true);

		edit_->setMarginWidth(2, 12);
		//	set margin number 1 accept markers 
		//	number 1 and 2 (binary mask 00000110 == 6)
		edit_->setMarginMarkerMask(1, 6);
		edit_->markerDefine(QsciScintilla::RightTriangle, 1);
		edit_->markerDefine(QsciScintilla::Background, 2);
		edit_->setMarkerForegroundColor(QColor(100, 100, 100));
		edit_->setMarkerBackgroundColor(TextDocSettings::markersColor());
		
		parent->setFocusProxy(edit_);
		parent->connect(edit_, SIGNAL(cursorPositionChanged(int, int)), parent, SIGNAL(cursorPositionChanged(int, int)));
	}
	~TDViewInterior() {
		delete edit_;
	}
	
	QsciScintilla* edit_;
	
	bool lineNumVisible_;
	bool adjustedByWidth_;
	QString syntax_;
};



TextDocView::TextDocView(QWidget* parent) : DocView(parent) {
	vInt_ = new TDViewInterior(this);

	connect(vInt_->edit_, SIGNAL(textChanged()), SLOT(updateLineNums()));
	
	updateLineNums();
}

TextDocView::~TextDocView() {
	delete vInt_;
}

void TextDocView::setDocument(Juff::Document* doc) {
	DocView::setDocument(doc);
	
	TextDoc* tDoc = qobject_cast<TextDoc*>(doc);
	if (tDoc == 0) {
		Log::debug("Wrong document");
		Log::print("Wrong document");
		return;
	}
	
	connect(vInt_->edit_, SIGNAL(modificationChanged(bool)), doc, SLOT(setModified(bool)));
	connect(vInt_->edit_, SIGNAL(modificationChanged(bool)), this, SIGNAL(modified(bool)));

	QString lexName = LexerStorage::instance()->lexerName(doc->fileName());
	setSyntax(lexName);
}

QString TextDocView::syntax() const {
	return vInt_->syntax_;
}
	
void TextDocView::setSyntax(const QString& lexName) {
	if (lexName.isEmpty())
		return;

	QFont font = TextDocSettings::font();
	vInt_->syntax_ = lexName;
	QsciLexer* lexer = LexerStorage::instance()->lexer(lexName, font);
	LexerStorage::instance()->updateLexer(lexName, font);
	
	vInt_->edit_->setLexer(lexer);
	vInt_->edit_->recolor();
}

void TextDocView::rehighlight() {
}

void TextDocView::setModified(bool mod) {
	vInt_->edit_->setModified(mod);
}

void TextDocView::resizeEvent(QResizeEvent*) {
	int h = height();
	int w = width();

	vInt_->edit_->setGeometry(0, 0, w, h);
}

void TextDocView::updateLineNums() {
	QString str = QString("00%1").arg(lineCount());
	if (vInt_->lineNumVisible_)
		vInt_->edit_->setMarginWidth(1, str);
	else
		vInt_->edit_->setMarginWidth(1, 0);
}

void TextDocView::setText(const QString& text) {
	vInt_->edit_->setText(text);
	updateLineNums();
}

void TextDocView::getText(QString& text) const {
	text = vInt_->edit_->text();
}

void TextDocView::insertText(const QString& text) {
	vInt_->edit_->insert(text);
}

void TextDocView::getSelectedText(QString& text) const {
	text = vInt_->edit_->selectedText();
}

void TextDocView::replaceSelectedText(const QString& text) {
	vInt_->edit_->beginUndoAction();
	vInt_->edit_->removeSelectedText();
	vInt_->edit_->insert(text);
	vInt_->edit_->endUndoAction();
}

void TextDocView::setSelection(int, int, int, int) {
	Log::debug("TextDocView::setSelection: not implemented yet");
}

bool TextDocView::lineNumIsVisible() const {
	return vInt_->lineNumVisible_;
}

void TextDocView::setLineNumVisible(bool visible) {
	vInt_->lineNumVisible_ = visible;
	updateLineNums();
}

void TextDocView::showHiddenSymbols(bool show) {
	vInt_->edit_->setWhitespaceVisibility(show ? QsciScintilla::WsVisible : QsciScintilla::WsInvisible);
	vInt_->edit_->setWrapVisualFlags(show ? QsciScintilla::WrapFlagByBorder : QsciScintilla::WrapFlagNone);
}

bool TextDocView::hiddenSymbolsVisible() const {
	return (vInt_->edit_->whitespaceVisibility() == QsciScintilla::WsVisible);
}

bool TextDocView::isAdjustedByWidth() const {
	return vInt_->adjustedByWidth_;
}

void TextDocView::setAdjustedByWidth(bool adjust) {
	vInt_->adjustedByWidth_ = adjust;
	if (adjust)
		vInt_->edit_->setWrapMode(QsciScintilla::WrapWord);
	else
		vInt_->edit_->setWrapMode(QsciScintilla::WrapNone);
}

void TextDocView::getCursorPos(int& row, int& col) const {
	vInt_->edit_->getCursorPosition(&row, &col);
}

void TextDocView::setCursorPos(int row, int col) {
	vInt_->edit_->setCursorPosition(row, col);
}

int TextDocView::scrollPos() const {
	QScrollBar* scr = vInt_->edit_->verticalScrollBar();
	if (scr != 0)
		return scr->value();
	else
		return 0;
}

void TextDocView::setScrollPos(int pos) {
	QScrollBar* scr = vInt_->edit_->verticalScrollBar();
	if (scr != 0)
		scr->setValue(pos);
}

void TextDocView::gotoLine(int line) const {
	vInt_->edit_->setCursorPosition(line - 1, 0);
}

int TextDocView::lineCount() const {
	return vInt_->edit_->lines();
}

void TextDocView::applySettings() {
	QFont font = TextDocSettings::font();

	LexerStorage::instance()->updateLexer(vInt_->syntax_, font);

	vInt_->edit_->setTabWidth(TextDocSettings::tabStopWidth());
	vInt_->edit_->setIndentationsUseTabs(!TextDocSettings::replaceTabsWithSpaces());

	int lInd = TextDocSettings::lineLengthIndicator();
	if (lInd > 0) {
		vInt_->edit_->setEdgeMode(QsciScintilla::EdgeLine);
		vInt_->edit_->setEdgeColumn(lInd);	
	}
	else {
		vInt_->edit_->setEdgeMode(QsciScintilla::EdgeNone);
	}
	
	updateLineNums();
	vInt_->edit_->setCaretLineVisible(TextDocSettings::highlightCurrentLine());
	vInt_->edit_->setIndentationGuides(TextDocSettings::showIndents());
	vInt_->edit_->setBackspaceUnindents(TextDocSettings::backspaceUnindents());
	vInt_->edit_->setMarkerBackgroundColor(TextDocSettings::markersColor());
	vInt_->edit_->setCaretLineBackgroundColor(TextDocSettings::curLineColor());
}



////////////////////////////////////////////////////////////
//	EDIT
void TextDocView::cut() {
	vInt_->edit_->cut();
}

void TextDocView::copy() {
	vInt_->edit_->copy();
}

void TextDocView::paste() {
	vInt_->edit_->paste();
}

void TextDocView::undo() {
	vInt_->edit_->undo();
}

void TextDocView::redo() {
	vInt_->edit_->redo();
}

void TextDocView::unindentSelectedLines() {
	int lineFrom(-1), lineTo(-1), colFrom(-1), colTo(-1);
	vInt_->edit_->getSelection(&lineFrom, &colFrom, &lineTo, &colTo);
	if (colTo == 0)
		--lineTo;
	unindentLines(lineFrom, lineTo);
}
void TextDocView::unindentLines(int from, int to) {
	if (from <= to && from >= 0) {
		for (int line = from; line <= to; ++line)
			vInt_->edit_->unindent(line);
	}
}
//	EDIT
////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
//	FIND
void prepareForFind(QsciScintilla* edit, const QString& str, bool back) {
	if (back) {
		if (edit->hasSelectedText()) {
			int fromRow, fromCol, toRow, toCol;
			edit->getSelection(&fromRow, &fromCol, &toRow, &toCol);
			if (fromRow == toRow) {
				QString selection = edit->selectedText();
				if (selection.compare(str) == 0)
					edit->setCursorPosition(fromRow, fromCol);
			}
		}
	}
}

bool TextDocView::continueOverTheEnd(bool back) {
	QString msg;
	int row(0), col(0);
	if (back) {
		msg = tr("The search has reached the beginning of file.\nContinue from the end?");
		row = lineCount() - 1;
		col = vInt_->edit_->text(row).length();
	}
	else {
		msg = tr("The search has reached the end of file.\nContinue from the beginning?");
		row = 0;
		col = 0;
	}
	QMessageBox::StandardButton choice = QMessageBox::question(this, tr("Find"),
			msg, QMessageBox::Ok | QMessageBox::Cancel);
	
	if (choice == QMessageBox::Ok) {
		vInt_->edit_->setCursorPosition(row, col);
		return true;
	}
	else {
		return false;
	}
}

void TextDocView::find(const QString& str, bool isRegExp, DocFindFlags flags) {
	prepareForFind(vInt_->edit_, str, flags.backward);
	
	bool found = vInt_->edit_->findFirst(str, isRegExp, flags.matchCase, false, false, !flags.backward);
	if (!found) {
		//	not found
		if (continueOverTheEnd(flags.backward))
			find(str, isRegExp, flags);
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

bool TextDocView::doReplace(const QString& text, bool& replaceAll) {
	if (!replaceAll) {
		//	ask confirmation if replace all hasn't been chosen yet
		Answer conf = confirm(this);
		if (conf == Cancel)
			return false;
		else if (conf == Yes)
			vInt_->edit_->replace(text);
		else if (conf == All) {
			replaceAll = true;
			vInt_->edit_->replace(text);
		}
	}
	else {
		//	just replace, because there has been chosen "select all"
		vInt_->edit_->replace(text);
	}
	return true;
}

void TextDocView::replace(const QString& from, bool isRegExp, const QString& to, DocFindFlags flags) {
	prepareForFind(vInt_->edit_, from, flags.backward);
	
	bool cancelled = false;
	bool replaceAll = false;
	while (vInt_->edit_->findFirst(from, isRegExp, flags.matchCase, false, false, !flags.backward)) {
		if (!doReplace(to, replaceAll)) {
			cancelled = true;
			break;
		}
	}
	if (!cancelled) {
		//	reached the end or the beginning
		if (continueOverTheEnd(flags.backward))
			replace(from, isRegExp, to, flags);
	}
}
//	FIND & REPLACE
////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
//	MARKERS
void TextDocView::toggleMarker() {
	int line(-1), col(-1);
	vInt_->edit_->getCursorPosition(&line, &col);

	//	determine if this line comtains marker 
	//	number 1 (binary mask 00000010 == 2)
	if (vInt_->edit_->markersAtLine(line) & 2) {
		vInt_->edit_->markerDelete(line, 1);
		vInt_->edit_->markerDelete(line, 2);
	}
	else {
		vInt_->edit_->markerAdd(line, 1);
		vInt_->edit_->markerAdd(line, 2);
	}
}

void TextDocView::gotoNextMarker() {
	int row(-1), col(-1);
	vInt_->edit_->getCursorPosition(&row, &col);

	int mLine = vInt_->edit_->markerFindNext(row + 1, 2);
	if (mLine >= 0) {
		gotoLine(mLine + 1);
	}
	else {
		mLine = vInt_->edit_->markerFindNext(0, 2);
		if (mLine >= 0) {
			gotoLine(mLine + 1);
		}
	}
}

void TextDocView::gotoPrevMarker() {
	int row(-1), col(-1);
	vInt_->edit_->getCursorPosition(&row, &col);

	int mLine = vInt_->edit_->markerFindPrevious(row - 1 , 2);
	if (mLine >= 0) {
		gotoLine(mLine + 1);
	}
	else {
		mLine = vInt_->edit_->markerFindPrevious(lineCount() - 1, 2);
		if (mLine >= 0) {
			gotoLine(mLine + 1);
		}
	}
}

void TextDocView::removeAllMarkers() {
	vInt_->edit_->markerDeleteAll();
}

IntList TextDocView::markers() const {
	IntList list;
	int line = 0;
	while ((line = vInt_->edit_->markerFindNext(line, 2)) >= 0) {
		list << ++line;
	}
	return list;
}

QString TextDocView::markedLine(int line) const {
	return vInt_->edit_->text(line - 1);
}
//	MARKERS
////////////////////////////////////////////////////////////
