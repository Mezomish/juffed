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

#ifndef __JUFFED_JUMP_TO_FILE_DLG_H__
#define __JUFFED_JUMP_TO_FILE_DLG_H__

#include <QDialog>

#include "ui_JumpToFileDlg.h"

class JumpToFileDlg : public QDialog {
Q_OBJECT
public:
	JumpToFileDlg(const QStringList&, QWidget*);

	QString fileName() const;

public slots:
	void onItemDoubleClicked(QListWidgetItem*);
	void onTextEdited(const QString&);
	void onReturnPressed();

protected:
	virtual bool eventFilter(QObject*, QEvent*);

private:
	Ui::JumpToFileDlg ui_;
};

#endif // __JUFFED_JUMP_TO_FILE_DLG_H__
