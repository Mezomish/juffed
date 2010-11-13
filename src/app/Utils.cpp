#include "Utils.h"

#include "CommandStorage.h"
#include "IconManager.h"

CommandStorageInt* Utils::commandStorage_ = NULL;
IconManagerInt* Utils::iconManager_ = NULL;

CommandStorageInt* Utils::commandStorage() {
	if ( commandStorage_ == NULL ) {
		commandStorage_ = new CommandStorage(iconManager());
	}
	return commandStorage_;
}

IconManagerInt* Utils::iconManager() {
	if ( iconManager_ == NULL ) {
		iconManager_ = new IconManager();
	}
	return iconManager_;
}

