#ifndef _MENCI_HTML_PARSER_LEXER_H
#define _MENCI_HTML_PARSER_LEXER_H

#include "StringEx/StringEx.h"

#include <vector>
#include "Token.h"

std::vector<Token> getTokens(const StringEx &htmlContents);

#endif // _MENCI_HTML_PARSER_LEXER_H
