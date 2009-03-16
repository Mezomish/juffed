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
#include "JuffScintilla.h"
#include "TextDocSettings.h"

#include <Qsci/qsciscintilla.h>
#include <Qsci/qsciprinter.h>

#include <QtCore/QFile>
#include <QtCore/QTextCodec>
#include <QtCore/QTextStream>
#include <QtGui/QMenu>
#include <QtGui/QMessageBox>
#include <QtGui/QPrintDialog>
#include <QtGui/QScrollBar>
#include <QtGui/QSplitter>

namespace Juff {

class SciDoc::Interior {
public:
	Interior() {
		syntax_ = "none";
		codec_ = QTextCodec::codecForLocale();
		charsetName_ = codec_->name();
		
		edit1_ = createEdit();
		edit2_ = createEdit();
		edit2_->setDocument(edit1_->document());
		
		spl_ = new QSplitter(Qt::Vertical);
		spl_->addWidget(edit1_);
		spl_->addWidget(edit2_);
		
		spl_->setSizes(QList<int>() << 0 << spl_->height());
		spl_->setFocusProxy(edit2_);
		edit2_->setFocus();
	}
	
	
	~Interior() {
		delete spl_;
	}
	
	JuffScintilla* createEdit() {
		JuffScintilla* edit = new JuffScintilla();
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
//		edit->setWhitespaceVisibility(QsciScintilla::WsVisible);
		
		return edit;
	}

	QSplitter* spl_;
	JuffScintilla* edit1_;
	JuffScintilla* edit2_;
	
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
		
		//	line endings
		QFile file(fileName);
		if ( file.open(QIODevice::ReadOnly) ) {
			QString line = QString::fromLocal8Bit(file.readLine().constData());
			QRegExp re(".*(\r?\n?)");
			if ( re.exactMatch(line) ) {
				QString ending = re.cap(1);
				if ( ending == "\r\n" ) {
					docInt_->edit1_->setEolMode(QsciScintilla::EolWindows);
				}
				else if ( ending == "\r" ) {
					docInt_->edit1_->setEolMode(QsciScintilla::EolMac);
				}
				else {
					docInt_->edit1_->setEolMode(QsciScintilla::EolUnix);
				}	
			}
			file.close();
		}
	}

	connect(docInt_->edit1_, SIGNAL(modificationChanged(bool)), this, SIGNAL(modified(bool)));
	connect(docInt_->edit1_, SIGNAL(cursorPositionChanged(int, int)), this, SIGNAL(cursorPositionChanged(int, int)));
	connect(docInt_->edit2_, SIGNAL(cursorPositionChanged(int, int)), this, SIGNAL(cursorPositionChanged(int, int)));

	QString lexName = LexerStorage::instance()->lexerName(fileName);
	setSyntax(lexName);
	
	applySettings();
	
	QAction* unindentAct = new QAction(this);
	unindentAct->setShortcut(QKeySequence("Shift+Tab"));
	connect(unindentAct, SIGNAL(activated()), this, SLOT(unindent()));
	widget()->addAction(unindentAct);
}

SciDoc::~SciDoc() {
	delete docInt_;
}

void SciDoc::setFileName(const QString& fileName) {
	Document::setFileName(fileName);
	QString lexName = LexerStorage::instance()->lexerName(fileName);
	if ( lexName != docInt_->syntax_ )
		setSyntax(lexName);
}

void SciDoc::print() {
	QsciPrinter prn;
	QPrintDialog dlg(&prn, widget());
	if (dlg.exec() == QDialog::Accepted) {
		prn.setWrapMode(TextDocSettings::widthAdjust() ? QsciScintilla::WrapWord : QsciScintilla::WrapNone);
		
		int line1(-1), line2(-1), col1(-1), col2(-1);
		JuffScintilla* edit = getActiveEdit();
		if ( edit ) {
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
}

QWidget* SciDoc::widget() {
	return docInt_->spl_;
}

JuffScintilla* SciDoc::getActiveEdit() const {
	return qobject_cast<JuffScintilla*>(docInt_->spl_->focusProxy());
}

bool SciDoc::isModified() const {
	return docInt_->edit1_->isModified();
}

void SciDoc::setModified(bool mod) {
	return docInt_->edit1_->setModified(mod);
}

void SciDoc::undo() {
	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return;
	
	edit->undo();
}

void SciDoc::redo() {
	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return;
	
	edit->redo();
}

void SciDoc::cut() {
	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return;

	edit->cut();
}

void SciDoc::copy() {
	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return;

	edit->copy();
}

void SciDoc::paste() {
	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return;

	edit->paste();
}

void SciDoc::unindent() {
	JUFFENTRY;
	
	int line1(-1), line2(-1), col1(-1), col2(-1);
	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return;
	
	if ( edit->hasSelectedText() ) {
		edit->getSelection(&line1, &col1, &line2, &col2);
		if ( col2 == 0 )
			--line2;
		
		if (line1 <= line2 && line1 >= 0) {
			for (int l = line1; l <= line2; ++l) {
				edit->unindent(l);
			}
		}
	}
	else {
		edit->getCursorPosition(&line1, &col1);
		if ( line1 >= 0 ) {
			edit->unindent(line1);
		}
	}
}



void prepareForFind(QsciScintilla* edit, const QString& s, const DocFindFlags& flags) {
	QString str(s);
	if ( flags.backwards && edit->hasSelectedText() ) {
		int fromRow, fromCol, toRow, toCol;
		edit->getSelection(&fromRow, &fromCol, &toRow, &toCol);
		if ( fromRow == toRow ) {
			QString selection = edit->selectedText();
			if ( flags.isRegExp ) {
				QRegExp r(str);
				r.setCaseSensitivity(flags.matchCase ? Qt::CaseSensitive : Qt::CaseInsensitive);
				if ( r.exactMatch(selection) ) {
					edit->setCursorPosition(fromRow, fromCol);
				}
			}
			else {
				if ( !flags.matchCase ) {
					str = str.toLower();
					selection = selection.toLower();
				}
				if ( selection.compare(str) == 0 ) {
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
	
	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return;
	
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
	
	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return;
	
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

bool SciDoc::doReplace(JuffScintilla* edit, const QString& str1, const QString& str2, const DocFindFlags& flags, bool& replaceAll) {
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
	
	QFile file(fileName);
	if ( file.open(QIODevice::WriteOnly) ) {
		QString text("");
		text = docInt_->edit1_->text();
		file.write(docInt_->codec_->fromUnicode(text));
		file.close();
		Document::save(fileName, error);
		return true;
	}
	else {
		error = tr("Can't open file for writing");
		return false;
	}
}

void SciDoc::reload() {
	if ( !Juff::isNoname(fileName()) ) {
		int line, col;
		getCursorPos(line, col);
		int scroll = curScrollPos();
		readDoc();
		setModified(false);
		if ( line >=0 && col >= 0 ) {
			setCursorPos(line, col);
			setScrollPos(scroll);
		}
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
	if ( lInd > 0 ) {
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
}

void SciDoc::addContextMenuActions(const ActionList& list) {
	docInt_->edit1_->addContextMenuActions(list);
	docInt_->edit2_->addContextMenuActions(list);
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

	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return;
	
	edit->setCursorPosition(line, 0);
}

int SciDoc::curLine() const {
	int line = -1, col = -1;
	JuffScintilla* edit = getActiveEdit();
	if ( !edit ) 
		return -1;
	
	edit->getCursorPosition(&line, &col);
	return line;
}

QString SciDoc::text() const { 
	return docInt_->edit1_->text(); 
}

QString SciDoc::selectedText() const {
	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return QString();
	
	return edit->selectedText(); 
}

void SciDoc::getCursorPos(int& line, int& col) const {
	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return;
	
	edit->getCursorPosition(&line, &col);
}

void SciDoc::setCursorPos(int line, int col) {
	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return;
	
	edit->setCursorPosition(line, col);
	edit->setFocus();
}

void SciDoc::getSelection(int& line1, int& col1, int& line2, int& col2) const {
	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return;
	
	edit->getSelection(&line1, &col1, &line2, &col2);
}

void SciDoc::setSelection(int line1, int col1, int line2, int col2) {
	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return;
	
	edit->setSelection(line1, col1, line2, col2);
}

void SciDoc::insertText(const QString& text) {
	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return;
	
	edit->insert(text);
}

void SciDoc::removeSelectedText() {
	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return;
	
	edit->removeSelectedText();
}


int SciDoc::curScrollPos() const {
	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return 0;
	
	QScrollBar* scr = edit->verticalScrollBar();
	if ( scr ) {
		return scr->value();
	}
	else {
		return 0;
	}
}

void SciDoc::setScrollPos(int pos) {
	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return;
	
	QScrollBar* scr = edit->verticalScrollBar();
	if ( scr ) {
		scr->setValue(pos);
	}
}



IntList SciDoc::markers() const {
	IntList list;
	int line = 0;
	while ( (line = docInt_->edit1_->markerFindNext(line, 2)) >= 0 ) {
		list << line++;
	}
	return list;
}

void SciDoc::toggleMarker() {
	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return;
	
	int line, col;
	edit->getCursorPosition(&line, &col);
	
	if ( edit->markersAtLine(line) & 2 ) {
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
	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return;
	
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
	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return;
	
	int row(-1), col(-1);
	edit->getCursorPosition(&row, &col);

	int mLine = edit->markerFindPrevious(row - 1 , 2);
	if ( mLine >= 0 ) {
		gotoLine(mLine);
	}
	else {
		mLine = edit->markerFindPrevious(lineCount() - 1, 2);
		if ( mLine >= 0 ) {
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
	if ( codec != 0 ) {
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

EolMode SciDoc::eolMode() const {
	if ( docInt_->edit1_->eolMode() == QsciScintilla::EolWindows ) {
		return EolWin;
	}
	else if ( docInt_->edit1_->eolMode() == QsciScintilla::EolMac ) {
		return EolMac;
	}
	else {
		return EolUnix;
	}
}

void SciDoc::setEolMode(EolMode eol) {
	switch ( eol ) {
		case EolWin: 
			docInt_->edit1_->convertEols(QsciScintilla::EolWindows);
			docInt_->edit1_->setEolMode(QsciScintilla::EolWindows); 
			break;
		
		case EolMac: 
			docInt_->edit1_->convertEols(QsciScintilla::EolMac); 
			docInt_->edit1_->setEolMode(QsciScintilla::EolMac); 
			break;
		
		case EolUnix: 
			docInt_->edit1_->convertEols(QsciScintilla::EolUnix); 
			docInt_->edit1_->setEolMode(QsciScintilla::EolUnix); 
			break;
	}
}



}

