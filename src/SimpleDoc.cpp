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

#include "SimpleDoc.h"

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>

#include "Functions.h"
#include "Log.h"

namespace Juff {

class SimpleDoc::Interior {
public:
	Interior() {
		w_ = new QWidget();
		ed_ = new QTextEdit();
		ed_->setFont(QFont("Monospace", 8));
		ed_->setTabStopWidth(QFontMetrics(w_->font()).width(" ") * 4);
		QVBoxLayout* vb = new QVBoxLayout();
		vb->addWidget(ed_);
		w_->setLayout(vb);
		w_->setFocusProxy(ed_);
	}
	~Interior() {
		delete w_;
	}
	
	QWidget* w_;
	QTextEdit* ed_;
};

SimpleDoc::SimpleDoc(const QString& fileName) : Document(fileName) {
	docInt_ = new Interior();
	if ( !isNoname(fileName) ) {
		readDoc();
		docInt_->ed_->document()->setModified(false);
	}
	
	connect(docInt_->ed_->document(), SIGNAL(modificationChanged(bool)), this, SIGNAL(modified(bool)));
}

SimpleDoc::~SimpleDoc() {
	delete docInt_;
}

QWidget* SimpleDoc::widget() {
	return docInt_->w_;
}

bool SimpleDoc::isModified() const {
	return docInt_->ed_->document()->isModified();
}

void SimpleDoc::setModified(bool mod) {
	docInt_->ed_->document()->setModified(mod);
}

void SimpleDoc::readDoc() {
	QString text;
	QFile file(fileName());
	if ( file.open(QIODevice::ReadOnly) ) {
		QTextStream ts(&file);
//		ts.setCodec(docInt_->codec_);	TODO :
		docInt_->ed_->setPlainText(ts.readAll());
	}
}

bool SimpleDoc::save(const QString& fileName, QString& error) {
	JUFFENTRY;
	
	QString name(fileName);

	Log::debug(fileName);
	
	QFile file(name);
	if (file.open(QIODevice::WriteOnly)) {
		QString text("");
//		if (tdView != 0) {
//			tdView->getText(text);
			text = docInt_->ed_->toPlainText();
//			file.write(docInt_->codec_->fromUnicode(text));
			file.write(text.toUtf8());
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

}	//	namespace Juff
