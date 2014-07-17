#include "DocListPanel.h"

#include <QHeaderView>

DocListPanel::DocListPanel() : QWidget() {
	setWindowTitle(tr("Documents"));
	
	tree_ = new TreeWidget();
#if QT_VERSION < 0x050200
	filter_ = new FilterLineEdit();
	filter_->setMaximumHeight(24);
#else
	filter_ = new QLineEdit;
	filter_->setPlaceholderText(tr("Filter"));
	filter_->setClearButtonEnabled(true);
#endif
	QVBoxLayout* vBox = new QVBoxLayout();
	vBox->setMargin(0);
	vBox->setSpacing(2);
	setLayout(vBox);
	
	vBox->addWidget(tree_);
	vBox->addWidget(filter_);
	
	QStringList labels;
	labels << "Name" << "Full name";
	tree_->setHeaderLabels(labels);

	tree_->header()->hide();
	tree_->setColumnHidden(1, true);
	tree_->setRootIsDecorated(false);
	
#if QT_VERSION < 0x050200
	connect(filter_->lineEd_, SIGNAL(textChanged(const QString&)), SLOT(filterItems(const QString&)));
	connect(filter_->clearBtn_, SIGNAL(clicked()), SLOT(clear()));
#else
	connect(filter_, SIGNAL(textChanged(const QString&)), SLOT(filterItems(const QString&)));
#endif
}

void DocListPanel::filterItems(const QString& text) {
	int n = tree_->topLevelItemCount();
	for (int i = 0; i < n; ++i) {
		QTreeWidgetItem* item = tree_->topLevelItem(i);
		if ( item->text(0).toLower().contains(text.toLower()) ) {
			item->setHidden(false);
		}
		else {
			item->setHidden(true);
		}
	}
}

#if QT_VERSION < 0x050200
void DocListPanel::clear() {
	filter_->lineEd_->setText("");
}
#endif
