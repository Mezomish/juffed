#ifndef __DOC_LIST_PLUGIN_PANEL_H__
#define __DOC_LIST_PLUGIN_PANEL_H__

#include <QContextMenuEvent>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QWidget>
#include <QTreeWidget>

static const int BtnSize = 24;

#if QT_VERSION < 0x050200
class FilterLineEdit : public QWidget {
public:
	FilterLineEdit(QWidget* parent = 0) : QWidget(parent) {
		lineEd_ = new QLineEdit("", this);
		clearBtn_ = new QPushButton(QIcon(":clear"), "", this);
		clearBtn_->setFlat(true);
		clearBtn_->setFocusPolicy(Qt::NoFocus);
		clearBtn_->setCursor(Qt::ArrowCursor);
		clearBtn_->setMaximumWidth(BtnSize);
		
		QHBoxLayout* hBox = new QHBoxLayout(this);
		hBox->setMargin(0);
		hBox->setSpacing(0);
		hBox->addWidget(lineEd_);
#if QT_VERSION >= 0x040500
		lineEd_->setTextMargins(0, 0, BtnSize, 0);
		clearBtn_->setParent(lineEd_);
#else
		hBox->addWidget(clearBtn_);
#endif
	}
	
#if QT_VERSION >= 0x040500
	virtual void resizeEvent(QResizeEvent*) {
		clearBtn_->setGeometry(lineEd_->width() - BtnSize, (lineEd_->height() - BtnSize) / 2, BtnSize, BtnSize);
	}
#endif
	
	QLineEdit* lineEd_;
	QPushButton* clearBtn_;
};
#endif

class TreeWidget : public QTreeWidget {
public:
	TreeWidget() : QTreeWidget() {
		contextMenu_ = new QMenu();
//		contextMenu_->addAction(CommandStorage::instance()->action(Juff::FileSave));
//		contextMenu_->addAction(CommandStorage::instance()->action(Juff::FileClose));
	}
	virtual ~TreeWidget() {
		delete contextMenu_;
	}
	
	virtual void contextMenuEvent(QContextMenuEvent * event) {
		contextMenu_->popup(event->globalPos());
	}
	
	QMenu* contextMenu_;
};



class DocListPanel : public QWidget {
Q_OBJECT
public:
	DocListPanel();

	// Yes, it's an incapsulation violation but I'll better keep
	// it this way (for now) than introduce either 5 additional
	// methods to manage these members or two methods disclosing them
	// completely (which has no difference between it and currently 
	// selected approach).
	TreeWidget* tree_;
#if QT_VERSION < 0x050200
	FilterLineEdit* filter_;
#else
	QLineEdit* filter_;
#endif

public slots:
	void filterItems(const QString& text);
#if QT_VERSION < 0x050200
	void clear();
#endif
};

#endif
