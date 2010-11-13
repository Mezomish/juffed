/*
JuffEd - An advanced text editor
Copyright 2007-2010 Mikhail Murzin

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

#ifndef __JUFF_LOG_H__
#define __JUFF_LOG_H__

#include "LibConfig.h"

#include <QtCore/QString>
#include <QtCore/QRect>

namespace Log {
	void LIBJUFF_EXPORT printToLog(const QString&, bool canBeSkipped = false);
	void LIBJUFF_EXPORT printToLog(int, bool canBeSkipped = false);
	void LIBJUFF_EXPORT debug(const QString&, bool canBeSkipped = false);
	void LIBJUFF_EXPORT debug(const char*, bool canBeSkipped = false);
	void LIBJUFF_EXPORT debug(int, bool canBeSkipped = false);
	void LIBJUFF_EXPORT debug(const QRect&, bool canBeSkipped = false);
	void LIBJUFF_EXPORT warning(const QString&, bool canBeSkiped = false);
};

#ifdef JUFF_FULL_DEBUG

#include <QtCore/QFileInfo>

#define JUFFDEBUG(x) Log::debug(QString("DEBUG (%1:%2): %3").arg(QFileInfo(__FILE__).fileName()).arg(__LINE__).arg(x))
#define JUFFDEBUG2(x) Log::debug(QString("DEBUG2 (%1:%2): %3").arg(QFileInfo(__FILE__).fileName()).arg(__LINE__).arg(x))

#else	//	JUFF_FULL_DEBUG

//-----------------
#ifdef JUFF_DEBUG

#include <QtCore/QFileInfo>

#define JUFFDEBUG(x) Log::debug(QString("DEBUG (%1:%2): %3").arg(QFileInfo(__FILE__).fileName()).arg(__LINE__).arg(x))

#else	//	JUFF_DEBUG

#define JUFFDEBUG(x) ;

#endif	//	JUFF_DEBUG
//-----------------

#define JUFFDEBUG2(x) ;
#define JUFFENTRY2 ;

#endif	//	JUFF_FULL_DEBUG

class LIBJUFF_EXPORT Logger {
public:
	Logger(const char*, const char*, int);
	~Logger();

private:
	static int indent_;
	QString func_;
	QString file_;
	int line_;
};

#define LOGGER Logger l(__FUNCTION__, __FILE__, __LINE__)

#endif	//	__JUFF_LOG_H__
