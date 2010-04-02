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

#ifndef __JUFFED_MESSAGE_WIDGET_H__
#define __JUFFED_MESSAGE_WIDGET_H__

#include "ui_MessageWidget.h"

class MessageWidget : public QWidget {
Q_OBJECT
public:
	MessageWidget(const QIcon&, const QString&, const QString&, int, QWidget*);
	virtual ~MessageWidget();

public slots:
//	void closeMsg();

protected:
	virtual void mouseDoubleClickEvent(QMouseEvent*);
	virtual void timerEvent(QTimerEvent*);

private:
	Ui::MessageWidget ui_;
	int countDown_;
};

#endif // __JUFFED_MESSAGE_WIDGET_H__
