#ifndef HTTPNORMALIZE_UTILS_HPP
# define HTTPNORMALIZE_UTILS_HPP

# include <iostream>
# include <string>
# include <exception>

# include "../Macro/Colors.macro.hpp"
# include "HttpSyntax.utils.hpp"

// Standard
void	normalize_scheme(std::string & scheme);
void	normalize_host(std::string & host);
void	normalize_path(std::string & path);
void	normalize_port(std::string & port);

// Utils
void	normalize_to_lowercase(std::string & str);
void	normalize_to_uppercase(std::string & str);
char	normalize_hex_to_ascii(std::string const & str);
void	normalize_percent_encoding(std::string & str);

#endif