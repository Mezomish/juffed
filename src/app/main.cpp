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
#include "AppInfo.h"
#include "JuffEd.h"
#include "MainSettings.h"

#include <QFileInfo>
#include <QTranslator>

void initApp(QApplication& app) {
	app.setOrganizationName("juff");
	app.setApplicationName("juffed");

	QString lng = AppInfo::language();
	QTranslator* translator = new QTranslator();
	if ( translator->load("juffed_" + lng, AppInfo::translationPath()) ) {
		if ( !translator->isEmpty() ) {
			app.installTranslator(translator);
		}
	}
	
}

void processParams(JuffEd& juffed, QStringList params) {
	params.removeFirst();
	foreach (QString param, params) {
		if ( QFileInfo(param).exists() )
			juffed.openDoc(QFileInfo(param).absoluteFilePath());
	}
}

int runSingle(int argc, char* argv[]) {
	QtSingleApplication app(argc, argv);
	initApp(app);

	// check if instance already exists
	QStringList fileList;
	foreach (QString param, app.arguments()) {
		fileList << QFileInfo(param).absoluteFilePath();
	}
	if ( app.sendMessage(fileList.join("\n")) )
		return 0;

	// instance doesn't exist yet
	JuffEd juffed;
	QObject::connect(&app, SIGNAL(messageReceived(const QString&)), &juffed, SLOT(onMessageReceived(const QString&)));
	app.setActivationWindow(juffed.mainWindow());

	juffed.mainWindow()->show();
	processParams(juffed, app.arguments());

	return app.exec();
}

int runNotSingle(int argc, char* argv[]) {
	QApplication app(argc, argv);
	initApp(app);

	JuffEd juffed;

	juffed.mainWindow()->show();
	processParams(juffed, app.arguments());

	return app.exec();
}

int main(int argc, char* argv[]) {
	Settings::read("juff", "juffed");
	if ( MainSettings::get(MainSettings::SingleInstance) )
		return runSingle(argc, argv);
	else 
		return runNotSingle(argc, argv);
}
