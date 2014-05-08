SET ( juffed_lib_MOC_HEADERS
    include/Document.h
    include/JuffAPI.h
    include/Project.h
    include/PluginNotifier.h
    include/StatusLabel.h
    include/ColorButton.h
    include/SettingsPage.h
    include/SettingsCheckItem.h
    include/SettingsColorItem.h
    include/SettingsItem.h
    include/SettingsSelectItem.h
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
    src/lib/Utils.cpp

    src/lib/CommandStorage.cpp
    src/lib/Document.cpp
    src/lib/DocEngine.cpp
    src/lib/IconManager.cpp
    src/lib/JuffAPI.cpp
    src/lib/Log.cpp
    src/lib/NullDoc.cpp
    src/lib/Project.cpp
    src/lib/PluginNotifier.cpp
    src/lib/SearchResults.cpp

    src/lib/StatusLabel.cpp
    src/lib/ColorButton.cpp
    src/lib/SettingsPage.cpp
    src/lib/SettingsCheckItem.cpp
    src/lib/SettingsColorItem.cpp
    src/lib/SettingsItem.cpp
    src/lib/SettingsSelectItem.cpp
)

SET ( juffed_lib_RESOURCES
    src/lib/LibResources.qrc
)

SET ( juffed_app_MOC_HEADERS
    src/app/JuffEd.h
    src/app/SearchEngine.h

    src/app/ui/AboutDlg.h
    src/app/ui/DocViewer.h
#    src/app/ui/FindDlg.h
    src/app/ui/JuffMW.h
    src/app/ui/JumpToFileDlg.h
    src/app/ui/Popup.h
    src/app/ui/SearchComboBox.h
    src/app/ui/SearchPopup.h
    src/app/ui/SelectFilesDlg.h
    src/app/ui/SessionDlg.h
    src/app/ui/TabBar.h
    src/app/ui/TabWidget.h

    # settings
    src/app/ui/settings/CharsetsSettingsPage.h
    src/app/ui/settings/MultiPage.h
    src/app/ui/settings/PluginPage.h
    src/app/ui/settings/SettingsDlg.h
)

SET ( juffed_app_SRCS
#    src/app/DocManager.cpp
    src/app/JuffEd.cpp
    src/app/main.cpp
    src/app/PluginManager.cpp
    src/app/SearchEngine.cpp

    src/app/ui/AboutDlg.cpp
    src/app/ui/DocViewer.cpp
#    src/app/ui/FindDlg.cpp
    src/app/ui/JuffMW.cpp
    src/app/ui/JumpToFileDlg.cpp
    src/app/ui/Popup.cpp
    src/app/ui/SearchPopup.cpp
    src/app/ui/SearchComboBox.cpp
    src/app/ui/SelectFilesDlg.cpp
    src/app/ui/SessionDlg.cpp
    src/app/ui/TabBar.cpp
    src/app/ui/TabWidget.cpp

    # settings
    src/app/ui/settings/CharsetsSettingsPage.cpp
    src/app/ui/settings/MultiPage.cpp
    src/app/ui/settings/PluginPage.cpp
    src/app/ui/settings/SettingsDlg.cpp
)

SET ( juffed_qsci_SRCS
    src/app/qsci/JuffScintilla.cpp
    src/app/qsci/LexerStorage.cpp
    src/app/qsci/QSciSettings.cpp
    src/app/qsci/SciDoc.cpp
    src/app/qsci/SciDocEngine.cpp
    src/app/qsci/settings/PrintSettings.cpp
    src/app/qsci/settings/FileTypesPage.cpp

    # lexers
    src/app/qsci/lexers/qscilexerada.cpp
    src/app/qsci/lexers/qscilexerasm.cpp
    src/app/qsci/lexers/qscilexerhaskell.cpp
    src/app/qsci/lexers/qscilexerlisp.cpp
    src/app/qsci/lexers/qscilexernsis.cpp
    src/app/qsci/lexers/my/QsciLexerMatlab.cpp
)

SET ( juffed_qsci_MOC_HEADERS
    src/app/qsci/JuffScintilla.h
    src/app/qsci/SciDoc.h
    src/app/qsci/SciDocEngine.h
    src/app/qsci/settings/FileTypesPage.h

    # lexers
    src/app/qsci/lexers/qscilexerada.h
    src/app/qsci/lexers/qscilexerasm.h
    src/app/qsci/lexers/qscilexerhaskell.h
    src/app/qsci/lexers/qscilexerlisp.h
    src/app/qsci/lexers/qscilexernsis.h
    src/app/qsci/lexers/my/QsciLexerMatlab.h
)

SET ( juffed_qsci_UIS
    src/app/qsci/settings/FileTypesPage.ui
)

IF ( WIN32 )
    SET ( juffed_app_SRCS
        ${juffed_app_SRCS}
    )
ENDIF ( WIN32 )

IF ( UNIX )
    SET ( juffed_app_SRCS
        ${juffed_app_SRCS}
    )
ENDIF ( UNIX )

SET ( juffed_app_UIS
#    src/app/ui/forms/FindDlg.ui
    src/app/ui/forms/JumpToFileDlg.ui
    src/app/ui/forms/SearchPopup.ui
#
    # settings
    src/app/ui/forms/AutocompleteSettingsPage.ui
    src/app/ui/forms/CharsetsSettingsPage.ui
    src/app/ui/forms/EditorSettingsPage.ui
    src/app/ui/forms/MainSettingsPage.ui
    src/app/ui/forms/SessionDlg.ui
    src/app/ui/forms/SelectFilesDlg.ui
    src/app/ui/forms/ViewSettingsPage.ui
)

SET ( juffed_app_RESOURCES
    src/app/Resources.qrc
)

SET ( juffed_TS
    l10n/juffed_da.ts
    l10n/juffed_de.ts
    l10n/juffed_fr.ts
    l10n/juffed_pl.ts
    l10n/juffed_pt.ts
    l10n/juffed_ru.ts
    l10n/juffed_sk.ts
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
    include/CommandStorageInt.h
    include/Constants.h
    include/DocEngine.h
    include/DocHandlerInt.h
    include/Document.h
    include/EditorSettings.h
    include/Enums.h
    include/FileTypeSettings.h
    include/IconManagerInt.h
    include/JuffAPI.h
    include/JuffPlugin.h
    include/KeySettings.h
    include/LibConfig.h
    include/Log.h
    include/MainSettings.h
    include/NullDoc.h
    include/PluginNotifier.h
    include/PluginSettings.h
    include/Project.h
    include/SearchResults.h
    include/Settings.h
    include/StatusLabel.h
    include/Types.h
    include/Utils.h
)
