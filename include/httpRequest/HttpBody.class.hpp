#ifndef HTTPBODY_CLASS_HPP
# define HTTPBODY_CLASS_HPP

# include <iostream>
# include <string>
# include <vector>
# include <exception>

# include "../Macro/Colors.macro.hpp"
#include "../Error/HttpRequestStatus.error.hpp"

class	HttpBody {
	public:
										HttpBody();
										HttpBody(std::string const & body);
										HttpBody(HttpBody const & source);
										~HttpBody();

			std::string const &			get_body(void) const;
			void						set_body(std::string const & body);

			HttpBody &				    operator=(HttpBody const & source);

	private:
			std::string					_body;

			void						_parse_body(std::string const & body);

			friend std::ostream &		operator<<(std::ostream & out, HttpBody const & in);
};

#endif