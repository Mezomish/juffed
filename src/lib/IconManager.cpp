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

#include <QIcon>
#include <QMap>

#include "Constants.h"

static QMap<QString, QString> IconFileNames;

IconManager* IconManager::instance_ = NULL;

IconManager::IconManager() {
	size_ = 16;
	
	IconFileNames[FILE_NEW]        = "document-new.png";
	IconFileNames[FILE_OPEN]       = "document-open.png";
	IconFileNames[FILE_SAVE]       = "document-save.png";
	IconFileNames[FILE_SAVE_AS]    = "document-save-as.png";
	IconFileNames[FILE_RELOAD]     = "view-refresh.png";
	IconFileNames[FILE_PRINT]      = "document-print.png";
	IconFileNames[FILE_EXIT]       = "system-log-out.png";
	
	IconFileNames[EDIT_UNDO]       = "edit-undo.png";
	IconFileNames[EDIT_REDO]       = "edit-redo.png";
	IconFileNames[EDIT_CUT]        = "edit-cut.png";
	IconFileNames[EDIT_COPY]       = "edit-copy.png";
	IconFileNames[EDIT_PASTE]      = "edit-paste.png";
	
	IconFileNames[SEARCH_FIND]     = "edit-find.png";
	IconFileNames[SEARCH_REPLACE]  = "edit-find-replace.png";
	
	IconFileNames[VIEW_ZOOM_IN]    = "zoomIn.png";
	IconFileNames[VIEW_ZOOM_OUT]   = "zoomOut.png";
	IconFileNames[VIEW_ZOOM_100]   = "zoom100.png";
	IconFileNames[VIEW_FULLSCREEN] = "view-fullscreen.png";
	
	IconFileNames[TOOLS_SETTINGS]  = "preferences-system.png";
}

IconManager* IconManager::instance() {
	if ( instance_ == NULL )
		instance_ = new IconManager();
	return instance_;
}

void IconManager::setSize(int sz) {
	size_ = sz;
}

int IconManager::size() const {
	return size_;
}

QIcon IconManager::icon(const QString& key) const {
	QString module = key.section(':', 0, 0);
	QString name = key.section(':', 1, 1);
	if ( module.compare("main") == 0 ) {
		return defaultIcon(key);
	}
	
	return QIcon();
}

QIcon IconManager::defaultIcon(const QString& key) const {
	if ( IconFileNames.contains(key) ) {
		return QIcon(QString(":%1/%2").arg(size_).arg(IconFileNames[key]));
	}
	else
		return QIcon();
}
