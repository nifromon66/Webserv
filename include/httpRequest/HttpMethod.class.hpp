#ifndef HTTPMETHOD_CLASS_HPP
# define HTTPMETHOD_CLASS_HPP

# include <iostream>
# include <string>
# include <vector>
# include <exception>

# include "../Macro/Colors.macro.hpp"
# include "../Macro/HttpMethod.macro.hpp"
#include "../Error/HttpRequestStatus.error.hpp"

class	HttpMethod {
	public:
										HttpMethod();
										HttpMethod(std::string const & method);
										HttpMethod(HttpMethod const & source);
										~HttpMethod();

			std::string const &			get_method(void) const;
			void						set_method(std::string const & method);

			HttpMethod &				operator=(HttpMethod const & source);

	private:
			std::vector<std::string>	_allowed_methods_list;
			std::vector<std::string>	_unallowed_methods_list;
			std::string					_method;

			void						_init_all_methods_list(void);
			void						_init_allowed_methods_list(void);
			void						_init_unallowed_methods_list(void);

			void						_parse_method(std::string const & method);

			friend std::ostream &		operator<<(std::ostream & out, HttpMethod const & in);
};

#endif