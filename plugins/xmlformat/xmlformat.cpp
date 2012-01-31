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

#include "xmlformat.h"

#include <QtCore>
#include <QtGui>
#include <QDomDocument>

#include "Document.h"



XmlformatPlugin::XmlformatPlugin() : QObject(), JuffPlugin()
{
    actDoc = new QAction(QIcon(":xmlwrap"), tr("Format XML Document"), this);
    connect(actDoc, SIGNAL(triggered()), this, SLOT(formatDocument()));
}

void XmlformatPlugin::init() {
}

XmlformatPlugin::~XmlformatPlugin() {
}

QString XmlformatPlugin::name() const {
	return "XML Formatter";
}

QString XmlformatPlugin::targetEngine() const {
	return "all";
}

QString XmlformatPlugin::description() const {
	return "Plugin that allows to format XML document - idenation, verification...";
}

QToolBar* XmlformatPlugin::toolBar() const
{
    QToolBar * bar = new QToolBar("XML Formatter");
    bar->addAction(actDoc);
    bar->setObjectName("XMLFormatterToolBar");
    return bar;
}

Juff::ActionList XmlformatPlugin::mainMenuActions(Juff::MenuID id) const
{
    Juff::ActionList list;
    if ( Juff::MenuTools == id )
    {
        list << actDoc;
    }
    return list;
}

void XmlformatPlugin::formatDocument()
{
    Juff::Document* doc = api()->currentDocument();
    // api()->currentDocument() never returns NULL, it returns 
    // NullDoc if there is no docs.
    if ( doc->isNull() )
        return;

    QString content;
    if (! doc->getText(content))
        return;

    QDomDocument dom;
    QString errmsg;
    int errline, errcolumn;
    if (dom.setContent(content, false, &errmsg, &errline, &errcolumn))
    {
        QString newContent = dom.toString(4);
        doc->setText(newContent);
    }
    else
    {
        QMessageBox::information(0, tr("XML format error"),
                                 tr("Cannot format XML due error (line: %1, column: %2)").arg(errline).arg(errcolumn)
                                    + "<br/>"
                                    + errmsg);
    }
}


#include "xmlformat.moc"

Q_EXPORT_PLUGIN2(xmlformat, XmlformatPlugin)

