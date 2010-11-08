SET ( juffed_lib_MOC_HEADERS
	include/Document.h
	include/JuffAPI.h
	include/Project.h
	include/PluginNotifier.h
)

SET ( juffed_lib_SRCS
	src/lib/AutocompleteSettings.cpp
	src/lib/CharsetSettings.cpp
	src/lib/EditorSettings.cpp
	src/lib/FileTypeSettings.cpp
	src/lib/KeySettings.cpp
	src/lib/MainSettings.cpp
	src/lib/PluginSettings.cpp
	src/lib/Settings.cpp
	src/lib/QSciSettings.cpp
	
	src/lib/CommandStorage.cpp
	src/lib/Document.cpp
	src/lib/DocEngine.cpp
	src/lib/Functions.cpp
	src/lib/IconManager.cpp
	src/lib/JuffAPI.cpp
	src/lib/Log.cpp
	src/lib/NullDoc.cpp
	src/lib/Project.cpp
	src/lib/PluginNotifier.cpp
	src/lib/SearchResults.cpp

        src/3rd_party/utf/Utf8_16.cxx
)

SET ( juffed_lib_RESOURCES
	src/lib/LibResources.qrc
)

SET ( juffed_app_MOC_HEADERS
	src/app/JuffEd.h
	src/app/SearchEngine.h
	src/app/qsci/JuffScintilla.h
	src/app/qsci/SciDoc.h
	src/app/qsci/SciDocEngine.h
	
	# lexers
	src/app/qsci/lexers/qscilexerada.h
	src/app/qsci/lexers/qscilexerasm.h
	src/app/qsci/lexers/qscilexerhaskell.h
	src/app/qsci/lexers/qscilexerlisp.h
	src/app/qsci/lexers/qscilexernsis.h
	src/app/qsci/lexers/my/QsciLexerMatlab.h
	
	src/app/ui/AboutDlg.h
	src/app/ui/DocViewer.h
#	src/app/ui/FindDlg.h
	src/app/ui/JuffMW.h
	src/app/ui/JumpToFileDlg.h
	src/app/ui/Popup.h
	src/app/ui/SearchComboBox.h
	src/app/ui/SearchPopup.h
	src/app/ui/SelectFilesDlg.h
	src/app/ui/StatusLabel.h
	src/app/ui/TabBar.h
	src/app/ui/TabWidget.h
	src/3rd_party/qtsingleapplication/qtsinglecoreapplication.h
	src/3rd_party/qtsingleapplication/qtsingleapplication.h
	src/3rd_party/qtsingleapplication/qtlocalpeer.h
	
	# settings
	src/app/ui/settings/CharsetsSettingsPage.h
	src/app/ui/settings/ColorButton.h
	src/app/ui/settings/FileTypesPage.h
	src/app/ui/settings/MultiPage.h
	src/app/ui/settings/PluginPage.h
	src/app/ui/settings/SettingsDlg.h
	src/app/ui/settings/SettingsCheckItem.h
	src/app/ui/settings/SettingsItem.h
	src/app/ui/settings/SettingsPage.h
	src/app/ui/settings/SettingsSelectItem.h
)

SET ( juffed_app_SRCS
#	src/app/DocManager.cpp
	src/app/JuffEd.cpp
	src/app/main.cpp
	src/app/PluginManager.cpp
	src/app/SearchEngine.cpp
	src/app/qsci/JuffScintilla.cpp
	src/app/qsci/LexerStorage.cpp
	src/app/qsci/SciDoc.cpp
	src/app/qsci/SciDocEngine.cpp
	
	# lexers
	src/app/qsci/lexers/qscilexerada.cpp
	src/app/qsci/lexers/qscilexerasm.cpp
	src/app/qsci/lexers/qscilexerhaskell.cpp
	src/app/qsci/lexers/qscilexerlisp.cpp
	src/app/qsci/lexers/qscilexernsis.cpp
	src/app/qsci/lexers/my/QsciLexerMatlab.cpp
	
	src/app/ui/AboutDlg.cpp
	src/app/ui/DocViewer.cpp
#	src/app/ui/FindDlg.cpp
	src/app/ui/JuffMW.cpp
	src/app/ui/JumpToFileDlg.cpp
	src/app/ui/Popup.cpp
	src/app/ui/SearchPopup.cpp
	src/app/ui/SearchComboBox.cpp
	src/app/ui/SelectFilesDlg.cpp
	src/app/ui/StatusLabel.cpp
	src/app/ui/TabBar.cpp
	src/app/ui/TabWidget.cpp
	src/3rd_party/qtsingleapplication/qtsinglecoreapplication.cpp
	src/3rd_party/qtsingleapplication/qtsingleapplication.cpp
	src/3rd_party/qtsingleapplication/qtlockedfile.cpp
	src/3rd_party/qtsingleapplication/qtlocalpeer.cpp
	src/3rd_party/utf/Utf8_16.cxx
	
	# settings
	src/app/ui/settings/CharsetsSettingsPage.cpp
	src/app/ui/settings/ColorButton.cpp
	src/app/ui/settings/FileTypesPage.cpp
	src/app/ui/settings/MultiPage.cpp
	src/app/ui/settings/PluginPage.cpp
	src/app/ui/settings/SettingsDlg.cpp
	src/app/ui/settings/SettingsCheckItem.cpp
	src/app/ui/settings/SettingsItem.cpp
	src/app/ui/settings/SettingsPage.cpp
	src/app/ui/settings/SettingsSelectItem.cpp
)

IF ( WIN32 )
	SET ( juffed_app_SRCS
		${juffed_app_SRCS}
		src/3rd_party/qtsingleapplication/qtlockedfile_win.cpp
	)
ENDIF ( WIN32 )

IF ( UNIX )
	SET ( juffed_app_SRCS
		${juffed_app_SRCS}
		src/3rd_party/qtsingleapplication/qtlockedfile_unix.cpp
	)
ENDIF ( UNIX )

SET ( juffed_app_UIS
#	src/app/ui/forms/FindDlg.ui
	src/app/ui/forms/JumpToFileDlg.ui
	src/app/ui/forms/SearchPopup.ui
#	
	# settings
	src/app/ui/forms/AutocompleteSettingsPage.ui
	src/app/ui/forms/CharsetsSettingsPage.ui
	src/app/ui/forms/EditorSettingsPage.ui
	src/app/ui/forms/FileTypesPage.ui
	src/app/ui/forms/MainSettingsPage.ui
	src/app/ui/forms/SessionDlg.ui
	src/app/ui/forms/SelectFilesDlg.ui
	src/app/ui/forms/ViewSettingsPage.ui
	src/app/qsci/forms/QSciSettings.ui
)

SET ( juffed_app_RESOURCES
	src/app/Resources.qrc
)

SET ( juffed_TS
	l10n/juffed_de.ts
	l10n/juffed_fr.ts
	l10n/juffed_pl.ts
	l10n/juffed_pt.ts
	l10n/juffed_ru.ts
	l10n/juffed_sp.ts
	l10n/juffed_zh.ts
	l10n/juffed_cs.ts
)

SET ( juffed_DEV_HEADERS
	include/AppInfo.h
	${CMAKE_BINARY_DIR}/AppInfo.win.h
	${CMAKE_BINARY_DIR}/AppInfo.nix.h
	${CMAKE_BINARY_DIR}/AppInfo.apple.h
	include/AutocompleteSettings.h
	include/CharsetSettings.h
	include/CommandStorage.h
	include/Constants.h
	include/DocEngine.h
	include/DocHandlerInt.h
	include/Document.h
	include/EditorSettings.h
	include/Enums.h
	include/FileTypeSettings.h
	include/Functions.h
	include/IconManager.h
	include/JuffAPI.h
	include/JuffPlugin.h
	include/KeySettings.h
	include/Log.h
	include/MainSettings.h
	include/NullDoc.h
	include/PluginNotifier.h
	include/PluginSettings.h
	include/Project.h
	include/QSciSettings.h
	include/SearchResults.h
	include/Settings.h
	include/Types.h
)
