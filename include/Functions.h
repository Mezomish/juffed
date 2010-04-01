#ifndef __JUFFED_FUNCTIONS_H__
#define __JUFFED_FUNCTIONS_H__

#include <QString>

namespace Juff {

class Document;

bool isNoname(const QString& fileName);
bool isNoname(Juff::Document*);

QString docTitle(const QString& fileName, bool modified);
QString docTitle(Juff::Document*);

}

#endif // __JUFFED_FUNCTIONS_H__
