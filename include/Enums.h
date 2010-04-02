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
	EditGotoLine,
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
