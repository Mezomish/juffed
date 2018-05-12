#ifndef COMPARE_PLUGIN_H
#define COMPARE_PLUGIN_H

/*
 * Compare plugin for Juffed editor
 *
 * Portions Copyright (C) 2018 Aleksey Komarov <q4arus@ya.ru>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation;  only version 2 of
 * the License is valid for this program.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <QtCore/QObject>

#include "JuffPlugin.h"
#include "Document.h"


/*! Try to format compare 2 files with meld
 */
class ComparePlugin : public QObject, public JuffPlugin {

	Q_OBJECT
#if QT_VERSION >= 0x050000
	Q_PLUGIN_METADATA(IID JuffPlugin_iid)
#endif
	Q_INTERFACES(JuffPlugin)

public:
	ComparePlugin();
	~ComparePlugin();
	void init();

	QString name() const;
	QString title() const;
	QString description() const;
	QString targetEngine() const;

	QToolBar* toolBar() const;
	Juff::ActionList mainMenuActions(Juff::MenuID) const;

public slots:
	void compare();

private:
	QAction * actDoc;

};

#endif
