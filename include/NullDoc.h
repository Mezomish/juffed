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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef __JUFFED_NULL_DOC_H__
#define __JUFFED_NULL_DOC_H__

#include "LibConfig.h"

#include "Document.h"

class LIBJUFF_EXPORT NullDoc : public Juff::Document {
public:
	static Juff::Document* instance();
	virtual bool isNull() const { return true; }
	
	virtual QString type() const { return ""; }
//	virtual Juff::Document* createClone() { return this; }

private:
	NullDoc();
	NullDoc(const QString&);
	static Juff::Document* instance_;
};

#endif // __JUFFED_NULL_DOC_H__
