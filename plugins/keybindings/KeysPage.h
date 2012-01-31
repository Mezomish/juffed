#ifndef __KEYS_PAGE_H__
#define __KEYS_PAGE_H__

#include <QList>

#include "ui_KeysPage.h"
#include "CommandStorageInt.h"

class KeysPage : public QWidget {
Q_OBJECT
public:
	KeysPage(CommandStorageInt*);

	void init();
	void apply();

protected:
	virtual void keyPressEvent(QKeyEvent*);

private slots:
	void onItemDoubleClicked(QTreeWidgetItem*, int);

private:
	void restore();

	Ui::KeysPage ui;
	QTreeWidgetItem* cur_;
	QString oldText_;
	QList<int> changedItems_;
	CommandStorageInt* storage_;
};

#endif /* __KEYS_PAGE_H__ */
