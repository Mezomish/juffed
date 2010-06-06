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

#ifndef __JUFFED_SEARCH_RESULTS_H__
#define __JUFFED_SEARCH_RESULTS_H__

#include <QVector>

namespace Juff {

struct SearchOccurence {
	int startRow;
	int startCol;
	int endRow;
	int endCol;
	
	SearchOccurence() {
		startRow = startCol = endRow = endCol = -1;
	}
	SearchOccurence(int row1, int col1, int row2, int col2) {
		startRow = row1;
		startCol = col1;
		endRow = row2;
		endCol = col2;
	}
};

class SearchResults {
public:
	int count() const;
	SearchOccurence occurence(int) const;
	int findIndexByCursorPos(int row, int col, bool forward);
	void addOccurence(int, int, int, int);

private:
	QVector<SearchOccurence> occurences_;
};

}

#endif // __JUFFED_SEARCH_RESULTS_H__
