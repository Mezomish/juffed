#ifndef __JUFFED_UTILS_H__
#define __JUFFED_UTILS_H__

#include "CommandStorageInt.h"
#include "IconManagerInt.h"

class Utils {
public:
	static CommandStorageInt* commandStorage();
	static IconManagerInt* iconManager();

private:
	static CommandStorageInt* commandStorage_;
	static IconManagerInt* iconManager_;
};

#endif // __JUFFED_UTILS_H__
