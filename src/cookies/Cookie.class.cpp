#include "../../include/cookies/Cookie.class.hpp"

///////////////////////////////////////////////// Constructor / Destructor
Cookie::Cookie() {
	_init();
	return ;
}

Cookie::Cookie(std::string const & name, std::string const & value) :
	_name(name),
	_value(value)
{
	_init();
	return ;
}

Cookie::Cookie(std::string const & name, std::string const & value, std::string const & maxAge) :
	_name(name),
	_value(value),
	_maxAge(maxAge)
{
	_init();
	return ;
}

Cookie::Cookie(Cookie const & source) {
	*this = source;
	return ;
}

Cookie::~Cookie() {
	return ;
}

///////////////////////////////////////////////// Public member functions

std::string const & Cookie::getName(void) const {
	return (_name);
}

std::string const &	Cookie::getValue(void) const
{
	return (_value);
}

std::string const & Cookie::getPath(void) const {
	return (_path);
}

std::string const &	Cookie::getMaxAge(void) const {
	return (_maxAge);
}

bool const &	Cookie::getSecure(void) const {
	return (_secure);
}

bool const &	Cookie::getHttpOnly(void) const {
	return (_httpOnly);
}

std::string const &	Cookie::getSameSite(void) const {
	return (_sameSite);
}

time_t const & Cookie::getExpiry(void) const {
	return (_expiry);
}

void	Cookie::initialize_session_id(void) {
	_value = _generate_session_id(32);
	_expiry = time(NULL) + std::atoi(_maxAge.c_str());
	return ;
}

std::string Cookie::serializeCookie(void) const {
	std::string	str = _name + "=" + _value;

	if (!_path.empty())
		str += "; Path=" + _path;
	if (!_maxAge.empty())
		str += "; Max-Age=" + _maxAge;
	if (_secure)
		str += "; Secure";
	if (_httpOnly)
		str += "; HttpOnly";
	if (!_sameSite.empty())
		str += "; SameSite=" + _sameSite;

	return (str);
}

std::string Cookie::serializeCookieClear(void) const {
	std::string	str = _name + "=" + _value;

	if (!_path.empty())
		str += "; Path=" + _path;
	if (!_maxAge.empty())
		str += "; Max-Age=0";
	if (_secure)
		str += "; Secure";
	if (_httpOnly)
		str += "; HttpOnly";
	if (!_sameSite.empty())
		str += "; SameSite=" + _sameSite;

	return (str);
}


///////////////////////////////////////////////// Private member functions

void	Cookie::_init(void) {
	if (_name.empty())
		_name = "session_id";
	if (_path.empty())
		_path = "/";
	if (_maxAge.empty())
		_maxAge = "3600"; // 1h
	_secure = true;
	_httpOnly = true;
	_sameSite = "Lax";

}

std::string	Cookie::_generate_session_id(std::size_t bytes_len) {
	std::ifstream	urandom("/dev/urandom", std::ios::in | std::ios::binary);
	if (!urandom)
		throw	std::runtime_error("cannot open /dev/urandom");

	std::string	session_id;
	session_id.reserve(bytes_len * 2);

	static const char hex[] = "0123456789abcdef";

	for (std::size_t i = 0; i < bytes_len; ++i) {
		unsigned char c;
		urandom.read(reinterpret_cast<char*>(&c), 1);
		if (!urandom)
			throw	 std::runtime_error("failed to read on /dev/urandom");
		session_id.push_back(hex[c >> 4]);
		session_id.push_back(hex[c & 0x0f]);
	}

	return (session_id);
}

///////////////////////////////////////////////// Operator overloads

Cookie &	Cookie::operator=(Cookie const & source) {
	if (this != &source) {
		_name = source._name;
		_value = source._value;
		_path = source._path;
		_maxAge = source._maxAge;
		_secure = source._secure;
		_httpOnly = source._httpOnly;
		_sameSite = source._sameSite;
		_expiry = source._expiry;
	}
	return (*this);
}

bool	Cookie::operator==(Cookie const & compare) {
	if (_name == compare._name
		&& _value == compare._value
		&& _path == compare._path
		&& _maxAge == compare._maxAge
		&& _secure == compare._secure
		&& _httpOnly == compare._httpOnly
		&& _sameSite == compare._sameSite)
		return (true);
	return (false);
}