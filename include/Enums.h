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

#ifndef __JUFFED_ENUMS_H__
#define __JUFFED_ENUMS_H__

namespace Juff {

typedef enum {
	MenuFile,
	MenuEdit,
	MenuView,
	MenuSearch,
	MenuFormat,
	MenuTools,
	MenuHelp,
} MenuID;

typedef enum {
	NullID,
	Separator,
	FileNew,
	FileOpen,
	FileClone,
	FileSave,
	FileSaveAs,
	FileSaveAll,
	FileReload,
	FileRename,
	FileClose,
	FileCloseAll,
	FilePrint,
	FileExit,
	//
	PrjNew,
	PrjOpen,
	PrjClose,
	PrjSaveAs,
	PrjAddFile,
	PrjRename,
	//
	EditUndo,
	EditRedo,
	EditCut,
	EditCopy,
	EditPaste,
	EditFind,
	EditFindNext,
	EditFindPrev,
	EditReplace,
	GotoLine,
	JumpToFile,
	//
	ViewLineNumbers,
	ViewWrapWords,
	ViewWhitespaces,
	ViewLineEndings,
	ViewZoomIn,
	ViewZoomOut,
	ViewZoom100,
	ViewFullscreen,
	//
	About,
	AboutQt,
//	LastAction
} ActionID;

}

#endif // __JUFFED_ENUMS_H__
