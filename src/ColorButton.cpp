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
	Log::print("Color btn destroyed");
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

	Log::print(QString("%1 %2").arg(btn_->width()).arg(btn_->height()));
	QPixmap pm(btn_->width() - 10, btn_->height() - 10);
	QPainter p(&pm);
	p.setBrush(color);
	p.setPen(QPen(color));
	p.drawRect(0, 0, pm.width(), pm.height());
	btn_->setIconSize(pm.size());
	btn_->setIcon(QIcon(pm));
}

