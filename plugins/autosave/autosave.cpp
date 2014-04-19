/***************************************************************************
  A Autosave plugin for JuffEd editor.

  Copyright: 2011 Aleksey Romanenko <slimusgm@gmail.com>


  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License.
  version 2 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
***************************************************************************/

#include <QTimer>
#include <QHBoxLayout>
#include <QLabel>

#include <PluginSettings.h>
#include "autosave.h"
#include <Document.h>
#include <Log.h>


AutosavePlugin::AutosavePlugin(): QObject(), JuffPlugin()
{
}

AutosavePlugin::~AutosavePlugin()
{
}

void AutosavePlugin::init()
{
    mSaveInterval = PluginSettings::getInt(this, "interval", 60);

    settingSpin = new QSpinBox();
    settingSpin->setValue(mSaveInterval);

    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(onTimer()));
    mTimer->start(mSaveInterval * 1000);
}

void AutosavePlugin::onTimer()
{
    Log::debug("Autosaving...");
    //save document;
    QStringList docs = api()->docList();
    foreach (QString docName, docs) {
        Juff::Document *doc = api()->document(docName);
        if (!doc->isNull() && !doc->isNoname() && doc->isModified()) {
            QString error;
            doc->save(error);
        }
    }
}

QString AutosavePlugin::name() const
{
    return "Autosave";
}

QString AutosavePlugin::title() const
{
    return tr("Autosave");
}

QString AutosavePlugin::description() const
{
    return "";
}

QString AutosavePlugin::targetEngine() const
{
    return "all";
}

void AutosavePlugin::applySettings()
{
    mSaveInterval = settingSpin->value();
    PluginSettings::set(this, "interval", mSaveInterval);

    mTimer->stop();
    mTimer->start(mSaveInterval * 1000);
}

QWidget * AutosavePlugin::settingsPage() const
{
        QWidget * cfg = new QWidget();
        QHBoxLayout * l = new QHBoxLayout();
        cfg->setLayout(l);

        settingSpin->setSuffix(tr(" sec"));

        QLabel *label = new QLabel(tr("Save document every: "));

        l->addWidget(label);
        l->addWidget(settingSpin);
        l->addItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));
        return cfg;
}


Q_EXPORT_PLUGIN2(autosave, AutosavePlugin)
