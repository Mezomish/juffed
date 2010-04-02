/*
JuffEd - An advanced text editor
Copyright 2007-2010 Mikhail Murzin

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License 
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "IconManager.h"

#include "Log.h"

#include <QMap>

IconManager* IconManager::instance_ = NULL;

class IconManager::Interior {
public:
	Interior() {
		size_ = 16;
		theme_ = "<default>";
		
		iconNames_[Juff::FileNew]      = "document-new.png";
		iconNames_[Juff::FileOpen]     = "document-open.png";
		iconNames_[Juff::FileSave]     = "document-save.png";
		iconNames_[Juff::FileSaveAs]   = "document-save-as.png";
//		iconNames_[Juff::FileSaveAll]  = "";
		iconNames_[Juff::FileReload]   = "view-refresh.png";
		iconNames_[Juff::FilePrint]    = "document-print.png";
//		iconNames_[Juff::FileRename]   = "";
//		iconNames_[Juff::FileClose]    = "";
//		iconNames_[Juff::FileCloseAll] = "";
		iconNames_[Juff::FilePrint]    = "document-print";
		iconNames_[Juff::FileExit]     = "system-log-out";

		iconNames_[Juff::EditUndo]     = "edit-undo.png";
		iconNames_[Juff::EditRedo]     = "edit-redo.png";
		iconNames_[Juff::EditCut]      = "edit-cut.png";
		iconNames_[Juff::EditCopy]     = "edit-copy.png";
		iconNames_[Juff::EditPaste]    = "edit-paste.png";
		iconNames_[Juff::EditFind]     = "edit-find.png";
//		iconNames_[Juff::EditFindNext] = "";
//		iconNames_[Juff::EditFindPrev] = "";
		iconNames_[Juff::EditReplace]  = "edit-find-replace.png";
		iconNames_[Juff::GotoLine] = "";
		
		iconNames_[Juff::ViewZoomIn]     = "zoomIn.png";
		iconNames_[Juff::ViewZoomOut]    = "zoomOut.png";
		iconNames_[Juff::ViewZoom100]    = "zoom100.png";
		iconNames_[Juff::ViewFullscreen] = "view-fullscreen.png";
	}
	
	int size_;
	QString theme_;
//	QMap<Juff::ActionID, QIcon> icons_;
	QMap<Juff::ActionID, QString> iconNames_;
};

IconManager::IconManager() {
	LOGGER;
	int_ = new Interior();
}

IconManager* IconManager::instance() {
	if ( instance_ == NULL)
		instance_ = new IconManager();
	return instance_;
}

int IconManager::iconSize() const {
	return int_->size_;
}

void IconManager::setIconSize(int size) {
	int_->size_ = size;
}

QString IconManager::iconTheme() const {
	return int_->theme_;
}

void IconManager::setIconTheme(const QString& theme) {
	int_->theme_ = theme;
}

QIcon IconManager::icon(Juff::ActionID id) const {
//	LOGGER;
//	if ( icons_.contains(id) )
//		return icons_[id];

	if ( int_->theme_.compare("<default>") == 0 ) {
		return defaultIcon(id);
	}
	
	return QIcon();
}

QIcon IconManager::defaultIcon(Juff::ActionID id) const {
//	LOGGER;
	if ( int_->iconNames_.contains(id) ) {
		return QIcon(QString(":%1/%2").arg(int_->size_).arg(int_->iconNames_[id]));
	}
	else
		return QIcon();
}
