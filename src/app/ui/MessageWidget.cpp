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

#include "MessageWidget.h"

MessageWidget::MessageWidget(const QIcon&, const QString& title, const QString& msg, int timeout, QWidget* parent) : QWidget(parent) {
	ui_.setupUi(this);
	setMaximumHeight(50);
	setAttribute(Qt::WA_DeleteOnClose);
	
	ui_.textL->setText(msg);
	ui_.timerL->setText(QString::number(timeout));
	ui_.okBtn->setIcon(QIcon(":close"));
	ui_.okBtn->setText("");
	connect(ui_.okBtn, SIGNAL(clicked()), SLOT(close()));
	countDown_ = timeout;
	
	startTimer(1000);
}

MessageWidget::~MessageWidget() {
}

void MessageWidget::mouseDoubleClickEvent(QMouseEvent*) {
	close();
}

//void McloseMsg();

void MessageWidget::timerEvent(QTimerEvent*) {
	--countDown_;
	if ( countDown_ == 0 )
		close();
	else
		ui_.timerL->setText(QString::number(countDown_));
}
