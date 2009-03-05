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

bool RichDoc::save(const QString&, QString&) {
	return true;
}

void RichDoc::print() { 
}

void RichDoc::reload() { 
}

}	//	namespace Juff
