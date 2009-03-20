/*
JuffEd - An advanced text editor
Copyright 2007-2009 Mikhail Murzin

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

#ifndef _JUFF_SESSION_DLG_H_
#define _JUFF_SESSION_DLG_H_

#include <QtCore/QStringList>
#include <QtGui/QDialog>

#include "ui_SessionDlg.h"

namespace Juff {
namespace GUI {

class SessionDlg : public QDialog {
Q_OBJECT
public:
	SessionDlg(QWidget*);
	virtual ~SessionDlg();

	int result() const;
	QString curSessionName() const;
	int sessionCount() const;
	static QStringList sessionList();

private slots:
	void openSession();
	void newSession();
	void activated(QTreeWidgetItem*, int);

private:
	Ui::SessionDlg ui;
	int result_;
};

}	//	namespace GUI
}	//	namespace Juff

#endif
