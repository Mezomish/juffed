#ifndef __COLOR_FORMAT_DLG_H__
#define __COLOR_FORMAT_DLG_H__

class JuffPlugin;

#include <QDialog>

#include "ui_ColorFormatDlg.h"

class ColorFormatDlg : public QDialog {
Q_OBJECT
public:
	ColorFormatDlg(JuffPlugin* plugin, const QColor&, QWidget * parent = 0);
	QString colorStr() const;

private:
	Ui::ColorFormatDlg _ui;
	JuffPlugin* _plugin;
};

#endif // __COLOR_FORMAT_DLG_H__
