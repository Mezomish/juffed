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
