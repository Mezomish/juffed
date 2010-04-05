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

#include "CommandStorage.h"

#include "IconManager.h"
#include "Log.h"

#include <QAction>
#include <QKeySequence>
#include <QMap>

CommandStorage* CommandStorage::instance_ = NULL;

class CommandStorage::Interior {
public:
	Interior() {
	}
	
	QMap<Juff::ActionID, QAction*> actions_;
//	QMap<Juff::ActionID, QKeySequence> shortcuts_;
};

CommandStorage::CommandStorage() {
	int_ = new Interior();
	createActions();
}

CommandStorage* CommandStorage::instance() {
	if ( instance_ == NULL )
		instance_ = new CommandStorage();
	return instance_;
}

QAction* CommandStorage::action(Juff::ActionID id) const {
	return int_->actions_.value(id, NULL);
}

void CommandStorage::createActions() {
	LOGGER;
	
	Juff::ActionID ids[] = {
		Juff::FileNew,
		Juff::FileOpen,
		Juff::FileSave,
		Juff::FileSaveAs,
		Juff::FileSaveAll,
		Juff::FileReload,
		Juff::FileRename,
		Juff::FileClose,
		Juff::FileCloseAll,
		Juff::FilePrint,
		Juff::FileExit,
		//
		Juff::PrjNew,
		Juff::PrjOpen,
		Juff::PrjClose,
		Juff::PrjSaveAs,
		Juff::PrjAddFile,
		//
		Juff::EditUndo,
		Juff::EditRedo,
		Juff::EditCut,
		Juff::EditCopy,
		Juff::EditPaste,
		Juff::EditFind,
		Juff::EditFindNext,
		Juff::EditFindPrev,
		Juff::EditReplace,
		Juff::GotoLine,
		Juff::JumpToFile,
		//
		Juff::ViewLineNumbers,
		Juff::ViewWrapWords,
		Juff::ViewWhitespaces,
		Juff::ViewLineEndings,
		Juff::ViewZoomIn,
		Juff::ViewZoomOut,
		Juff::ViewZoom100,
		Juff::ViewFullscreen,
		//
		Juff::Settings,
		//
		Juff::About,
		Juff::AboutQt,
		//
		Juff::NullID
	};
	
	for (int i = 0; ; ++i) {
		Juff::ActionID id = ids[i];
		if ( id == Juff::NullID )
			break;
		
		QString text = title(id);
		QIcon icon = IconManager::instance()->icon(id);
		int_->actions_.insert(id, new QAction(icon, text, 0));
		int_->actions_[id]->setShortcut(shortcut(id));
	}
	
	int_->actions_[Juff::ViewLineNumbers]->setCheckable(true);
	int_->actions_[Juff::ViewWrapWords]->setCheckable(true);
	int_->actions_[Juff::ViewWhitespaces]->setCheckable(true);
	int_->actions_[Juff::ViewLineEndings]->setCheckable(true);
	int_->actions_[Juff::ViewFullscreen]->setCheckable(true);
}

QString CommandStorage::title(Juff::ActionID id) const {
//	LOGGER;
	
	switch (id) {
		case Juff::FileNew :      return QObject::tr("New");
		case Juff::FileOpen :     return QObject::tr("Open");
		case Juff::FileSave :     return QObject::tr("Save");
		case Juff::FileSaveAs :   return QObject::tr("Save as");
		case Juff::FileSaveAll :  return QObject::tr("Save all");
		case Juff::FileReload :   return QObject::tr("Reload");
		case Juff::FileRename :   return QObject::tr("Rename");
		case Juff::FileClose :    return QObject::tr("Close");
		case Juff::FileCloseAll : return QObject::tr("Close all");
		case Juff::FilePrint :    return QObject::tr("Print");
		case Juff::FileExit :     return QObject::tr("Exit");
			
		case Juff::PrjNew :       return QObject::tr("New project");
		case Juff::PrjOpen :      return QObject::tr("Open project");
		case Juff::PrjRename :    return QObject::tr("Rename project");
		case Juff::PrjClose :     return QObject::tr("Close project");
		case Juff::PrjSaveAs :    return QObject::tr("Save project as");
		case Juff::PrjAddFile :   return QObject::tr("Add file to project");
			
		case Juff::EditUndo :     return QObject::tr("Undo");
		case Juff::EditRedo :     return QObject::tr("Redo");
		case Juff::EditCut :      return QObject::tr("Cut");
		case Juff::EditCopy :     return QObject::tr("Copy");
		case Juff::EditPaste :    return QObject::tr("Paste");
		case Juff::EditFind :     return QObject::tr("Find");
		case Juff::EditFindNext : return QObject::tr("Find next");
		case Juff::EditFindPrev : return QObject::tr("Find previous");
		case Juff::EditReplace :  return QObject::tr("Replace");
		case Juff::GotoLine :     return QObject::tr("Go to line");
		case Juff::JumpToFile:    return QObject::tr("Jump to file");
		
		case Juff::ViewLineNumbers : return QObject::tr("Show line numbers");
		case Juff::ViewWrapWords :   return QObject::tr("Wrap words");
		case Juff::ViewWhitespaces : return QObject::tr("Show whitespaces");
		case Juff::ViewLineEndings : return QObject::tr("Show line endings");
		case Juff::ViewZoomIn :      return QObject::tr("Zoom in");
		case Juff::ViewZoomOut :     return QObject::tr("Zoom out");
		case Juff::ViewZoom100 :     return QObject::tr("Zoom 100%");
		case Juff::ViewFullscreen :  return QObject::tr("Fullscreen");
		
		case Juff::Settings:         return QObject::tr("Settings");
		
		case Juff::About:            return QObject::tr("About");
		case Juff::AboutQt:          return QObject::tr("About Qt");
		
		default:
			return "<no title>";
	}
}

QKeySequence CommandStorage::shortcut(Juff::ActionID id) const {
//	LOGGER;
	
//	QKeySequence seq = KeySettings::keySequence(id);
//	if ( !seq.isEmpty() )
//		return seq;

	switch (id) {
		case Juff::FileNew :      return QKeySequence("Ctrl+N");
		case Juff::FileOpen :     return QKeySequence("Ctrl+O");
		case Juff::FileSave :     return QKeySequence("Ctrl+S");
		case Juff::FileSaveAs :   return QKeySequence("Ctrl+Shift+S");
//		case Juff::FileSaveAll :  return QKeySequence("");
		case Juff::FileReload :   return QKeySequence("F5");
//		case Juff::FileRename :   return QKeySequence("");
		case Juff::FileClose :    return QKeySequence("Ctrl+W");
//		case Juff::FileCloseAll : return QKeySequence("");
		case Juff::FilePrint :    return QKeySequence("Ctrl+P");
		case Juff::FileExit :     return QKeySequence("Ctrl+Q");
		
		case Juff::EditUndo :     return QKeySequence("Ctrl+Z");
		case Juff::EditRedo :     return QKeySequence("Ctrl+Y");
		case Juff::EditCut :      return QKeySequence("Ctrl+X");
		case Juff::EditCopy :     return QKeySequence("Ctrl+C");
		case Juff::EditPaste :    return QKeySequence("Ctrl+V");
		case Juff::EditFind :     return QKeySequence("Ctrl+F");
		case Juff::EditFindNext : return QKeySequence("F3");
		case Juff::EditFindPrev : return QKeySequence("Shift+F3");
		case Juff::EditReplace :  return QKeySequence("Ctrl+R");
		case Juff::GotoLine :     return QKeySequence("Ctrl+G");
		case Juff::JumpToFile :   return QKeySequence("Shift+Ctrl+G");
		
//		case Juff::ViewLineNumbers : return QKeySequence("");
		case Juff::ViewWrapWords :   return QKeySequence("F10");
		case Juff::ViewWhitespaces : return QKeySequence("Ctrl+I");
//		case Juff::ViewLineEndings : return QKeySequence("");
		case Juff::ViewZoomIn :      return QKeySequence("Ctrl+=");
		case Juff::ViewZoomOut :     return QKeySequence("Ctrl+-");
		case Juff::ViewZoom100 :     return QKeySequence("Ctrl+0");
		case Juff::ViewFullscreen :  return QKeySequence("F11");
		
		case Juff::About:            return QKeySequence("F1");
		
		default:
			return QKeySequence("");
	}
//	return shortcuts_.value(id, QKeySequence());
}
