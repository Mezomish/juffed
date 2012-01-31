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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <QtCore/QObject>

#include "JuffPlugin.h"

/*! Try to format content of the current dcument as a XML or
  similar markup language (HTML, SGML,...)
 */
class XmlformatPlugin : public QObject, public JuffPlugin {

    Q_OBJECT
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
    void formatDocument();

private:
    QAction * actDoc;
};

#endif
