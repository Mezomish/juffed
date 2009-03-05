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

#include "SciDoc.h"

#include "CommandStorage.h"
#include "Functions.h"
#include "Log.h"
#include "LexerStorage.h"
#include "TextDocSettings.h"

#include <Qsci/qsciscintilla.h>
#include <Qsci/qsciprinter.h>

#include <QtCore/QFile>
#include <QtCore/QTextCodec>
#include <QtCore/QTextStream>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QMenu>
#include <QtGui/QMessageBox>
#include <QtGui/QPrintDialog>
#include <QtGui/QScrollBar>
#include <QtGui/QSplitter>

namespace Juff {

class MyQScintilla : public QsciScintilla {
public:
	MyQScintilla(QWidget* parent) : QsciScintilla(parent) {
		contextMenu_ = new QMenu();
/*		contextMenu_->addAction(CommandStorage::instance()->action(ID_EDIT_CUT));
		contextMenu_->addAction(CommandStorage::instance()->action(ID_EDIT_COPY));
		contextMenu_->addAction(CommandStorage::instance()->action(ID_EDIT_PASTE));
		contextMenu_->addSeparator();
		contextMenu_->addAction(CommandStorage::instance()->action(ID_EDIT_UNDO));
		contextMenu_->addAction(CommandStorage::instance()->action(ID_EDIT_REDO));
		contextMenu_->addSeparator();
		contextMenu_->addAction(CommandStorage::instance()->action(ID_FIND));
		contextMenu_->addAction(CommandStorage::instance()->action(ID_FIND_NEXT));
		contextMenu_->addAction(CommandStorage::instance()->action(ID_FIND_PREV));
		contextMenu_->addAction(CommandStorage::instance()->action(ID_REPLACE));
//		contextMenu_->addSeparator();
//		contextMenu_->addAction(CommandStorage::instance()->action(ID_GOTO_LINE));*/
	}
	virtual ~MyQScintilla() {
		delete contextMenu_;
	}

	bool find(const QString& s, const DocFindFlags& flags) {
		QString str(s);
		QString text = this->text();
		QStringList lines = text.split(QRegExp("\r?\n"));
		int row(-1), col(-1);
		this->getCursorPosition(&row, &col);
		int lineIndex(0);
		if (row < 0 || col < 0)
			return false;

		if (!flags.backwards) {
			foreach (QString line, lines) {
				if (lineIndex < row) {
				}
				else {
					int indent(0);			
					if (lineIndex == row) {
						line = line.right(line.length() - col);
						indent = col;
					}
					int index(-1);
					QRegExp regExp(str);
					regExp.setCaseSensitivity(flags.matchCase ? Qt::CaseSensitive : Qt::CaseInsensitive);
					if (flags.isRegExp)
						index = line.indexOf(regExp);
					else {
						if (!flags.matchCase) {
							str = str.toLower();
							line = line.toLower();
						}
						index = line.indexOf(str);
					}

					if (index >= 0) {
						if (flags.isRegExp) {
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
			while (lineIndex >= 0) {
				if (lineIndex > row) {
				}
				else {
					QString line = *it;
					if (lineIndex == row) {
						line = line.left(col);
					}

					int index(-1);
					QRegExp regExp(str);
					regExp.setCaseSensitivity(flags.matchCase ? Qt::CaseSensitive : Qt::CaseInsensitive);
					if (flags.isRegExp)
						index = line.lastIndexOf(regExp);
					else {
						if (!flags.matchCase) {
							str = str.toLower();
							line = line.toLower();
						}
						index = line.lastIndexOf(str);
					}

					if (index >= 0) {
						if (flags.isRegExp) {
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

	void replaceSelected(const QString& targetText, bool backwards) {
		beginUndoAction();
		removeSelectedText();
		int r, c;
		getCursorPosition(&r, &c);
		insert(targetText);
		if ( !backwards )
			setCursorPosition(r, c + targetText.length());
		endUndoAction();
	}
	
protected:
	virtual void dragEnterEvent(QDragEnterEvent* e) {
		if ( !e->mimeData()->hasUrls() )
			QsciScintilla::dragEnterEvent(e);
	}

	virtual void dropEvent(QDropEvent* e) {
		if ( !e->mimeData()->hasUrls() )
			QsciScintilla::dropEvent(e);
	}

	virtual void contextMenuEvent(QContextMenuEvent* e) {
		contextMenu_->exec(e->globalPos());
	}

private:
	QMenu* contextMenu_;
};

class SciDoc::Interior {
public:
	Interior() {
		syntax_ = "none";
		codec_ = QTextCodec::codecForLocale();
		charsetName_ = codec_->name();
		
//		w_ = new SciWidget();
		edit1_ = createEdit();
		edit2_ = createEdit();
		edit2_->setDocument(edit1_->document());
		
		spl_ = new QSplitter(Qt::Vertical);
		spl_->addWidget(edit1_);
		spl_->addWidget(edit2_);
		
		spl_->setSizes(QList<int>() << 0 << spl_->height());
		spl_->setFocusProxy(edit2_);
//		edit1_->resize(100, 0);

//		parent->setFocusProxy(edit1_);
//		parent->connect(edit1_, SIGNAL(cursorPositionChanged(int, int)), parent, SIGNAL(cursorPositionChanged(int, int)));

	}
	
	
	~Interior() {
		delete spl_;
	}
	
	MyQScintilla* createEdit() {
		MyQScintilla* edit = new MyQScintilla(0);
		edit->setUtf8(true);
		edit->setCaretLineBackgroundColor(TextDocSettings::curLineColor());
		edit->setIndentationGuidesForegroundColor(QColor(250, 200, 200));
		edit->setFolding(QsciScintilla::BoxedTreeFoldStyle);
		edit->setAutoIndent(true);
		edit->setBraceMatching(QsciScintilla::SloppyBraceMatch);
		edit->setMatchedBraceBackgroundColor(QColor(255, 255, 120));
		
		edit->setMarginLineNumbers(1, true);
		edit->setMarginWidth(2, 12);
		//	set the 1st margin accept markers 
		//	number 1 and 2 (binary mask 00000110 == 6)
		edit->setMarginMarkerMask(1, 6);
		edit->markerDefine(QsciScintilla::RightTriangle, 1);
		edit->markerDefine(QsciScintilla::Background, 2);
		edit->setMarkerForegroundColor(QColor(100, 100, 100));
		edit->setMarkerBackgroundColor(TextDocSettings::markersColor());
		
		return edit;
	}

	QSplitter* spl_;
	MyQScintilla* edit1_;
	MyQScintilla* edit2_;
	
	QString syntax_;
	QTextCodec* codec_;
	QString charsetName_;
};

SciDoc::SciDoc(const QString& fileName) : Document(fileName) {
	docInt_ = new Interior();

	wrapText(TextDocSettings::widthAdjust());
	showLineNumbers(TextDocSettings::showLineNumbers());

	if ( !isNoname(fileName) ) {
		readDoc();
		docInt_->edit1_->setModified(false);
	}

	connect(docInt_->edit1_, SIGNAL(modificationChanged(bool)), this, SIGNAL(modified(bool)));

	QString lexName = LexerStorage::instance()->lexerName(fileName);
	setSyntax(lexName);
	
//	docInt_->edit1_->setTabWidth(TextDocSettings::tabStopWidth());
	applySettings();
}

SciDoc::~SciDoc() {
	delete docInt_;
}

void SciDoc::print() {
	QsciPrinter prn;
	QPrintDialog dlg(&prn, widget());
	if (dlg.exec() == QDialog::Accepted) {
		prn.setWrapMode(TextDocSettings::widthAdjust() ? QsciScintilla::WrapWord : QsciScintilla::WrapNone);
		
		int line1(-1), line2(-1), col1(-1), col2(-1);
		MyQScintilla* edit = getActiveEdit();
		edit->getSelection(&line1, &col1, &line2, &col2);
		if (line1 >=0 && line2 >= 0 && col1 >= 0 && col2 >= 0) {
			//	We have selection. Print it.
			
			--line2;
			prn.printRange(edit, line1, line2);
		}
		else {
			//	We don't have selection. Print the whole text.
			prn.printRange(edit, 0);
		}
	}
}

QWidget* SciDoc::widget() {
	return docInt_->spl_;
}

MyQScintilla* SciDoc::getActiveEdit() const {
	return (docInt_->edit1_->hasFocus() ? docInt_->edit1_ : docInt_->edit2_);
}

bool SciDoc::isModified() const {
	return docInt_->edit1_->isModified();
}

void SciDoc::setModified(bool mod) {
	return docInt_->edit1_->setModified(mod);
}

void SciDoc::undo() {
	docInt_->edit1_->undo();
}

void SciDoc::redo() {
	docInt_->edit1_->redo();
}

void SciDoc::cut() {
	if ( docInt_->edit1_->hasFocus() )
		docInt_->edit1_->cut();
	else
		docInt_->edit2_->cut();
}

void SciDoc::copy() {
	if ( docInt_->edit1_->hasFocus() )
		docInt_->edit1_->copy();
	else
		docInt_->edit2_->copy();
}

void SciDoc::paste() {
	if ( docInt_->edit1_->hasFocus() )
		docInt_->edit1_->paste();
	else
		docInt_->edit2_->paste();
}

void prepareForFind(QsciScintilla* edit, const QString& s, const DocFindFlags& flags) {
	QString str(s);
	if ( flags.backwards ) {
		if ( edit->hasSelectedText() ) {
			int fromRow, fromCol, toRow, toCol;
			edit->getSelection(&fromRow, &fromCol, &toRow, &toCol);
			if ( fromRow == toRow ) {
				QString selection = edit->selectedText();
				if ( flags.isRegExp ) {
					QRegExp r(str);
					r.setCaseSensitivity(flags.matchCase ? Qt::CaseSensitive : Qt::CaseInsensitive);
					if ( r.exactMatch(selection) )
						edit->setCursorPosition(fromRow, fromCol);
				}
				else {
					if ( !flags.matchCase ) {
						str = str.toLower();
						selection = selection.toLower();
					}
					if ( selection.compare(str) == 0 )
						edit->setCursorPosition(fromRow, fromCol);
				}
			}
		}
	}
}

bool continueOverTheEnd(QsciScintilla* edit, bool back) {
	QString msg;
	int row(0), col(0);
	if ( back ) {
		msg = QObject::tr("The search has reached the beginning of file.\nContinue from the end?");
		row = edit->lines() - 1;
		col = edit->text(row).length();
	}
	else {
		msg = QObject::tr("The search has reached the end of file.\nContinue from the beginning?");
		row = 0;
		col = 0;
	}
	
	QMessageBox::StandardButton choice = QMessageBox::question(edit, QObject::tr("Find"),
			msg, QMessageBox::Ok | QMessageBox::Cancel);
	
	if ( choice == QMessageBox::Ok ) {
		edit->setCursorPosition(row, col);
		return true;
	}
	else {
		return false;
	}
}

void SciDoc::find(const QString& str, const DocFindFlags& flags) {
	JUFFENTRY;
	
	JUFFDEBUG("_________");
	JUFFDEBUG(str);
	JUFFDEBUG( flags.replace ? "replace" : "not replace" );
	JUFFDEBUG( flags.matchCase ? "match case" : "not match case" );
	JUFFDEBUG( flags.backwards ? "backwards" : "forward" );
	JUFFDEBUG( flags.isRegExp ? "regexp" : "plain text" );
	JUFFDEBUG("_________");
	
	MyQScintilla* edit = getActiveEdit();
	prepareForFind(edit, str, flags);

	bool found = edit->find(str, flags);
	if ( !found ) {
		//	not found
		if ( continueOverTheEnd(edit, flags.backwards) )
			find(str, flags);
	}
}

void SciDoc::replace(const QString& str1, const QString& str2, const DocFindFlags& flags) {
	JUFFENTRY;
	
	MyQScintilla* edit = getActiveEdit();
	prepareForFind(edit, str1, flags);
	
	bool cancelled = false;
	bool replaceAll = false;
	while ( edit->find(str1, flags) ) {
		if ( !doReplace(edit, str1, str2, flags, replaceAll) ) {
			cancelled = true;
			break;
		}
	}
	if ( !cancelled ) {
		//	reached the end or the beginning
		if ( continueOverTheEnd(edit, flags.backwards) )
			replace(str1, str2, flags);
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

bool SciDoc::doReplace(MyQScintilla* edit, const QString& str1, const QString& str2, const DocFindFlags& flags, bool& replaceAll) {
	QString selectedText = edit->selectedText();
	QString targetText(str2);
	if ( flags.isRegExp ) {
		QRegExp regExp(str1);
		regExp.setCaseSensitivity(flags.matchCase ? Qt::CaseSensitive : Qt::CaseInsensitive);
		if ( regExp.exactMatch(selectedText) ) {
			QStringList matches = regExp.capturedTexts();
			int n = matches.size();
			for ( int i = 0; i < n; ++i ) {
				targetText.replace(QString("\\%1").arg(i), matches[i]);
			}
		}
		else
			return false;
	}

	if ( !replaceAll ) {
		//	ask for confirmation if replace all hasn't been chosen yet
		
		Answer conf = confirm(widget());
		if ( conf == Cancel )
			return false;
		else if ( conf == Yes ) {
			edit->replaceSelected(targetText, flags.backwards);
			edit->selectAll(false);
		}
		else if ( conf == All ) {
			edit->replaceSelected(targetText, flags.backwards);
			edit->selectAll(false);
			replaceAll = true;
		}
	}
	else {
		//	just replace, because there has been chosen "select all"
		edit->replaceSelected(targetText, flags.backwards);
		edit->selectAll(false);
	}
	return true;
}

void SciDoc::showLineNumbers(bool show) {
	JUFFENTRY;
	
	if ( show ) {
		QString str = QString("00%1").arg(lineCount());
		docInt_->edit1_->setMarginWidth(1, str);
		docInt_->edit2_->setMarginWidth(1, str);
	}
	else {
		docInt_->edit1_->setMarginWidth(1, 0);
		docInt_->edit2_->setMarginWidth(1, 0);
	}
}

void SciDoc::wrapText(bool wrap) {
	JUFFENTRY;
	
	if ( wrap ) {
		docInt_->edit1_->setWrapMode(QsciScintilla::WrapWord);
		docInt_->edit2_->setWrapMode(QsciScintilla::WrapWord);
	}
	else {
		docInt_->edit1_->setWrapMode(QsciScintilla::WrapNone);
		docInt_->edit2_->setWrapMode(QsciScintilla::WrapNone);
	}
	TextDocSettings::setWidthAdjust(wrap);
}





bool SciDoc::save(const QString& fileName, QString& error) {
	JUFFENTRY;
	
//	Log::debug(fileName);
	
	QFile file(fileName);
	if (file.open(QIODevice::WriteOnly)) {
		QString text("");
//		if (tdView != 0) {
//			tdView->getText(text);
			text = docInt_->edit1_->text();
			file.write(docInt_->codec_->fromUnicode(text));
//			file.write(text.toUtf8());
			file.close();

/*			tdView->setFocus();
			if (changeName) {
				//	We save document with new file name and set
				//	new name as doc's fileName
				setFileName(name);
				setLastModified(QFileInfo(file).lastModified());
				if (tdView->syntax().compare("none") == 0) {
					QString syntax = LexerStorage::instance()->lexerName(name);
					tdView->setSyntax(syntax);
				}
				tdView->setModified(false);
			}*/
/*		}
		else {
			res = StatusUnknownError;
		}*/
//		docInt_->w_->document()->setModified(false);
		return true;
	}
	else {
		error = tr("Can't open file for writing");
		return false;
	}
}

void SciDoc::readDoc() {
	QString text;
	QFile file(fileName());
	if ( file.open(QIODevice::ReadOnly) ) {
		QTextStream ts(&file);
		ts.setCodec(docInt_->codec_);
		docInt_->edit1_->setText(ts.readAll());
	}
}

void SciDoc::applySettings() {
	JUFFENTRY;
	
	QFont font = TextDocSettings::font();
	JUFFDEBUG(font.pointSize());

	LexerStorage::instance()->updateLexer(docInt_->syntax_, font);

	docInt_->edit1_->setTabWidth(TextDocSettings::tabStopWidth());
	docInt_->edit1_->setIndentationsUseTabs(!TextDocSettings::replaceTabsWithSpaces());

	int lInd = TextDocSettings::lineLengthIndicator();
	if (lInd > 0) {
		docInt_->edit1_->setEdgeMode(QsciScintilla::EdgeLine);
		docInt_->edit1_->setEdgeColumn(lInd);	
		docInt_->edit2_->setEdgeMode(QsciScintilla::EdgeLine);
		docInt_->edit2_->setEdgeColumn(lInd);	
	}
	else {
		docInt_->edit1_->setEdgeMode(QsciScintilla::EdgeNone);
		docInt_->edit2_->setEdgeMode(QsciScintilla::EdgeNone);
	}
	
	showLineNumbers(TextDocSettings::showLineNumbers());

	docInt_->edit1_->setCaretLineVisible(TextDocSettings::highlightCurrentLine());
	docInt_->edit1_->setCaretLineBackgroundColor(TextDocSettings::curLineColor());
	docInt_->edit1_->setIndentationGuides(TextDocSettings::showIndents());
	docInt_->edit1_->setBackspaceUnindents(TextDocSettings::backspaceUnindents());
	docInt_->edit1_->setMarkerBackgroundColor(TextDocSettings::markersColor());
	docInt_->edit2_->setCaretLineVisible(TextDocSettings::highlightCurrentLine());
	docInt_->edit2_->setCaretLineBackgroundColor(TextDocSettings::curLineColor());
	docInt_->edit2_->setIndentationGuides(TextDocSettings::showIndents());
	docInt_->edit2_->setBackspaceUnindents(TextDocSettings::backspaceUnindents());
	docInt_->edit2_->setMarkerBackgroundColor(TextDocSettings::markersColor());

	//	autocompletion
/*	docInt_->edit1_->setAutoCompletionThreshold(AutocompleteSettings::threshold());
	docInt_->edit1_->setAutoCompletionReplaceWord(AutocompleteSettings::replaceWord());
	if (AutocompleteSettings::useDocument()) {
		if (AutocompleteSettings::useApis())
			docInt_->edit1_->setAutoCompletionSource(QsciScintilla::AcsAll);
		else
			docInt_->edit1_->setAutoCompletionSource(QsciScintilla::AcsDocument);
	}
	else {
		if (AutocompleteSettings::useApis())
			docInt_->edit1_->setAutoCompletionSource(QsciScintilla::AcsAPIs);
		else
			docInt_->edit1_->setAutoCompletionSource(QsciScintilla::AcsNone);
	}*/
}

void SciDoc::updateActivated() {
	wrapText(TextDocSettings::widthAdjust());
	showLineNumbers(TextDocSettings::showLineNumbers());
	
	//	request for menu to update it
//	emit getMarkersMenu();
}



int SciDoc::lineCount() const {
	return docInt_->edit1_->lines();
}

QString SciDoc::getLine(int line) const {
	return docInt_->edit1_->text(line);
}

void SciDoc::zoomIn() {
	docInt_->edit1_->zoomIn();
	docInt_->edit2_->zoomIn();
}

void SciDoc::zoomOut() {
	docInt_->edit1_->zoomOut();
	docInt_->edit2_->zoomOut();
}

void SciDoc::zoom100() {
	docInt_->edit1_->zoomTo(0);
	docInt_->edit2_->zoomTo(0);
}


void SciDoc::gotoLine(int line) {
	JUFFENTRY;

	MyQScintilla* edit = getActiveEdit();
	edit->setCursorPosition(line, 0);
}

int SciDoc::curLine() const {
	int line, col;
	getActiveEdit()->getCursorPosition(&line, &col);
	return line;
}

QString SciDoc::text() const { 
	return docInt_->edit1_->text(); 
}

QString SciDoc::selectedText() const { 
	return getActiveEdit()->selectedText(); 
}

void SciDoc::getCursorPos(int& line, int& col) const {
	getActiveEdit()->getCursorPosition(&line, &col);
}

void SciDoc::setCursorPos(int line, int col) {
	getActiveEdit()->setCursorPosition(line, col);
	getActiveEdit()->setFocus();
}

void SciDoc::getSelection(int& line1, int& col1, int& line2, int& col2) const {
	getActiveEdit()->getSelection(&line1, &col1, &line2, &col2);
}

void SciDoc::setSelection(int line1, int col1, int line2, int col2) {
	getActiveEdit()->setSelection(line1, col1, line2, col2);
}

void SciDoc::insertText(const QString& text) {
	getActiveEdit()->insert(text);
}

void SciDoc::removeSelectedText() {
	getActiveEdit()->removeSelectedText();
}


int SciDoc::curScrollPos() const {
	QScrollBar* scr = getActiveEdit()->verticalScrollBar();
	if ( scr )
		return scr->value();
	else
		return 0;
}

void SciDoc::setScrollPos(int pos) {
	QScrollBar* scr = getActiveEdit()->verticalScrollBar();
	if ( scr )
		scr->setValue(pos);
}



IntList SciDoc::markers() const {
	IntList list;
	int line = 0;
	while ((line = docInt_->edit1_->markerFindNext(line, 2)) >= 0) {
		list << line++;
	}
	return list;
}

void SciDoc::toggleMarker() {
	MyQScintilla* edit = getActiveEdit();
	int line, col;
	edit->getCursorPosition(&line, &col);
	
	if ( edit->markersAtLine(line) & 2) {
		edit->markerDelete(line, 1);
		edit->markerDelete(line, 2);
	}
	else {
		edit->markerAdd(line, 1);
		edit->markerAdd(line, 2);
	}
	
	//	Markers will be added to/deleted from the 2nd edit 
	//	automatically since they share the same document
}

void SciDoc::nextMarker() {
	MyQScintilla* edit = getActiveEdit();
	int row(-1), col(-1);
	edit->getCursorPosition(&row, &col);

	int mLine = edit->markerFindNext(row + 1, 2);
	if ( mLine >= 0 ) {
		gotoLine(mLine);
	}
	else {
		mLine = edit->markerFindNext(0, 2);
		if ( mLine >= 0 ) {
			gotoLine(mLine);
		}
	}
}

void SciDoc::prevMarker() {
	MyQScintilla* edit = getActiveEdit();
	int row(-1), col(-1);
	edit->getCursorPosition(&row, &col);

	int mLine = edit->markerFindPrevious(row - 1 , 2);
	if (mLine >= 0) {
		gotoLine(mLine);
	}
	else {
		mLine = edit->markerFindPrevious(lineCount() - 1, 2);
		if (mLine >= 0) {
			gotoLine(mLine);
		}
	}
}

void SciDoc::removeAllMarkers() {
	docInt_->edit1_->markerDeleteAll();
	
	//	Markers will be deleted from the 2nd edit 
	//	automatically since they share the same document
}

QString SciDoc::charset() const {
	return docInt_->charsetName_;
}

void SciDoc::setCharset(const QString& charset) {
	JUFFENTRY;
	
	QTextCodec* codec = QTextCodec::codecForName(charset.toAscii());
	if (codec != 0) {
		docInt_->codec_ = codec;
		docInt_->charsetName_ = charset;
		readDoc();
	}
}

QString SciDoc::syntax() const {
	return docInt_->syntax_;
}

void SciDoc::setSyntax(const QString& lexName) {
	if ( lexName.isEmpty() )
		return;

	QFont font = TextDocSettings::font();
	docInt_->syntax_ = lexName;
	QsciLexer* lexer = LexerStorage::instance()->lexer(lexName, font);
	LexerStorage::instance()->updateLexer(lexName, font);
	
	//	find autocompletion API
//	loadAutocompletionAPI(lexName, lexer);
	
	docInt_->edit1_->setLexer(lexer);
	docInt_->edit1_->recolor();
	docInt_->edit2_->setLexer(lexer);
	docInt_->edit2_->recolor();
}




}

