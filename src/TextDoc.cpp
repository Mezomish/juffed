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

#include "TextDoc.h"

//	Qt headers
#include <QtCore/QDateTime>
#include <QtCore/QFile>
#include <QtCore/QTextCodec>
#include <QtCore/QTextStream>
#include <QtGui/QFileDialog>
#include <QtGui/QCheckBox>
#include <QtGui/QInputDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QTextDocument>

//	local headers
#include "Log.h"
#include "FindDlg.h"
#include "LexerStorage.h"
#include "MainSettings.h"
#include "TextDocSettings.h"
#include "TextDocView.h"
#include "types.h"

using namespace Juff;

class TextDocInterior {
public:
	TextDocInterior() {
		codec_ = QTextCodec::codecForLocale();
		charsetName_ = codec_->name();
		
		++count_;
	}
	~TextDocInterior() {

		--count_;
	}

	QTextCodec* codec_;
	QString charsetName_;
	QDateTime modDateTime_;
	
	//	tests
	static int count_;
};

//	Test
int TextDocInterior::count_ = 0;


TextDoc::TextDoc(const QString& fileName, DocView* view) : Document(fileName, view) {
	view->setDocument(this);
	
	docInt_ = new TextDocInterior();

	TextDocView* tdView = qobject_cast<TextDocView*>(view);
	if (tdView == 0) {
		Log::debug("TextDoc was initialized with a wrong view");
	}
	else {
		tdView->setLineNumVisible(TextDocSettings::showLineNumbers());
		tdView->setAdjustedByWidth(TextDocSettings::widthAdjust());
	}
}

TextDoc::~TextDoc() {
	JUFFDTOR;

	delete docInt_;
}

/**
 *	Converts document view to TextDocView and returns it if
 *	conversion is possible. Otherwise returns 0.
 */
TextDocView* TextDoc::textDocView() const {
	return qobject_cast<TextDocView*>(view());
}

QString TextDoc::getSaveFileName(bool& changeName) {
	QFileDialog saveDlg(view(), tr("Save as"), MainSettings::lastSaveDir());
	saveDlg.setConfirmOverwrite(true);
	saveDlg.setAcceptMode(QFileDialog::AcceptSave);

	QLayout* layout = saveDlg.layout();
	QCheckBox* saveAsCopyChk = new QCheckBox(tr("Save as a copy"));
	saveAsCopyChk->setChecked(!changeName);
	layout->addWidget(saveAsCopyChk);

	if (!isNoname()) {
		saveDlg.selectFile(QFileInfo(fileName()).fileName());
	}
	saveDlg.setLabelText(QFileDialog::Accept, tr("Save"));
		
	QString fName("");
	if (saveDlg.exec() == QDialog::Accepted) {
		fName = saveDlg.selectedFiles()[0];
		changeName = !saveAsCopyChk->isChecked();
	}
	
	return fName;
}

Document::Status TextDoc::save() {
	Document::Status res = Document::StatusSuccess;
	
	if (isNoname()) {
		res = saveAs();
	}
	else {
		QString name = fileName();
		if (MainSettings::makeBackupOnSave()) {
			QFile::remove(name + "~");
			QFile::copy(name, name + "~");
		}
		
		bool changeName(true);

		//	check for read-only
		if (QFile::exists(name) && !QFileInfo(name).isWritable()) {
			//	File exists and it is read-only for the current user.
			//	Ask for what to do with it.
			QString msg = tr("File '%1' is read-only.").arg(QFileInfo(name).fileName()) + "\n";
			msg += tr("What do you want to do?");
			QMessageBox msgBox(QMessageBox::Information, tr("Warning"), msg, QMessageBox::NoButton, view());
			QPushButton* owrBtn = msgBox.addButton(tr("Overwrite"), QMessageBox::YesRole);
			QPushButton* savBtn = msgBox.addButton(tr("Save as"), QMessageBox::ApplyRole);
			msgBox.addButton(QMessageBox::Cancel);
			bool resolved = false;
			do {
				msgBox.exec();
				QAbstractButton* btn = msgBox.clickedButton();
				if (btn == owrBtn) {
					//	Try to change permissions and save
					QFile::Permissions perm = QFile::permissions(name);
					if (QFile::setPermissions(name, perm | QFile::WriteUser)) {
						resolved = true;
					}
					else {
						//	Can't change permissions
						QMessageBox::warning(view(), tr("Warning"), tr("Can't change permissions: Access denied"));
					}
				}
				else if (btn == savBtn) {
					//	Choose file name
					QString fName = getSaveFileName(changeName);
					if (!fName.isEmpty()) {
						name = fName;
						resolved = true;
					}
				}
				else {
					return res;
				}
			} while (!resolved);
		}
		res = writeContent(name, changeName);
	}
	
	return res;
}

Document::Status TextDoc::saveAs() {
	Document::Status res = Document::StatusSuccess;

	bool changeName(true);
	QString fName = getSaveFileName(changeName);

	if (!fName.isEmpty()) {
		MainSettings::setLastSaveDir(QFileInfo(fName).absolutePath());
		res = writeContent(fName, changeName);
	}
	else {
		res = Document::StatusCancel;
	}
	
	return res;
}

Document::Status TextDoc::reload() {
	if (!isNoname()) {
		return readContent(fileName());
	}
	return Document::StatusSuccess;
}

void TextDoc::print() const {
	TextDocView* tdView = textDocView();
	if (tdView == 0)
		return;

	tdView->print();
}

void TextDoc::printSelected() const {
	TextDocView* tdView = textDocView();
	if (tdView == 0)
		return;

	tdView->printSelected();
}

Document::Status TextDoc::open() {
	return readContent(fileName());
}

Document::SaveRequest TextDoc::confirmForClose() {
	Document::SaveRequest res = Document::SaveYes;
	if (isModified()) {
		QString str = tr("The document ") + fileName();
		str += tr(" has been modified.\nDo you want to save your changes?");
		int ret = QMessageBox::warning(view(), tr("Close document"),
					str, QMessageBox::Save | QMessageBox::Discard
					| QMessageBox::Cancel, QMessageBox::Save);
		switch (ret) {
		case QMessageBox::Save:
			save();
			res = Document::SaveYes;
			break;
		case QMessageBox::Discard:
			res = Document::SaveNo;
			break;
		case QMessageBox::Cancel:
			res = Document::SaveCancel;
			break;
		}
	}

	return res;
}

void TextDoc::find(const QString& str, const DocFindFlags& flags) {
	TextDocView* tdView = textDocView();
	if (tdView == 0)
		return;
		
	if (str.isEmpty()) {
		FindDlg dlg(view());
		if (dlg.exec() == QDialog::Accepted) {
			QString text = dlg.text();
			DocFindFlags fl = dlg.flags();
			if (dlg.isReplaceMode()) {
				tdView->replace(text, dlg.replaceTo(), fl);
			}
			else {
				tdView->find(text, fl);
			}
		}
	}
	else {
		tdView->find(str, flags);
	}
}

void TextDoc::processTheCommand(CommandID id) {
	TextDocView* tdView = textDocView();
	if (tdView == 0) {
		Log::debug(QString("Wrong view in %1:%2").arg(__FILE__).arg(__LINE__));
		Log::debug("TextDoc was initialized with a wrong view");
		return;
	}
	
	switch (id) {

	case ID_EDIT_CUT :
		tdView->cut();
		break;
		
	case ID_EDIT_COPY :
		tdView->copy();
		break;
		
	case ID_EDIT_PASTE :
		tdView->paste();
		break;
		
	case ID_EDIT_UNDO :
		tdView->undo();
		break;
		
	case ID_EDIT_REDO :
		tdView->redo();
		break;
		
	case ID_FIND :
		find("", DocFindFlags());
		break;
		
	case ID_FIND_NEXT :
		find(FindDlg::lastText(), DocFindFlags(FindDlg::lastMatchCase(), false, FindDlg::lastRegExpMode()));
		break;

	case ID_FIND_PREV :
		find(FindDlg::lastText(), DocFindFlags(FindDlg::lastMatchCase(), true, FindDlg::lastRegExpMode()));
		break;

	case ID_GOTO_LINE : {
			bool ok = false;
			int line = QInputDialog::getInteger(tdView, tr("Go to line"), 
						tr("Go to line") + QString(" (1 - %1):").arg(tdView->lineCount()), 
						1, 1, tdView->lineCount(), 1, &ok);
			if (ok)
				tdView->gotoLine(line);
			break;
		}

	case ID_UNINDENT :
		tdView->unindentSelectedLines();
		break;
	
	case ID_VIEW_SHOW_LINE_NUMBERS :
		if (tdView != 0)
			tdView->setLineNumVisible(!tdView->lineNumIsVisible());
		break;
		
	case ID_VIEW_WIDTH_ADJUST :
		if (tdView != 0)
			tdView->setAdjustedByWidth(!tdView->isAdjustedByWidth());
		break;

	case ID_VIEW_SHOW_HIDDEN_SYMBOLS:
		tdView->showHiddenSymbols(!tdView->hiddenSymbolsVisible());
		break;

	default:
		Log::debug(QString("Unknown action ID: %1").arg(id));
		break;
	}
}



QString TextDoc::charset() const {
	return docInt_->charsetName_;
}

bool TextDoc::setCharset(const QString& charset) {
	QTextCodec* codec = QTextCodec::codecForName(charset.toAscii());
	if (codec != 0) {
		docInt_->codec_ = codec;
		docInt_->charsetName_ = charset;
		readContent(fileName());
		return true;
	}
	return false;
}

void TextDoc::applySettings() {
	view()->applySettings();
}



Document::Status TextDoc::readContent(const QString& name) {
	if (name.isEmpty())
		return Document::StatusWrongFileName;

	Document::Status res = Document::StatusSuccess;
	
	QFile file(name);
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QString text = "";
		QTextStream ts(&file);
		ts.setCodec(docInt_->codec_);
		while (!ts.atEnd()) {
			text += ts.readLine() + "\n";
		}
		setLastModified(QFileInfo(file).lastModified());
		file.close();

		TextDocView* tdView = qobject_cast<TextDocView*>(view());
		if (tdView == 0) {
			Log::debug("TextDoc was initialized with a wrong view");
			res = Document::StatusUnknownError;
		}
		else {
			bool mod = isModified();
			tdView->setText(text);
			tdView->setModified(mod);
			
			//	check for read-only
			if (!QFileInfo(name).isWritable() && tdView->isVisible()) {
				QMessageBox::information(tdView, tr("Warning"), tr("File '%1' is read-only.").arg(QFileInfo(name).fileName()));
			}
		}
	}
	else
		res = Document::StatusErrorOpenFile;

	return res;
}

Document::Status TextDoc::writeContent(const QString& fileName, bool changeName /*= true*/) {
	QString name(fileName);
	Document::Status res = Document::StatusSuccess;
	TextDocView* tdView = textDocView();

	QFile file(name);
	if (file.open(QIODevice::WriteOnly)) {
		QString text("");
		if (tdView != 0) {
			tdView->getText(text);
			file.write(docInt_->codec_->fromUnicode(text));
			file.close();

			tdView->setFocus();
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
			}
		}
		else {
			res = StatusUnknownError;
		}
	}
	else {
		res = StatusErrorSaveFile;
	}
	
	return res;
}



//	Test function
int TextDoc::count() {
	return TextDocInterior::count_;
}
