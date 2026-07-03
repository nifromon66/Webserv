#include "../../include/Utils/HttpNormalize.utils.hpp"
#include <sstream>

// Utils
void	normalize_to_lowercase(std::string & str) {

	for (size_t i = 0; i < str.length(); ++i) {
		if (str[i] > 64 && str[i] < 91)
			str[i] += 32;
	}
}

void	normalize_to_uppercase(std::string & str) {

	for (size_t i = 0; i < str.length(); ++i) {
		if (str[i] > 'a' && str[i] < 'z')
			str[i] -= 32;
	}
}

char	normalize_hex_to_ascii(std::string const & str) {
	if (str.length() != 2)
		return (0);

	std::string	hexLow("0123456789abcdef");
	if (hexLow.find(str[0]) != std::string::npos && hexLow.find(str[1]) != std::string::npos) {
		return ((hexLow.find(str[0]) * 16) + (hexLow.find(str[1]) * 1));
	}

	std::string	hexUpp("0123456789ABCDEF");
	if (hexUpp.find(str[0]) != std::string::npos && hexUpp.find(str[1]) != std::string::npos) {
		return ((hexUpp.find(str[0]) * 16) + (hexUpp.find(str[1]) * 1));
	}

	return (0);
}

void	normalize_percent_encoding(std::string & str) {
	if (str.find('%') == std::string::npos)
		return ;

	for (size_t i = 0; i < str.length(); ++i) {
		if (str[i] == '%') {
			str[i] = normalize_hex_to_ascii(str.substr(i + 1, 2));
			str.erase(i + 1, 2);
		}
	}

	return ;
}

// Standard
void	normalize_scheme(std::string & scheme) {
	if (scheme.empty()) {
		scheme = "http";
		return ;
	}
	normalize_to_lowercase(scheme);
	return ;
}

void	normalize_host(std::string & host) {
	if (host.empty()) {
		return ;
	}
	normalize_to_lowercase(host);
	return ;
}

void	normalize_path(std::string & path) {
	if (path.empty()) {
		path = "/";
		return ;
	}
	return ;
}

void	normalize_port(std::string & port) {
	if (port.empty()) {
		port = "80";
		return ;
	}
	return ;
}