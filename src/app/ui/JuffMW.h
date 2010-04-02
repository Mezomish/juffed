#ifndef __JUFFED_MAIN_WINDOW_H__
#define __JUFFED_MAIN_WINDOW_H__

class QVBoxLayout;

#include <QMainWindow>

namespace Juff {
	class Document;
}

class JuffMW : public QMainWindow {
Q_OBJECT
public:
	JuffMW();

	void setViewer(QWidget*);
	QString getOpenFileName(const QString& dir, const QString& filters);
	QStringList getOpenFileNames(const QString& dir, const QString& filters);
	QString getSaveFileName(const QString& curFileName, const QString& filters);
	QString getSavePrjName(const QString& title);
	int getGotoLineNumber(int);
	int askForSave(const QString& fileName);
	bool askForSave(const QStringList&, QStringList&);

	// information display
	void addStatusWidget(QWidget*);
	void message(const QIcon& icon, const QString& title, const QString& message, int timeout = 10);

signals:
	void closeRequested(bool&);

protected:
	virtual void closeEvent(QCloseEvent*);
	
private:
	QWidget* viewer_;
	QWidget* mainWidget_;
	QVBoxLayout* vBox_;
};

#endif // __JUFFED_MAIN_WINDOW_H__
