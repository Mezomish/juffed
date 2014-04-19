#ifndef _COLOR_PICKER_PLUGIN_H_
#define _COLOR_PICKER_PLUGIN_H_

#include <QtCore/QObject>

#include <JuffPlugin.h>

class ColorPickerPlugin : public QObject, public JuffPlugin {
Q_OBJECT
Q_INTERFACES(JuffPlugin)
public:
	ColorPickerPlugin();
	virtual ~ColorPickerPlugin();

	virtual QString name() const;
	virtual QString description() const;
	virtual QString targetEngine() const;

	virtual QToolBar* toolBar() const;
	virtual Juff::ActionList mainMenuActions(Juff::MenuID) const;

private slots:
	void pickColor();

private:
	QToolBar* _toolBar;
	QAction* _pickColorAct;
};

#endif
