#include "AutocompleteSettings.h"

bool AutocompleteSettings::useDocument() {
	return Settings::boolValue("autocomplete", "useDocument", true);
}
bool AutocompleteSettings::useApis() {
	return Settings::boolValue("autocomplete", "useApis", true);
}
bool AutocompleteSettings::replaceWord() {
	return Settings::boolValue("autocomplete", "replaceWord", true);
}
int AutocompleteSettings::threshold() {
	return Settings::intValue("autocomplete", "threshold", 2);
}


void AutocompleteSettings::setUseDocument(bool use) {
	Settings::setValue("autocomplete", "useDocument", use);
}
void AutocompleteSettings::setUseApis(bool use) {
	Settings::setValue("autocomplete", "useApis", use);
}
void AutocompleteSettings::setReplaceWord(bool replace) {
	Settings::setValue("autocomplete", "replaceWord", replace);
}
void AutocompleteSettings::setThreshold(int thr) {
	Settings::setValue("autocomplete", "threshold", thr);
}
