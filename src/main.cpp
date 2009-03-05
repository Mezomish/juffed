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

#include <QtCore/QLocale>
#include <QtCore/QTranslator>
#include <QtGui/QApplication>

#include "gui/GUI.h"
#include "AppInfo.h"
#include "Log.h"
#include "Manager.h"
#include "Settings.h"

class JuffApp : public QApplication {
public:
	JuffApp(int& argc, char** argv) : QApplication(argc, argv) {
		setOrganizationName("juff");
		setApplicationName("juffed");

		checkForFirstRun();

		//	translator
		QString lng = QLocale::system().name().left(2);
		if ( translator_.load("juffed_" + lng, AppInfo::translationPath()) ) {
			if ( !translator_.isEmpty() )
				installTranslator(&translator_);
		}

		Settings::read();
	}
	
	virtual ~JuffApp() {
		Settings::write();
	}
	
private:
	void checkForFirstRun() {
		//	highlight schemes and API lists
		copyToLocalDir("hlschemes");
		copyToLocalDir("apis");
		
		//	sessions
		QString configPath = AppInfo::configDirPath();
		QDir sessionDir(configPath + "/sessions/");
		if (!sessionDir.exists())
			sessionDir.mkpath(configPath + "/sessions/");
	}
	
	void copyToLocalDir(const QString& subDirName) {
		QDir dir = QDir(AppInfo::appDirPath() + "/" + subDirName);
		QString localPath = AppInfo::configDirPath() + "/" + subDirName;

		QDir localDir(localPath);
		if ( !localDir.exists() )
			localDir.mkpath(localPath);
		foreach (QString file, dir.entryList(QDir::Files)) {
			if (!QFileInfo(localPath + "/" + file).exists()) {
				QFile::copy(dir.absolutePath() + "/" + file, localPath + "/" + file);
			}
		}
	}
	
	QTranslator translator_;
};

int main(int argc, char* argv[])
{
	JuffApp app(argc, argv);
		
	Juff::GUI::GUI gui;
	Juff::Manager manager(&gui);
	gui.show();
	
	bool openFiles = false;
	for (int i = 1; i < argc; ++i) {
		QString argument = QString::fromLocal8Bit(argv[i]);
		if (argument[0] == '-') {
			//	TODO : command line options
		}
		else {
			manager.openDoc(QFileInfo(argument).absoluteFilePath());
			openFiles = true;
		}
	}

	if ( !openFiles )
		manager.restoreSession();

	return app.exec();
}
