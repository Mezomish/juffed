#include "TerminalPlugin.h"

#include <QtCore>
#include <QAction>
#include <QWidget>

#include <Log.h>
//#include <MainSettings.h>
#include <PluginSettings.h>

#include "qtermwidget.h"

Preferences::Preferences(QTermWidget * term, const QObject * parent,
                         int colorIX, const QString & emulation,
                         const QFont & font)
    : QWidget()
{
    setupUi(this);

    QStringList colorSchemes;
    colorSchemes << "White On Black" << \
                    "Green On Black" << \
                    "Black On Light Yellow";
    colorSchemaCombo->addItems(colorSchemes);
    colorSchemaCombo->setCurrentIndex(colorIX);
    connect(colorSchemaCombo, SIGNAL(currentIndexChanged(int)),
            parent, SLOT(colorSchemaChanged(int)));

//    emulationComboBox->addItems(term->availableKeyBindings());
    int eix = emulationComboBox->findText(emulation);
    emulationComboBox->setCurrentIndex(eix != -1 ? eix : 0 );
    connect(emulationComboBox, SIGNAL(currentIndexChanged(const QString&)),
            parent, SLOT(emulationChanged(const QString &)));

    fontComboBox->setCurrentFont(font);
    connect(fontComboBox, SIGNAL(currentFontChanged(const QFont &)),
            parent, SLOT(fontChanged(const QFont &)));

    sizeSpinBox->setValue(font.pointSize());
    connect(sizeSpinBox, SIGNAL(valueChanged(int)),
            parent, SLOT(fontSizeChanged(int)));
}


TerminalPlugin::TerminalPlugin() : QObject(), JuffPlugin() {
//	w_ = new QWidget();
	w_ = new QTermWidget();
	w_->setScrollBarPosition(QTermWidget::ScrollBarRight);
	w_->setWindowTitle(tr("Terminal"));
        init();
	
	termAct_ = new QAction(tr("Show/Hide terminal"), 0);
	termAct_->setShortcut(QKeySequence("Ctrl+Alt+T"));
	connect(termAct_, SIGNAL(triggered()), SLOT(showTerminal()));
}

void TerminalPlugin::init()
{
    prefFont.fromString(PluginSettings::getString(this, "TerminalFont"));
    prefFontSize = prefFont.pointSize();
    prefColorScheme = PluginSettings::getInt(this, "TerminalColorScheme");
    prefEmulation = PluginSettings::getString(this, "TerminalEmulation");
    w_->setColorScheme(prefColorScheme);
    w_->setTerminalFont(prefFont);
//    w_->setKeyBindings(prefEmulation);
}

TerminalPlugin::~TerminalPlugin() {
	delete w_;
}

QString TerminalPlugin::name() const {
	return "Terminal";
}

QString TerminalPlugin::targetEngine() const {
	return "all";
}

QString TerminalPlugin::description() const {
	return "Terminal emulator";
}

QWidgetList TerminalPlugin::dockList() const {
	QWidgetList wList;
	wList << w_;
	return wList;
}

Juff::ActionList TerminalPlugin::mainMenuActions(Juff::MenuID id) const
{
	Juff::ActionList list;
	if ( Juff::MenuTools == id ) {
		list << termAct_;
	}
	return list;
}

QWidget * TerminalPlugin::settingsPage() const
{
    return new Preferences(w_, this, prefColorScheme, prefEmulation, prefFont);
}

void TerminalPlugin::applySettings()
{
    prefFont.setPointSize(prefFontSize);
    PluginSettings::set(this, "TerminalFont", prefFont.toString());
    PluginSettings::set(this, "TerminalEmulation", prefEmulation);
    PluginSettings::set(this, "TerminalColorScheme", prefColorScheme);
    w_->setColorScheme(prefColorScheme);
    w_->setTerminalFont(prefFont);
//    w_->setKeyBindings(prefEmulation);
}

void TerminalPlugin::colorSchemaChanged(int val)
{
    prefColorScheme = val + 1;
}

void TerminalPlugin::emulationChanged(const QString & val)
{
    prefEmulation = val;
}

void TerminalPlugin::fontChanged(const QFont & val)
{
    prefFont = val;
}

void TerminalPlugin::fontSizeChanged(int val)
{
    prefFontSize = val;
}

void TerminalPlugin::showTerminal()
{
	QWidget* dock = w_->parentWidget();
	if ( NULL == dock )
		return;
	
	if ( dock->isHidden() ) {
		dock->show();
		w_->setFocus();
	}
	else {
		dock->hide();

		// return focus to current document
		Juff::Document* doc = api()->currentDocument();
		if ( !doc->isNull() )
			api()->openDoc(doc->fileName());
	}
}

Q_EXPORT_PLUGIN2(terminal, TerminalPlugin)
