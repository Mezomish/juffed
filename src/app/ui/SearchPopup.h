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
	void setFocusOnFind(bool selectAll = false);
	void setFocusOnReplace(bool selectAll = false);
	Juff::SearchParams searchParams() const;
	void setSearchStatus(int, int);
	bool isCollapsed() const;

public slots:
	void dismiss();
	void expand(bool);

signals:
	void searchParamsChanged(const Juff::SearchParams&);
//	void searchRequested();
	void findNextRequested();
	void findPrevRequested();
	void closed();
	void replaceNextRequested();
	void replacePrevRequested();
	void replaceAllRequested();

private slots:
	void onFindTextChanged(const QString&);
	void onCaseSensitiveChecked(bool);
	void slotFindNext();
	void slotFindPrev();
	void slotReplaceNext();
	void slotReplacePrev();
	void slotReplaceAll();
	void slotModeChanged(int);

private:
	Ui::SearchPopup ui;
	Juff::SearchParams params_;
	bool collapsed_;
};

#endif // __JUFFED_SEARCH_POPUP_H__
