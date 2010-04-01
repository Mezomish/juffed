#include "TabBar.h"

#include "Log.h"
#include "TabWidget.h"

#include <QMouseEvent>

namespace Juff {

TabBar::TabBar(Juff::TabWidget* widget) : QTabBar(), index_(-1), tabMenu_(NULL), tabWidget_(widget) {
	setFocusPolicy(Qt::NoFocus);
	
//	tabMenu_ = new QMenu();
//	tabMenu_->addAction(tr("Clone"), this, SLOT(cloneDoc()));
//	tabMenu_->addAction(tr("Close"), this, SLOT(closeDoc()));
//	tabMenu_->addSeparator();
//	tabMenu_->addAction(CommandStorage::instance()->action(ID_FILE_CLOSE_ALL));
//	tabMenu_->addAction(CommandStorage::instance()->action(ID_FILE_SAVE_ALL));

#if QT_VERSION >= 0x040500
	setMovable(true);
#endif
}

void TabBar::mouseReleaseEvent(QMouseEvent* e) {

	if ( e->button() & Qt::MidButton ) {
		int index = tabAt(e->pos());
		emit requestTabClose(index);
	}
	else if ( e->button() & Qt::RightButton ) {
		index_ = tabAt(e->pos());
		if ( tabMenu_ != NULL )
			delete tabMenu_;
		
		tabMenu_ = new QMenu();
		tabWidget_->initDocMenu(index_, tabMenu_);
		
		if ( !tabMenu_->isEmpty() )
			tabMenu_->addSeparator();
		
		tabMenu_->addAction(tr("Close"), this, SLOT(closeDoc()));
		
		tabMenu_->popup(e->globalPos());
	}
	
	QTabBar::mouseReleaseEvent(e);
}


/*void TabBar::cloneDoc() {
}*/

void TabBar::closeDoc() {
	LOGGER;
	
	emit requestTabClose(index_);
}

} // namespace Juff
