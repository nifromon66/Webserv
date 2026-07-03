#ifndef HTTPREQUEST_CLASS_HPP
# define HTTPREQUEST_CLASS_HPP

# include <iostream>
# include <string>
# include <vector>
# include <map>
# include <exception>

# include "../Macro/Http.macro.hpp"

# include "../httpRequest/HttpMethod.class.hpp"
# include "../httpRequest/HttpTarget.class.hpp"
# include "../httpRequest/HttpVersion.class.hpp"
# include "../httpRequest/HttpHeader.class.hpp"
# include "../httpRequest/HttpBody.class.hpp"
#include "../Error/HttpRequestStatus.error.hpp"

class	HttpRequest {
	public:
										HttpRequest();
										HttpRequest(std::string const & request);
										HttpRequest(HttpRequest const & source);
										~HttpRequest();

			HttpMethod const &			get_method(void) const;
            HttpTarget const &			get_target(void) const;
			HttpVersion const &			get_version(void) const;
			HttpHeader const &			get_header(void) const;
			HttpBody const &			get_body(void) const;

			void						set_first_line(std::string const & first_line);
			void						set_header(std::string const & header);
			void						set_body(std::string const & body);
			void						set_request(std::string const & request);

			HttpRequest &				operator=(HttpRequest const & source);

	private:
			HttpMethod					_method;
            HttpTarget					_target;
            HttpVersion					_version;
            HttpHeader					_header;
            HttpBody					_body;

			void						_parse_request(std::string const & request);
			void						_parse_first_line(std::string const & first_line);

			friend std::ostream &		operator<<(std::ostream & out, HttpRequest const & in);
};

#endif