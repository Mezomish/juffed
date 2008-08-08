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

#ifndef _LOG_H_
#define _LOG_H_

class QString;
class QRect;

namespace Log {
	void printToLog(const QString&, bool canBeSkipped = false);
	void printToLog(int, bool canBeSkipped = false);
	void debug(const QString&, bool canBeSkipped = false);
	void debug(const char*, bool canBeSkipped = false);
	void debug(int, bool canBeSkipped = false);
	void debug(const QRect&, bool canBeSkipped = false);
};

//	TODO: remove it from here to configure and .pro file

#ifdef JUFF_DEBUG

#define JUFFENTRY Log::debug(QString("Entering %1 (%2:%3)").arg(__FUNCTION__).arg(__FILE__).arg(__LINE__))
#define JUFFDTOR Log::debug(QString("Destructor: %1").arg(__FUNCTION__))
#define JUFFDEBUG(x) Log::debug(x)
#else

#define JUFFENTRY ;
#define JUFFDTOR ;
#define JUFFDEBUG(x) ;
#endif

#endif
