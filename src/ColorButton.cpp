/*
JuffEd - A simple text editor
Copyright 2007-2008 Mikhail Murzin

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
	setBtnColor(color);
}

ColorButton::~ColorButton() {
	JUFFDTOR;
}

void ColorButton::clicked() {
	QColor c = QColorDialog::getColor(color_);
	if (c.isValid()) {
		setBtnColor(c);
	}
}

void ColorButton::setBtnColor(const QColor& color) {
	if (btn_ == 0)
		return;

	color_ = color;

	QPixmap pm(btn_->width() - 10, btn_->height() - 10);
	QPainter p(&pm);
	p.setBrush(color);
	p.setPen(QPen(color));
	p.drawRect(0, 0, pm.width(), pm.height());
	btn_->setIconSize(pm.size());
	btn_->setIcon(QIcon(pm));
}

