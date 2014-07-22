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

#include "xmlformat.h"

#include <QMessageBox>
#include <QDomDocument>

#include "EditorSettings.h"


XmlformatPlugin::XmlformatPlugin() : QObject(), JuffPlugin()
{
    actDoc = new QAction(QIcon(":xmlwrap"), tr("Format XML Document"), this);
    connect(actDoc, SIGNAL(triggered()), this, SLOT(format()));
}

void XmlformatPlugin::init() {
}

XmlformatPlugin::~XmlformatPlugin() {
}

QString XmlformatPlugin::name() const {
	return "XML Formatter";
}

QString XmlformatPlugin::title() const {
	return tr("XML Formatter");
}

QString XmlformatPlugin::targetEngine() const {
	return "all";
}

QString XmlformatPlugin::description() const {
	return "Plugin that allows to format XML document - idenation, verification...";
}

QToolBar* XmlformatPlugin::toolBar() const
{
    QToolBar * bar = new QToolBar(tr("XML Formatter"));
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

void XmlformatPlugin::format()
{
    Juff::Document* doc = api()->currentDocument();
    // api()->currentDocument() never returns NULL, it returns 
    // NullDoc if there is no docs.
    if ( doc->isNull() )
        return;

    // if there is a selection - try to format a selection.
    // In the other case format all document
    if (doc->hasSelectedText())
    {
        formatSelection(doc);
    }
    else
    {
        formatDocument(doc);
    }
}

void XmlformatPlugin::formatDocument(Juff::Document *doc)
{
    QString content;
    QDomDocument dom;
    QString errmsg;
    int errline, errcolumn;

    if (! doc->getText(content))
        return;

    if (dom.setContent(content, false, &errmsg, &errline, &errcolumn))
    {
        QString newContent = dom.toString(EditorSettings::get(EditorSettings::TabWidth));

        if (EditorSettings::get(EditorSettings::UseTabs))
            newContent = changeSpacesToTabs(newContent);

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

void XmlformatPlugin::formatSelection(Juff::Document *doc)
{
    QString content;
    QDomDocument dom;
    QString errmsg;
    int errline, errcolumn;

    if (! doc->getSelectedText(content))
        return;

    int line, column, tmp1, tmp2;
    doc->getSelection(line, column, tmp1, tmp2);

    if (dom.setContent(content, false, &errmsg, &errline, &errcolumn))
    {
        QString newContent = dom.toString(EditorSettings::get(EditorSettings::TabWidth));

        if (EditorSettings::get(EditorSettings::UseTabs))
            newContent = changeSpacesToTabs(newContent);

        doc->replaceSelectedText(newContent);
    }
    else
    {
        QMessageBox::information(0, tr("XML format error"),
                                 tr("Cannot format XML due error (line: %1, column: %2)").arg(errline+line).arg(errline==1 ? errcolumn+column : errcolumn)
                                    + "<br/>"
                                    + errmsg);
    }
}

QString XmlformatPlugin::changeSpacesToTabs(const QString &text)
{
    /* hack and slash implementation of <TAB> leading lines.
     * Qt itself do not handle <TAB>s in QDomDocument::toString internally
     * so output is always <SPACE> indented.
     * Here, if is Juffed setup to use <TAB> for indentation, are leading
     * spaces converted to tabs.
     * Currently it's implemented line by line searching for 1st non-whitespace
     * character in the line. Then are spaces replaced by tabs. One tab per "indent"
     * spaces.
     * Finally the new QString is returned.
     */
    QString tab('\t');
    QStringList ret;
    int indent = EditorSettings::get(EditorSettings::TabWidth);

    foreach (QString line, text.split('\n'))
    {
        int ix = line.indexOf(QRegExp("\\S"));
        if (ix < 1)
            ret << line;
        else
        {
            line = line.replace(0, ix, tab.repeated(ix / indent));
            ret << line;
        }
    }

    return ret.join("\n");
}


#include "xmlformat.moc"

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(xmlformat, XmlformatPlugin)
#endif
