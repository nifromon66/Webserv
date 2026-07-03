#ifndef HTTPREQUESTSTATUS_ERROR_HPP
# define HTTPREQUESTSTATUS_ERROR_HPP

# include <iostream>
# include <string>
# include <exception>

# include "../Macro/Colors.macro.hpp"

class HttpRequestStatus
{
	public:
									HttpRequestStatus();
									HttpRequestStatus(int const & code, std::string const & message);
									HttpRequestStatus(HttpRequestStatus const & source);
									~HttpRequestStatus();

			int const &				getCode(void) const;
			std::string	const &		getMessage(void) const;

			void					setCode(int const & code);
			void					setMessage(std::string const & message);

			HttpRequestStatus &				operator=(HttpRequestStatus const & source);

	private:
			int						_code;
			std::string				_message;

			friend std::ostream &	operator<<(std::ostream & out, HttpRequestStatus const & in);
};

#endif