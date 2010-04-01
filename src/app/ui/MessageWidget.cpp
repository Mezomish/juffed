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
