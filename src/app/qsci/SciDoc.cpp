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

#include <QDebug>

#include "SciDoc.h"

#include "AppInfo.h"
#include "AutocompleteSettings.h"
#include "Constants.h"
#include "JuffScintilla.h"
#include "Log.h"
#include "EditorSettings.h"
#include "LexerStorage.h"
#include "MainSettings.h"
#include "SearchResults.h"
#include "QSciSettings.h"
#include "settings/PrintSettings.h"

#include <QDir>
#include <QFile>
#include <QPainter>
#include <QPixmap>
#include <QPixmapCache>
#include <QPrintDialog>
#include <QScrollBar>
#include <QSplitter>
#include <QTextCodec>
#include <QTextStream>
#include <QVBoxLayout>

#include <Qsci/qsciprinter.h>
#include <Qsci/qscilexer.h>
#include <Qsci/qsciapis.h>

SciDoc::Eol guessEol(const QString& fileName) {
	SciDoc::Eol eol = SciDoc::EolUnix;
	if ( !Juff::Document::isNoname(fileName) ) {
		QFile file(fileName);
		if ( file.open(QIODevice::ReadOnly) ) {
			QString line = QString::fromLocal8Bit(file.readLine().constData());
			QRegExp re(".*(\r?\n?)");
			if ( re.exactMatch(line) ) {
				QString ending = re.cap(1);
				if ( ending == "\r\n" ) {
					eol = SciDoc::EolWin;
				}
				else if ( ending == "\r" ) {
					eol = SciDoc::EolMac;
				}
				else {
					eol = SciDoc::EolUnix;
				}
			}
			file.close();
		}
	}
	else {
#ifdef Q_OS_WIN32
		eol = SciDoc::EolWin;
#else
		eol = SciDoc::EolUnix;
#endif
	}
	return eol;
}

QPixmap markerPixmap(const QColor& color, const QColor& bgColor) {
	// create unique string-name for color combinations
	QString cacheName(color.name()+bgColor.name());
	QPixmap px(16, 16);

	// The method signature was changed: it's
	// bool QPixmapCache::find ( const QString & key, QPixmap & pm ) in Qt <= 4.5
	// and
	// bool QPixmapCache::find ( const QString & key, QPixmap * pm ) in Qt >= 4.6
#if QT_VERSION >= 0x040600
	if ( QPixmapCache::find(cacheName, &px) ) {
#else
	if ( QPixmapCache::find(cacheName, px) ) {
#endif
		return px;
	}

	px.fill(bgColor);
	
	QPainter p(&px);
	// As we are using pixmap cache for most pixmap access
	// we can use more resource and time consuming rendering
	// to get better results (smooth rounded bullet in this case).
	// Remember: painting is performed only once per running juffed
	//           in the ideal case.
	p.setRenderHint(QPainter::Antialiasing);

	int red   = color.red();
	int green = color.green();
	int blue  = color.blue();
	
	QColor light(red + (255 - red) / 2, green + (255 - green) / 2, blue + (255 - blue) / 2);
	QColor dark(red / 2, green / 2, blue / 2);
	
	QRadialGradient gr(0.4, 0.4, 0.5, 0.4, 0.4);
	gr.setCoordinateMode(QGradient::ObjectBoundingMode);
	gr.setColorAt(0, light);
	gr.setColorAt(1, dark);
	p.setPen(dark);
	p.setBrush(gr);
	p.drawEllipse(1, 1, 14, 14);

	p.end();

	QPixmapCache::insert(cacheName, px);
	return px;
}

class SciDoc::Interior {
public:
	Interior(QWidget* w) {
//		LOGGER;
		
		curEdit_ = NULL;
		
		spl_ = new QSplitter(Qt::Vertical);
		QVBoxLayout* vBox = new QVBoxLayout();
		vBox->setContentsMargins(0, 0, 0, 0);
		vBox->addWidget(spl_);
		w->setLayout(vBox);
		
		edit1_ = createEdit();
		edit2_ = createEdit();
		spl_->addWidget(edit1_);
		spl_->addWidget(edit2_);
		edit1_->setDocument(edit2_->document());
		w->setFocusProxy(spl_);
		spl_->setSizes(QList<int>() << 0 << spl_->height());
	}
	
	JuffScintilla* createEdit() {
//		LOGGER;
		JuffScintilla* edit = new JuffScintilla();
		edit->setFocusPolicy(Qt::ClickFocus);
		edit->setUtf8(true);
		edit->setFolding(QsciScintilla::BoxedTreeFoldStyle);
		edit->setAutoIndent(true);
		edit->setBraceMatching(QsciScintilla::SloppyBraceMatch);
		
		// margins
		edit->setMarginLineNumbers(0, false);
		edit->setMarginLineNumbers(1, true);
		edit->setMarginSensitivity(0, true);
		edit->setMarginWidth(0, 20);
		edit->setMarginWidth(2, 12);
		
		// markers
		edit->markerDefine(QsciScintilla::Background, 2);
		//	Set the 0th margin accept markers numbered 1 and 2
		//	Binary mask for markers 1 and 2 is 00000110 ( == 6 )
		edit->setMarginMarkerMask(0, 6);
		edit->setMarginMarkerMask(1, 0);

		return edit;
	}
	
	void setCurrentEdit(JuffScintilla* edit) {
//		LOGGER;
		
		curEdit_ = edit;
		spl_->setFocusProxy(edit);
	}
	
	JuffScintilla* edit1_;
	JuffScintilla* edit2_;
	JuffScintilla* curEdit_;
	QString syntax_;
	QSplitter* spl_;
	QTimer* hlTimer_;
};

SciDoc::SciDoc(const QString& fileName) : Juff::Document(fileName) {
//	LOGGER;
	
	int_ = new Interior(this);
	
	JuffScintilla* edits[] = { int_->edit1_, int_->edit2_ };
	for ( int i = 0; i < 2; ++i) {
		JuffScintilla* edit = edits[i];
		connect(edit, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(onCursorMoved(int, int)));
	//	connect(int_->edit1_, SIGNAL(contextMenuCalled(int, int)), this, SIGNAL(contextMenuCalled(int, int)));
		connect(edit, SIGNAL(marginClicked(int, int, Qt::KeyboardModifiers)), SLOT(onMarginClicked(int, int, Qt::KeyboardModifiers)));
		connect(edit, SIGNAL(focusReceived()), SLOT(onEditFocused()));
		connect(edit, SIGNAL(markersMenuRequested(const QPoint&)), SIGNAL(markersMenuRequested(const QPoint&)));
		connect(edit, SIGNAL(escapePressed()), SIGNAL(escapePressed()));
	}
	connect(int_->edit1_, SIGNAL(modificationChanged(bool)), this, SIGNAL(modified(bool)));
	connect(int_->edit1_, SIGNAL(linesChanged()), SLOT(onLineCountChanged()));
	connect(int_->edit1_, SIGNAL(textChanged()), this, SIGNAL(textChanged()));
	
	QString lexName = "none";
	SciDoc::Eol eol = guessEol(fileName);
	if ( !fileName.isEmpty() && !isNoname() ) {
		QString codecName = Document::guessCharset(fileName);
		if ( !codecName.isEmpty() )
			setCharset(codecName);
		readFile();
		setEol(eol);
		int_->edit1_->setModified(false);

		//	syntax highlighting
		lexName = LexerStorage::instance()->lexerName(fileName);
	}
	else
		setEol(eol);

	setLexer(lexName);
	
	applySettings();
	
	QAction* hlWordAct = new QAction("", this);
	hlWordAct->setShortcut(QKeySequence("Ctrl+H"));
	connect(hlWordAct, SIGNAL(triggered()), SLOT(highlightWord()));
	addAction(hlWordAct);
	
	int_->hlTimer_ = new QTimer();
	connect(int_->hlTimer_, SIGNAL(timeout()), SLOT(highlightWord()));
	int_->hlTimer_->setSingleShot(true);
}

/*SciDoc::SciDoc(Juff::Document* doc) : Juff::Document(doc) {
	SciDoc* d = qobject_cast<SciDoc*>(doc);
	if ( d != 0 ) {
		int_->edit1_->setDocument(d->int_->edit1_->document());
		int_->edit2_->setDocument(d->int_->edit2_->document());
	}
}*/

SciDoc::~SciDoc() {
	delete int_;
}

QString SciDoc::type() const {
	return "QSci";
}

bool SciDoc::supportsAction(Juff::ActionID id) const {
	switch (id) {
		case Juff::FileClone : return true;
		default :              return Juff::Document::supportsAction(id);
	}
}

/*Juff::Document* SciDoc::createClone() {
	if ( hasClone() )
		return NULL;
	else
		return new SciDoc(this);
}

void SciDoc::updateClone() {
	LOGGER;
	
//	SciDoc* cln = qobject_cast<SciDoc*>(clone());
//	if ( cln != 0 ) {
//		if ( cln->int_->syntax_ != int_->syntax_ ) {
//			cln->int_->syntax_ = int_->syntax_;
//			QsciLexer* lexer = LexerStorage::instance()->lexer(int_->syntax_);
//			cln->int_->edit1_->setLexer(lexer);
//			cln->int_->edit2_->setLexer(lexer);
//		}
//	}
	
	Juff::Document::updateClone();
}*/

void SciDoc::init() {
	int_->setCurrentEdit(int_->edit2_);
}

void SciDoc::reload() {
	if ( !isNoname() ) {
		int line, col;
		getCursorPos(line, col);
		int scroll = scrollPos();
		readFile();
		setModified(false);
		if ( line >= 0 && col >= 0 ) {
			setCursorPos(line, col);
			setScrollPos(scroll);
		}
	}
}

void SciDoc::print() {
	QsciPrinter prn;
	QPrintDialog dlg(&prn, this);
	if (dlg.exec() == QDialog::Accepted) {
		prn.setWrapMode(EditorSettings::get(EditorSettings::WrapWords) || PrintSettings::get(PrintSettings::AlwaysWrap) ? QsciScintilla::WrapWord : QsciScintilla::WrapNone);
		
		int line1(-1), line2(-1), col1(-1), col2(-1);
		JuffScintilla* edit = int_->curEdit_;
		if ( edit ) {
			QsciLexer* lexer = edit->lexer();
			if ( !PrintSettings::get(PrintSettings::KeepBgColor) ) {
				lexer->setDefaultPaper(Qt::white);
				lexer->setPaper(Qt::white);
				lexer->setDefaultColor(Qt::black);
			}
			if ( !PrintSettings::get(PrintSettings::KeepColors) ) {
				lexer->setColor(Qt::black);
			}
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
			QFont font = EditorSettings::font();
			LexerStorage::instance()->updateLexers(font);
		}
	}
}



////////////////////////////////////////////////////////////////////////////////
// Document API implementation

int SciDoc::lineCount() const {
	return int_->edit1_->lines();
}

bool SciDoc::isModified() const {
	return int_->edit1_->isModified();
}

bool SciDoc::hasSelectedText() const {
	if ( int_->curEdit_ == NULL ) return false;
	
	return int_->curEdit_->hasSelectedText();
}

bool SciDoc::getSelection(int& line1, int& col1, int& line2, int& col2) const {
	if ( int_->curEdit_ == NULL ) return false;
	
	int_->curEdit_->getSelection(&line1, &col1, &line2, &col2);
	return true;
}

bool SciDoc::getSelectedText(QString& text) const {
	if ( int_->curEdit_ == NULL ) return false;
	
	text = int_->curEdit_->selectedText();
	return true;
}

bool SciDoc::getText(QString& text) const {
	if ( int_->curEdit_ == NULL ) return false;
	
	text = int_->curEdit_->text();
	return true;
}

void SciDoc::setText(const QString& text) {
	if ( int_->curEdit_ == NULL ) return;
	
	int_->curEdit_->beginUndoAction();
	int_->curEdit_->selectAll();
	replaceSelectedText(text, false);
	int_->curEdit_->endUndoAction();
}

bool SciDoc::getTextLine(int line, QString& str) const {
	if ( int_->curEdit_ == NULL ) return false;
	
	if ( line >=0 && line < lineCount() ) {
		str = int_->curEdit_->text(line);
		return true;
	}
	else {
		return false;
	}
}

bool SciDoc::getCursorPos(int& line, int& col) const {
	if ( int_->curEdit_ == NULL ) return false;

	int_->curEdit_->getCursorPosition(&line, &col);
	return true;
}

QString SciDoc::syntax() const {
	return int_->syntax_;
}

void SciDoc::setModified(bool modified) {
	int_->edit1_->setModified(modified);
}

void SciDoc::setSelection(int line1, int col1, int line2, int col2) {
	if ( int_->curEdit_ == NULL ) return;
	
	int_->curEdit_->setSelection(line1, col1, line2, col2);
	int_->curEdit_->ensureCursorVisible();
}

void SciDoc::removeSelectedText() {
	if ( int_->curEdit_ == NULL ) return;
	
	int_->curEdit_->removeSelectedText();
}

void SciDoc::replaceSelectedText(const QString& text, bool cursorToTheEnd) {
	if ( int_->curEdit_ == NULL ) return;
	
	int line1, col1, line2, col2;
	int_->curEdit_->getSelection(&line1, &col1, &line2, &col2);
	
	int_->curEdit_->beginUndoAction();
	
	// hack! hack! hack!
	disconnect(int_->edit1_, SIGNAL(textChanged()), this, SIGNAL(textChanged()));
	removeSelectedText();
	// hack! hack! hack!
	connect(int_->edit1_, SIGNAL(textChanged()), this, SIGNAL(textChanged()));
	
	if ( text.isEmpty() ) {
		// need to emit the signal manually
		emit textChanged();
	}
	else {
		insertText(text);
	}
	
	if ( cursorToTheEnd ) {
		int lineEndsCount = text.count(LineSeparatorRx);
		if ( lineEndsCount == 0 ) {
			int_->curEdit_->setCursorPosition(line1, col1 + text.length());
		}
		else {
			QStringList insertedLines = text.split(LineSeparatorRx);
			QString lastLine = insertedLines[insertedLines.count() - 1];
			int_->curEdit_->setCursorPosition(line1 + lineEndsCount, lastLine.length());
		}
	}
	else {
		int_->curEdit_->setCursorPosition(line1, col1);
	}
	int_->curEdit_->endUndoAction();
}

void SciDoc::insertText(const QString& text) {
	if ( int_->curEdit_ == NULL ) return;
	
	int row, col;
	getCursorPos(row, col);
	int newLines = text.count(LineSeparatorRx);
	int_->curEdit_->insert(text);
	if ( newLines == 0 )
		setCursorPos(row, col + text.length());
}

void SciDoc::setCursorPos(int line, int col) {
	if ( int_->curEdit_ == NULL ) return;
	
	int_->curEdit_->setCursorPosition(line, col);
	int_->curEdit_->setFocus();
}

void SciDoc::setSyntax(const QString& lexName) {
//	LOGGER;

	if ( lexName.isEmpty() )
		return;
	
	QString oldSyntax = int_->syntax_;
	setLexer(lexName);
//	updateClone();
	
	// notify plugins
	emit syntaxChanged(oldSyntax);
}
// End of Document API implementation
////////////////////////////////////////////////////////////////////////////////

int SciDoc::scrollPos() const {
	if ( int_->curEdit_ == NULL ) return 0;
	return int_->curEdit_->verticalScrollBar()->value();
}

void SciDoc::setScrollPos(int pos) {
	if ( int_->curEdit_ == NULL ) return;
	int_->curEdit_->verticalScrollBar()->setValue(pos);
}



void SciDoc::undo() {
//	LOGGER;
	if ( int_->curEdit_ != NULL )
		int_->curEdit_->undo();
}

void SciDoc::redo() {
	if ( int_->curEdit_ != NULL )
		int_->curEdit_->redo();
}

void SciDoc::cut() {
	if ( int_->curEdit_ != NULL )
		int_->curEdit_->cut();
}

void SciDoc::copy() {
	if ( int_->curEdit_ != NULL )
		int_->curEdit_->copy();
}

void SciDoc::paste() {
	if ( int_->curEdit_ != NULL )
		int_->curEdit_->paste();
}

void SciDoc::gotoLine(int line) {
	if ( int_->curEdit_ == NULL ) return;
	
	int lCount = lineCount();
	int_->curEdit_->ensureLineVisible( line >= 10 ? line-10 : 0 );
	int_->curEdit_->ensureLineVisible( line >= lCount - 10 ? lCount : line+10 );
	
	int_->curEdit_->setCursorPosition(line, 0);
	int_->curEdit_->setFocus();
}

void SciDoc::setWrapWords(bool wrap) {
//	LOGGER;
	
	if ( wrap ) {
		int_->edit1_->setWrapMode(QsciScintilla::WrapWord);
		int_->edit2_->setWrapMode(QsciScintilla::WrapWord);
	}
	else {
		int_->edit1_->setWrapMode(QsciScintilla::WrapNone);
		int_->edit2_->setWrapMode(QsciScintilla::WrapNone);
	}
}

void SciDoc::setShowLineNumbers(bool show) {
//	LOGGER;
	
	int_->edit1_->showLineNumbers(show);
	int_->edit2_->showLineNumbers(show);
}

void SciDoc::setShowWhitespaces(bool show) {
//	LOGGER;
	
	int_->edit1_->setWhitespaceVisibility(show ? QsciScintilla::WsVisible : QsciScintilla::WsInvisible);
	int_->edit2_->setWhitespaceVisibility(show ? QsciScintilla::WsVisible : QsciScintilla::WsInvisible);
	int_->edit1_->setWrapVisualFlags(show ? QsciScintilla::WrapFlagByBorder : QsciScintilla::WrapFlagNone);
	int_->edit2_->setWrapVisualFlags(show ? QsciScintilla::WrapFlagByBorder : QsciScintilla::WrapFlagNone);
	EditorSettings::set(EditorSettings::ShowWhitespaces, show);
}

void SciDoc::setShowLineEndings(bool show) {
//	LOGGER;
	
	int_->edit1_->setEolVisibility(show);
	int_->edit2_->setEolVisibility(show);
}

bool SciDoc::wrapWords() const {
	return int_->edit1_->wrapMode() == QsciScintilla::WrapWord;
}

bool SciDoc::lineNumbersVisible() const {
	return int_->edit1_->lineNumbersVisible();
}

bool SciDoc::whitespacesVisible() const {
	return int_->edit1_->whitespaceVisibility() == QsciScintilla::WsVisible;
}

bool SciDoc::lineEndingsVisible() const {
	return int_->edit1_->eolVisibility();
}

void SciDoc::zoomIn() {
	LOGGER;
	
	int_->edit1_->zoomIn();
	int_->edit2_->zoomIn();
}

void SciDoc::zoomOut() {
	LOGGER;
	
	int_->edit1_->zoomOut();
	int_->edit2_->zoomOut();
}

void SciDoc::zoom100() {
	LOGGER;
	
	int_->edit1_->zoomTo(0);
	int_->edit2_->zoomTo(0);
}

void SciDoc::toUpperCase() {
	if ( int_->curEdit_ != NULL ) {
		int_->curEdit_->SendScintilla(QsciScintilla::SCI_UPPERCASE);
	}
}

void SciDoc::toLowerCase() {
	if ( int_->curEdit_ != NULL ) {
		int_->curEdit_->SendScintilla(QsciScintilla::SCI_LOWERCASE);
	}
}

void SciDoc::swapLines() {
	if ( int_->curEdit_ != NULL ) {
		int_->curEdit_->SendScintilla(QsciScintilla::SCI_LINETRANSPOSE);
	}
}

void SciDoc::moveUp() {
	if ( int_->curEdit_ == NULL ) return;
	
	if ( hasSelectedText() ) {
		int line1, line2, col1, col2;
		getSelection(line1, col1, line2, col2);
		
		if ( line1 == 0 )
			return;
		
		int realLine2 = line2;
		if ( col2 == 0 )
			--line2;
		
		int_->curEdit_->beginUndoAction();
		for (int line = line1; line <= line2; ++line) {
			int_->curEdit_->setCursorPosition(line, 0);
			swapLines();
		}
		
		setSelection(line1 - 1, col1, realLine2 - 1, col2);
		int_->curEdit_->endUndoAction();
	}
	else {
		int line, col;
		int_->curEdit_->getCursorPosition(&line, &col);
		if ( line > 0 ) {
			swapLines();
			int_->curEdit_->setCursorPosition(line - 1, col);
		}
	}
}

void SciDoc::moveDown() {
	if ( int_->curEdit_ == NULL ) return;
	
	if ( hasSelectedText() ) {
		int line1, line2, col1, col2;
		getSelection(line1, col1, line2, col2);
		
		int realLine2 = line2;
		if ( col2 == 0 )
			--line2;
		
		if ( line2 == lineCount() - 1 )
			return;
		
		int_->curEdit_->beginUndoAction();
		for (int line = line2 + 1; line >= line1 + 1; --line) {
			int_->curEdit_->setCursorPosition(line, 0);
			swapLines();
		}
		
		setSelection(line1 + 1, col1, realLine2 + 1, col2);
		int_->curEdit_->endUndoAction();
	}
	else {
		int line, col;
		int_->curEdit_->getCursorPosition(&line, &col);
		if ( line < lineCount() - 1 ) {
			int_->curEdit_->setCursorPosition(line + 1, 0);
			swapLines();
			int_->curEdit_->setCursorPosition(line + 1, col);
		}
	}
}

void SciDoc::toggleCommentLines() {
	LOGGER;
	
	JuffScintilla* edit = int_->curEdit_;
	if ( edit == NULL ) return;

	QString comment;
	QString s = syntax();
	if ( s == "C++" || s == "PHP" || s == "C#" || s == "Java" || s == "JavaScript" )
		comment = "//";
	else if ( s == "Bash" || s == "Python" || s == "CMake" || s == "Makefile" || s == "Perl")
		comment = "#";
	else if ( s == "Fortran" )
		comment = "!";
	else if ( s == "SQL")
		comment = "--";
	else if ( s == "Qore" || s == "Qorus")
		comment = "#";
	//	TODO : need to add more syntaxes

	if ( comment.isEmpty() )
		return;

	if ( edit->hasSelectedText() ) {
		int line1, col1, line2, col2, curLine, curCol;
		edit->getSelection(&line1, &col1, &line2, &col2);
		edit->getCursorPosition(&curLine, &curCol);

		QString str1 = edit->text(line1);
		QString ln = str1.simplified();
		bool toComment = true;
		if ( ln.startsWith(comment) ) {
			toComment = false;
		}

		if ( col2 == 0 )
			--line2;

		edit->beginUndoAction();
		for ( int line = line1; line <= line2; ++line ) {
			str1 = edit->text(line);
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
		QString str1 = edit->text(line1);

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

void SciDoc::toggleCommentBlock() {
	LOGGER;
	
	JuffScintilla* edit = int_->curEdit_;
	if ( edit == NULL ) return;

	QString commBeg, commEnd;
	QString s = syntax();
	if ( s == "C++" || s == "Java" || s == "C#" || s == "PHP" || s == "CSS" || s == "JavaScript"
	     || s == "SQL") {
		commBeg = "/*";
		commEnd = "*/";
	}
	else if ( s == "HTML" || s == "XML" ) {
		commBeg = "<!--";
		commEnd = "-->";
	}
	else if ( s == "Python" ) {
		commBeg = "'''";
		commEnd = "'''";
	}
	else if ( s == "Qore" || s == "Qorus") {
		commBeg = "/*";
		commEnd = "*/";
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

void SciDoc::duplicateText() {
	if ( int_->curEdit_ == NULL ) return;
	
	int_->curEdit_->beginUndoAction();
	if ( int_->curEdit_->hasSelectedText() ) {
		int col1, row1, col2, row2;
		getSelection(col1, row1, col2, row2);
		int_->curEdit_->SendScintilla(QsciScintilla::SCI_SELECTIONDUPLICATE);
		setSelection(col1, row1, col2, row2);
	}
	else {
		int_->curEdit_->SendScintilla(QsciScintilla::SCI_LINEDUPLICATE);
	}
	int_->curEdit_->endUndoAction();
}

void SciDoc::unindent() {
	LOGGER;
	
	JuffScintilla* edit = int_->curEdit_;
	if ( edit == NULL ) return;
	
	int line1(-1), line2(-1), col1(-1), col2(-1);
	if ( edit->hasSelectedText() ) {
		edit->getSelection(&line1, &col1, &line2, &col2);
		if ( col2 == 0 )
			--line2;
		
		if (line1 <= line2 && line1 >= 0) {
			edit->beginUndoAction();
			for (int l = line1; l <= line2; ++l) {
				edit->unindent(l);
			}
			edit->endUndoAction();
		}
		edit->setSelection(line1, 0, line2 + 1, 0);
	}
	else {
		edit->getCursorPosition(&line1, &col1);
		if ( line1 >= 0 ) {
			edit->unindent(line1);
		}
	}
}

void SciDoc::insertTab() {
	LOGGER;
	
	JuffScintilla* edit = int_->curEdit_;
	if ( edit == NULL ) return;
	
	edit->insert("\t");
}

void SciDoc::removeLine() {
	LOGGER;
	
	JuffScintilla* edit = int_->curEdit_;
	if ( edit == NULL ) return;
	
	int line1(-1), line2(-1), col1(-1), col2(-1);
	if ( edit->hasSelectedText() ) {
		edit->getSelection(&line1, &col1, &line2, &col2);
		if ( col2 == 0 )
			--line2;
		
		if (line1 <= line2 && line1 >= 0) {
			setCursorPos(line1, 0);
			edit->beginUndoAction();
			for (int l = line1; l <= line2; ++l) {
				edit->SendScintilla(QsciScintilla::SCI_LINEDELETE);
			}
			edit->endUndoAction();
		}
	}
	else {
		edit->getCursorPosition(&line1, &col1);
		if ( line1 >= 0 ) {
			edit->SendScintilla(QsciScintilla::SCI_LINEDELETE);
		}
	}
}

void SciDoc::removeLineLeft() {
	JuffScintilla* edit = int_->curEdit_;
	if ( edit == NULL ) return;
	edit->SendScintilla(QsciScintilla::SCI_DELLINELEFT);
}

void SciDoc::removeLineRight() {
	JuffScintilla* edit = int_->curEdit_;
	if ( edit == NULL ) return;
	edit->SendScintilla(QsciScintilla::SCI_DELLINERIGHT);
}

void SciDoc::foldUnfoldAll() {
	JuffScintilla* edit = int_->curEdit_;
	if ( edit == NULL ) return;
	edit->foldAll(true);
}

void SciDoc::highlightWord() {
	LOGGER;
	
	JuffScintilla* edit = int_->curEdit_;
	if ( edit == NULL ) return;
	
	if ( edit->hasSelectedText() )
		return;
	
	QString word = edit->wordUnderCursor();
	edit->highlightText(JuffScintilla::HLCurrentWord, Juff::SearchParams());
}

void SciDoc::highlightSearchResults() {
	Juff::SearchResults* results = searchResults();
	if ( results == NULL )
		return;
	
	// keep cursor position or selection for edit1
	bool ed1_hasSelection = int_->edit1_->hasSelectedText();
	int ed1_r1, ed1_c1, ed1_r2, ed1_c2;
	int ed1_scrollPos = int_->edit1_->verticalScrollBar()->value();
	if ( ed1_hasSelection ) {
		int_->edit1_->getSelection(&ed1_r1, &ed1_c1, &ed1_r2, &ed1_c2);
	}
	else {
		int_->edit1_->getCursorPosition(&ed1_r1, &ed1_c1);
	}
	
	// keep cursor position or selection for edit2
	bool ed2_hasSelection = int_->edit2_->hasSelectedText();
	int ed2_r1, ed2_c1, ed2_r2, ed2_c2;
	int ed2_scrollPos = int_->edit2_->verticalScrollBar()->value();
	if ( ed2_hasSelection ) {
		int_->edit2_->getSelection(&ed2_r1, &ed2_c1, &ed2_r2, &ed2_c2);
	}
	else {
		int_->edit2_->getCursorPosition(&ed2_r1, &ed2_c1);
	}

	int count = results->count();
	for ( int i = 0; i < count; i++ ) {
		const Juff::SearchOccurence& occ = results->occurence(i);
		int_->edit1_->highlight(JuffScintilla::HLSearch, occ.startRow, occ.startCol, occ.endRow, occ.endCol);
		int_->edit2_->highlight(JuffScintilla::HLSearch, occ.startRow, occ.startCol, occ.endRow, occ.endCol);
	}
	
	// restore cursor position or selection for edit1
	if ( ed1_hasSelection ) {
		int_->edit1_->setSelection(ed1_r1, ed1_c1, ed1_r2, ed1_c2);
	}
	else {
		int_->edit1_->setCursorPosition(ed1_r1, ed1_c1);
	}
	int_->edit1_->verticalScrollBar()->setValue(ed1_scrollPos);
	
	// restore cursor position or selection for edit2
	if ( ed2_hasSelection ) {
		int_->edit2_->setSelection(ed2_r1, ed2_c1, ed2_r2, ed2_c2);
	}
	else {
		int_->edit2_->setCursorPosition(ed2_r1, ed2_c1);
	}
	int_->edit2_->verticalScrollBar()->setValue(ed2_scrollPos);
}

void SciDoc::clearHighlighting() {
//	LOGGER;
	int_->edit1_->highlightText(JuffScintilla::HLSearch, Juff::SearchParams());
	int_->edit2_->highlightText(JuffScintilla::HLSearch, Juff::SearchParams());
}


void SciDoc::readFile() {
//	LOGGER;

	QString text;
	QFile file(fileName());
	if ( file.open(QIODevice::ReadOnly) ) {
		
//		if ( !keepCharset ) {
//			QString codecName = Document::guessCharset(fileName());
//			QTextCodec* c = QTextCodec::codecForName(codecName.toAscii());
//			if ( c ) {
//				setCodec(c);
//				setCharset(codecName);
//			}
//		}
		
		QTextStream ts(&file);
		ts.setCodec(codec());
		int_->edit1_->setText(ts.readAll());
	}
}

bool SciDoc::save(QString& error) {
	LOGGER;

	if ( isNoname() ) {
		error = "This is a Noname file and shouldn't be saved directly";
		return false;
	}

	if ( MainSettings::get(MainSettings::StripTrailingSpaces) )
		stripTrailingSpaces();

	bool result;
	stopWatcher();
	QFile file(fileName());
	if ( file.open(QIODevice::WriteOnly) ) {
		QString text("");
		text = int_->edit1_->text();
		file.write(codec()->fromUnicode(text));
		file.close();
//		Document::save(error);
		int_->edit1_->setModified(false);
		result = true;
	}
	else {
		error = tr("Can't open file for writing");
		result = false;
	}
	startWatcher();
	
	return result;
}

bool SciDoc::saveAs(const QString& fileName, QString& error) {
	bool result = Document::saveAs(fileName, error);
	if ( result ) {
		QString lexName = LexerStorage::instance()->lexerName(this->fileName());
		setLexer(lexName);
	}
	return result;
}

void SciDoc::setLexer(const QString& lexName) {
//	LOGGER;

	if ( lexName.isEmpty() )
		return;
	
	int_->syntax_ = lexName;
	QsciLexer* lexer = LexerStorage::instance()->lexer(lexName);
	loadAutocompletionAPI(lexName, lexer);
	int_->edit1_->setLexer(lexer);
	int_->edit2_->setLexer(lexer);
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
			// HACK: hardcoded dependencies
			if (lexName == "Qorus") {
				apis->load(dir.absoluteFilePath("qore.api"));
			}
			apis->prepare();
			lexer->setAPIs(apis);
		}
		else {
			delete apis;
		}
	}
}

//void SciDoc::showLineNumbers(bool show) {
//	int_->edit1_->showLineNumbers(show);
//	int_->edit2_->showLineNumbers(show);
//}
SciDoc::Eol SciDoc::eol() const {
	switch ( int_->curEdit_->eolMode() ) {
		case QsciScintilla::EolWindows :
			return EolWin;
		case QsciScintilla::EolMac:
			return EolMac;
		default :
			return EolUnix;
	}
}

void SciDoc::setEol(SciDoc::Eol eol) {
	switch ( eol ) {
		case EolWin :
			int_->edit1_->setEolMode(QsciScintilla::EolWindows);
			int_->edit2_->setEolMode(QsciScintilla::EolWindows);
			int_->edit1_->convertEols(QsciScintilla::EolWindows);
			int_->edit2_->convertEols(QsciScintilla::EolWindows);
			break;
		
		case EolUnix :
			int_->edit1_->setEolMode(QsciScintilla::EolUnix);
			int_->edit2_->setEolMode(QsciScintilla::EolUnix);
			int_->edit1_->convertEols(QsciScintilla::EolUnix);
			int_->edit2_->convertEols(QsciScintilla::EolUnix);
			break;
		
		case EolMac :
			int_->edit1_->setEolMode(QsciScintilla::EolMac);
			int_->edit2_->setEolMode(QsciScintilla::EolMac);
			int_->edit1_->convertEols(QsciScintilla::EolMac);
			int_->edit2_->convertEols(QsciScintilla::EolMac);
			break;
	}
}

void SciDoc::toggleMarker(int line) {
	QsciScintilla* edit = int_->curEdit_;
	if ( edit == NULL )
		return;
	
	if ( edit->markersAtLine(line) & 2 ) {
		edit->markerDelete(line, 1);
		edit->markerDelete(line, 2);
	}
	else {
		edit->markerAdd(line, 1);
		edit->markerAdd(line, 2);
	}
}

void SciDoc::removeAllMarkers() {
	int_->edit1_->markerDeleteAll();
	int_->edit2_->markerDeleteAll();
}

QList<int> SciDoc::markers() const {
	QList<int> markers;
	int line = 0;
	while ( (line = int_->edit1_->markerFindNext(line, 2)) >= 0 ) {
		markers << line++;
	}
	return markers;
}



void SciDoc::applySettings() {
//	LOGGER;
	
	setShowLineNumbers(EditorSettings::get(EditorSettings::ShowLineNumbers));
	
	QFont font = EditorSettings::font();
	LexerStorage::instance()->updateLexers(font);
	QColor textColor = EditorSettings::get(EditorSettings::DefaultFontColor);
	QColor bgColor = EditorSettings::get(EditorSettings::DefaultBgColor);
	
	QsciScintilla* edits[] = { int_->edit1_, int_->edit2_, NULL };
	for (int i = 0; edits[i] != NULL; ++i ) {
		QsciScintilla* edit = edits[i];
		
		// indents
		edit->setTabWidth(EditorSettings::get(EditorSettings::TabWidth));
		edit->setIndentationsUseTabs(EditorSettings::get(EditorSettings::UseTabs));
		edit->setBackspaceUnindents(EditorSettings::get(EditorSettings::BackspaceUnindents));
		
		// colors
		edit->setIndentationGuides(QSciSettings::get(QSciSettings::ShowIndents));
		edit->setIndentationGuidesForegroundColor(QSciSettings::get(QSciSettings::IndentsColor));
		edit->setIndentationGuidesBackgroundColor(bgColor);
		
		QColor selBgColor = EditorSettings::get(EditorSettings::SelectionBgColor);
		edit->setSelectionBackgroundColor(selBgColor);
		if ( selBgColor.red() + selBgColor.green() + selBgColor.blue() < 3 * 255 / 2)
			edit->setSelectionForegroundColor(QColor(255, 255, 255));
		else
			edit->setSelectionForegroundColor(QColor(0, 0, 0));
		
		if ( QSciSettings::get(QSciSettings::HighlightMatchingBrace) ) {
			edit->setMatchedBraceBackgroundColor(QSciSettings::get(QSciSettings::MatchingBraceBgColor));
			edit->setMatchedBraceForegroundColor(QSciSettings::get(QSciSettings::MatchingBraceFgColor));
		}
		else {
			edit->setMatchedBraceBackgroundColor(bgColor);
			edit->setMatchedBraceForegroundColor(textColor);
		}
		
		edit->setCaretLineBackgroundColor(LexerStorage::instance()->curLineColor(syntax()));
		edit->setMarkerBackgroundColor(QSciSettings::get(QSciSettings::MarkersColor));
		edit->setCaretForegroundColor(textColor);
		
		QColor marginsBgColor = QSciSettings::get(QSciSettings::MarginsBgColor);
		edit->setMarginsBackgroundColor(marginsBgColor);
		edit->setMarginsForegroundColor(textColor);
		edit->setFoldMarginColors(marginsBgColor, bgColor);
		
		// markers
		edit->markerDefine(markerPixmap(QSciSettings::get(QSciSettings::MarkersColor), marginsBgColor), -1);
		edit->setCaretLineVisible(QSciSettings::get(QSciSettings::HighlightCurLine));
		
		
		// line length indicator
		int lInd = EditorSettings::get(EditorSettings::LineLengthIndicator);
		if ( lInd > 0 ) {
			edit->setEdgeMode(QsciScintilla::EdgeLine);
			edit->setEdgeColumn(lInd);
		}
		else {
			edit->setEdgeMode(QsciScintilla::EdgeNone);
		}
		
		// selection
/*		QColor selBgColor = TextDocSettings::selectionBgColor();
		edit->setSelectionBackgroundColor(selBgColor);
		if ( selBgColor.red() + selBgColor.green() + selBgColor.blue() < 3 * 255 / 2)
			edit->setSelectionForegroundColor(QColor(255, 255, 255));
		else
			edit->setSelectionForegroundColor(QColor(0, 0, 0));
*/
		//	autocompletion
		edit->setAutoCompletionThreshold(AutocompleteSettings::get(AutocompleteSettings::Threshold));
		edit->setAutoCompletionReplaceWord(AutocompleteSettings::get(AutocompleteSettings::ReplaceWord));
//		edit->setAutoCompletionCaseSensitivity(AutocompleteSettings::get(AutocompleteSettings::CaseSensitive));
		if ( AutocompleteSettings::get(AutocompleteSettings::UseDocument) ) {
			if ( AutocompleteSettings::get(AutocompleteSettings::UseApis) )
				edit->setAutoCompletionSource(QsciScintilla::AcsAll);
			else
				edit->setAutoCompletionSource(QsciScintilla::AcsDocument);
		}
		else {
			if ( AutocompleteSettings::get(AutocompleteSettings::UseApis) )
				edit->setAutoCompletionSource(QsciScintilla::AcsAPIs);
			else
				edit->setAutoCompletionSource(QsciScintilla::AcsNone);
		}
		edit->setAutoCompletionCaseSensitivity(false);
	}
}



////////////////////////////////////////////////////////////////////////////////
// SLOTS

void SciDoc::onCursorMoved(int line, int col) {
	if ( int_->hlTimer_->isActive() )
		int_->hlTimer_->stop();
	if ( QSciSettings::get(QSciSettings::HighlightCurWord) )
		int_->hlTimer_->start(500);
	emit cursorPosChanged(line, col);
}

void SciDoc::onMarginClicked(int, int line, Qt::KeyboardModifiers) {
	toggleMarker(line);
}

void SciDoc::onLineCountChanged() {
	emit lineCountChanged(lineCount());
}

void SciDoc::onEditFocused() {
	LOGGER;

	if ( sender() == int_->edit1_ ) {
		int_->setCurrentEdit(int_->edit1_);
	}
	else {
		int_->setCurrentEdit(int_->edit2_);
	}
	emit focused();
}


////////////////////////////////////////////////////////////////////////////////
// Helper functions
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

void SciDoc::stripTrailingSpaces() {
	LOGGER;
	if ( int_->curEdit_ == NULL ) return;

	int line, col;
	getCursorPos(line, col);
	QString text = int_->curEdit_->text();
	QStringList lines = text.split(LineSeparatorRx);
	QRegExp rx("[ \t]+$");
	int i = 0;
	foreach (QString str, lines) {
		int pos = str.indexOf(rx);
		if ( pos >= 0 ) {
			int_->curEdit_->setSelection(i, 0, i, str.length());
			str.truncate(pos);
			replaceSelectedText(str);
		}
		++i;
	}
	setCursorPos(line, col);
}


void SciDoc::setSessionParams(const Juff::SessionParams& params) {
	Juff::SessionParams::const_iterator it = params.begin();
	while ( it != params.end() ) {
		QString key = it.key();
		QString value = it.value();
		if ( key.compare("scrollPos") == 0 ) {
			setScrollPos(value.toInt());
		}
		else if ( key.compare("cursorPos") == 0 ) {
			int row = value.section(';', 0, 0).toInt();
			int col = value.section(';', 1, 1).toInt();
			setCursorPos(row, col);
		}
		it++;
	}
}

Juff::SessionParams SciDoc::sessionParams() const {
	Juff::SessionParams params = Juff::Document::sessionParams();
	
	int row = -1, col = -1;
	getCursorPos(row, col);
	params["scrollPos"] = QString("%1").arg(scrollPos());
	params["cursorPos"] = QString("%1;%2").arg(row).arg(col);
	
	return params;
}
