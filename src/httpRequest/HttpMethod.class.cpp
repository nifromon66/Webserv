#include "../../include/httpRequest//HttpMethod.class.hpp"

///////////////////////////////////////////////// Constructor / Destructor

HttpMethod::HttpMethod() {
	//std::cout << BLACK_ON_GREEN "HttpMethod default constructor called." RESET << std::endl;
	_init_all_methods_list();
	return ;
}

HttpMethod::HttpMethod(std::string const & method) {
	//std::cout << BLACK_ON_PURPLE "HttpMethod parametric constructor called." RESET << std::endl;
	_init_all_methods_list();
	_parse_method(method);
	return ;
}

HttpMethod::HttpMethod(HttpMethod const & source) {
	//std::cout << BLACK_ON_CYAN "HttpMethod copy constructor called." RESET << std::endl;
	*this = source;
	return ;
}

HttpMethod::~HttpMethod() {
	//std::cout << BLACK_ON_RED "HttpMethod default destructor called." RESET << std::endl;
	return ;
}

///////////////////////////////////////////////// Public member functions

std::string const &	HttpMethod::get_method(void) const {
	return (_method);
}

void	HttpMethod::set_method(std::string const & method) {
	_parse_method(method);

	return ;
}

///////////////////////////////////////////////// Private member functions

void	HttpMethod::_init_all_methods_list(void) {
	_init_allowed_methods_list();
	_init_unallowed_methods_list();

	return ;
}

void	HttpMethod::_init_allowed_methods_list(void) {
	_allowed_methods_list.push_back(METHOD_GET);
	_allowed_methods_list.push_back(METHOD_POST);
	_allowed_methods_list.push_back(METHOD_DELETE);

	return ;
}

void	HttpMethod::_init_unallowed_methods_list(void) {
	_unallowed_methods_list.push_back(METHOD_HEAD);
	_unallowed_methods_list.push_back(METHOD_PUT);
	_unallowed_methods_list.push_back(METHOD_OPTIONS);
	_unallowed_methods_list.push_back(METHOD_TRACE);
	_unallowed_methods_list.push_back(METHOD_CONNECT);
	_unallowed_methods_list.push_back(METHOD_PATCH);

	return ;
}

void	HttpMethod::_parse_method(std::string const & method) {
	for (size_t i = 0; i != _allowed_methods_list.size(); ++i) {
		if (method == _allowed_methods_list[i]) {
			_method = method;
			return ;
		}
	}
	for (size_t i = 0; i != _unallowed_methods_list.size(); ++i) {
		if (method == _unallowed_methods_list[i]) {
			if (_unallowed_methods_list[i] == METHOD_HEAD)
				throw HttpRequestStatus(405, "Method Not Allowed");
			else
				throw HttpRequestStatus(501, "Not Implemented");
		}
	}
	throw HttpRequestStatus(400, "Bad Request");

	return ;
}

///////////////////////////////////////////////// Operator overloads

HttpMethod &	HttpMethod::operator=(HttpMethod const & source) {
	//std::cout << BLACK_ON_WHITE "HttpMethod assigment operator called." RESET << std::endl;
	if (this != &source) {
		_allowed_methods_list = source._allowed_methods_list;
		_unallowed_methods_list = source._unallowed_methods_list;
		_method = source._method;
	}
	return (*this);
}

std::ostream & operator<<(std::ostream & out, HttpMethod const & in) {
	out << "HttpMethod : " << std::endl;
	out << "{" << std::endl;
	out << "\tMethod : " << "'" << in._method << "'." << std::endl;
	out << "};" << std::endl;

	return (out);
}