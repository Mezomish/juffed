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

#include "SearchResults.h"

namespace Juff {

SearchResults::SearchResults(const Juff::SearchParams& params) {
	params_ = params;
	visible_ = false;
}

int SearchResults::count() const {
	return occurences_.count();
}

SearchOccurence SearchResults::occurence(int index) const {
	if ( index < 0 || index >= occurences_.count() )
		return SearchOccurence();
	else
		return occurences_[index];
}

int SearchResults::findIndexByCursorPos(int row, int col, bool forward) {
	if ( occurences_.count() == 0 )
		return -1;
	
	if ( forward ) {
		QVector<SearchOccurence>::iterator it = occurences_.begin();
		int index = 0;
		while ( it != occurences_.end() ) {
			const SearchOccurence& occ = *it;
			if ( occ.startRow > row || ( occ.startRow == row && occ.startCol >= col ) )
				return index;
			it++;
			index++;
		}
		// If we got here then nothing was found after the cursor - return the 1st one
		return 0;
	}
	else {
		QVector<SearchOccurence>::iterator it = occurences_.end();
		int index = occurences_.count();
		do {
			it--;
			index--;
			const SearchOccurence& occ = *it;
			if ( occ.startRow < row || ( occ.startRow == row && occ.startCol < col ) )
				return index;
			
		} while ( it != occurences_.begin() );
		// If we got here then nothing was found before the cursor - return the last one
		return occurences_.count() - 1;
	}
}

void SearchResults::addOccurence(int row1, int col1, int row2, int col2) {
	occurences_.append(SearchOccurence(row1, col1, row2, col2));
}

const Juff::SearchParams& SearchResults::params() const {
	return params_;
}

void SearchResults::setVisible(bool visible) {
	visible_ = visible;
}

bool SearchResults::isVisible() const {
	return visible_;
}

}
