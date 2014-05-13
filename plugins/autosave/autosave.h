#ifndef AUTOSAVE_PLUGIN_H
#define AUTOSAVE_PLUGIN_H
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

#include <QtCore/QObject>
#include <QSpinBox>

#include "JuffPlugin.h"

class AutosavePlugin : public QObject, public JuffPlugin {

    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID JuffPlugin_iid)
#endif
    Q_INTERFACES(JuffPlugin)

public:
    AutosavePlugin();
    ~AutosavePlugin();
    void init();

    QString name() const;
    QString title() const;
    QString description() const;
    QString targetEngine() const;

    virtual void applySettings();
    QWidget * settingsPage() const;

private slots:
    void onTimer();

private:
    int mSaveInterval;
    QTimer *mTimer;
    QSpinBox *settingSpin;

};

#endif
