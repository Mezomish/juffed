#ifndef SORT_PLUGIN_H
#define SORT_PLUGIN_H
/***************************************************************************
  A Sort plugin for JuffEd editor.

  Copyright: 2010 Aleksey Romanenko <slimusgm@gmail.com>


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

#include "JuffPlugin.h"
#include "QuickSettings.h"

class QuickSettings;


class SortDocumentPlugin : public QObject, public JuffPlugin {

    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID JuffPlugin_iid)
#endif
    Q_INTERFACES(JuffPlugin)

public:
    SortDocumentPlugin();
    ~SortDocumentPlugin();
    void init();

    QString name() const;
    QString title() const;
    QString description() const;
    QString targetEngine() const;

    Juff::ActionList mainMenuActions(Juff::MenuID) const;

public slots:
    void sortDocument();
    void openSettings();

private:
    QAction * actDoc;
    //bool caseInsensitiveLessThan(const QString &s1, const QString &s2);
};

bool caseInsensitiveLessThan(const QString &s1, const QString &s2);
bool caseInsensitiveMoreThan(const QString &s1, const QString &s2);
bool caseSensitiveLessThan(const QString &s1, const QString &s2);
bool caseSensitiveMoreThan(const QString &s1, const QString &s2);

#endif
