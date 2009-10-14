/*
JuffEd - An advanced text editor
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

#ifndef _JUFF_APP_H_
#define _JUFF_APP_H_

class QLocalSocket;

#include "gui/GUI.h"
#include "AppInfo.h"
#include "Log.h"
#include "Manager.h"
#include "Settings.h"
#include "SocketListener.h"

#include <QtCore/QLocale>
#include <QtCore/QTranslator>
#include <QtGui/QApplication>


class JuffApp : public QApplication {
Q_OBJECT
public:
	JuffApp(int& argc, char** argv);
	virtual ~JuffApp();

	bool sentFilesToExistingInstance();
	static QString language();

private:
	void init(int& argc, char** argv);
	bool findExistingInstance(QLocalSocket& sock);
	bool sendFileNames(QLocalSocket& sock, const QString& list);
	void checkForFirstRun();
	void copyToLocalDir(const QString& subDirName);
	
	Juff::GUI::GUI* gui_;
	Juff::Manager* manager_;
	QTranslator translator_;
	SocketListener* listener_;
	bool sent_;
	static QString language_;
};

#endif
