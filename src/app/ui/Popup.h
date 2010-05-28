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

class QLabel;
class QTimer;
class QTimeLine;

#include <QFrame>

class Popup : public QFrame {
Q_OBJECT
public:
	Popup(QWidget* parent);
	void popup(const QString& header, const QString& msg, int seconds = 10);
	void dismiss();

protected slots:
	void onTimer();
	void makeStep(int);

protected:
	virtual void enterEvent(QEvent* event);
	virtual void leaveEvent(QEvent* event);
	virtual void mousePressEvent(QMouseEvent*);

private:
	void setAlpha(int alpha);

	QLabel* headerL_;
	QLabel* messageL_;
	QLabel* timerL_;
	int timerTicks_;
	QTimer* timer_;
	QTimeLine* timeLine_;
	bool hidden_;
	QString styleSheet_;
};

#endif // __JUFFED_POPUP_H__
