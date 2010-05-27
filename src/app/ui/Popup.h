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

#ifndef __JUFFED_POPUP_H__
#define __JUFFED_POPUP_H__

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>

class Popup : public QFrame {
Q_OBJECT
public:
	Popup(QWidget* parent);
	void popup(const QString& msg, int seconds = 10);

protected slots:
	void onTimer();

protected:
	virtual void enterEvent(QEvent* event);
	virtual void leaveEvent(QEvent* event);
	virtual void mousePressEvent(QMouseEvent*);

private:
	void setAlpha(int alpha);

	QLabel* messageL_;
	QLabel* timerL_;
	QTimer* timer_;
	int timerTicks_;
};

#endif // __JUFFED_POPUP_H__
