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

#include "SocketListener.h"

#include <QtGlobal>

#ifdef Q_OS_UNIX

#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <QtGui/QApplication>
#include <QtGui/QWidget>

#include "AppInfo.h"
#include "Log.h"

#endif	//	Q_OS_UNIX


SocketListener::SocketListener(QObject* parent) : QThread(parent) {
}

SocketListener::~SocketListener() {
#ifdef Q_OS_UNIX
	close(socket_);
#endif	//	Q_OS_UNIX
}

void SocketListener::run() {
#ifdef Q_OS_UNIX
	//	Remove socket file if exists. Can safely do this
	//	because the only case we could reach this point
	//	is if connection to existing socket failed. That's
	//	why if the socket path still exists, it isn't accociated
	//	with any sockets
	if (QFile::exists(AppInfo::socketPath()))
		QFile::remove(AppInfo::socketPath());
	
	socket_ = socket(AF_UNIX, SOCK_STREAM, 0);
	if (socket_ == -1) {
		//	error
		Log::print("Failed to create socket");
	}
	else {
		//	Socket was created successfully.
		//	Trying to bind it.
	    struct sockaddr_un addr;
	    memset(&addr, 0, sizeof(struct sockaddr_un));

	    addr.sun_family = AF_UNIX;
	    strncpy(addr.sun_path, AppInfo::socketPath().toLocal8Bit().constData(), sizeof(addr.sun_path) - 1);

	    int res = bind(socket_, (struct sockaddr *) &addr, sizeof(struct sockaddr_un));

		if (res == -1) {
			//	error
			Log::print("Failed to bind socket");
		}
		else {
			//	Socket was binded successfully. Now it 
			//	starts listening and accepting connections
			int res = listen(socket_, 0);
			if (res == -1) {
				//	error
				Log::print("Failed to start listening");
			}
			else {
				sockaddr cl_addr;
				socklen_t addr_len = sizeof(cl_addr);
				char buf[1024];
				
				//	Here we have infinte loop which waits for 
				//	connection. If accept function fails 10 
				//	times, thread exits
				int fails = 0;
				while( true ) {
					Log::debug("Listening...");
					int connected_sock = accept(socket_, &cl_addr, &addr_len);
					if (connected_sock == -1) {
						//	error
						Log::print("Accept failed");
						++fails;
					}
					else {
						int bytes_read = read(connected_sock, buf, sizeof(buf) - 1);
						buf[bytes_read] = '\0';
						QStringList list = QString::fromLocal8Bit(buf).split(";");
						int count = list.count();
						for (int i = 0; i < count; ++i) {
							QString arg = list[i];
							if (arg[0] == '-') {
								//	command line options
								if (arg.compare("--newfile") == 0) {
									emit newFileRequested();
								}
							}
							else {
								emit fileRecieved(QFileInfo(arg).absoluteFilePath());
							}
						}
					}
					
					if (fails >= 10) {
						exit(-1);
					}
				}
			}
		}
	}
#endif	//	Q_OS_UNIX
}
