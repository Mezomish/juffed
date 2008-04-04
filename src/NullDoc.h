/*
JuffEd - A simple text editor
Copyright 2007-2008 Mikhail Murzin

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

class NullDoc : public Document {
public:
	static Document* instance();
	virtual ~NullDoc();
	
	virtual Document::Status save();
	virtual Document::Status saveAs();
	virtual Document::Status open();	
	virtual Document::Status reload();
	virtual Document::Status close();
	virtual Document::SaveRequest confirmForClose();

	virtual void processTheCommand(CommandID);

	/**
	 *	Only in this class this method must return 'true' !!!
	 */
	virtual bool isNull() { return true; }

protected:
	NullDoc();
	
private:
	void scream() const;
	static Document* doc_;
};

#endif
