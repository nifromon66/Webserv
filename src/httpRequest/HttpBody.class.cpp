#include "../../include/httpRequest/HttpBody.class.hpp"

///////////////////////////////////////////////// Constructor / Destructor

HttpBody::HttpBody() {
	//std::cout << BLACK_ON_GREEN "HttpBody default constructor called." RESET << std::endl;
	return ;
}

HttpBody::HttpBody(std::string const & body) {
	//std::cout << BLACK_ON_PURPLE "HttpBody parametric constructor called." RESET << std::endl;
	_parse_body(body);
	return ;
}

HttpBody::HttpBody(HttpBody const & source) {
	//std::cout << BLACK_ON_CYAN "HttpBody copy constructor called." RESET << std::endl;
	*this = source;
	return ;
}

HttpBody::~HttpBody() {
	//std::cout << BLACK_ON_RED "HttpBody default destructor called." RESET << std::endl;
	return ;
}

///////////////////////////////////////////////// Public member functions

std::string const &	HttpBody::get_body(void) const {
	return (_body);
}

void	HttpBody::set_body(std::string const & body) {
	_parse_body(body);
	return ;
}

///////////////////////////////////////////////// Private member functions

void	HttpBody::_parse_body(std::string const & body) {
	_body = body;
	return ;
}

///////////////////////////////////////////////// Operator overloads

HttpBody &	HttpBody::operator=(HttpBody const & source) {
	//std::cout << BLACK_ON_WHITE "HttpBody assigment operator called." RESET << std::endl;
	if (this != &source) {
		_body = source._body;
	}
	return (*this);
}

std::ostream & operator<<(std::ostream & out, HttpBody const & in) {
	out << "HttpBody : " << std::endl;
	out << "{" << std::endl;
	out << "\tBody : " << "'" << in._body << "'." << std::endl;
	out << "};" << std::endl;

	return (out);
}