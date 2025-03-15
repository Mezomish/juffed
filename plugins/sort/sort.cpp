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


#include "sort.h"

#include "Document.h"


SortDocumentPlugin::SortDocumentPlugin(): QObject(), JuffPlugin()
{
  actDoc = new QAction(tr("Sort Document"), this);
  //connect(actDoc, SIGNAL(triggered()), this, SLOT(sortDocument()));
  connect(actDoc, SIGNAL(triggered()), this, SLOT(openSettings()));
}

SortDocumentPlugin::~SortDocumentPlugin()
{
}

void SortDocumentPlugin::init()
{
}

QString SortDocumentPlugin::name() const
{
    return tr("Sort Document");
}

QString SortDocumentPlugin::title() const
{
    return tr("Sort Document");
}

QString SortDocumentPlugin::description() const
{
    return "";
}

QString SortDocumentPlugin::targetEngine() const
{
    return "all";
}

Juff::ActionList SortDocumentPlugin::mainMenuActions(Juff::MenuID id) const
{
    Juff::ActionList list;
    if ( Juff::MenuTools == id )
    {
        list << actDoc;
    }
    return list;
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#define qStableSort std::stable_sort
#endif
void SortDocumentPlugin::sortDocument()
{
  Juff::Document* doc = api()->currentDocument();
    if ( doc->isNull() )
        return;

    QString content;
    if (! doc->getText(content))
        return;
    QStringList list;
    list = content.split("\n");

    //list.removeDuplicates();
    //list.sort();
    qStableSort(list.begin(), list.end());

    content = list.join("\n");

    doc->setText(content);

//  qDebug() << list;
}

void SortDocumentPlugin::openSettings()
{
  QuickSettings dlg(api()->mainWindow());
  if(dlg.exec() == QDialog::Accepted) {
      Juff::Document* doc = api()->currentDocument();
      if ( doc->isNull() )
          return;

      QString content;
      if (doc->hasSelectedText()) {
        if(! doc->getSelectedText(content)) {
          return;
        }
      } else {
        if (! doc->getText(content))
            return;
      }
      QStringList list;
      list = content.split("\n");

      if (dlg.ui.remove_duplicates->isChecked()) {
        list.removeDuplicates();
      }

      if (dlg.ui.remove_blank->isChecked()) {
          foreach(const QString &str, list) {
              if (str.trimmed().isEmpty()) {
                  list.removeOne(str);
                  //qDebug() << "Empty line detected!!!111";
              }
          }
      }
        //list.sort();
      if (dlg.ui.case_ins->isChecked()) {
        if (dlg.ui.radioButton_2->isChecked()) {
          qStableSort(list.begin(), list.end(), caseInsensitiveLessThan);
        } else {
          qStableSort(list.begin(), list.end(), caseInsensitiveMoreThan);
        }
      } else {
        if (dlg.ui.radioButton_2->isChecked()) {
          qStableSort(list.begin(), list.end(), caseSensitiveLessThan);
        } else {
          qStableSort(list.begin(), list.end(), caseSensitiveMoreThan);
        }
      }

      content = list.join("\n");

      if (doc->hasSelectedText()) {
        doc->replaceSelectedText(content);
      } else {
        doc->setText(content);
      }
  }
}

bool caseInsensitiveLessThan(const QString &s1, const QString &s2)
{
    return s1.toLower() < s2.toLower();
}

bool caseInsensitiveMoreThan(const QString &s1, const QString &s2)
{
    return s1.toLower() > s2.toLower();
}

bool caseSensitiveLessThan(const QString &s1, const QString &s2)
{
    return s1 < s2;
}

bool caseSensitiveMoreThan(const QString &s1, const QString &s2)
{
    return s1 > s2;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(sortdoc, SortDocumentPlugin)
#endif
