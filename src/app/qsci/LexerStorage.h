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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef __JUFF_LEXER_STORAGE_H__
#define __JUFF_LEXER_STORAGE_H__

#include "QSciLibConfig.h"

class QsciLexer;

class QFont;
class QString;
#include <QtGlobal>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
using QStringList = QList<QString>;
#else
class QStringList;
#endif

#include <QColor>

namespace Juff {

class LSInterior;

class QSCI_ENGINE_EXPORT LexerStorage {
public:
	/**
	* Returns lexer by name
	*/
	QsciLexer* lexer(const QString& lexerName);

	/**
	* Returns lexer by file name
	*/
	QsciLexer* lexerByFileName(const QString& fileName);

	/**
	* Returns lexer name by file name
	*/
	QString lexerName(const QString& fileName);
	
	/**
	* Returns a list of available lexers
	*/
	QStringList lexersList() const;

	/**
	* Updates currently existing lexers with new font and 
	* sets the default font for new lexers
	*/
	void updateLexers(const QFont&);

	/**
	* Returns current line highlighting color for specified lexer
	*/
	QColor curLineColor(const QString&) const;
	
	/**
	* Returns selection background color for specified lexer
	*/
	QColor selectionBgColor(const QString&) const;
	
	/**
	* Returns LexerStorage singleton
	*/
	static LexerStorage* instance();

	/**
	* Destructor
	*/
	~LexerStorage();

protected:
	LexerStorage();
	
private:
	static LexerStorage* instance_;
	LSInterior* lsInt_;
};

}

#endif // __JUFF_LEXER_STORAGE_H__
