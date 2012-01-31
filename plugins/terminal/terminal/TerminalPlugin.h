#ifndef _TERMINAL_PLUGIN_H_
#define _TERMINAL_PLUGIN_H_

class QWidget;
class QTermWidget;

#include <QtCore>
#include <QtCore/QObject>
//#include <QtCore/QStack>
//#include <QtGui/QDirModel>

#include <JuffPlugin.h>

#include "ui_preferences.h"


/*! \brief Terminal preferences.
  It's based on the code from qterminal
  \author Petr Vanek <petr@scribus.info>
 */
class Preferences : public QWidget, Ui::Preferences
{
    Q_OBJECT

    public:
        Preferences(QTermWidget * term, const QObject * parent,
                    int colorIX, const QString & emulation,
                    const QFont & font);
};

class TerminalPlugin : public QObject, public JuffPlugin {
Q_OBJECT
Q_INTERFACES(JuffPlugin)
public:
	TerminalPlugin();
	virtual ~TerminalPlugin();

	//	info
        QString name() const;
        QString description() const;
        QString targetEngine() const;

	//	controls
        QWidgetList dockList() const;
        Juff::ActionList mainMenuActions(Juff::MenuID) const;

        QWidget* settingsPage() const;
        void applySettings();

public slots:
        void colorSchemaChanged(int val);
        void emulationChanged(const QString & val);
        void fontChanged(const QFont & val);
        void fontSizeChanged(int val);

private slots:
	void showTerminal();

private:
        QTermWidget* w_;
	QAction* termAct_;

        QTermWidget * m_term;
        int prefColorScheme;
        QString prefEmulation;
        QFont prefFont;
        int prefFontSize;

        void init();
};

#endif
