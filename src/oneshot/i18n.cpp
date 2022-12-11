#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "i18n.h"

char **strdict = 0;
unsigned int nStr = 0;

const int MAX_LANGUAGES = 20;
const int LANGCODE_SIZE = 16;
const int LANGFONT_SIZE = 128;

char *currentLocale = 0;

struct LanguageFontAndSize
{
	char *lang_code = 0;
	int size = 0;
	char *font_name = 0;
};

LanguageFontAndSize **languageMetadata = 0;

void loadLocale(const char *locale)
{
	char line[1024];
	char pathbuf[100];

	FILE *locfile;

	unloadLocale();

	currentLocale = (char*)calloc(128, sizeof(char));
	strncpy(currentLocale, locale, 128 - 1);

	// Currently there are 52, but 100 should be plenty if we ever do add more
	int dictSize = 100;
	strdict = (char**)malloc(sizeof(char*) * dictSize);

	sprintf(pathbuf, "Languages/internal/%s.po", locale);
	locfile = fopen(pathbuf, "r");

	if (locfile) {
		while (fgets(line, 1024, locfile)) {
			if (strncmp("msgstr \"", line, 8) == 0) {
				char *lineWithoutMsgid = line + 8;
				char *endQuoteAddress = strrchr(lineWithoutMsgid, '"');

				// End string at last quotation mark
				if (endQuoteAddress != 0)
					endQuoteAddress[0] = 0;

				decodeEscapeChars(lineWithoutMsgid);

				int lineLen = strlen(lineWithoutMsgid);

				strdict[nStr] = (char*)malloc(lineLen + 1);
				strcpy(strdict[nStr], lineWithoutMsgid);

				nStr++;
			}
		}
		fclose(locfile);
	}
}

void unloadLocale()
{
	for (unsigned int i = 0; i < nStr; i++)
		free(strdict[i]);

	free(strdict);
	free(currentLocale);

	strdict = 0;
	nStr = 0;
}

void loadLanguageMetadata()
{
	char line[256];

	// Clear old metadata if have
	if (languageMetadata)
		unloadLanguageMetadata();

	// Load language fonts names
	languageMetadata = (LanguageFontAndSize**)calloc(MAX_LANGUAGES, sizeof(LanguageFontAndSize*));
	FILE *fontsFile = fopen("Languages/internal/language_fonts.ini", "r");

	if (fontsFile) {
		int languageMetadataIndex = 0;
		while (fgets(line, 1024, fontsFile)) {
			char *indexOfEquals = strchr(line, '=');
			if (indexOfEquals) {
				// Splitting the string in place here
				indexOfEquals[0] = 0;
				char *indexOfFontName = indexOfEquals + 1;

				// Remove new line from end of font name
				char *indexOfNewLine = strchr(indexOfFontName, '\n');
				if (indexOfNewLine)
					indexOfNewLine[0] = 0;

				// Make new strings for code and font name
				char *langCode = (char*)calloc(LANGCODE_SIZE, sizeof(char));
				char *langFont = (char*)calloc(LANGFONT_SIZE, sizeof(char));

				strcpy(langCode, line);
				strcpy(langFont, indexOfFontName);

				// Allocate language metadata memory
				LanguageFontAndSize *metadata = (LanguageFontAndSize*) calloc(1, sizeof(LanguageFontAndSize));
				metadata->font_name = langFont;
				metadata->lang_code = langCode;

				// Put into language metadata array and check if we're going
				// to go over the language limit
				languageMetadata[languageMetadataIndex] = metadata;
				languageMetadataIndex++;
				if (languageMetadataIndex >= MAX_LANGUAGES)
					break;
			}
		}
		fclose(fontsFile);
	}

	/* Load language fonts sizes */
	FILE *fontSizesFile = fopen("Languages/internal/language_sizes.ini", "r");

	if (fontSizesFile) {
		while (fgets(line, 1024, fontSizesFile)) {
			char *indexOfEquals = strchr(line, '=');
			if (indexOfEquals) {
				// Splitting the string in place here
				indexOfEquals[0] = 0;
				char *indexOfFontSize = indexOfEquals + 1;

				// Remove new line from end of font size
				char *indexOfNewLine = strchr(indexOfFontSize, '\n');
				if (indexOfNewLine)
					indexOfNewLine[0] = 0;

				int fontSize = atoi(indexOfFontSize);

				for (int i = 0; i < MAX_LANGUAGES; i++) {
					// Search for corresponding langCode in metadata array
					// to populate font size in the appropriate metadata
					LanguageFontAndSize *metadata = languageMetadata[i];
					if (metadata && strcmp(line, metadata->lang_code) == 0) {
						metadata->size = fontSize;
						break;
					}
				}
			}
		}
		fclose(fontSizesFile);
	}
}

void unloadLanguageMetadata()
{
	if (languageMetadata) {
		for (int i = 0; i < MAX_LANGUAGES; ++i) {
			LanguageFontAndSize *ldata = languageMetadata[i];
			if (ldata) {
				if (ldata->font_name)
					free(ldata->font_name);

				if (ldata->lang_code)
					free(ldata->lang_code);

				free(ldata);
			}
		}
	}
	free(languageMetadata);
}

int getFontSize()
{
	int fontSize = 12;

	if (currentLocale == NULL)
		return fontSize;

	for (int i = 0; i < MAX_LANGUAGES; i++) {
		LanguageFontAndSize *metadata = languageMetadata[i];

		if (metadata && strcmp(currentLocale, metadata->lang_code) == 0)
			fontSize = metadata->size;
	}

	return fontSize;
}

const char *getFontName()
{
	const char *fontName = "Terminus (TTF)";

	if (currentLocale == NULL)
		return fontName;

	for (int i = 0; i < MAX_LANGUAGES; i++) {
		LanguageFontAndSize* metadata = languageMetadata[i];

		if (metadata && strcmp(currentLocale, metadata->lang_code) == 0)
			fontName = metadata->font_name;
	}

	return fontName;
}

const char *findtext(unsigned int msgid, const char *fallback)
{
	if (msgid >= nStr)
		return fallback;
	else
		return strdict[msgid];
}

/* Replaces escape characters with their actual values in-place in a string
 * can do it in-place because the replacement values are always smaller
 * than the original value it only handles \\ and \",
 * but this can be expanded to include more if needed.
 */
void decodeEscapeChars(char *s)
{
	char *dest = s;
	char *src = s;

	while (src[0] != 0) {
		switch (src[0]) {
			case '\\':
				switch (src[1]) {
					case '\\':
						dest[0] = '\\';
						dest++;
						src += 2;
						break;

					case '\"':
						dest[0] = '\"';
						dest++;
						src += 2;
						break;

					// End of the string?
					// So only advance forward 1 character
					case 0:
						dest[0] = '\\';
						dest++;
						src++;
						break;

					// Not escape, handle normally
					default:
						dest[0] = '\\';
						dest[1] = src[1];
						dest += 2;
						src += 2;
						break;
				}
				break;

			default:
				dest[0] = src[0];
				dest++;
				src++;
		}
	}

	// Cap the end of the string
	dest[0] = 0;
}
