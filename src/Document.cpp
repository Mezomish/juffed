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

#include "Document.h"

#include "Log.h"

namespace Juff {
//namespace Data {

Document::Document(const QString& fName) : QObject() {
	if ( fName.isEmpty() ) {
		static int i = 0;
		fileName_ = QString("Noname %1").arg(i++);
	}
	else {
		fileName_ = fName;
	}
}

Document::~Document() {
}

bool Document::isNull() const {
	return false;
}

QString Document::fileName() const {
	return fileName_;
}

void Document::setFileName(const QString& fileName) {
	JUFFENTRY;
	
	QString oldFileName = fileName_;
	
	Log::debug(fileName);

	if ( fileName_ != fileName ) {
		fileName_ = fileName;
		emit fileNameChanged(oldFileName);
	}
}

QString Document::type() const {
	return type_;
}

/*void Document::emitActivated() {
	emit activated();
}*/

//}	//	namespace Data
}	//	namespace Juff
