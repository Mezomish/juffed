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

#include "SingleInstance.h"

#include <QtGlobal>

#ifdef Q_OS_UNIX

#include <sys/socket.h>
#include <errno.h>

#include "AppInfo.h"
#include "Log.h"

#endif	//	Q_OS_UNIX

bool findExistingInstance(int& sock) {
#ifdef Q_OS_UNIX

	//	Trying to connect to existing socket
	int s = socket(AF_UNIX, SOCK_STREAM, 0);
	sockaddr addr;
    memset(&addr, 0, sizeof(struct sockaddr));
    
	addr.sa_family = AF_UNIX;
    strncpy(addr.sa_data, AppInfo::socketPath().toLocal8Bit().constData(), sizeof(addr.sa_data) - 1);
	
	int res = connect(s, &addr, sizeof(addr));
	sock = s;
	
	return ( res == 0 );

#else	

//	Q_OS_UNIX not defined
	return false;

#endif	//	Q_OS_UNIX
}

bool sendFileNames(int sock, const QString& list) {
#ifdef Q_OS_UNIX

	bool res = true;
	if (!list.isEmpty()) {
		QByteArray buf = list.toLocal8Bit();
		int res = write(sock, buf.constData(), buf.size());
		if (res == -1)
			res = false;
	}
	else {
		char nf[] = "--newfile";
		int res = write(sock, nf, strlen(nf));
		if (res == -1)
			res = false;
	}
	return res;
	
#else	

//	Q_OS_UNIX not defined
	return false;

#endif	//	Q_OS_UNIX
}
