#include "KeysPage.h"

#include <QAction>
#include <QHeaderView>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPushButton>

#include "KeySettings.h"
#include "Log.h"

QAction* action(QTreeWidgetItem*, CommandStorageInt*);

KeysPage::KeysPage(CommandStorageInt* storage) : QWidget(), cur_(0) {
//	LOGGER;
	storage_ = storage;
	
	ui.setupUi(this);
	ui.unsavedL->hide();
	
	QStringList headers;
	headers << "" << "Action" << "Shortcut";
	ui.keysTree->setHeaderLabels(headers);
	ui.keysTree->setRootIsDecorated(false);
	ui.keysTree->setAllColumnsShowFocus(true);
#if QT_VERSION < 0x050000
	ui.keysTree->header()->setResizeMode(0, QHeaderView::Fixed);
	ui.keysTree->header()->setResizeMode(1, QHeaderView::Stretch);
	ui.keysTree->header()->setResizeMode(2, QHeaderView::Fixed);
#else
	ui.keysTree->header()->setSectionResizeMode(0, QHeaderView::Fixed);
	ui.keysTree->header()->setSectionResizeMode(1, QHeaderView::Stretch);
	ui.keysTree->header()->setSectionResizeMode(2, QHeaderView::Fixed);
#endif
	ui.keysTree->setColumnWidth(0, style()->pixelMetric(QStyle::PM_SmallIconSize) + 2 * style()->pixelMetric(QStyle::PM_HeaderMargin));
	ui.keysTree->setColumnWidth(2, 50);
	
	connect(ui.keysTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));
}

void KeysPage::init() {
	QStringList ids = storage_->actionIDs();
	foreach (QString id, ids) {
		QAction* a = storage_->action(id);
		if ( NULL != a ) {
			QStringList list;
			// let's remove "&" from action's text to fix #6: Shortcut manager shows extra "&"
			list << ""<< a->text().replace("&", "") << a->shortcut().toString();
			QTreeWidgetItem* item = new QTreeWidgetItem(list);
			item->setIcon(0, a->icon());
			item->setData(3, Qt::UserRole + 1, id);
			ui.keysTree->addTopLevelItem(item);
		}
	}
}

void KeysPage::apply() {
//	LOGGER;
	foreach (int index, changedItems_ ) {
		QTreeWidgetItem* item = ui.keysTree->topLevelItem(index);
		if ( item != NULL ) {
			QString id = item->data(3, Qt::UserRole + 1).toString();
			if ( !id.isEmpty() ) {
				{ // if ( id != NULL ) {
					KeySettings::setKeySequence(id, item->text(2));
					storage_->setShortcut(id,  item->text(2));
				}
			}
		}
	}
	changedItems_.clear();
	ui.unsavedL->hide();
}

void KeysPage::onItemDoubleClicked(QTreeWidgetItem* item, int col) {
//	LOGGER;
	if ( item != 0 && col == 2 ) {
		if ( cur_ != 0 ) {
			restore();
		}
		oldText_ = item->text(2);
		item->setText(2, tr("Press a key sequence"));
		cur_ = item;
	}
}

void KeysPage::keyPressEvent(QKeyEvent* e) {
//	LOGGER;
	if ( cur_ != 0 ) {
		if ( e->key() == Qt::Key_Escape ) {
			restore();
			cur_ = 0;
		}
		else {
			QString text, mods;
			bool singleAllowed = false;
			Qt::KeyboardModifiers m = e->modifiers();
			if ( m & Qt::ShiftModifier )
				mods += "Shift+";
			if ( m & Qt::ControlModifier )
				mods += "Ctrl+";
			if ( m & Qt::AltModifier )
				mods += "Alt+";
			switch ( e->key() ) {
				case Qt::Key_A : text = "A"; break;
				case Qt::Key_B : text = "B"; break;
				case Qt::Key_C : text = "C"; break;
				case Qt::Key_D : text = "D"; break;
				case Qt::Key_E : text = "E"; break;
				case Qt::Key_F : text = "F"; break;
				case Qt::Key_G : text = "G"; break;
				case Qt::Key_H : text = "H"; break;
				case Qt::Key_I : text = "I"; break;
				case Qt::Key_J : text = "J"; break;
				case Qt::Key_K : text = "K"; break;
				case Qt::Key_L : text = "L"; break;
				case Qt::Key_M : text = "M"; break;
				case Qt::Key_N : text = "N"; break;
				case Qt::Key_O : text = "O"; break;
				case Qt::Key_P : text = "P"; break;
				case Qt::Key_Q : text = "Q"; break;
				case Qt::Key_R : text = "R"; break;
				case Qt::Key_S : text = "S"; break;
				case Qt::Key_T : text = "T"; break;
				case Qt::Key_U : text = "U"; break;
				case Qt::Key_V : text = "V"; break;
				case Qt::Key_W : text = "W"; break;
				case Qt::Key_X : text = "X"; break;
				case Qt::Key_Y : text = "Y"; break;
				case Qt::Key_Z : text = "Z"; break;

				case Qt::Key_0 : text = "0"; break;
				case Qt::Key_1 : text = "1"; break;
				case Qt::Key_2 : text = "2"; break;
				case Qt::Key_3 : text = "3"; break;
				case Qt::Key_4 : text = "4"; break;
				case Qt::Key_5 : text = "5"; break;
				case Qt::Key_6 : text = "6"; break;
				case Qt::Key_7 : text = "7"; break;
				case Qt::Key_8 : text = "8"; break;
				case Qt::Key_9 : text = "9"; break;

				case Qt::Key_F1 :         text = "F1"; singleAllowed = true; break;
				case Qt::Key_F2 :         text = "F2"; singleAllowed = true; break;
				case Qt::Key_F3 :         text = "F3"; singleAllowed = true; break;
				case Qt::Key_F4 :         text = "F4"; singleAllowed = true; break;
				case Qt::Key_F5 :         text = "F5"; singleAllowed = true; break;
				case Qt::Key_F6 :         text = "F6"; singleAllowed = true; break;
				case Qt::Key_F7 :         text = "F7"; singleAllowed = true; break;
				case Qt::Key_F8 :         text = "F8"; singleAllowed = true; break;
				case Qt::Key_F9 :         text = "F9"; singleAllowed = true; break;
				case Qt::Key_F10 :        text = "F10"; singleAllowed = true; break;
				case Qt::Key_F11 :        text = "F11"; singleAllowed = true; break;
				case Qt::Key_F12 :        text = "F12"; singleAllowed = true; break;
				case Qt::Key_Escape :     text = "Esc"; singleAllowed = true; break;

				case Qt::Key_Apostrophe : text = "'"; break;
				case Qt::Key_QuoteDbl :   text = "'"; break;
				case Qt::Key_Equal :      text = "="; break;
				case Qt::Key_Minus :      text = "-"; break;
				case Qt::Key_Underscore : text = "-"; break;
				case Qt::Key_Comma :      text = ","; break;
				case Qt::Key_Less:        text = ","; break;
				case Qt::Key_Period :     text = "."; break;
				case Qt::Key_Greater:     text = "."; break;
				case Qt::Key_Semicolon:   text = ";"; break;
				case Qt::Key_Colon:       text = ";"; break;
				
				case Qt::Key_Exclam:      text = mods.contains("Shift") ? "1" : "!"; break;
				case Qt::Key_At:          text = mods.contains("Shift") ? "2" : "@"; break;
				case Qt::Key_NumberSign:  text = mods.contains("Shift") ? "3" : "#"; break;
				case Qt::Key_Dollar:      text = mods.contains("Shift") ? "4" : "$"; break;
				case Qt::Key_Percent:     text = mods.contains("Shift") ? "5" : "%"; break;
				case Qt::Key_AsciiCircum: text = mods.contains("Shift") ? "6" : "^"; break;
				case Qt::Key_Ampersand:   text = mods.contains("Shift") ? "7" : "&"; break;
				case Qt::Key_Asterisk:    text = mods.contains("Shift") ? "8" : "*"; break;
				case Qt::Key_ParenLeft:   text = mods.contains("Shift") ? "9" : "("; break;
				case Qt::Key_ParenRight:  text = mods.contains("Shift") ? "0" : ")"; break;
				case Qt::Key_Plus:        text = mods.contains("Shift") ? "=" : "+"; break;

				case Qt::Key_Left:        text = "Left"; break;
				case Qt::Key_Right:       text = "Right"; break;
				case Qt::Key_Up:          text = "Up"; break;
				case Qt::Key_Down:        text = "Down"; break;

				default: return;
			}
			if ( mods.isEmpty() && !singleAllowed ) {
				return;
			}
			
			QKeySequence newSeq(mods+text);
			int count = ui.keysTree->topLevelItemCount();
			bool conflict = false;
			for (int i = 0; i < count; ++i) {
				QTreeWidgetItem* it = ui.keysTree->topLevelItem(i);
				QKeySequence ks(it->text(2));
				if ( ks == newSeq ) {
					conflict = true;
					QMessageBox mBox(this);
					QString text = tr("This shortcut \"%1\" is already assigned to action \"%2\".\n"
					                  "Do you want to reassign it to the current action?").arg(ks.toString()).arg(it->text(1));
					mBox.setText(text);
					mBox.setWindowTitle(tr("Warning"));
					mBox.setIcon(QMessageBox::Warning);
					QPushButton* yesBtn = mBox.addButton(QMessageBox::Yes);
					mBox.addButton(QMessageBox::No);
					mBox.exec();
					if ( mBox.clickedButton() == yesBtn ) {
						it->setText(2, "");
						changedItems_ << i;
						cur_->setText(2, newSeq.toString());
					}
					else {
						restore();
						cur_ = 0;
					}
					break;
				}
			}
			if ( !conflict ) {
				cur_->setText(2, mods+text);
			}
			
			
			// TODO : change later (this patr was copy-pasted below)
			int index = ui.keysTree->indexOfTopLevelItem(cur_);
			if ( index >= 0 ) {
				QAction* a = action(cur_, storage_);
				if ( a != NULL ) {
					if ( a->shortcut() == QKeySequence(mods+text) ) {
						changedItems_.removeAll(index);
						if ( changedItems_.isEmpty() )
							ui.unsavedL->hide();
					}
					else {
						changedItems_ << index;
						ui.unsavedL->show();
					}
				}
			}
			// end of copy-paste
			
			cur_ = 0;
		}
	}
	else {
		if ( e->key() == Qt::Key_Delete ) {
			QTreeWidgetItem* item = ui.keysTree->currentItem();
			if ( NULL != item ) {
				item->setText(2, "");
				
				// TODO : change later (it's a copy-paste from above)
				int index = ui.keysTree->indexOfTopLevelItem(item);
				if ( index >= 0 ) {
					QAction* a = action(item, storage_);
					if ( a != NULL ) {
						if ( a->shortcut() == QKeySequence("") ) {
							changedItems_.removeAll(index);
							if ( changedItems_.isEmpty() )
								ui.unsavedL->hide();
						}
						else {
							changedItems_ << index;
							ui.unsavedL->show();
						}
					}
				}
				// end of copy-paste
			}
		}
		else {
			QWidget::keyPressEvent(e);
		}
	}
}

void KeysPage::restore() {
//	LOGGER;
	if ( NULL != cur_ ) {
		cur_->setText(2, oldText_);
	}
}

QAction* action(QTreeWidgetItem* item, CommandStorageInt* storage) {
//	LOGGER;
	QString id = item->data(3, Qt::UserRole + 1).toString();
	QAction* a = storage->action(id);
	return a;
}
