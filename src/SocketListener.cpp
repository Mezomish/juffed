#include <QDebug>

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

#include "SocketListener.h"

#include <QtGlobal>
#include <QFile>
#include <QLocalServer>
#include <QLocalSocket>

#include "AppInfo.h"
#include "Log.h"


SocketListener::SocketListener(QObject* parent) : QObject(parent) {
	//	Remove socket file if exists. Can safely do this
	//	because the only case we could reach this point
	//	is if connection to existing socket failed. That's
	//	why if the socket path still exists, it isn't accociated
	//	with any sockets
	if ( QFile::exists(AppInfo::socketPath()) )
		QFile::remove(AppInfo::socketPath());
	server_ = new QLocalServer();
	connect(server_, SIGNAL(newConnection()), SLOT(onNewConnection()));
}

SocketListener::~SocketListener() {
#if QT_VERSION >= 0x040500
	server_->removeServer(AppInfo::socketPath());
#endif
}

void SocketListener::onNewConnection() {
	JUFFENTRY;
	
	QLocalSocket* socket = server_->nextPendingConnection();
	if ( !socket->waitForReadyRead(1000) ) {
		qDebug() << "Couldn't read data:" << socket->errorString();
		return;
	}
	
	QByteArray data = socket->readAll();
	JUFFDEBUG(QString::fromLocal8Bit(data));
	if ( data.isEmpty() ) {
		return;
	}
	
	QStringList list = QString::fromLocal8Bit(data).split(";");
	foreach (QString arg, list) {
		if ( arg[0] == '-' ) {
			if ( arg.compare("--newfile") == 0 ) {
				emit newFileRequested();
			}
		}
		else {
			if ( !arg.isEmpty() )
				emit fileRecieved(QFileInfo(arg).absoluteFilePath());
		}
	}
}

void SocketListener::start() {
	server_->listen(AppInfo::socketPath());
}
