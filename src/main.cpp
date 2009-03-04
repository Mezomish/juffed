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

#include <QtGui/QApplication>

#include "gui/GUI.h"
#include "Manager.h"
//#include "DocHandler.h"
#include "Settings.h"

class JuffApp : public QApplication {
public:
	JuffApp(int& argc, char** argv) : QApplication(argc, argv) {
		setOrganizationName("juff");
		setApplicationName("juffed");
		Settings::read();
	}
	
	virtual ~JuffApp() {
		Settings::write();
	}
};

int main(int argc, char* argv[])
{
	JuffApp app(argc, argv);
		
	Juff::GUI::GUI gui;
	Juff::Manager manager(&gui);
//	Juff::DocHandler handler;
	
//	handler.setGUI(&gui);
	gui.show();
	
	return app.exec();
}
