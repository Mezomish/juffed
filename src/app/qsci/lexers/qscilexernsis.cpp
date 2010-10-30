#include "qscilexernsis.h"

QsciLexerNSIS::QsciLexerNSIS(QObject *parent) :
    QsciLexer(parent)
{
}

QsciLexerNSIS::~QsciLexerNSIS()
{
}

const char * QsciLexerNSIS::keywords(int set) const
{
  // Functions
  if (set == 1)
    return "Abort AddBrandingImage AddSize AllowRootDirInstall AllowSkipFiles AutoCloseWindow BGFont BGGradient BrandingText BringToFront Call CallInstDLL Caption ChangeUI CheckBitmap ClearErrors CompletedText ComponentText CopyFiles CRCCheck CreateDirectory CreateFont CreateShortCut Delete DeleteINISec DeleteINIStr DeleteRegKey DeleteRegValue DetailPrint DetailsButtonText DirShow DirText DirVar DirVerify DisabledBitmap EnabledBitmap EnableWindow EnumRegKey EnumRegValue Exch Exec ExecShell ExecWait ExpandEnvStrings File FileBufSize FileClose FileErrorText FileOpen FileRead FileReadByte FileSeek FileWrite FileWriteByte FindClose FindFirst FindNext FindWindow FlushINI Function FunctionEnd GetCurInstType GetCurrentAddress GetDlgItem GetDLLVersion GetDLLVersionLocal GetErrorLevel GetFileTime GetFileTimeLocal GetFullPathName GetFunctionAddress GetInstDirError GetLabelAddress GetTempFileName Goto HideWindow Icon IfAbort IfErrors IfFileExists IfRebootFlag IfSilent InitPluginsDir InstallButtonText InstallColors InstallDir InstallDirRegKey InstProgressFlags InstType InstTypeGetText InstTypeSetText IntCmp IntCmpU IntFmt IntOp IsWindow LangString LangStringUP LicenseBkColor LicenseData LicenseForceSelection LicenseLangString LicenseText LoadLanguageFile LockWindow LogSet LogText MessageBox MiscButtonText Name OutFile Page PageEx PageExEnd PluginDir Pop Push Quit ReadEnvStr ReadINIStr ReadRegDWORD ReadRegStr Reboot RegDLL Rename RequestExecutionLevel ReserveFile Return RMDir SearchPath Section SectionDivider SectionEnd SectionGetFlags SectionGetInstTypes SectionGetSize SectionGetText SectionGroup SectionGroupEnd SectionIn SectionSetFlags SectionSetInstTypes SectionSetSize SectionSetText SendMessage SetAutoClose SetBrandingImage SetCompress SetCompressionLevel SetCompressor SetCompressorDictSize SetCtlColors SetCurInstType SetDatablockOptimize SetDateSave SetDetailsPrint SetDetailsView SetErrorLevel SetErrors SetFileAttributes SetFont SetOutPath SetOverwrite SetPluginUnload SetRebootFlag SetShellVarContext SetSilent SetStaticBkColor ShowInstDetails ShowUninstDetails ShowWindow SilentInstall SilentUnInstall Sleep SpaceTexts StrCmp StrCmpS StrCpy StrLen SubSection SubSectionEnd UninstallButtonText UninstallCaption UninstallEXEName UninstallIcon UninstallSubCaption UninstallText UninstPage UnRegDLL Var VIAddVersionKey VIProductVersion WindowIcon WriteINIStr WriteRegBin WriteRegDWORD WriteRegExpandStr WriteRegStr WriteUninstaller XPStyle !AddIncludeDir !AddPluginDir !appendfile !cd !define !delfile !echo !else !endif !error !execute !ifdef !ifmacrodef !ifmacrondef !ifndef !include !insertmacro !macro !macroend !packhdr !system !tempfile !undef !verbose !warning";

  // Variables
  if (set == 2)
    return "$0 $1 $2 $3 $4 $5 $6 $7 $8 $9 $R0 $R1 $R2 $R3 $R4 $R5 $R6 $R7 $R8 $R9 $APPDATA $CMDLINE $DESKTOP $EXEDIR $HWNDPARENT $INSTDIR $OUTDIR $PROGRAMFILES ${NSISDIR} $\n $\r $QUICKLAUNCH $SMPROGRAMS $SMSTARTUP $STARTMENU $SYSDIR $TEMP $WINDIR";

  if (set == 3)
    return "ARCHIVE CUR END FILE_ATTRIBUTE_ARCHIVE FILE_ATTRIBUTE_HIDDEN FILE_ATTRIBUTE_NORMAL FILE_ATTRIBUTE_OFFLINE FILE_ATTRIBUTE_READONLY FILE_ATTRIBUTE_SYSTEM FILE_ATTRIBUTE_TEMPORARY HIDDEN HKCC HKCR HKCU HKDD HKEY_CLASSES_ROOT HKEY_CURRENT_CONFIG HKEY_CURRENT_USER HKEY_DYN_DATA HKEY_LOCAL_MACHINE HKEY_PERFORMANCE_DATA HKEY_USERS HKLM HKPD HKU IDABORT IDCANCEL IDIGNORE IDNO IDOK IDRETRY IDYES MB_ABORTRETRYIGNORE MB_DEFBUTTON1 MB_DEFBUTTON2 MB_DEFBUTTON3 MB_DEFBUTTON4 MB_ICONEXCLAMATION MB_ICONINFORMATION MB_ICONQUESTION MB_ICONSTOP MB_OK MB_OKCANCEL MB_RETRYCANCEL MB_RIGHT MB_SETFOREGROUND MB_TOPMOST MB_USERICON MB_YESNO MB_YESNOCANCEL NORMAL OFFLINE READONLY SET SHCTX SW_HIDE SW_SHOWMAXIMIZED SW_SHOWMINIMIZED SW_SHOWNORMAL SYSTEM TEMPORARY all auto both bottom bzip2 checkbox colored current false force hide ifdiff ifnewer lastused leave left listonly lzma nevershow none normal off on pop push radiobuttons right show silent silentlog smooth textonly top true try zlib";

  return 0;
}

QString QsciLexerNSIS::description(int style) const
{
  switch (style)
  {
  case SCE_NSIS_DEFAULT:
    return tr("Default");
  case SCE_NSIS_COMMENT:
    return tr("Comment");
  case SCE_NSIS_STRINGDQ:
    return tr("Double-quoted String");
  case SCE_NSIS_STRINGLQ:
    return tr("Left-quoted String");
  case SCE_NSIS_STRINGRQ:
    return tr("Right-quoted String");
  case SCE_NSIS_FUNCTION:
    return tr("Function");
  case SCE_NSIS_VARIABLE:
    return tr("Variable");
  case SCE_NSIS_LABEL:
    return tr("Label");
  case SCE_NSIS_USERDEFINED:
    return tr("User Defined");
  case SCE_NSIS_SECTIONDEF:
    return tr("Section Definition");
  case SCE_NSIS_SUBSECTIONDEF:
    return tr("Subsection Definition");
  case SCE_NSIS_IFDEFINEDEF:
    return tr("IFDEF");
  case SCE_NSIS_MACRODEF:
    return tr("Macro");
  case SCE_NSIS_STRINGVAR:
    return tr("String Variable");
  case SCE_NSIS_NUMBER:
    return tr("Number");
  case SCE_NSIS_SECTIONGROUP:
    return tr("Section Group");
  case SCE_NSIS_PAGEEX:
    return tr("Page");
  case SCE_NSIS_FUNCTIONDEF:
    return tr("Function Definition");
  case SCE_NSIS_COMMENTBOX:
    return tr("Comment Box");
  }

  return "";
}

QColor QsciLexerNSIS::defaultColor(int style) const
{
  switch (style)
  {
  case SCE_NSIS_DEFAULT:
    return Qt::black;

  case SCE_NSIS_COMMENT:
  case SCE_NSIS_COMMENTBOX:
    return Qt::darkGreen;

  case SCE_NSIS_STRINGDQ:
  case SCE_NSIS_STRINGLQ:
  case SCE_NSIS_STRINGRQ:
    return Qt::darkCyan;

  case SCE_NSIS_FUNCTION:
    return Qt::darkBlue;

  case SCE_NSIS_VARIABLE:
    return Qt::darkMagenta;

  case SCE_NSIS_LABEL:
    return QColor(0xff6666);

  case SCE_NSIS_SECTIONDEF:
  case SCE_NSIS_SUBSECTIONDEF:
    return Qt::magenta;

  case SCE_NSIS_IFDEFINEDEF:
    return Qt::darkYellow;

  case SCE_NSIS_MACRODEF:
    return Qt::blue;

  case SCE_NSIS_STRINGVAR:
    return Qt::darkRed;

  case SCE_NSIS_NUMBER:
    return Qt::blue;

  case SCE_NSIS_FUNCTIONDEF:
    return Qt::red;

  case SCE_NSIS_SECTIONGROUP:
    return Qt::darkMagenta;

  case SCE_NSIS_PAGEEX:
    return Qt::darkMagenta;
  }

  return Qt::black;
}
