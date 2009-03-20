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

#ifndef _JUFF_PARAMETER_H_
#define _JUFF_PARAMETER_H_

#include <QtCore/QString>
#include <QtCore/QVariant>

namespace Juff {

class Param {
public:
	Param() { type_ = None; }
	Param(int n) { type_ = Int; data_ = n; }
	Param(bool b) { type_ = Bool; data_ = b; }
	Param(const QString& str) { type_ = String; data_ = str; }
	
	bool isEmpty() const { return type_ == None; }
	bool isInt() const { return type_ == Int; }
	bool isBool() const { return type_ == Bool; }
	bool isString() const { return type_ == String; }

	QString toString() const { return data_.toString(); }
	int toInt() const { return data_.toInt(); }
	bool toBool() const { return data_.toBool(); }

private:
	enum Type {
		None,
		Int,
		Bool,
		String,
	};
	
	Type type_;
	QVariant data_;
};

}	//	namespace Juff

#endif
