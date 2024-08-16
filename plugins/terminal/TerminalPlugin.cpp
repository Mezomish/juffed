#include "TerminalPlugin.h"

#include <QtGlobal>
#include <QtCore>
#include <QAction>
#include <QWidget>

#include <Log.h>
#include <PluginSettings.h>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <qtermwidget5/qtermwidget.h>
#else
#include <qtermwidget4/qtermwidget.h>
#endif

Preferences::Preferences(const QObject *parent,
                         const QString &color,
                         const QString &emulation,
                         const QFont &font)
    : QWidget()
{
    setupUi(this);

    colorSchemaCombo->addItems(QTermWidget::availableColorSchemes());
    int cix = colorSchemaCombo->findText(color);
    colorSchemaCombo->setCurrentIndex(cix != -1 ? cix : 0);
    connect(colorSchemaCombo, SIGNAL(currentIndexChanged(const QString&)),
            parent, SLOT(colorSchemaChanged(const QString &)));

    emulationComboBox->addItems(QTermWidget::availableKeyBindings());
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
    prefColorScheme = PluginSettings::getString(this, "TerminalColorScheme");
    prefEmulation = PluginSettings::getString(this, "TerminalEmulation");
    w_->setColorScheme(prefColorScheme);
    w_->setTerminalFont(prefFont);
    w_->setKeyBindings(prefEmulation);
}

TerminalPlugin::~TerminalPlugin() {
	if ( w_ ) {
		w_->deleteLater();
	}
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
    return new Preferences(this, prefColorScheme, prefEmulation, prefFont);
}

void TerminalPlugin::applySettings()
{
    prefFont.setPointSize(prefFontSize);
    PluginSettings::set(this, "TerminalFont", prefFont.toString());
    PluginSettings::set(this, "TerminalEmulation", prefEmulation);
    PluginSettings::set(this, "TerminalColorScheme", prefColorScheme);
    w_->setColorScheme(prefColorScheme);
    w_->setTerminalFont(prefFont);
    w_->setKeyBindings(prefEmulation);
}

void TerminalPlugin::colorSchemaChanged(const QString & val)
{
    prefColorScheme = val;
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

#include "TerminalPlugin.moc"

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(terminal, TerminalPlugin)
#endif

