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

#ifndef __JUFFED_SEARCH_POPUP_H__
#define __JUFFED_SEARCH_POPUP_H__

#include <QWidget>

#include "Types.h"
#include "ui_SearchPopup.h"

class SearchPopup : public QWidget {
Q_OBJECT
public:
	SearchPopup();

	void setFindText(const QString&);
	void setFindFocus(bool selectAll = false);
	Juff::SearchParams searchParams() const;
	void highlightRed(bool highlight = true);

public slots:
	void dismiss();
	void showReplace();
	void hideReplace();

signals:
	void searchRequested();
	void findNext();
	void findPrev();
	void closed();
//	void replaceNext();
//	void replacePrev();
//	void replaceAll();

private slots:
	void onTextChanged(const QString&);
	void onCaseSensitiveChecked(bool);
	void onWholeWordsChecked(bool);
	void slotFindNext();
	void slotFindPrev();

private:
	Ui::SearchPopup ui;
	Juff::SearchParams params_;
};

#endif // __JUFFED_SEARCH_POPUP_H__
