#ifndef __MANAGE_DLG_H__
#define __MANAGE_DLG_H__

#include "ui_ManageDlg.h"

class JuffPlugin;

class ManageDlg : public QDialog {
Q_OBJECT
public:
	ManageDlg(QWidget*, JuffPlugin*);

private slots:
	void deleteItem();
	void close();

private:
	Ui::ManageDlg ui;
	QStringList favorites_;
	JuffPlugin* plugin_;
};

#endif /* __MANAGE_DLG_H__ */
