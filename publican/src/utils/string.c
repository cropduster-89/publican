#include <stdbool.h>
#include <stdint.h>

#include "utils.h"

extern bool IsLineEnd(char c)
{
	return (c == '\n') || (c == '\r');
}

extern bool IsWhitespace(char c)
{
	return (c == ' ') || (c == '\t') || (c == '\v') || (c == '\f') || IsLineEnd(c);
}

extern bool StringsAreEqual(uintptr_t length,
				   			char *a,
				   			char *b)
{
	bool result = false;

	if(b) {
		char *at = b;
		for(uintptr_t i = 0; i < length; ++i, ++at) {
			if((*at == 0) || (a[i] != *at)) {
				result = false;
				break;
			}
		}
		result = (*at == 0);
	} else {
		result = (length == 0);
	}

	return result;
}