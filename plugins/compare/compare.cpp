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

#include "compare.h"
#include <QProcess>
#include "EditorSettings.h"


ComparePlugin::ComparePlugin() : QObject(), JuffPlugin()
{
	actDoc = new QAction(QIcon(":meld"), tr("Compare"), this);
	connect(actDoc, SIGNAL(triggered()), this, SLOT(compare()));
}

void ComparePlugin::init() {
}

ComparePlugin::~ComparePlugin() {
}

QString ComparePlugin::name() const {
	return "Compare";
}

QString ComparePlugin::title() const {
	return tr("Compare");
}

QString ComparePlugin::targetEngine() const {
	return "all";
}

QString ComparePlugin::description() const {
	return "Plugin that allows to compare 2 files with meld";
}

QToolBar* ComparePlugin::toolBar() const
{
	QToolBar * bar = new QToolBar(tr("Compare"));
	bar->addAction(actDoc);
	bar->setObjectName("CompareToolBar");
	return bar;
}

Juff::ActionList ComparePlugin::mainMenuActions(Juff::MenuID id) const
{
	Juff::ActionList list;
	if ( Juff::MenuTools == id )
	{
		list << actDoc;
	}
	return list;
}

void ComparePlugin::compare()
{
	Juff::Document* docR = api()->currentDocument(Juff::PanelRight);
	if ( docR->isNull() )
		return;
	Juff::Document* docL = api()->currentDocument(Juff::PanelLeft);
	if ( docL->isNull() )
		return;

	QProcess *process = new QProcess(this);

	process->startDetached("meld", QStringList() << docL->fileName() << docR->fileName());
	//process->start("meld", QStringList() << docL->fileName() << docR->fileName());
}

#include "compare.moc"

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(compare, ComparePlugin)
#endif
