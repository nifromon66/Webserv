#ifndef HTTPFIELD_CLASS_HPP
# define HTTPFIELD_CLASS_HPP

# include <iostream>
# include <string>
# include <vector>
# include <exception>

# include "../Macro/Colors.macro.hpp"
# include "../Utils/HttpNormalize.utils.hpp"
#include "../Error/HttpRequestStatus.error.hpp"


class	HttpField {
	public:
												HttpField();
												HttpField(std::string const & name);
												HttpField(std::string const & name, std::string const & forbidden_charset);
												HttpField(HttpField const & source);
												~HttpField();

			std::string const &					get_name(void) const;
			std::vector<std::string> const &	get_values(void) const;
			std::string const &					get_value(int const & index) const;

			void								set_name(std::string const & name);
			void								set_forbidden_charset(std::string const & forbidden_charset);
			void								set_values(std::vector<std::string> const & values);
			void								set_values(std::string const & values);

			HttpField &							operator=(HttpField const & source);

	private:
			std::string							_name;
			std::string							_forbidden_charset;
			std::vector<std::string>			_values;

			void								_parse_name(std::string const & name);
			void								_parse_forbidden_charset(std::string const & forbidden_charset);
			void								_parse_values(std::vector<std::string> const & values);
			void								_parse_values(std::string const & values);
			void								_parse_value(std::string const & value);

			friend std::ostream &				operator<<(std::ostream & out, HttpField const & in);
};

#endif