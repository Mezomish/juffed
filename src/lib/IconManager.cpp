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
#include <QtCore/QPair>
#include <QtCore/QString>

//#include "AppInfo.h"
#include "Log.h"
#include "MainSettings.h"

class IMInterior {
public:
	IMInterior() : default_(true) {
		idIconMap_[Juff::ID_FILE_NEW] = QPair<QString, QString>("actions/document-new.png", "fileNew.png");
		idIconMap_[Juff::ID_FILE_OPEN] = QPair<QString, QString>("actions/document-open.png", "fileOpen.png");
		idIconMap_[Juff::ID_FILE_SAVE] = QPair<QString, QString>("actions/document-save.png", "fileSave.png");
		idIconMap_[Juff::ID_FILE_SAVE_AS] = QPair<QString, QString>("actions/document-save-as.png", "fileSaveAs.png");
		idIconMap_[Juff::ID_FILE_CLOSE] = QPair<QString, QString>("", "");
		idIconMap_[Juff::ID_FILE_CLOSE_ALL] = QPair<QString, QString>("", "");
		idIconMap_[Juff::ID_EXIT] = QPair<QString, QString>("actions/exit.png", "exit.png");
//		idIconMap_[Juff::ID_FILE_RELOAD] = QPair<QString, QString>("actions/view-refresh.png", "");
		idIconMap_[Juff::ID_FILE_PRINT] = QPair<QString, QString>("actions/document-print.png", "filePrint.png");
		//
		idIconMap_[Juff::ID_EDIT_CUT] = QPair<QString, QString>("actions/edit-cut.png", "editCut.png");
		idIconMap_[Juff::ID_EDIT_COPY] = QPair<QString, QString>("actions/edit-copy.png", "editCopy.png");
		idIconMap_[Juff::ID_EDIT_PASTE] = QPair<QString, QString>("actions/edit-paste.png", "editPaste.png");
		idIconMap_[Juff::ID_EDIT_UNDO] = QPair<QString, QString>("actions/edit-undo.png", "editUndo.png");
		idIconMap_[Juff::ID_EDIT_REDO] = QPair<QString, QString>("actions/edit-redo.png", "editRedo.png");
//		idIconMap_[Juff::ID_UNINDENT] = QPair<QString, QString>("", "");
		idIconMap_[Juff::ID_ZOOM_IN] = QPair<QString, QString>("actions/zoom-in.png", "zoomIn.png");
		idIconMap_[Juff::ID_ZOOM_OUT] = QPair<QString, QString>("actions/zoom-out.png", "zoomOut.png");
		idIconMap_[Juff::ID_ZOOM_100] = QPair<QString, QString>("actions/zoom-original.png", "zoom100.png");
		//
		idIconMap_[Juff::ID_FIND] = QPair<QString, QString>("actions/edit-find.png", "find.png");
		idIconMap_[Juff::ID_FIND_NEXT] = QPair<QString, QString>("", "");
		idIconMap_[Juff::ID_FIND_PREV] = QPair<QString, QString>("", "");
		idIconMap_[Juff::ID_GOTO_LINE] = QPair<QString, QString>("", "");
		idIconMap_[Juff::ID_REPLACE] = QPair<QString, QString>("actions/edit-find-replace.png", "replace.png");
		//
/*		idIconMap_[Juff::ID_DOC_NEXT] = QPair<QString, QString>("", "");
		idIconMap_[Juff::ID_DOC_PREV] = QPair<QString, QString>("", ""); 
*/		//
		idIconMap_[Juff::ID_SETTINGS] = QPair<QString, QString>("actions/configure.png", "settings.png");
		idIconMap_[Juff::ID_ABOUT] = QPair<QString, QString>("apps/help.png", "about.png");
		idIconMap_[Juff::ID_ABOUT_QT] = QPair<QString, QString>("", "");
/*		idIconMap_[Juff::ID_APPLY_SETTINGS] = QPair<QString, QString>("", "");
		//
		idIconMap_[Juff::ID_SESSION_NEW] = QPair<QString, QString>("", "");
		idIconMap_[Juff::ID_SESSION_OPEN] = QPair<QString, QString>("", "");
		idIconMap_[Juff::ID_SESSION_SAVE] = QPair<QString, QString>("", "");
		idIconMap_[Juff::ID_SESSION_SAVE_AS] = QPair<QString, QString>("", "");
		//
		idIconMap_[Juff::ID_VIEW_SHOW_LINE_NUMBERS] = QPair<QString, QString>("", "");
		idIconMap_[Juff::ID_VIEW_WIDTH_ADJUST] = QPair<QString, QString>("", "");
		idIconMap_[Juff::ID_VIEW_SHOW_HIDDEN_SYMBOLS] = QPair<QString, QString>("", "");
		//
		idIconMap_[Juff::ID_MARKER_TOGGLE] = QPair<QString, QString>("", "");
		idIconMap_[Juff::ID_MARKER_REMOVE_ALL] = QPair<QString, QString>("", "");
		idIconMap_[Juff::ID_MARKER_NEXT] = QPair<QString, QString>("", "");
		idIconMap_[Juff::ID_MARKER_PREV] = QPair<QString, QString>("", "");
		//
		idIconMap_[Juff::ID_EOL_WIN] = QPair<QString, QString>("", "");
		idIconMap_[Juff::ID_EOL_UNIX] = QPair<QString, QString>("", "");
		idIconMap_[Juff::ID_EOL_MAC] = QPair<QString, QString>("", "");
*/		
		theme_ = "<default>";
		size_ = 1;
		
		sizeSeq_.resize(3);
		sizeSeq_[0] << 16 << 24 << 32 << 48;
		sizeSeq_[1] << 24 << 32 << 48 << 16;
		sizeSeq_[2] << 32 << 48 << 24 << 16;
	}
	
	typedef QMap<QString, QIcon> IconMap;
	IconMap iconMap_;
//	QVector<IconMap> iconMaps_;
	bool default_;
	QMap<Juff::CommandID, QPair<QString, QString> > idIconMap_;
	
	QString theme_;
	int size_;
	typedef QVector< QVector<int> > SzSeq;
	SzSeq sizeSeq_;
};


void IconManager::setCurrentIconTheme(const QString& theme, int size) {
	imInt_->theme_ = theme;
	imInt_->size_ = size;
}

QStringList IconManager::themeList() const {
	QDir iconDir("/usr/share/icons");
	QStringList list = iconDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	for (QStringList::iterator it = list.begin(); it != list.end(); ++it) {
		QString fullDirPath = iconDir.absolutePath() + "/" + *it;
		QStringList items = QDir(fullDirPath).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		if ( !items.contains("16x16") && !items.contains("24x24") 
				&& !items.contains("32x32") && !items.contains("48x48"))
			list.erase(it);
	}
	return list;
}

QIcon IconManager::getIcon(Juff::CommandID id) {
	QString iconDir("/usr/share/icons");
	
	QString iconFileName = imInt_->idIconMap_[id].first;
	if ( iconFileName.isEmpty() )
		return QIcon();
	else {
		if ( imInt_->theme_.compare("<default>") == 0 ) {
			return getDefaultIcon(id);
		}
		else {
			for (int i = 0; i < 4; ++i) {
				int sz = imInt_->sizeSeq_[imInt_->size_][i];
				QString fileName = iconDir + "/" + imInt_->theme_ + "/" + 
								QString("%1x%2/").arg(sz).arg(sz) + iconFileName;
				if ( QFileInfo(fileName).exists() )
					return QIcon(fileName);
			}
			return getDefaultIcon(id);
		}
	}
}

QIcon IconManager::getDefaultIcon(Juff::CommandID id) {
	JUFFENTRY;

	QString iconFileName = imInt_->idIconMap_[id].second;
	for (int i = 0; i < 4; ++i) {
//		Log::debug(imInt_->size_);
		int sz = imInt_->sizeSeq_[imInt_->size_][i];
//		Log::debug(sz);
		QString fileName = QString(":%1/").arg(sz) + iconFileName;
		if ( QFileInfo(fileName).exists() ) {
			return QIcon(fileName);
		}
	}
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
