#ifndef __JUFFED_JUMP_TO_FILE_DLG_H__
#define __JUFFED_JUMP_TO_FILE_DLG_H__

#include <QDialog>

#include "ui_JumpToFileDlg.h"

class JumpToFileDlg : public QDialog {
Q_OBJECT
public:
	JumpToFileDlg(const QStringList&, QWidget*);

	QString fileName() const;

public slots:
	void onItemDoubleClicked(QListWidgetItem*);
	void onTextEdited(const QString&);
	void onReturnPressed();

protected:
	virtual bool eventFilter(QObject*, QEvent*);

private:
	Ui::JumpToFileDlg ui_;
};

#endif // __JUFFED_JUMP_TO_FILE_DLG_H__
