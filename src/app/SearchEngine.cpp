#include "SearchEngine.h"

#include "DocHandlerInt.h"
#include "Document.h"
#include "JuffMW.h"
#include "Log.h"

SearchEngine::SearchEngine(Juff::DocHandlerInt* handler, JuffMW* mw) : QObject() {
	handler_ = handler;
	mw_ = mw;
	
	connect(mw_, SIGNAL(searchRequested(const Juff::SearchParams&)), SLOT(onSearchRequested(const Juff::SearchParams&)));
}

void SearchEngine::find(Juff::Document*) {
	mw_->showFindDialog();
}

void SearchEngine::findNext(Juff::Document*) {
}

void SearchEngine::findPrev(Juff::Document*) {
}


void SearchEngine::onSearchRequested(const Juff::SearchParams& params) {
	LOGGER;
	
	Juff::Document* doc = handler_->curDoc();
	if ( doc->isNull() ) return;
	
	QString text;
	doc->getText(text);
	
	int startLine, startCol;
	bool searchSteppedOver_ = false;
	doc->getCursorPos(startLine, startCol);
//	docInt_->searchStartingScroll_ = edit->verticalScrollBar()->value();
	
//	startFind(edit, str, flags);
	startFind(doc, params);
}

bool SearchEngine::startFind(Juff::Document* doc, const Juff::SearchParams& params) {
	if ( doc->isNull() ) return false;
//	doc->find(params);
	
	
	
	
	QString text;
	doc->getText(text);
	
//	if ( params.multiLine )
//		return findML(s, flags);

	QString str = params.findWhat;
	QStringList lines = text.split(QRegExp("\r\n|\n|\r"));
	int row(-1), col(-1);
	doc->getCursorPos(row, col);
	int lineIndex(0);
	if (row < 0 || col < 0)
		return false;

	if ( !params.backwards ) {
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
				if ( params.wholeWords ) {
					regExp = QRegExp(QString("\\b%1\\b").arg(QRegExp::escape(str)));
				}
				else
					regExp = QRegExp(str);
				regExp.setCaseSensitivity(params.caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive);
				if ( params.regExp || params.wholeWords ) {
					index = line.indexOf(regExp);
				}
				else {
					if ( !params.caseSensitive ) {
						str = str.toLower();
						line = line.toLower();
					}
					index = line.indexOf(str);
				}

				if ( index >= 0 ) {
					if ( params.regExp ) {
						doc->setSelection(lineIndex, index + indent, lineIndex, index + indent + regExp.matchedLength());
//						this->ensureCursorVisible();
					}
					else {
						doc->setSelection(lineIndex, index + indent, lineIndex, index + indent + str.length());
//						this->ensureCursorVisible();
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
				if ( params.wholeWords )
					regExp = QRegExp(QString("\\b%1\\b").arg(QRegExp::escape(str)));
				else
					regExp = QRegExp(str);
				regExp.setCaseSensitivity(params.caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive);
				if ( params.regExp || params.wholeWords )
					index = line.lastIndexOf(regExp);
				else {
					if ( !params.caseSensitive ) {
						str = str.toLower();
						line = line.toLower();
					}
					index = line.lastIndexOf(str);
				}

				if ( index >= 0 ) {
					if ( params.regExp ) {
						doc->setSelection(lineIndex, index, lineIndex, index + regExp.matchedLength());
//						this->ensureCursorVisible();
					}
					else {
						doc->setSelection(lineIndex, index, lineIndex, index + str.length());
//						this->ensureCursorVisible();
					}
					return true;
				}
			}
			lineIndex--;
			it--;
		}
	}

	return false;
	
	
	
	
	
	
	
	
	
	
	
	
//	prepareForFind(edit, str, flags);

//	bool found = edit->find(str, flags);
//	if ( !found ) {
		//	not found
//		if ( !docInt_->searchSteppedOver_ ) {
//			docInt_->searchSteppedOver_ = true;
//			stepOver(edit, flags.backwards);
//			startFind(edit, str, flags);
//		}
//		else {
//			edit->setCursorPosition(docInt_->searchStartingLine_, docInt_->searchStartingCol_);
//			edit->verticalScrollBar()->setValue(docInt_->searchStartingScroll_);
//			QMessageBox::information(edit, tr("Information"), tr("Text '%1' was not found").arg(str));
//			return;
//		}
//	}
}

