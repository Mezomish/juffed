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

#include "Log.h"

//	Qt headers
#include <QtCore/QObject>

//	local headers
#include "AppInfo.h"

namespace Log {
	void print(int n) {
		print(QString::number(n));
	}
	
	void print(const QString& str) {
		QFile log(AppInfo::logFile());
		if (log.open(QIODevice::Append)) {
			log.write(str.toLocal8Bit());
			log.write("\r\n");
			log.close();
		}
		else {
			debug(QString("Can't open log file: ") + log.errorString());
		}
	}
	
	void debug(const QString& str) {
		qDebug(qPrintable(str));
	}
	
	void debug(const char* str)  {
		qDebug(str);
	}

	void debug(int n) {
		debug(QString::number(n));
	}

};

