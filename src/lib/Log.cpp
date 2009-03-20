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

#include "Log.h"

//	Qt headers
#include <QtCore/QDateTime>
#include <QtCore/QFile>
#include <QtCore/QObject>

#ifdef Q_OS_WIN
#include <QtGui/QMessageBox>
#endif

//	local headers
#include "AppInfo.h"

namespace Log {

	void printToLog(int n, bool canBeSkipped) {
		printToLog(QString::number(n), canBeSkipped);
	}
	
	void printToLog(const QString& str, bool canBeSkipped) {
		QFile log(AppInfo::logFile());
		if (log.open(QIODevice::Append)) {
			log.write(QDateTime::currentDateTime().toString("[hh:mm:ss] %1").arg(str).toLocal8Bit());
			log.write("\r\n");
			log.close();
		}
		else {
#ifdef Q_OS_WIN
			if (!canBeSkipped) {
				QMessageBox::warning(0, "Logging error", 
						QString("Can't open log file (reason: '%1') for writing the message\n\n '%2'")
						.arg(log.errorString()).arg(str));
			}
			else {
				if (!logFileErrorShowed) {
					QMessageBox::warning(0, "Error", 
							QString("Can't open log file (reason: '%1').\nNothing will be logged.")
							.arg(log.errorString()));

					logFileErrorShowed = true;
				}
			}
#else
			Q_UNUSED(canBeSkipped);
			qDebug(qPrintable(QString("Can't open log file: ") + log.errorString()));
			qDebug(qPrintable(QString("Log string: ") + str));
#endif
		}
	}
	
	void debug(const QString& str, bool canBeSkipped) {
#ifdef Q_OS_WIN
		printToLog(str, canBeSkipped);
#else
		qDebug(qPrintable(QDateTime::currentDateTime().toString("[hh:mm:ss] %1").arg(str)));
#endif
}
	
	void debug(const char* str, bool canBeSkipped)  {
		debug(QString(str), canBeSkipped);
	}

	void debug(int n, bool canBeSkipped) {
		debug(QString::number(n), canBeSkipped);
	}

	void debug(const QRect& r, bool canBeSkipped) {
		debug(QString("%1,%2,%3,%4").arg(r.left()).arg(r.top()).arg(r.width()).arg(r.height()), canBeSkipped);
	}
};

