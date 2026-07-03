#ifndef HTTPVERSION_CLASS_HPP
# define HTTPVERSION_CLASS_HPP

# include <iostream>
# include <string>
# include <vector>
# include <exception>

# include "../Macro/Colors.macro.hpp"
# include "../Utils/HttpSyntax.utils.hpp"
#include "../Error/HttpRequestStatus.error.hpp"


class	HttpVersion {
	public:
										HttpVersion();
										HttpVersion(std::string const & version);
										HttpVersion(HttpVersion const & source);
										~HttpVersion();

			std::string const &			get_version(void) const;
			void						set_version(std::string const & version);

			HttpVersion &				operator=(HttpVersion const & source);

	private:
			std::string					_version;

			void						_parse_version(std::string const & version);

			friend std::ostream &		operator<<(std::ostream & out, HttpVersion const & in);
};

#endif