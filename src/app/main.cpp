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

#include "../3rd_party/qtsingleapplication/qtsingleapplication.h"
#include "JuffEd.h"

int main(int argc, char* argv[]) {
	QtSingleApplication app(argc, argv);

	// check single run
	QString params = app.arguments().join("\n");
	if (app.sendMessage(params))
	  return 0;

	app.setOrganizationName("juff");
	app.setApplicationName("juffed");

	JuffEd juffed;
	QObject::connect(&app, SIGNAL(messageReceived(const QString&)), &juffed, SLOT(onMessageReceived(const QString&)));
//	JuffMW mw;
//	mw.show();
	juffed.mainWindow()->show();
	
	return app.exec();
}
