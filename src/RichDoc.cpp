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

#include "RichDoc.h"

#include <QtCore/QFile>
#include <QtGui/QTextEdit>

namespace Juff {

class RichDoc::Interior {
public:
	Interior() {
		w_ = new QTextEdit();
	}
	~Interior() {
		delete w_;
	}
	
	QTextEdit* w_;
};

RichDoc::RichDoc(const QString& fileName) : Document(fileName) {
	docInt_ = new Interior();
	
	connect(docInt_->w_->document(), SIGNAL(modificationChanged(bool)), this, SIGNAL(modified(bool)));
}

RichDoc::~RichDoc() {
	delete docInt_;
}

QWidget* RichDoc::widget() {
	return docInt_->w_;
}

bool RichDoc::isModified() const {
	return docInt_->w_->document()->isModified();
}

void RichDoc::setModified(bool) {
}

bool RichDoc::save(const QString& fileName, QString& error) {
	JUFFENTRY;
	
	QFile file(fileName);
	if ( file.open(QIODevice::WriteOnly) ) {
		QString text("");
		text = docInt_->w_->toHtml();
		file.write(text.toUtf8());
		file.close();
		Document::save(fileName, error);
		return true;
	}
	else {
		error = tr("Can't open file for writing");
		return false;
	}
}

void RichDoc::print() { 
}

void RichDoc::reload() { 
}

void RichDoc::bold() {
	JUFFENTRY;
	QTextCursor cur = docInt_->w_->textCursor();
	QTextCharFormat fmt = cur.charFormat();
	QTextCharFormat boldFmt;
	
	int pos = cur.position();
	int start = cur.selectionStart();
	int end = cur.selectionEnd();

	//	If the cursor at the beginning of the selection, then
	//	'fmt' contains the format of the previous symbol, but we 
	//	don't need it since we don't change it. Get the format of the
	//	last symbol of the selection.
	if ( pos == start ) {
		cur.setPosition(end);
		fmt = cur.charFormat();
		cur.setPosition(start, QTextCursor::KeepAnchor);
	}
	
	if ( fmt.fontWeight() == QFont::Bold ) {
		boldFmt.setFontWeight(QFont::Normal);
	}
	else {
		boldFmt.setFontWeight(QFont::Bold);
	}
	cur.mergeCharFormat(boldFmt);
}

void RichDoc::italic() {
	JUFFENTRY;
	QTextCursor cur = docInt_->w_->textCursor();
	QTextCharFormat fmt = cur.charFormat();
	QTextCharFormat italicFmt;
	
	int pos = cur.position();
	int start = cur.selectionStart();
	int end = cur.selectionEnd();
	
	//	If the cursor at the beginning of the selection, then
	//	'fmt' contains the format of the previous symbol, but we 
	//	don't need it since we don't change it. Get the format of the
	//	last symbol of the selection.
	if ( pos == start ) {
		cur.setPosition(end);
		fmt = cur.charFormat();
		cur.setPosition(start, QTextCursor::KeepAnchor);
	}
	
	if ( fmt.fontItalic() ) {
		italicFmt.setFontItalic(false);
	}
	else {
		italicFmt.setFontItalic(true);
	}
	cur.mergeCharFormat(italicFmt);
}

void RichDoc::underline() {
	JUFFENTRY;
	QTextCursor cur = docInt_->w_->textCursor();
	QTextCharFormat fmt = cur.charFormat();
	QTextCharFormat ulFmt;
	
	int pos = cur.position();
	int start = cur.selectionStart();
	int end = cur.selectionEnd();
	
	//	If the cursor at the beginning of the selection, then
	//	'fmt' contains the format of the previous symbol, but we 
	//	don't need it since we don't change it. Get the format of the
	//	last symbol of the selection.
	if ( pos == start ) {
		cur.setPosition(end);
		fmt = cur.charFormat();
		cur.setPosition(start, QTextCursor::KeepAnchor);
	}
	
	if ( fmt.underlineStyle() == QTextCharFormat::NoUnderline ) {
		ulFmt.setUnderlineStyle(QTextCharFormat::SingleUnderline);
	}
	else {
		ulFmt.setUnderlineStyle(QTextCharFormat::NoUnderline);
	}
	cur.mergeCharFormat(ulFmt);
}

}	//	namespace Juff
