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

#include "MW.h"

#include "Log.h"
#include "MainSettings.h"

#include <QtCore/QUrl>
#include <QtGui/QCloseEvent>

namespace Juff {
namespace GUI {

MW::MW() : QMainWindow() {
	setAcceptDrops(true);
	setGeometry(MainSettings::windowRect());
}

void MW::closeEvent(QCloseEvent* e) {
	JUFFENTRY;
	
	bool confirmed = true;
	emit closeRequested(confirmed);
	if ( confirmed ) {
		MainSettings::setWindowRect(geometry());
		e->accept();
	}
	else {
		e->ignore();
	}
}

///////////////////////////////////////////////////////////////////////
//	Drag & Drop
///////////////////////////////////////////////////////////////////////

void MW::dragEnterEvent(QDragEnterEvent* e) {
	JUFFENTRY;

	if (e->mimeData()->hasUrls()) {
		e->acceptProposedAction();
	}
}

void MW::dropEvent(QDropEvent* e) {
	JUFFENTRY;

	if ( e->mimeData()->hasUrls() ) {
		QList<QUrl> urls = e->mimeData()->urls();
		foreach (QUrl url, urls) {
			QString name = url.path();

#ifdef Q_OS_WIN32			
			//	hack to protect of strings with filenames like /C:/doc/file.txt
			if ( name[0] == '/' )
				name.remove(0, 1);
#endif

			if ( !name.isEmpty() ) {
				emit docOpenRequested(name);
			}
		}
	}
}

}	//	namespace GUI
}	//	namespace Juff
