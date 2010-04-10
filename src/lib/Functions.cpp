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

#include "Functions.h"

#include "Document.h"

#include <QFileInfo>

namespace Juff {

bool isNoname(const QString& fileName) {
	return fileName.left(6) == "Noname";
}

bool isNoname(Juff::Document* doc) {
	return isNoname(doc->fileName());
}

QString docTitle(const QString& fileName, bool modified) {
	if ( isNoname(fileName) ) {
		return QString(modified ? "*" : "") + QObject::tr("Noname %1").arg(fileName.section(' ', 1, 1).toInt());
	}
	else {
		return QString(modified ? "*" : "") + QFileInfo(fileName).fileName();
	}
}

QString docTitle(Juff::Document* doc) {
	return docTitle(doc->fileName(), doc->isModified());
}

QIcon docIcon(Juff::Document* doc) {
	return QIcon( (doc->isModified() ? ":doc_icon_red" : ":doc_icon") );
}

}
