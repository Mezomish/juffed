#include <QDebug>

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

#include "SciDoc.h"

#include "AppInfo.h"
#include "AutocompleteSettings.h"
#include "CommandStorage.h"
#include "Functions.h"
#include "Log.h"
#include "LexerStorage.h"
#include "MainSettings.h"
#include "JuffScintilla.h"
#include "TextDocSettings.h"

#include <qsciapis.h>
#include <qscilexer.h>
//#include <qscimacro.h>
#include <qsciprinter.h>
#include <qsciscintilla.h>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QTextCodec>
#include <QtCore/QTextStream>
#include <QtGui/QMenu>
#include <QtGui/QMessageBox>
#include <QtGui/QPrintDialog>
#include <QtGui/QScrollBar>
#include <QtGui/QSplitter>

#include <stdio.h>

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
	}


	~Interior() {
		delete spl_;
	}

	JuffScintilla* createEdit() {
		JuffScintilla* edit = new JuffScintilla();
		edit->setUtf8(true);
		edit->setIndentationGuidesForegroundColor(TextDocSettings::defaultFontColor());
		edit->setIndentationGuidesBackgroundColor(TextDocSettings::defaultBgColor());
		edit->setFolding(QsciScintilla::BoxedTreeFoldStyle);
		edit->setAutoIndent(true);
		edit->setBraceMatching(QsciScintilla::SloppyBraceMatch);
		edit->setMatchedBraceBackgroundColor(TextDocSettings::matchedBraceBgColor());

		edit->setMarginLineNumbers(1, true);
		edit->setMarginSensitivity(1, true);
		edit->setMarginWidth(2, 12);
		//	set the 1st margin accept markers 
		//	number 1 and 2 (binary mask 00000110 == 6)
		edit->setMarginMarkerMask(1, 6);
		edit->markerDefine(QsciScintilla::RightTriangle, 1);
		edit->markerDefine(QsciScintilla::Background, 2);
		edit->setMarkerForegroundColor(QColor(100, 100, 100));
		edit->setMarkerBackgroundColor(TextDocSettings::markersColor());
//		edit->setMarginsBackgroundColor(QColor(50, 50, 50));
//		edit->setMarginsForegroundColor(QColor(150, 150, 150));
//		edit->setFoldMarginColors(QColor(150, 150, 150), QColor(50, 50, 50));

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
	showInvisibleSymbols(TextDocSettings::showInvisibleSymbols());

	if ( !fileName.isEmpty() && !isNoname(fileName) ) {
		qDebug() << "               Reading doc";
		readDoc();
		docInt_->edit1_->setModified(false);

		//	syntax highlighting
		QString lexName = LexerStorage::instance()->lexerName(fileName);
		qDebug() << "               Syntax highlighting:" << lexName;
		setSyntax(lexName);
		
		qDebug() << "               Guessing line endings";
		
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
		qDebug() << "               Done with all that";
	}
	else {
		setSyntax("none");
	}

	connect(docInt_->edit1_, SIGNAL(modificationChanged(bool)), this, SIGNAL(modified(bool)));
	connect(docInt_->edit1_, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(onCursorMove(int, int)));
	connect(docInt_->edit2_, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(onCursorMove(int, int)));
	connect(docInt_->edit1_, SIGNAL(contextMenuCalled(int, int)), this, SIGNAL(contextMenuCalled(int, int)));
	connect(docInt_->edit2_, SIGNAL(contextMenuCalled(int, int)), this, SIGNAL(contextMenuCalled(int, int)));
	connect(docInt_->edit1_, SIGNAL(marginClicked(int, int, Qt::KeyboardModifiers)), SLOT(onMarginClicked(int, int, Qt::KeyboardModifiers)));
	connect(docInt_->edit2_, SIGNAL(marginClicked(int, int, Qt::KeyboardModifiers)), SLOT(onMarginClicked(int, int, Qt::KeyboardModifiers)));
	connect(docInt_->edit1_, SIGNAL(linesChanged()), SLOT(onLinesCountChanged()));

	qDebug() << "               Applying settings...";
	applySettings();
	qDebug() << "               ....done";
	
	QAction* unindentAct = new QAction(this);
	unindentAct->setShortcut(QKeySequence("Shift+Tab"));
	connect(unindentAct, SIGNAL(activated()), this, SLOT(unindent()));
	widget()->addAction(unindentAct);
}

void SciDoc::init() {
	docInt_->spl_->setFocusProxy(docInt_->edit2_);
	docInt_->edit2_->setFocus();
}

SciDoc::~SciDoc() {
	delete docInt_;
}

void SciDoc::setFileName(const QString& fileName) {
	Document::setFileName(fileName);
	QString lexName = LexerStorage::instance()->lexerName(fileName);
	if ( lexName != docInt_->syntax_ ) {
		setSyntax(lexName);
	}
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

	if ( str.isEmpty() )
		return;

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

	if ( str1.isEmpty() )
		return;

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
	JUFFENTRY2;
	
	docInt_->edit1_->showLineNumbers(show);
	docInt_->edit2_->showLineNumbers(show);
}

void SciDoc::wrapText(bool wrap) {
	JUFFENTRY2;
	
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

void SciDoc::showInvisibleSymbols(bool show) {
	docInt_->edit1_->setWhitespaceVisibility(show ? QsciScintilla::WsVisible : QsciScintilla::WsInvisible);
	docInt_->edit1_->setWrapVisualFlags(show ? QsciScintilla::WrapFlagByBorder : QsciScintilla::WrapFlagNone);
	docInt_->edit2_->setWhitespaceVisibility(show ? QsciScintilla::WsVisible : QsciScintilla::WsInvisible);
	docInt_->edit2_->setWrapVisualFlags(show ? QsciScintilla::WrapFlagByBorder : QsciScintilla::WrapFlagNone);
	TextDocSettings::setShowInvisibleSymbols(show);
}




void SciDoc::stripTrailingSpaces() {
	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return;

	int line, col;
	getCursorPos(line, col);
	QString text = edit->text();
	QStringList lines = text.split(QRegExp("\r\n|\r|\n"));
	QRegExp rx("[ \t]+$");
	int i = 0;
	foreach (QString str, lines) {
		int pos = str.indexOf(rx);
		if ( pos >= 0 ) {
			edit->setSelection(i, 0, i, str.length());
			str.truncate(pos);
			replaceSelectedText(str);
		}
		++i;
	}
	setCursorPos(line, col);
}

bool SciDoc::save(const QString& fileName, const QString& charset, QString& error) {
	JUFFENTRY;

	if ( MainSettings::stripTrailingSpaces() )
		stripTrailingSpaces();

	QFile file(fileName);
	if ( file.open(QIODevice::WriteOnly) ) {
		QString text("");
		text = docInt_->edit1_->text();
		QTextCodec* codec = QTextCodec::codecForName(charset.toAscii());
		if ( !charset.isEmpty() && codec ) {
			file.write(codec->fromUnicode(text));
		}
		else {
			file.write(docInt_->codec_->fromUnicode(text));
		}
		file.close();
		Document::save(fileName, charset, error);
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

void SciDoc::readDoc(bool keepCharset /* = false*/) {
	QString text;
	QFile file(fileName());
	if ( file.open(QIODevice::ReadOnly) ) {
		
		if ( !keepCharset ) {
			QString codecName = guessCharset();
			QTextCodec* codec = QTextCodec::codecForName(codecName.toAscii());
			if ( codec ) {
				docInt_->codec_ = codec;
				docInt_->charsetName_ = codecName;
			}
		}
		
		QTextStream ts(&file);
		ts.setCodec(docInt_->codec_);
		docInt_->edit1_->setText(ts.readAll());
	}
}

void SciDoc::applySettings() {
	JUFFENTRY2;
	
	QFont font = TextDocSettings::font();
	LexerStorage::instance()->updateLexers(font);
	showLineNumbers(TextDocSettings::showLineNumbers());

	QsciScintilla* edits[] = { docInt_->edit1_, docInt_->edit2_ };
	for (int i = 0; i < 2; ++i ) {
		QsciScintilla* edit = edits[i];


		edit->setTabWidth(TextDocSettings::tabStopWidth());
		edit->setIndentationsUseTabs(!TextDocSettings::replaceTabsWithSpaces());

		int lInd = TextDocSettings::lineLengthIndicator();
		if ( lInd > 0 ) {
			edit->setEdgeMode(QsciScintilla::EdgeLine);
			edit->setEdgeColumn(lInd);
		}
		else {
			edit->setEdgeMode(QsciScintilla::EdgeNone);
		}
		
		edit->setCaretLineVisible(TextDocSettings::highlightCurrentLine());
		edit->setCaretLineBackgroundColor(LexerStorage::instance()->curLineColor(docInt_->syntax_));
		edit->setIndentationGuides(TextDocSettings::showIndents());
		edit->setBackspaceUnindents(TextDocSettings::backspaceUnindents());
		edit->setMarkerBackgroundColor(TextDocSettings::markersColor());
		if ( QsciLexer* lexer = edit->lexer() ) {
			edit->setCaretForegroundColor(lexer->defaultColor());
			edit->setIndentationGuidesForegroundColor(TextDocSettings::indentsColor());
			edit->setIndentationGuidesBackgroundColor(lexer->defaultPaper());
		}
		edit->setMatchedBraceBackgroundColor(TextDocSettings::matchedBraceBgColor());
		edit->setSelectionBackgroundColor(TextDocSettings::selectionBgColor());

		//	autocompletion
		edit->setAutoCompletionThreshold(AutocompleteSettings::threshold());
		edit->setAutoCompletionReplaceWord(AutocompleteSettings::replaceWord());
		edit->setAutoCompletionCaseSensitivity(AutocompleteSettings::caseSensitive());
		if ( AutocompleteSettings::useDocument() ) {
			if ( AutocompleteSettings::useApis() )
				edit->setAutoCompletionSource(QsciScintilla::AcsAll);
			else
				edit->setAutoCompletionSource(QsciScintilla::AcsDocument);
		}
		else {
			if ( AutocompleteSettings::useApis() )
				edit->setAutoCompletionSource(QsciScintilla::AcsAPIs);
			else
				edit->setAutoCompletionSource(QsciScintilla::AcsNone);
		}
	}
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

void SciDoc::onCursorMove(int line, int col) {
	JuffScintilla* edit = getActiveEdit();
	if ( edit == sender() )
		emit cursorPositionChanged(line, col);
}


void SciDoc::gotoLine(int line) {
	JUFFENTRY2;

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

QString SciDoc::text(int line) const { 
	return docInt_->edit1_->text(line); 
}

QString SciDoc::wordUnderCursor() const {
	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return QString();
	
	return edit->wordUnderCursor(); 
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
	
	int row, col;
	getCursorPos(row, col);
	int newLines = text.count(QRegExp("\r\n|\n|\r"));
	edit->insert(text);
	if ( newLines == 0 )
		setCursorPos(row, col + text.length());
}

void SciDoc::removeSelectedText() {
	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return;
	
	edit->removeSelectedText();
}

void SciDoc::replaceSelectedText(const QString& text) {
	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return;

	edit->beginUndoAction();
	removeSelectedText();
	insertText(text);
	edit->endUndoAction();
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


void SciDoc::onMarginClicked (int margin, int line, Qt::KeyboardModifiers state) {
	JUFFENTRY;
	
	if ( margin == 1 ) {
		//	margin that contains line numbers
		toggleMarker(line);
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

void SciDoc::toggleMarker(int line) {
	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return;

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

void SciDoc::toggleMarker() {
	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return;

	int line, col;
	edit->getCursorPosition(&line, &col);
	toggleMarker(line);
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
		readDoc(true);
	}
}

QString SciDoc::syntax() const {
	return docInt_->syntax_;
}

void SciDoc::setSyntax(const QString& lexName) {
	qDebug() << "Syntax:" << lexName;
	if ( lexName.isEmpty() )
		return;

	docInt_->syntax_ = lexName;

	qDebug() << "               Getting the lexer";
	QsciLexer* lexer = LexerStorage::instance()->lexer(lexName);
	printf("Lexer returned\n   -- Lexer pointer:%p\n", lexer);

	qDebug() << "               Loading autocompletion";
	loadAutocompletionAPI(lexName, lexer);
	printf("Autocomplete loaded\n   -- Lexer pointer:%p\n", lexer);
	
	qDebug() << "               Setting the lexer";
	docInt_->edit1_->setLexer(lexer);
	printf("Lexer set to edit1\n   -- Lexer pointer:%p\n", lexer);
	docInt_->edit2_->setLexer(lexer);
	printf("Lexer set to edit2\n   -- Lexer pointer:%p\n", lexer);
}

void SciDoc::loadAutocompletionAPI(const QString& lexName, QsciLexer* lexer) {
	if ( NULL == lexer )
		return;
	
	QDir dir(AppInfo::configDirPath() + "/apis");
	QString fileName = lexName.toLower() + ".api";
	fileName.replace(QString("+"), "plus").replace(QString("#"), "sharp");
	if ( dir.entryList(QDir::Files).contains(fileName) ) {
		QsciAPIs* apis = new QsciAPIs(lexer);
		if ( apis->load(dir.absoluteFilePath(fileName)) ) {
			apis->prepare();
			lexer->setAPIs(apis);
		}
		else {
			delete apis;
		}
	}
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

/*QsciMacro* SciDoc::newMacro() {
	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return 0;
	
	return new QsciMacro(edit);
}

void SciDoc::runMacro(const QString& mcr) {
	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return;

	QsciMacro macro(edit);
	if ( macro.load(mcr) ) {
		macro.play();
	}
}*/

void SciDoc::goToMatchingBrace() {
	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return;
	
	edit->moveToMatchingBrace();
}

void SciDoc::selectToMatchingBrace() {
	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return;
	
	edit->selectToMatchingBrace();
}

void SciDoc::commentLine(JuffScintilla* edit, int line, const QString& str1, const QString& comment) {
	QString str2 = comment + str1;
	edit->setSelection(line, 0, line + 1, 0);
	replaceSelectedText(str2);
}

void SciDoc::uncommentLine(JuffScintilla* edit, int line, const QString& str1, const QString& comment) {
	int pos = str1.indexOf(comment);
	QString str2 = str1;
	str2.replace(pos, comment.length(), "");
	edit->setSelection(line, 0, line + 1, 0);
	replaceSelectedText(str2);
}

void SciDoc::toggleLineComment() {
	JUFFENTRY;

	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return;

	QString comment;
	QString& s = docInt_->syntax_;
	if ( s == "C++" || s == "PHP" || s == "C#" || s == "Java" || s == "JavaScript" )
		comment = "//";
	else if ( s == "Bash" || s == "Python" || s == "CMake" || s == "Makefile" )
		comment = "#";
	else if ( s == "Fortran" )
		comment = "!";
	//	TODO : need to add more syntaxes

	if ( comment.isEmpty() )
		return;

	if ( edit->hasSelectedText() ) {
		int line1, col1, line2, col2, curLine, curCol;
		edit->getSelection(&line1, &col1, &line2, &col2);
		edit->getCursorPosition(&curLine, &curCol);

		QString str1 = text(line1);
		QString ln = str1.simplified();
		bool toComment = true;
		if ( ln.startsWith(comment) ) {
			toComment = false;
		}

		if ( col2 == 0 )
			--line2;

		edit->beginUndoAction();
		for ( int line = line1; line <= line2; ++line ) {
			str1 = text(line);
			if ( toComment ) {
				if ( !str1.simplified().startsWith(comment) )
					commentLine(edit, line, str1, comment);
			}
			else {
				if ( str1.simplified().startsWith(comment) )
					uncommentLine(edit, line, str1, comment);
			}
		}
		edit->endUndoAction();
		if ( curCol > 0 )
			edit->setCursorPosition(curLine, curCol + comment.length() * (toComment ? 1 : -1) );
		else
			edit->setCursorPosition(curLine, curCol);
	}
	else {
		int line1, col1;
		edit->getCursorPosition(&line1, &col1);
		QString str1 = text(line1);

		QString ln = str1.simplified();
		if ( ln.startsWith(comment) ) {
			uncommentLine(edit, line1, str1, comment);
			edit->setCursorPosition(line1, col1 - comment.length());
		}
		else {
			commentLine(edit, line1, str1, comment);
			edit->setCursorPosition(line1, col1 + comment.length());
		}
	}
}

void SciDoc::toggleBlockComment() {
	JUFFENTRY;

	JuffScintilla* edit = getActiveEdit();
	if ( !edit )
		return;

	QString commBeg, commEnd;
	QString& s = docInt_->syntax_;
	if ( s == "C++" || s == "Java" || s == "C#" || s == "PHP" || s == "CSS" || s == "JavaScript" ) {
		commBeg = "/*";
		commEnd = "*/";
	}
	else if ( docInt_->syntax_ == "HTML" || docInt_->syntax_ == "XML" ) {
		commBeg = "<!--";
		commEnd = "-->";
	}
	else if ( docInt_->syntax_ == "Python" ) {
		commBeg = "'''";
		commEnd = "'''";
	}
	//	TODO : need to add more syntaxes

	if ( commBeg.isEmpty() || commEnd.isEmpty() )
		return;

	if ( edit->hasSelectedText() ) {
		int line1, col1, line2, col2, curLine, curCol;
		edit->getSelection(&line1, &col1, &line2, &col2);
		edit->getCursorPosition(&curLine, &curCol);
		
		QString str1 = edit->selectedText();
		bool toComment = true;
		if ( str1.startsWith(commBeg) && str1.endsWith(commEnd) )
			toComment = false;

		QString str2;
		if ( toComment )
			str2 = commBeg + str1 + commEnd;
		else {
			str2 = str1;
			str2.chop(commEnd.length());
			str2.remove(0, commBeg.length());
		}
		replaceSelectedText(str2);
		if ( line1 == line2 ) {
			if ( curCol == col1 )
				edit->setCursorPosition(curLine, curCol);
			else
				edit->setCursorPosition(curLine, curCol + (commBeg.length() + commEnd.length()) * (toComment ? 1 : -1));
		}
		else {
			if ( curLine == line2 && curCol == col2)
				edit->setCursorPosition(curLine, curCol + commEnd.length() * (toComment ? 1 : -1) );
			else
				edit->setCursorPosition(curLine, curCol);
		}
	}
}

void SciDoc::duplicateLine() {
	JUFFENTRY;
}

void SciDoc::moveLineUp() {
	JUFFENTRY;
}

void SciDoc::changeSplitOrientation() {
	JUFFENTRY;
	
	if ( docInt_->spl_->orientation() == Qt::Vertical ) {
		docInt_->spl_->setOrientation(Qt::Horizontal);
	}
	else {
		docInt_->spl_->setOrientation(Qt::Vertical);
	}
}

void SciDoc::onLinesCountChanged() {
	emit linesCountChanged(lineCount());
}

}

