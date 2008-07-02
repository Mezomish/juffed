#ifndef _COLOR_BUTTON_H_
#define _COLOR_BUTTON_H_

#include <QtCore/QObject>
#include <QtGui/QColor>

class QPushButton;
	
class ColorButton : public QObject {
Q_OBJECT
public:
	ColorButton(QPushButton*, const QColor& color);
	virtual ~ColorButton();

	QColor color() const {
		return color_;
	}

public slots:
	void clicked();

private:
	void setBtnColor(const QColor&);

	QColor color_;
	QPushButton* btn_;
};

#endif
