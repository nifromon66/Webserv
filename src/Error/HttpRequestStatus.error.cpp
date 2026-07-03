#include "../../include/Error/HttpRequestStatus.error.hpp"

///////////////////////////////////////////////// Constructor / Destructor

HttpRequestStatus::HttpRequestStatus() : _code(200), _message("OK") {
	//std::cout << BLACK_ON_GREEN "HttpRequestStatus default constructor called." RESET << std::endl;
	return ;
}

HttpRequestStatus::HttpRequestStatus(int const & code, std::string const & message) : _code(code), _message(message) {
	//std::cout << BLACK_ON_PURPLE "HttpRequestStatus parametric constructor called." RESET << std::endl;
	return ;
}

HttpRequestStatus::HttpRequestStatus(HttpRequestStatus const & source) {
	//std::cout << BLACK_ON_CYAN "HttpRequestStatus copy constructor called." RESET << std::endl;
	*this = source;
	return ;
}

HttpRequestStatus::~HttpRequestStatus() {
	//std::cout << BLACK_ON_RED "HttpRequestStatus default destructor called." RESET << std::endl;
	return ;
}

///////////////////////////////////////////////// Public member functions

int const &	HttpRequestStatus::getCode(void) const {
	return (_code);
}

std::string const &	HttpRequestStatus::getMessage(void) const {
	return (_message);
}

void	HttpRequestStatus::setCode(int const & code) {
	_code = code;
	return ;
}

void	HttpRequestStatus::setMessage(std::string const & message) {
	_message = message;
	return ;
}

///////////////////////////////////////////////// Private member functions

///////////////////////////////////////////////// Operator overloads

HttpRequestStatus &	HttpRequestStatus::operator=(HttpRequestStatus const & source) {
	//std::cout << BLACK_ON_WHITE "HttpRequestStatus assigment operator called." RESET << std::endl;
	if (this != &source) {
		_code = source._code;
		_message = source._message;
	}
	return (*this);
}

std::ostream & operator<<(std::ostream & out, HttpRequestStatus const & in)
{
	out << "HttpRequestStatus : '" << in._code << " : " << in._message <<  "'." << std::endl;
	return (out);
}