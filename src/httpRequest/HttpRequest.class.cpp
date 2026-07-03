#include "../../include/httpRequest/HttpRequest.class.hpp"

///////////////////////////////////////////////// Constructor / Destructor

HttpRequest::HttpRequest() {
	//std::cout << BLACK_ON_GREEN "HttpRequest default constructor called." RESET << std::endl;
	return ;
}

HttpRequest::HttpRequest(std::string const & request) {
	//std::cout << BLACK_ON_PURPLE "HttpRequest parametric constructor called." RESET << std::endl;
	_parse_request(request);
	return ;
}

HttpRequest::HttpRequest(HttpRequest const & source) {
	//std::cout << BLACK_ON_CYAN "HttpRequest copy constructor called." RESET << std::endl;
	*this = source;
	return ;
}

HttpRequest::~HttpRequest() {
	//std::cout << BLACK_ON_RED "HttpRequest default destructor called." RESET << std::endl;
	return ;
}

///////////////////////////////////////////////// Public member functions

HttpMethod const &	HttpRequest::get_method(void) const {
	return (_method);
}

HttpTarget const &	HttpRequest::get_target(void) const {
	return (_target);
}

HttpVersion const &	HttpRequest::get_version(void) const {
	return (_version);
}

HttpHeader const &	HttpRequest::get_header(void) const {
	return (_header);
}

HttpBody const &	HttpRequest::get_body(void) const {
	return (_body);
}

void	HttpRequest::set_first_line(std::string const & first_line) {
	size_t	i_first_line = first_line.find(WS_CRLF, 0);
	if (i_first_line == std::string::npos)
		throw HttpRequestStatus(400, "Bad Request");
	_parse_first_line(first_line.substr(0, i_first_line));

	return ;
}

void	HttpRequest::set_header(std::string const & header) {
	size_t	i_header = header.find(WS_CRLF_CRLF, 0);
	if (i_header == std::string::npos)
		throw HttpRequestStatus(400, "Bad Request");
	_header.set_header(header.substr(0, i_header));

	if (_header.find_field("Host") != -1) {
		if (_header.get_field(_header.find_field("Host")).get_values().size() != 1)
			throw HttpRequestStatus(400, "Bad Request");
	}
	else
		throw HttpRequestStatus(400, "Bad Request");

	if (_method.get_method() == METHOD_POST) {
		if (_header.find_field("Content-Length") != -1) {
			if (_header.get_field(_header.find_field("Content-Length")).get_values().size() != 1)
				throw HttpRequestStatus(411, "Length Required");
		}
	}

	return ;
}

void	HttpRequest::set_body(std::string const & body) {
	_body.set_body(body);

	return ;
}

void	HttpRequest::set_request(std::string const & request) {
	_parse_request(request);

	return ;
}

///////////////////////////////////////////////// Private member functions

void	HttpRequest::_parse_request(std::string const & request) {

	size_t	i_first_line = request.find(WS_CRLF, 0);
	if (i_first_line == std::string::npos)
		throw HttpRequestStatus(400, "Bad Request");
	_parse_first_line(request.substr(0, i_first_line));
	i_first_line += 2;

	size_t	i_header = request.find(WS_CRLF_CRLF, i_first_line);
	if (i_header == std::string::npos)
		throw HttpRequestStatus(400, "Bad Request");
	_header.set_header(request.substr(i_first_line, (i_header - i_first_line)));
	i_header += 4;

	_body.set_body(request.substr(i_header));

	return ;
}

void	HttpRequest::_parse_first_line(std::string const & first_line) {

	size_t	i_method = first_line.find(WS_SP);
	if (i_method == std::string::npos)
		throw HttpRequestStatus(400, "Bad Request");
	_method.set_method(first_line.substr(0, i_method));
	i_method++;

	size_t	i_target = first_line.find(WS_SP, i_method);
	if (i_target == std::string::npos)
		throw HttpRequestStatus(400, "Bad Request");
	_target.set_target(first_line.substr(i_method, (i_target - i_method)));
	i_target++;

	_version.set_version(first_line.substr(i_target));

	return ;
}

///////////////////////////////////////////////// Operator overloads

HttpRequest &	HttpRequest::operator=(HttpRequest const & source) {
	//std::cout << BLACK_ON_WHITE "HttpRequest assigment operator called." RESET << std::endl;
	if (this != &source) {
		_method = source._method;
		_target = source._target;
		_version = source._version;
		_header = source._header;
		_body = source._body;
	}
	return (*this);
}

std::ostream & operator<<(std::ostream & out, HttpRequest const & in) {
	out << "HttpRequest : " << std::endl;
	out << in._method << std::endl;
	out << in._target << std::endl;
	out << in._version << std::endl;
	out << in._header << std::endl;
	out << in._body << std:: endl;

	return (out);
}