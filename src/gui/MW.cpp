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

#include <QtGui/QCloseEvent>

namespace Juff {
namespace GUI {

MW::MW() : QMainWindow() {
}

void MW::closeEvent(QCloseEvent* e) {
	JUFFENTRY;
	
	bool confirmed = true;
	emit closeRequested(confirmed);
	if ( confirmed )
		e->accept();
	else
		e->ignore();
}

}	//	namespace GUI
}	//	namespace Juff
