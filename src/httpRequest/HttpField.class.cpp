#include "../../include/httpRequest/HttpField.class.hpp"

///////////////////////////////////////////////// Constructor / Destructor

HttpField::HttpField() {
	//std::cout << BLACK_ON_GREEN "HttpField default constructor called." RESET << std::endl;
	return ;
}

HttpField::HttpField(std::string const & name) {
	//std::cout << BLACK_ON_PURPLE "HttpField parametric constructor called." RESET << std::endl;
	_parse_name(name);
	return ;
}

HttpField::HttpField(std::string const & name, std::string const & forbidden_charset) {
	//std::cout << BLACK_ON_PURPLE "HttpField parametric constructor called." RESET << std::endl;
	_parse_name(name);
	_parse_forbidden_charset(forbidden_charset);
	return ;
}

HttpField::HttpField(HttpField const & source) {
	//std::cout << BLACK_ON_CYAN "HttpField copy constructor called." RESET << std::endl;
	*this = source;
	return ;
}

HttpField::~HttpField() {
	//std::cout << BLACK_ON_RED "HttpField default destructor called." RESET << std::endl;
	return ;
}

///////////////////////////////////////////////// Public member functions

std::string const &	HttpField::get_name(void) const {
	return (_name);
}

std::vector<std::string> const & HttpField::get_values(void) const {
	return (_values);
}

std::string const & HttpField::get_value(int const & index) const {
	return (_values[index]);
}

void	HttpField::set_name(std::string const & name) {
	_parse_name(name);
	return ;
}

void	HttpField::set_values(std::vector<std::string> const & values) {
	_parse_values(values);
	return ;
}

void	HttpField::set_values(std::string const & value) {
	_parse_values(value);
	return ;
}

///////////////////////////////////////////////// Private member functions

void	HttpField::_parse_name(std::string const & name) {
	if (!((name[1] > 64 && name[1] < 91) || (name[1] > 96 && name[1] < 123)))
		throw HttpRequestStatus(400, "Bad Request");

	for (size_t i = 0; i < name.length(); ++i) {
		if (!((name[i] > 64 && name[i] < 91) || (name[i] > 96 && name[i] < 123) || (name[i] > 47 && name[i] < 58) || name[i] == '-'))
			throw HttpRequestStatus(400, "Bad Request");
		if (name[i] == '-' && (i == 0 || name[i + 1] == '\0'))
			throw HttpRequestStatus(400, "Bad Request");
	}

	_name = name;

	return ;
}

void	HttpField::_parse_forbidden_charset(std::string const & forbidden_charset) {
	_forbidden_charset = forbidden_charset;
	return ;
}

void	HttpField::_parse_values(std::vector<std::string> const & values) {

	for (size_t i = 0; i != values.size(); ++i) {
		_parse_value(values[i]);
	}

	return ;
}

void	HttpField::_parse_values(std::string const & values) {

	for (size_t i = 0; i < values.length(); ++i) {
		size_t	comma = values.find(", ", i);
		if (comma != std::string::npos)
			_parse_value(values.substr(i, (comma - i)));
		else if (comma == std::string::npos) {
			_parse_value(values.substr(i));
			break ;
		}
		i += (comma + 1) - i;
	}

	return ;
}

void	HttpField::_parse_value(std::string const & value) {
	if (_forbidden_charset.length() != 0) {
		for (size_t i = 0; i < value.length(); ++i) {
			char c = value[i];
			for (size_t i = 0; i < _forbidden_charset.length(); ++i) {
				if (c == _forbidden_charset[i])
					throw HttpRequestStatus(400, "Bad Request");
			}
		}
	}

	for (size_t i = 0; i < _values.size(); ++i) {
		if (value == _values[i])
			return ;
	}
	_values.push_back(value);

	return ;
}

///////////////////////////////////////////////// Operator overloads

HttpField &	HttpField::operator=(HttpField const & source) {
	//std::cout << BLACK_ON_WHITE "HttpField assigment operator called." RESET << std::endl;
	if (this != &source) {
		_name = source._name;
		_forbidden_charset = source._forbidden_charset;
		_values = source._values;
	}
	return (*this);
}

std::ostream & operator<<(std::ostream & out, HttpField const & in) {
	out << "HttpField : " << std::endl;
	out << "{" << std::endl;
	out << "\tName : " << "'" << in._name << "'." << std::endl;
	out << "\tForbidden Charset : " << "'" << in._forbidden_charset << "'." << std::endl;
	out << "\tValues : " << std::endl;
	out << "\t{" << std::endl;
	for (size_t i = 0; i < in._values.size(); ++i) {
		if (in._values[i].length() != 0)
			out << "\t\t'" << in._values[i] << "'." << std::endl;
	}
	out << "\t};" << std::endl;
	out << "};" << std::endl;

	return (out);
}