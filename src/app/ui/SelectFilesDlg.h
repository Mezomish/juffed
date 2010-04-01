#ifndef __JUFFED_SELECT_FILES_DLG_H__
#define __JUFFED_SELECT_FILES_DLG_H__

#include "ui_SelectFilesDlg.h"

class SelectFilesDlg : public QDialog {
Q_OBJECT
public:
	SelectFilesDlg(const QStringList&, QWidget*);

	QStringList checkedFiles() const;

	virtual void accept();
	virtual void reject();

private slots:
	void onAllClicked();
	void onItemChanged(QListWidgetItem*);

private:
	Ui::SelectFilesDlg ui_;
	QStringList list_;
};

#endif // __JUFFED_SELECT_FILES_DLG_H__
