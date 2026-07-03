#include "../../include/cookies/Session.class.hpp"

///////////////////////////////////////////////// Constructor / Destructor

Session::Session() {
	return ;
}

Session::Session(std::string const & username, std::string const & password) :
	_username(username),
	_password(password),
	_log(false)
{
	_session_id.initialize_session_id();
	return ;
}

Session::Session(Cookie const & session_id, std::string const & username, std::string const & password) :
    _session_id(session_id),
	_username(username),
    _password(password),
	_log(false)
{
    return ;
}

Session::Session(Session const & source) {
    *this = source;
    return ;
}

Session::~Session(void) {
    return ;
}

///////////////////////////////////////////////// Public member functions

Cookie const &	Session::getSessionId(void) const {
	return (_session_id);
}

std::string const &	Session::getUsername(void) const {
    return (_username);
}

void	Session::setUsername(std::string const & username) {
	_username = username;
	return ;
}

void	Session::setPassword(std::string const & password) {
	_password = password;
	return ;
}

bool	Session::getLog(void) const {
	return (_log);
}

void	Session::login(void) {
	_log = true;
}

void	Session::logout(void) {
	_log = false;
}

void	Session::checkExpiry(void) {
	if (_session_id.getExpiry() < time(NULL))
		logout();
	return ;
}

void	Session::initializeSession(void) {
	_session_id.initialize_session_id();
	_username.clear();
	_password.clear();
	_log = false;
	return ;
}

bool	Session::passwordVerification(std::string const & password) {
	if (password == _password)
		return (true);
	return (false);
}

bool	Session::SessionIdVerification(std::string const & session_id) {
	if (session_id == _session_id.getValue())
		return (true);
	return (false);
}

///////////////////////////////////////////////// Private member functions

///////////////////////////////////////////////// Operator overloads

Session const &	Session::operator=(Session const & source) {
	if (this != &source) {
		_session_id = source._session_id;
		_username = source._username;
		_password = source._password;
		_log = source._log;
	}
	return (*this);
}

bool	Session::operator==(Session const & compare) {
	if (_session_id == compare._session_id
		&& _username == compare._username
		&& _password == compare._password
		&& _log == compare._log)
		return (true);
	return (false);
}