#ifndef __JUFFED_CONSTANTS_H__
#define __JUFFED_CONSTANTS_H__

#include <QRegExp>
#include <QString>

const QRegExp LineSeparatorRx   = QRegExp("\r\n|\n|\r");

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

const QString VIEW_LINE_NUMBERS = "main:viewLineNumbers";
const QString VIEW_WRAP_WORDS   = "main:viewWrapWords";
const QString VIEW_WHITESPACES  = "main:viewWhitespaces";
const QString VIEW_LINE_ENDINGS = "main:viewLineEndings";
const QString VIEW_ZOOM_IN      = "main:viewZoomIn";
const QString VIEW_ZOOM_OUT     = "main:viewZoomOut";
const QString VIEW_ZOOM_100     = "main:viewZoom100";
const QString VIEW_FULLSCREEN   = "main:viewFullscreen";

const QString TOOLS_SETTINGS    = "main::toolsSettings";

const QString HELP_ABOUT        = "main:helpAbout";
const QString HELP_ABOUT_QT     = "main:helpAboutQt";


#endif // __JUFFED_CONSTANTS_H__
