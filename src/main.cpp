/*
JuffEd - A simple text editor
Copyright 2007-2008 Mikhail Murzin

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

//	Qt headers	
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QLocale>
#include <QtCore/QTranslator>
#include <QtGui/QApplication>
#include <QtGui/QIcon>

//	local headers
#ifdef Q_OS_WIN
#include "AppInfo.win.h"
#else
#include "AppInfo.h"
#endif

#include "DocHandler.h"
#include "JuffEd.h"
#include "Log.h"
#include "MainSettings.h"
#include "Settings.h"

#include "SingleInstance.h"

void checkForFirstRun() {
	//	localizations
	QString configPath = AppInfo::configDir();
	QString appPath = AppInfo::appDirPath();

	QDir configDir(configPath);
	if (!configDir.exists()) {
		configDir.mkpath(configPath);
		foreach(QString localization, QDir(appPath + "/l10n").entryList(QStringList("*.qm"))) {
			QFile::copy(appPath + "/l10n/" + localization, configPath + "/" + localization);
		}
	}

	//	highlight schemes
	QDir schemesDir = QDir(appPath + "/hlschemes");
	QString localSchemePath = AppInfo::configDir() + "/hlschemes";

	QDir localSchemeDir(localSchemePath);
	if (!localSchemeDir.exists())
		localSchemeDir.mkpath(localSchemePath);
	foreach (QString schemeFile, schemesDir.entryList(QDir::Files)) {
		if (!QFileInfo(localSchemePath + "/" + schemeFile).exists()) {
			QFile::copy(schemesDir.absolutePath() + "/" + schemeFile, localSchemePath + "/" + schemeFile);
		}
	}

	//	sessions
	QDir sessionDir(configPath + "/sessions/");
	if (!sessionDir.exists())
		sessionDir.mkpath(configPath + "/sessions/");
}

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	checkForFirstRun();

	//	translator
	QTranslator appTranslator;
	QString lng = QLocale::system().name().left(2);
	appTranslator.load(AppInfo::configDir() + "/juffed_" + lng);
	app.installTranslator(&appTranslator);

	Settings::read();

	//	Try to find existing instance of the app. If it 
	//	was found and there is something to send there, 
	//	send all file names to existing app. If no files
	//	given, new document will be opened
	int sock(-1);
	bool noSingle = false;
	bool instanceExists = findExistingInstance(sock);
	
	//	If -ns option exists, new window will be opened 
	//	in any case, but it will not be able to accept 
	//	external requests for opening files
	for (int i = 0; i < argc; ++i) {
		if (strcmp(argv[i], "-ns") == 0) {
			noSingle = true;
			break;
		}
	}
	
	if (!instanceExists || noSingle) {
		//	Instance of JuffEd not found or 
		//	"no single" option was used. 
		bool startListening = MainSettings::singleInstance() && !noSingle;
		DocHandler handler(startListening);
		JuffEd mw(&handler);

		//	Open files
		if (argc > 1) {
			for (int i = 1; i < argc; ++i) {
				QString argument = QString::fromLocal8Bit(argv[i]);
				if (argument[0] == '-') {
					//	command line options
				}
				else {
					handler.docOpen(QFileInfo(argument).absoluteFilePath());
				}
			}
		}

		if (handler.docCount() == 0) {
			handler.restoreSession();
		}

	
/*		if (handler.runAllTests()) {
			mw.show();
	
			return app.exec();
		}
		else 
			return -1;*/

		mw.show();
		return app.exec();

	}
	else {
		//	Instance exists and "no single" option
		//	wasn't used. In this case do not start 
		//	new app, just send given files to 
		//	existing instance
		QString list;
		for (int i = 1; i < argc; ++i) {
			QString argument = QString::fromLocal8Bit(argv[i]);
			if (argument[0] == '-') {
				//	command line options
			}
			else {
				if (!list.isEmpty())
					list += ";";
				list += QFileInfo(argument).absoluteFilePath();
			}
		}

		if (sendFileNames(sock, list)) {
			return 0;
		}
		else {
			Log::printToLog("Failed to send file names to the existing app");
			return -1;
		}
	}
}
