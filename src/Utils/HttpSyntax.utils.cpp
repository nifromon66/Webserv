#include "../../include/Utils/HttpSyntax.utils.hpp"

bool	is_CR(unsigned char c) {
	return (c == 0x0D);
}

bool	is_LF(unsigned char c) {
	return (c == 0x0A);
}

bool	is_SP(unsigned char c) {
	return (c == 0x20);
}

bool	is_HTAB(unsigned char c) {
	return (c == 0x09);
}

bool	is_DIGIT(unsigned char c) {
	return (c >= 0x30 && c <= 0x39);
}

bool	is_ALPHA(unsigned char c) {
	return ((c >= 0x41 && c <= 0x5A)
			|| (c >= 0x61 && c <= 0x7A));
}

bool	is_HEXDIG(unsigned char c) {
	return ((c >= 0x30 && c <= 0x39)
			|| (c >= 0x41 && c <= 0x46)
			|| (c >= 0x61 && c <= 0x66));
}

bool	is_VCHAR(unsigned char c) {
	return (c >= 0x21 && c <= 0x7E);
}

bool	is_CTL(unsigned char c) {
	return (( c <= 0x1F) || (c == 0x7F));
}

// bool	is_OCTET(unsigned char c) {
// 	return (c >= 0x00 && c <= 0xFF);
// }

bool	is_UNRESERVED(unsigned char c) {
	return (is_ALPHA(c) || is_DIGIT(c) || c == '-' || c == '.' || c == '_' || c == '~');
}

bool	is_GEN_DELIM(unsigned char c);
bool	is_SUB_DELIM(unsigned char c);

bool	is_RESERVED(unsigned char c);


bool	is_NUMBER(std::string str) {
	for (size_t i = 0; i < str.length(); ++i) {
		if (!is_DIGIT(str[i]))
			return (false);
	}
	return (true);
}