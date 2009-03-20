/*
JuffEd - An advanced text editor
Copyright 2007-2009 Mikhail Murzin

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

#ifndef _NULL_DOC_H_
#define _NULL_DOC_H_

#include "Document.h"
#include "Log.h"

namespace Juff {
	
class NullDoc : public Document {
public:
	static Document* instance();
	virtual ~NullDoc();
	
	virtual bool isModified() const;
	virtual void setModified(bool);
	virtual QWidget* widget();
	virtual bool save(const QString&, QString&);
	virtual void print();
	virtual void reload();

	/**
	 *	Only in class NullDoc this method must return 'true' !!!
	 */
	virtual bool isNull() const { return true; }

protected:
	NullDoc();
	
private:
	void scream() const;
	static Juff::Document* doc_;
};

}	//	namespace Juff

#endif
