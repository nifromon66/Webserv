#include "../../include/httpRequest/HttpHeader.class.hpp"

///////////////////////////////////////////////// Constructor / Destructor

HttpHeader::HttpHeader() {
	//std::cout << BLACK_ON_GREEN "HttpHeader default constructor called." RESET << std::endl;
	return ;
}

HttpHeader::HttpHeader(std::string const & header) {
	//std::cout << BLACK_ON_PURPLE "HttpHeader parametric constructor called." RESET << std::endl;
	_parse_header(header);
	return ;
}

HttpHeader::HttpHeader(HttpHeader const & source) {
	//std::cout << BLACK_ON_CYAN "HttpHeader copy constructor called." RESET << std::endl;
	*this = source;
	return ;
}

HttpHeader::~HttpHeader() {
	//std::cout << BLACK_ON_RED "HttpHeader default destructor called." RESET << std::endl;
	return ;
}

///////////////////////////////////////////////// Public member functions

int	HttpHeader::find_field(std::string const & field) {
	for (size_t i = 0; i < _header.size(); ++i) {
		if (_header[i].get_name() == field)
			return (i);
	}

	return (-1);
}

HttpField const &	HttpHeader::get_field(int pos) const {
	return (_header[pos]);
}

std::vector<HttpField> const &	HttpHeader::get_header(void) const {
	return (_header);
}

void	HttpHeader::set_header(std::string const & header) {
	_parse_header(header);
	return ;
}

///////////////////////////////////////////////// Private member functions

void	HttpHeader::_parse_header(std::string const & header) {

	for (size_t i = 0; i < header.length(); ++i) {
		size_t	new_line = header.find('\n', i);
		if (new_line != std::string::npos)
			_parse_field(header.substr(i, (new_line - i)));
		else if (new_line == std::string::npos) {
			_parse_field(header.substr(i));
			break ;
		}
		i += new_line - i;
	}

	return ;
}

void	HttpHeader::_parse_field(std::string const & field) {
	HttpField   tmp_field;

	size_t  i_equal = field.find(':');
	if (i_equal == std::string::npos)
		throw HttpRequestStatus(404, "Bad Request");

	tmp_field.set_name(field.substr(0, i_equal));
	i_equal++;
	for (size_t i = i_equal; i < field.length(); ++i) {
		if (field[i] == ' ')
			i_equal++;
	}
	tmp_field.set_values(field.substr(i_equal));

	_header.push_back(tmp_field);

	return ;
}

///////////////////////////////////////////////// Operator overloads

HttpHeader &	HttpHeader::operator=(HttpHeader const & source) {
	//std::cout << BLACK_ON_WHITE "HttpHeader assigment operator called." RESET << std::endl;
	if (this != &source) {
		_header = source._header;
	}
	return (*this);
}

std::ostream & operator<<(std::ostream & out, HttpHeader const & in)
{
	out << "HttpHeader : " << std::endl;
	out << "{" << std::endl;
	for (size_t i = 0; i < in._header.size(); ++i) {
		if (in._header[i].get_name().length() != 0)
			out << "\t" << in._header[i] << std::endl;
	}
	out << "};" << std::endl;

	return (out);
}