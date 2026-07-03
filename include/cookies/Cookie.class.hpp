#ifndef COOKIE_CLASS_HPP
# define COOKIE_CLASS_HPP

# include <iostream>
# include <ctime>
# include <cstdlib>
# include <fstream>
# include <string>
# include <vector>
# include <map>

class   Cookie {
    public:
                                Cookie();
                                Cookie(std::string const & name, std::string const & value);
                                Cookie(std::string const & name, std::string const & value, std::string const & maxAge);
                                Cookie(Cookie const & source);
                                ~Cookie();

            std::string const & getName(void) const;
            std::string const & getValue(void) const;
            std::string const & getPath(void) const;
            std::string const &	getMaxAge(void) const;
			bool const &		getSecure(void) const;
			bool const &		getHttpOnly(void) const;
			std::string const &	getSameSite(void) const;
			time_t const &		getExpiry(void) const;

			void				initialize_session_id(void);
            std::string			serializeCookie(void) const;
            std::string         serializeCookieClear(void) const;

			Cookie &			operator=(Cookie const & source);
			bool				operator==(Cookie const & compare);

    private:
            std::string         _name;
            std::string         _value;
            std::string         _path;
            std::string        	_maxAge;
            bool                _secure;
            bool                _httpOnly;
            std::string         _sameSite;
			time_t				_expiry;

            void				_init(void);
			std::string			_generate_session_id(std::size_t bytes_len);


};

#endif