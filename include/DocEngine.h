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

#ifndef __JUFFED_DOC_ENGINE_H__
#define __JUFFED_DOC_ENGINE_H__

#include "LibConfig.h"

class QAction;
class QMenu;

#include "Enums.h"
#include "Types.h"

#include <QColor>
#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QWidgetList>

namespace Juff {

class Document;

class LIBJUFF_EXPORT DocEngine {
public:
	struct ColorOption {
		QString title;
		QString section;
		QString key;
		QColor defaultColor;
		ColorOption(const QString& t, const QString& s, const QString& k, const QColor& c) {
			title = t;
			section = s;
			key = k;
			defaultColor = c;
		}
	};
	
	DocEngine();
	virtual ~DocEngine();

	// essential methods to be re-implemented
	virtual Juff::Document*  createDoc        (const QString& fileName) const = 0;
	virtual QString          type             () const = 0;

	// obtain some details
	virtual QStringList      syntaxList       () const { return QStringList(); }
	virtual Juff::ActionList mainMenuActions  (Juff::MenuID) { return Juff::ActionList(); }
	virtual QWidgetList      statusWidgets    () { return QWidgetList(); }
	// settings
	virtual bool             getSettingsPages (QStringList&, QWidgetList&) const { return false; }
	virtual void             getColorOptions  (QList<ColorOption>&) {}

	virtual void             activate         (bool act = true);
	virtual void             deactivate       (bool deact = true);
	virtual void             onDocActivated   (Juff::Document*);

	
protected:
	QAction*                 addAction        (Juff::MenuID, QAction*);
	Juff::Document*          curDoc           () const;

private:
	QMap< Juff::MenuID, QList<QAction*> > actionsMap_;
	Juff::Document* curDoc_;
};

} // namespace Juff

#endif // __JUFFED_DOC_ENGINE_H__
