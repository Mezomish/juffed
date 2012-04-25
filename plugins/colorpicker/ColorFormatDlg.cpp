#include "ColorFormatDlg.h"

#include <PluginSettings.h>

ColorFormatDlg::ColorFormatDlg( JuffPlugin* plugin, const QColor& color, QWidget* parent) : QDialog(parent) {
	_ui.setupUi( this );
	
	_plugin = plugin;
	
	connect( _ui.buttonBox, SIGNAL(accepted()), SLOT(accept()) );
	connect( _ui.buttonBox, SIGNAL(rejected()), SLOT(reject()) );
	
	int r, g, b;
	color.getRgb( &r, &g, &b );
	
	_ui.btnHtml->setText( color.name() );
	_ui.btnHex->setText( color.name().replace("#", "0x") );
	_ui.btnHexSplitted->setText( QString().sprintf("0x%02hX, 0x%02hX, 0x%02hX", (unsigned short)r, (unsigned short)g, (unsigned short)b) );
	_ui.btnSplitted->setText( QString().sprintf("%i, %i, %i", r, g, b) );
	
	_ui.buttonGroup->setId( _ui.btnHtml, 0 );
	_ui.buttonGroup->setId( _ui.btnHex, 1 );
	_ui.buttonGroup->setId( _ui.btnHexSplitted, 2 );
	_ui.buttonGroup->setId( _ui.btnSplitted, 3 );
	
//	_ui.btnHtml->setChecked( true );
	int id = PluginSettings::getInt( plugin, "format", 0 );
	QAbstractButton* btn = _ui.buttonGroup->button( id );
	if ( btn != 0 ) {
		btn->setChecked( true );
	}
	else {
		_ui.btnHtml->setChecked( true );
	}
}

QString ColorFormatDlg::colorStr() const {
	QAbstractButton* checkedBtn = _ui.buttonGroup->checkedButton();
	if ( checkedBtn != 0 ) {
		int id = _ui.buttonGroup->checkedId();
		PluginSettings::set( _plugin, "format", id );
		return checkedBtn->text();
	}
	else {
		return "";
	}
}
