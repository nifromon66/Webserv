#ifndef HTTPHEADER_CLASS_HPP
# define HTTPHEADER_CLASS_HPP

# include <iostream>
# include <string>
# include <vector>
# include <exception>

# include "../Macro/Colors.macro.hpp"
# include "../httpRequest/HttpField.class.hpp"
#include "../Error/HttpRequestStatus.error.hpp"

class	HttpHeader {
	public:
											HttpHeader();
											HttpHeader(std::string const & header);
											HttpHeader(HttpHeader const & source);
											~HttpHeader();

			std::vector<HttpField> const &	get_header(void) const;
			HttpField const &				get_field(int pos) const;
			int								find_field(std::string const & field);

			void							set_header(std::string const & header);

			HttpHeader &					operator=(HttpHeader const & source);

	private:
			std::vector<HttpField>			_header;

			void							_parse_header(std::string const & header);
			void							_parse_field(std::string const & header);

			void							_verif_header(std::string const & header);

			friend std::ostream &			operator<<(std::ostream & out, HttpHeader const & in);
};

#endif