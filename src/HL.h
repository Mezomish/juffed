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

#ifndef _HL_H_
#define _HL_H_

//	Local
#include "HLTypes.h"

//	Qt
#include <QtGui/QSyntaxHighlighter>

class HL : public QSyntaxHighlighter {
public:
	HL(QTextDocument*);
	virtual ~HL();

	void changeFileName(const QString&);

private:
	virtual void highlightBlock(const QString&);
	void highlightTheString(const QString&, int indent = 0, int blockType = -1);
	void applyRules(const QString&, int indent = 0);
	bool findBlockBegin(const QString&, int& blockIndex, int& begPos);

	HLScheme sch_;
};

#endif
