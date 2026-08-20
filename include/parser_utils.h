#ifndef PARSER_UTILS_H
#define PARSER_UTILS_H

#include <stddef.h>

void PreprocessExpression(
    const char *source,
    char *destination,
    size_t maxLength
);

#endif
