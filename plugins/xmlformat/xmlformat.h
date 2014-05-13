#ifndef XMLFORMAT_PLUGIN_H
#define XMLFORMAT_PLUGIN_H

/*
 * XML formatter plugin for Juffed editor
 *
 * Portions Copyright (C) 2010 Petr Vanek <petr@scribus.info>
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


/*! Try to format content of the current dcument as a XML or
  similar markup language (HTML, SGML,...)
 */
class XmlformatPlugin : public QObject, public JuffPlugin {

    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID JuffPlugin_iid)
#endif
    Q_INTERFACES(JuffPlugin)

public:
    XmlformatPlugin();
    ~XmlformatPlugin();
    void init();

    QString name() const;
    QString description() const;
    QString targetEngine() const;

    QToolBar* toolBar() const;
    Juff::ActionList mainMenuActions(Juff::MenuID) const;

public slots:
    void format();

private:
    QAction * actDoc;

    void formatDocument(Juff::Document *doc);
    void formatSelection(Juff::Document *doc);

    /*! QDomDocument::toString uses spaces for indent.
     * We have to change spaces to tabs manually to handle text by current
     * editor settings. See implementation notes in the method.
     * \param text a QString to be handled
     * \retval QString with leading TABs
     */
    QString changeSpacesToTabs(const QString &text);
};

#endif
