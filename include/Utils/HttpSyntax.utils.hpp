#ifndef HTTPSYNTAX_UTILS_HPP
# define HTTPSYNTAX_UTILS_HPP

#include <iostream>
#include <string>

# include	"../Macro/HttpSyntax.macro.hpp"

// Char
bool	is_CR(unsigned char c);
bool	is_LF(unsigned char c);
bool	is_SP(unsigned char c);
bool	is_HTAB(unsigned char c);
bool	is_DIGIT(unsigned char c);
bool	is_ALPHA(unsigned char c);
bool	is_HEXDIG(unsigned char c);
bool	is_VCHAR(unsigned char c);
bool	is_CTL(unsigned char c);
bool	is_OCTET(unsigned char c);
bool	is_UNRESERVED(unsigned char c);
bool	is_RESERVED(unsigned char c);
bool	is_GEN_DELIM(unsigned char c);
bool	is_SUB_DELIM(unsigned char c);

// String
bool	is_NUMBER(std::string str);

#endif