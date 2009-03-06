/*
JuffEd - A simple text editor
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

#ifndef _TAB_WIDGET_H_
#define _TAB_WIDGET_H_

//	Qt headers
#include <QtGui/QTabWidget>

//	local headers
#include "TabBar.h"

namespace Juff {
namespace GUI {

class TabWidget : public QTabWidget {
Q_OBJECT
public:
	TabWidget(QWidget* parent);

public slots:
	void nextWidget();
	void prevWidget();

signals:
	void tabCloseRequested(int);
	void requestFileName(int, QString&);

private:
	TabBar* tabBar_;
};

}	//	namespace GUI
}	//	namespace Juff

#endif
