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

#include "Parameter.h"

namespace Juff {

Param::Param() {
	type_ = None;
}
Param::Param(int n) {
	type_ = Int;
	data_ = n;
}
Param::Param(bool b) {
	type_ = Bool;
	data_ = b;
}
Param::Param(const QString& str) {
	type_ = String;
	data_ = str;
}

bool Param::isEmpty() const {
	return type_ == None;
}
bool Param::isInt() const {
	return type_ == Int;
}
bool Param::isBool() const {
	return type_ == Bool;
}
bool Param::isString() const {
	return type_ == String;
}

QString Param::toString() const {
	return data_.toString();
}


}	//	namespace Juff
