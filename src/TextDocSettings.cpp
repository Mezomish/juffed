#include "TextDocSettings.h"

QFont TextDocSettings::font() { 
	return QFont(Settings::value("editor", "fontFamily").toString(), Settings::value("editor", "fontSize").toInt()); 
}

int TextDocSettings::tabStopWidth() { 
	return Settings::intValue("editor", "tabStopWidth", 4); 
}

bool TextDocSettings::widthAdjust() { 
	return Settings::value("editor", "widthAdjust").toBool(); 
}

bool TextDocSettings::showLineNumbers() { 
	return Settings::boolValue("editor", "showLineNumbers", true); 
}
	
int TextDocSettings::lineLengthIndicator() { 
	return Settings::intValue("editor", "lineLengthIndicator", 80); 
}
