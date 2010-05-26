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

#include "QSciSettings.h"
#include "Settings.h"

bool QSciSettings::get(BoolKey key) {
	switch (key) {
		case ShowIndents :
			return Settings::instance()->boolValue("QSci", "showIndents");
		
		case HighlightMatchingBrace :
			return Settings::instance()->boolValue("QSci", "highlightMatchingBrace");
		
		case HighlightCurLine :
			return Settings::instance()->boolValue("QSci", "highlightCurLine");
		
		case HighlightCurWord :
			return Settings::instance()->boolValue("QSci", "highlightCurWord");
		
		case JumpOverWordParts :
			return Settings::instance()->boolValue("QSci", "jumpOverWordParts");
		
		
	}
	return false;
}

QColor QSciSettings::get(ColorKey key) {
	switch (key) {
		case MatchingBraceBgColor :
		{
			QColor c = Settings::instance()->value("QSci", "matchingBraceBgColor").value<QColor>();
			return c.isValid() ? c : Settings::instance()->defaultValue("QSci", "matchingBraceBgColor").value<QColor>();
		}
		case MatchingBraceFgColor :
		{
			QColor c = Settings::instance()->value("QSci", "matchingBraceFgColor").value<QColor>();
			return c.isValid() ? c : Settings::instance()->defaultValue("QSci", "matchingBraceFgColor").value<QColor>();
		}
		case IndentsColor :
		{
			QColor c = Settings::instance()->value("QSci", "indentsColor").value<QColor>();
			return c.isValid() ? c : Settings::instance()->defaultValue("QSci", "indentsColor").value<QColor>();
		}
		case WordHLColor :
		{
			QColor c = Settings::instance()->value("QSci", "wordHLColor").value<QColor>();
			return c.isValid() ? c : Settings::instance()->defaultValue("QSci", "wordHLColor").value<QColor>();
		}
		case CurLineColor :
		{
			QColor c = Settings::instance()->value("QSci", "curLineColor").value<QColor>();
			return c.isValid() ? c : Settings::instance()->defaultValue("QSci", "curLineColor").value<QColor>();
		}
	}
	return QColor();
}



void QSciSettings::set(BoolKey key, bool value) {
	switch (key) {
		case ShowIndents:
			Settings::instance()->setValue("QSci", "showIndents", value);
			break;
		
		case HighlightMatchingBrace :
			Settings::instance()->setValue("QSci", "highlightMatchingBrace", value);
			break;
		
		case HighlightCurLine :
			Settings::instance()->setValue("QSci", "highlightCurLine", value);
			break;
		
		case HighlightCurWord :
			Settings::instance()->setValue("QSci", "highlightCurWord", value);
			break;
		
		case JumpOverWordParts :
			Settings::instance()->setValue("QSci", "jumpOverWordParts", value);
			break;
	}
}

void QSciSettings::set(ColorKey key, const QColor& c) {
	switch (key) {
		case MatchingBraceBgColor :
			Settings::instance()->setValue("QSci", "matchingBraceBgColor", c);
			break;
		
		case MatchingBraceFgColor :
			Settings::instance()->setValue("QSci", "matchingBraceFgColor", c);
			break;
		
		case IndentsColor :
			Settings::instance()->setValue("QSci", "indentsColor", c);
			break;
		
		case WordHLColor :
			Settings::instance()->setValue("QSci", "wordHLColor", c);
			break;
		
		case CurLineColor :
			Settings::instance()->setValue("QSci", "curLineColor", c);
			break;
	}
}
