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

#include "ColorButton.h"

#include <QtGui/QColorDialog>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtGui/QPushButton>

#include "Log.h"

ColorButton::ColorButton(QPushButton* btn, const QColor& color) : QObject(btn) {
	btn_ = btn;
	
	connect(btn, SIGNAL(clicked()), this, SLOT(clicked()));
	setColor(color);
}

ColorButton::~ColorButton() {
}

void ColorButton::clicked() {
	QColor c = QColorDialog::getColor(color_);
	if (c.isValid()) {
		setColor(c);
	}
}

void ColorButton::setColor(const QColor& color) {
	if (btn_ == 0)
		return;

	color_ = color;

#ifdef Q_WS_MAC
	// Mac buttons are quite special. It looks like
	// the safest way how to do a color button is
	// to have a plain corner icon in the button.
	// Well, I can compute a button borders (rounded etc.)
	// but it's terrible slow...
	QPixmap pm(32, 16);
	QPainter p(&pm);
	p.setBrush(color);
	p.setPen(Qt::black);
	p.drawRect(0, 0, pm.width()-1, pm.height()-1);
#else
	QPixmap pm(btn_->width() - 10, btn_->height() - 10);
	QPainter p(&pm);
	//	Draw a rectangle of selected color
	//	with light grey border
	p.setBrush(color);
	p.setPen(QPen(QColor(200, 200, 200)));
	p.drawRect(0, 0, pm.width(), pm.height());
	//	Draw dark grey angle to make kind of a relief
	p.setPen(QPen(QColor(50, 50, 50)));
	p.drawLine(0, 0, pm.width(), 0);
	p.drawLine(0, 0, 0, pm.height());
#endif

	btn_->setIconSize(pm.size());
	btn_->setIcon(QIcon(pm));
}

