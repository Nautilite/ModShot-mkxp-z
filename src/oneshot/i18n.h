#ifndef I18N_H
#define I18N_H

#include "trstr.h"

void loadLocale(const char *locale);
void unloadLocale();

void loadLanguageMetadata();
void unloadLanguageMetadata();

int getFontSize();
const char *getFontName();

const char *findtext(unsigned int msgid, const char *fallback);
void decodeEscapeChars(char *s);

#endif // I18N_H
