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

#include "NullDoc.h"

namespace Juff {

Juff::Document* NullDoc::doc_ = NULL;

Juff::Document* NullDoc::instance() {
	if (doc_ == NULL) {
		doc_ = new NullDoc();
	}
	return doc_;
}
	
NullDoc::~NullDoc() {
}

bool NullDoc::save(const QString&, QString&) { 
	scream();
	return false;
}

void NullDoc::print() { 
	scream();
}

void NullDoc::reload() { 
	scream();
}

bool NullDoc::isModified() const {
	scream();
	return false;
}

void NullDoc::setModified(bool) {
	scream();
}

QWidget* NullDoc::widget() {
	scream();
	return NULL;
}

NullDoc::NullDoc() : Juff::Document("") {
}
	
void NullDoc::scream() const { 
	Log::debug("Empty document was called."); 
}

}	//	namespace Juff
