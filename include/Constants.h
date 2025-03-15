#ifndef __JUFFED_CONSTANTS_H__
#define __JUFFED_CONSTANTS_H__

#include <QString>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QRegularExpression>
const QRegularExpression LineSeparatorRx = QRegularExpression("\r\n|\n|\r");
#else
#include <QRegExp>
const QRegExp LineSeparatorRx   = QRegExp("\r\n|\n|\r");
#endif

const QString FILE_NEW          = "main:fileNew";
const QString FILE_OPEN         = "main:fileOpen";
const QString FILE_SAVE         = "main:fileSave";
const QString FILE_SAVE_AS      = "main:fileSaveAs";
const QString FILE_SAVE_ALL     = "main:fileSaveAll";
const QString FILE_RELOAD       = "main:fileReload";
const QString FILE_RENAME       = "main:fileRename";
const QString FILE_CLOSE        = "main:fileClose";
const QString FILE_CLOSE_ALL    = "main:fileCloseAll";
const QString FILE_PRINT        = "main:filePrint";
const QString FILE_EXIT         = "main:fileExit";

const QString SESSION_NEW       = "main:sessionNew";
const QString SESSION_OPEN      = "main:sessionOpen";
const QString SESSION_SAVE      = "main:sessionSave";
//const QString SESSION_          = "main:session";

const QString EDIT_UNDO         = "main:editUndo";
const QString EDIT_REDO         = "main:editRedo";
const QString EDIT_CUT          = "main:editCut";
const QString EDIT_COPY         = "main:editCopy";
const QString EDIT_PASTE        = "main:editPaste";

const QString SEARCH_FIND       = "main:find";
const QString SEARCH_FIND_NEXT  = "main:findNext";
const QString SEARCH_FIND_PREV  = "main:findPrev";
const QString SEARCH_REPLACE    = "main:replace";
const QString SEARCH_GOTO_LINE  = "main:gotoLine";
const QString SEARCH_GOTO_FILE  = "main:gotoFile";

const QString VIEW_ZOOM_IN      = "main:viewZoomIn";
const QString VIEW_ZOOM_OUT     = "main:viewZoomOut";
const QString VIEW_ZOOM_100     = "main:viewZoom100";
const QString VIEW_FULLSCREEN   = "main:viewFullscreen";
const QString MOVE_TO_OTHER_TAB = "main:MoveToOtherTab";

const QString TOOLS_SETTINGS    = "main::toolsSettings";

const QString HELP_ABOUT        = "main:helpAbout";
const QString HELP_ABOUT_QT     = "main:helpAboutQt";


#endif // __JUFFED_CONSTANTS_H__
