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

#include "JuffApp.h"
#include "MainSettings.h"

JuffApp::JuffApp(int& argc, char** argv) : QApplication(argc, argv), sent_(false) {
	JUFFENTRY;
	
	int sock = -1;
	gui_ = 0;
	manager_ = 0;
	listener_ = 0;
	
	setOrganizationName("juff");
	setApplicationName("juffed");
	checkForFirstRun();
	Settings::read();
	
	if ( MainSettings::singleInstance() && findExistingInstance(sock) ) {
		//	Instance exists. In this case do not start 
		//	new app, just send given files to 
		//	existing instance
		QString list;
		for (int i = 1; i < argc; ++i) {
			QString argument = QString::fromLocal8Bit(argv[i]);
			if ( argument[0] == '-' ) {
				//	command line options
			}
			else {
				if ( !list.isEmpty() )
					list += ";";
				list += QFileInfo(argument).absoluteFilePath();
			}
		}

		if ( sendFileNames(sock, list) ) {
			sent_ = true;
		}
		else {
			init(argc, argv);
			Log::printToLog("Failed to send file names to the existing app");
		}
	}
	else {
		init(argc, argv);
	}
}

JuffApp::~JuffApp() {
	if ( listener_ ) {
		listener_->exit();
		listener_->wait(500);
		delete listener_;
	}
	if ( gui_ )
		delete gui_;
	if ( manager_ )
		delete manager_;
	Settings::write();
}

bool JuffApp::sentFilesToExistingInstance() {
	return sent_;
}

void JuffApp::init(int& argc, char** argv) {
	JUFFENTRY;
		
	//	translator
	QString lng = QLocale::system().name();
	if ( translator_.load("juffed_" + lng, AppInfo::translationPath()) ) {
		if ( !translator_.isEmpty() ) {
			installTranslator(&translator_);
		}
	}
	
	gui_ = new Juff::GUI::GUI();
	manager_ = new Juff::Manager(gui_);
	
	if ( MainSettings::singleInstance() ) {
		listener_ = new SocketListener(this);
		connect(listener_, SIGNAL(fileRecieved(const QString&)), manager_, SLOT(openDoc(const QString&)));
		connect(listener_, SIGNAL(newFileRequested()), manager_, SLOT(fileNew()));
		listener_->start();
	}

	gui_->show();
	
	bool openFiles = false;
	for (int i = 1; i < argc; ++i) {
		QString argument = QString::fromLocal8Bit(argv[i]);
		if ( argument[0] == '-' ) {
			//	TODO : command line options
		}
		else {
			manager_->openDoc(QFileInfo(argument).absoluteFilePath());
			openFiles = true;
		}
	}

	if ( !openFiles )
		manager_->restoreSession();
}


bool JuffApp::findExistingInstance(int& sock) {
	JUFFENTRY;
#ifdef Q_OS_UNIX

	//	Trying to connect to existing socket
	int s = socket(AF_UNIX, SOCK_STREAM, 0);
	sockaddr addr;
	memset(&addr, 0, sizeof(struct sockaddr));

	addr.sa_family = AF_UNIX;
	strncpy(addr.sa_data, AppInfo::socketPath().toLocal8Bit().constData(), sizeof(addr.sa_data) - 1);
	
	int res = ::connect(s, &addr, sizeof(addr));
	sock = s;
	
	return ( res == 0 );

#else	

//	Q_OS_UNIX not defined
	return false;

#endif	//	Q_OS_UNIX
}

bool JuffApp::sendFileNames(int sock, const QString& list) {
#ifdef Q_OS_UNIX

	bool result = true;
	if ( !list.isEmpty() ) {
		QByteArray buf = list.toLocal8Bit();
		if ( write(sock, buf.constData(), buf.size()) == -1 )
			result = false;
	}
	else {
		char nf[] = "--newfile";
		if ( write(sock, nf, strlen(nf)) == -1 )
			result = false;
	}
	return result;
	
#else	

//	Q_OS_UNIX not defined
	return false;

#endif	//	Q_OS_UNIX
}

void JuffApp::checkForFirstRun() {
	//	highlight schemes and API lists
	copyToLocalDir("hlschemes");
	copyToLocalDir("apis");
	
	//	sessions
	QString configPath = AppInfo::configDirPath();
	QDir sessionDir(configPath + "/sessions/");
	if ( !sessionDir.exists() )
		sessionDir.mkpath(configPath + "/sessions/");
}

void JuffApp::copyToLocalDir(const QString& subDirName) {
	QDir dir = QDir(AppInfo::appDirPath() + "/" + subDirName);
	QString localPath = AppInfo::configDirPath() + "/" + subDirName;

	QDir localDir(localPath);
	if ( !localDir.exists() ) {
		localDir.mkpath(localPath);
	}
	foreach (QString file, dir.entryList(QDir::Files)) {
		if ( !QFileInfo(localPath + "/" + file).exists() ) {
			QFile::copy(dir.absolutePath() + "/" + file, localPath + "/" + file);
		}
	}
}
