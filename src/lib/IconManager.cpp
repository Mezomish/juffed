/*
JuffEd - A simple text editor
Copyright 2007-2009 Mikhail Murzin

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

#include <QtCore/QDir>
#include <QtCore/QMap>
#include <QtCore/QString>

#include "Log.h"
#include "MainSettings.h"

class IMInterior {
public:
	IMInterior() : default_(true) {
		defaultIcons_[Juff::ID_FILE_NEW]		= "fileNew.png";
		defaultIcons_[Juff::ID_FILE_OPEN]		= "fileOpen.png";
		defaultIcons_[Juff::ID_FILE_SAVE]		= "fileSave.png";
		defaultIcons_[Juff::ID_FILE_SAVE_AS]	= "fileSaveAs.png";
		defaultIcons_[Juff::ID_FILE_PRINT]		= "filePrint.png";
		defaultIcons_[Juff::ID_EXIT]			= "exit.png";
		defaultIcons_[Juff::ID_EDIT_CUT] 		= "editCut.png";
		defaultIcons_[Juff::ID_EDIT_COPY]		= "editCopy.png";
		defaultIcons_[Juff::ID_EDIT_PASTE]		= "editPaste.png";
		defaultIcons_[Juff::ID_EDIT_UNDO]		= "editUndo.png";
		defaultIcons_[Juff::ID_EDIT_REDO]		= "editRedo.png";
		defaultIcons_[Juff::ID_ZOOM_IN]			= "zoomIn.png";
		defaultIcons_[Juff::ID_ZOOM_OUT]		= "zoomOut.png";
		defaultIcons_[Juff::ID_ZOOM_100]		= "zoom100.png";
		defaultIcons_[Juff::ID_FIND]			= "find.png";
		defaultIcons_[Juff::ID_REPLACE]			= "replace.png";
		defaultIcons_[Juff::ID_SETTINGS]		= "settings.png";
		defaultIcons_[Juff::ID_ABOUT]			= "about.png";
		
		icons_[Juff::ID_FILE_NEW]		= "actions/document-new";
		icons_[Juff::ID_FILE_OPEN]		= "actions/document-open";
		icons_[Juff::ID_FILE_SAVE]		= "actions/document-save";
		icons_[Juff::ID_FILE_SAVE_AS]	= "actions/document-save-as";
		icons_[Juff::ID_FILE_PRINT]		= "actions/document-print";
		icons_[Juff::ID_FILE_RELOAD]	= "actions/view-refresh";
		icons_[Juff::ID_EXIT]			= "actions/exit";
		icons_[Juff::ID_EDIT_CUT] 		= "actions/edit-cut";
		icons_[Juff::ID_EDIT_COPY]		= "actions/edit-copy";
		icons_[Juff::ID_EDIT_PASTE]		= "actions/edit-paste";
		icons_[Juff::ID_EDIT_UNDO]		= "actions/edit-undo";
		icons_[Juff::ID_EDIT_REDO]		= "actions/edit-redo";
		icons_[Juff::ID_ZOOM_IN]		= "actions/zoom-in";
		icons_[Juff::ID_ZOOM_OUT]		= "actions/zoom-out";
		icons_[Juff::ID_ZOOM_100]		= "actions/zoom-original";
		icons_[Juff::ID_FIND]			= "actions/edit-find";
		icons_[Juff::ID_REPLACE]		= "actions/edit-find-replace";
		icons_[Juff::ID_SETTINGS]		= "actions/configure";
		icons_[Juff::ID_ABOUT]			= "apps/help";
		
		theme_ = "<default>";
		size_ = 1;
		
		sizeSeq_.resize(3);
		sizeSeq_[0] << 16 << 24 << 32 << 48;
		sizeSeq_[1] << 24 << 32 << 48 << 16;
		sizeSeq_[2] << 32 << 48 << 24 << 16;
	}
	
	typedef QMap<QString, QIcon> IconMap;
	IconMap iconMap_;
	bool default_;
	
	QString theme_;
	int size_;
	typedef QVector< QVector<int> > SzSeq;
	SzSeq sizeSeq_;
	QMap<Juff::CommandID, QString> defaultIcons_;
	QMap<Juff::CommandID, QString> icons_;
};


void IconManager::setCurrentIconTheme(const QString& theme, int size) {
	imInt_->theme_ = theme;
	imInt_->size_ = size;
}

QStringList IconManager::themeList() const {
	JUFFENTRY;
	
	QDir iconDir("/usr/share/icons");
	QStringList dirList = iconDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	QStringList thList;
	for (QStringList::iterator it = dirList.begin(); it != dirList.end(); ++it) {
		QString fullDirPath = iconDir.absolutePath() + "/" + *it;
		if ( QFile::exists(fullDirPath + "/32x32/actions/document-open.png") 
				|| QFile::exists(fullDirPath + "/24x24/actions/document-open.png") 
				|| QFile::exists(fullDirPath + "/16x16/actions/document-open.png") 
				|| QFile::exists(fullDirPath + "/scalable/actions/document-open.svg") 
		) 
		{
			thList << *it;
		}
	}
	return thList;
}

QIcon IconManager::getIcon(Juff::CommandID id) {
	JUFFENTRY;
	JUFFDEBUG((int)id);
	
	if ( !imInt_->icons_.contains(id) ) {
		return getDefaultIcon(id);
	}
	
	QString iconDir("/usr/share/icons");
	QString iconFileName = imInt_->icons_[id];
	if ( imInt_->theme_.compare("<default>") == 0 ) {
		return getDefaultIcon(id);
	}
	else {
		for (int i = 0; i < 4; ++i) {
			int sz = imInt_->sizeSeq_[imInt_->size_][i];
			QString fileName = iconDir + "/" + imInt_->theme_ + "/" + 
							QString("%1x%2/").arg(sz).arg(sz) + iconFileName + ".png";
			JUFFDEBUG(fileName);
			if ( QFileInfo(fileName).exists() ) {
				JUFFDEBUG("PNG icon found");
				return QIcon(fileName);
			}
		}
		//	If we are here then we didn't find a PNG icon. 
		//	Try to find SVG.
		QString svgIconFileName = iconDir + "/" + imInt_->theme_ + "/scalable/" + 
							iconFileName + ".svg";
		JUFFDEBUG(svgIconFileName);
		if ( QFileInfo(svgIconFileName).exists() ) {
			JUFFDEBUG("SVG icon found");
			return QIcon(svgIconFileName);
		}
		else {
			JUFFDEBUG("Not found. Returning default icon");
			return getDefaultIcon(id);
		}
	}
}

QIcon IconManager::getDefaultIcon(Juff::CommandID id) {
	JUFFENTRY;
	JUFFDEBUG((int)id);
	
	if ( imInt_->defaultIcons_.contains(id) ) {
		QString iconFileName = imInt_->defaultIcons_[id];
		for (int i = 0; i < 4; ++i) {
			int sz = imInt_->sizeSeq_[imInt_->size_][i];
			QString fileName = QString(":%1/").arg(sz) + iconFileName;
			JUFFDEBUG(fileName);
			if ( QFileInfo(fileName).exists() ) {
				return QIcon(fileName);
			}
		}
	}

	JUFFDEBUG("empty icon");
	return QIcon();
}

IconManager::IconManager() {
	JUFFENTRY;
	imInt_ = new IMInterior();
}

IconManager::~IconManager() {
	JUFFDTOR;
	delete imInt_;
}

IconManager* IconManager::instance() {
	if (instance_ == 0)
		instance_ = new IconManager();
	return instance_;
}

IconManager* IconManager::instance_ = 0;
