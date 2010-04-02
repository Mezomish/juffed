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

#ifndef __JUFF_DOC_MANAGER_H__
#define __JUFF_DOC_MANAGER_H__

class JuffMW;
class QMenu;
class QStatusBar;

#include "Enums.h"

#include <QMap>
#include <QString>

namespace Juff {
	class Document;
	class DocHandlerInt;
}
class DocEngine;

class DocManager {
public:
	DocManager(Juff::DocHandlerInt*);

	Juff::Document* newDoc(const QString& type = "");
	Juff::Document* openDoc(const QString& fileName, const QString& type = "");

	void initMenuActions(Juff::MenuID, QMenu*);
	void initStatusBar(QStatusBar*);
	void setCurDocType(const QString&);

private:
	void initEngines();

	QMap<QString, DocEngine*> engines_;
	Juff::DocHandlerInt* handler_;
};

#endif // __JUFF_DOC_MANAGER_H__
