#include "parser_utils.h"

#include <ctype.h>

void PreprocessExpression(
    const char *source,
    char *destination,
    size_t maxLength
)
{
    size_t j = 0;

    for (
        size_t i = 0;
        source[i] != '\0' && j < maxLength - 1;
        i++
    )
    {
        char current = source[i];

        destination[j++] = current;

        if (source[i + 1] != '\0')
        {
            char next = source[i + 1];

            int isCurrentDigit =
                isdigit((unsigned char)current);

            int isCurrentVariableLetter =
                (
                    current == 'x' ||
                    current == 'X' ||
                    current == 'e' ||
                    current == 'E'
                );

            int isCurrentClosingParenthesis =
                (current == ')');

            int isNextDigit =
                isdigit((unsigned char)next);

            int isNextVariableLetter =
                (
                    next == 'x' ||
                    next == 'X' ||
                    isalpha((unsigned char)next)
                );

            int isNextOpeningParenthesis =
                (next == '(');

            if (
                (
                    isCurrentDigit &&
                    (
                        isNextVariableLetter ||
                        isNextOpeningParenthesis
                    )
                )
                ||
                (
                    isCurrentVariableLetter &&
                    (
                        isNextDigit ||
                        isNextVariableLetter ||
                        isNextOpeningParenthesis
                    )
                )
                ||
                (
                    isCurrentClosingParenthesis &&
                    (
                        isNextDigit ||
                        isNextVariableLetter ||
                        isNextOpeningParenthesis
                    )
                )
            )
            {
                if (j < maxLength - 1)
                {
                    destination[j++] = '*';
                }
            }
        }
    }

    destination[j] = '\0';
}
