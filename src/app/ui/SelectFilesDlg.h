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

#ifndef __JUFFED_SELECT_FILES_DLG_H__
#define __JUFFED_SELECT_FILES_DLG_H__

#include "ui_SelectFilesDlg.h"

class SelectFilesDlg : public QDialog {
Q_OBJECT
public:
	SelectFilesDlg(const QStringList&, QWidget*);

	QStringList checkedFiles() const;

public slots:
	void slotSave();
	void slotDontSave();

private slots:
	void onAllClicked();
	void onItemChanged(QListWidgetItem*);

private:
	Ui::SelectFilesDlg ui_;
	QStringList list_;
};

#endif // __JUFFED_SELECT_FILES_DLG_H__
