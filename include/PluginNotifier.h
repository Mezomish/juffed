/*
JuffEd - An advanced text editor
Copyright 2007-2010 Mikhail Murzin

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License 
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef __JUFFED_PLUGIN_NOTIFIER_H__
#define __JUFFED_PLUGIN_NOTIFIER_H__

#include <QObject>
#include <QString>

#include "Enums.h"

namespace Juff {

class Project;
class Document;

class PluginNotifier : public QObject {
Q_OBJECT
public:
	PluginNotifier();

signals:
	// document notifications
	void docOpened(Juff::Document*, Juff::PanelIndex);
	void docActivated(Juff::Document*);
	void docClosed(Juff::Document*);
	void docRenamed(Juff::Document*, const QString& oldName);
	void docModified(Juff::Document*);
	void docTextChanged(Juff::Document*);
	void docSyntaxChanged(Juff::Document*, const QString& oldSyntax);
	void docCharsetChanged(Juff::Document*, const QString& oldCharset);

	// project notifications
	void projectOpened(Juff::Project*);
	void projectRenamed(Juff::Project*, const QString& oldName, const QString& oldPath);
	void projectFileAdded(Juff::Project*, const QString&);
	void projectFileRemoved(Juff::Project*, const QString&);
	void projectSubProjectAdded(Juff::Project*, Juff::Project*);
	void projectSubProjectRemoved(Juff::Project*, Juff::Project*);
	void projectAboutToBeClosed(Juff::Project*);

	// misc
	void settingsApplied();
};

}

#endif // __JUFFED_PLUGIN_NOTIFIER_H__
