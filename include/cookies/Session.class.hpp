#ifndef SESSION_CLASS_HPP
# define SESSION_CLASS_HPP

# include <iostream>
# include <ctime>
# include <cstdlib>
# include <fstream>
# include <string>
# include <vector>
# include <map>

# include "../cookies/Cookie.class.hpp"

class   Session {

    public:
												Session();
												Session(std::string const & username, std::string const & password);
            									Session(Cookie const & session_id, std::string const & username, std::string const & password);
                        						Session(Session const & source);
                        						~Session(void);

			Cookie const &						getSessionId(void) const;
			std::string const &					getUsername(void) const;
			bool								getLog(void) const;

			void								setUsername(std::string const & username);
			void								setPassword(std::string const & password);
			void								login(void);
			void								logout(void);
			void								checkExpiry(void);

			void								initializeSession(void);
			bool								passwordVerification(std::string const & password);
			bool								SessionIdVerification(std::string const & session_id);

			Session const &						operator=(Session const & source);
			bool								operator==(Session const & compare);
    private:
			Cookie								_session_id;
            std::string							_username;
            std::string 						_password;
			bool								_log;

};

#endif