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

#ifndef _TEXT_DOC_H_
#define _TEXT_DOC_H_

class DocFindFlags;
class TextDocInterior;
class TextDocView;

#include "Document.h"

class TextDoc : public Juff::Document {
Q_OBJECT
public:
	TextDoc(const QString& fileName, DocView* view);
	virtual ~TextDoc();
	
	virtual Juff::Document::Status save();
	virtual Juff::Document::Status saveAs();
	virtual Juff::Document::Status open();
	virtual Juff::Document::Status reload();
	virtual Juff::Document::SaveRequest confirmForClose();

	virtual void processTheCommand(CommandID);
	virtual void applySettings();

	QString charset() const;
	bool setCharset(const QString& charset);

	//	tests
	static int count();
	
private:
	TextDocView* textDocView() const;
	
	Juff::Document::Status readContent(const QString&);
	Juff::Document::Status writeContent(const QString&, bool getNewName = true);
	void find(const QString&, const DocFindFlags&);
	
	TextDocInterior* docInt_;
};

#endif
