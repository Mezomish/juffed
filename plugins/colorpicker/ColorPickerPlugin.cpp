#include "ColorPickerPlugin.h"

#include <QtCore>
#include <QtGui/QToolBar>
#include <QtGui/QColorDialog>

#include <Document.h>
#include <Log.h>

ColorPickerPlugin::ColorPickerPlugin() : QObject(), JuffPlugin() {
	_pickColorAct = new QAction(QIcon(":icon32"), "Pick a color", this);
	connect(_pickColorAct, SIGNAL(triggered()), SLOT(pickColor()));
	// toolbar
	_toolBar = new QToolBar("ColorPickerPlugin");
	_toolBar->setObjectName("ColorPickerToolbar");
	_toolBar->addAction(_pickColorAct);
}

ColorPickerPlugin::~ColorPickerPlugin() {
}

QString ColorPickerPlugin::name() const {
	return "Color Picker";
}

QString ColorPickerPlugin::targetEngine() const {
	return "all";
}

QString ColorPickerPlugin::description() const {
	return "Lets you pick a color easily";
}

QToolBar* ColorPickerPlugin::toolBar() const {
	return _toolBar;
}

Juff::ActionList ColorPickerPlugin::mainMenuActions(Juff::MenuID id) const {
	Juff::ActionList list;
	if ( id == Juff::MenuTools ) {
		list << _pickColorAct;
	}
	return list;
}

void ColorPickerPlugin::pickColor() {
	QColor color = QColorDialog::getColor(Qt::white, api()->mainWindow());
	if ( color.isValid() ) {
		Juff::Document* doc = api()->currentDocument();
		if ( !doc->isNull() ) {
			doc->replaceSelectedText(color.name());
		}
	}
}

Q_EXPORT_PLUGIN2(colorpicker, ColorPickerPlugin)
