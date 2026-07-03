#include "../../include/httpRequest/HttpVersion.class.hpp"

///////////////////////////////////////////////// Constructor / Destructor

HttpVersion::HttpVersion() {
	//std::cout << BLACK_ON_GREEN "HttpVersion default constructor called." RESET << std::endl;
	return ;
}

HttpVersion::HttpVersion(std::string const & version) {
	//std::cout << BLACK_ON_PURPLE "HttpVersion parametric constructor called." RESET << std::endl;
	_parse_version(version);
	return ;
}

HttpVersion::HttpVersion(HttpVersion const & source) {
	//std::cout << BLACK_ON_CYAN "HttpVersion copy constructor called." RESET << std::endl;
	*this = source;
	return ;
}

HttpVersion::~HttpVersion() {
	//std::cout << BLACK_ON_RED "HttpVersion default destructor called." RESET << std::endl;
	return ;
}

///////////////////////////////////////////////// Public member functions

std::string const &	HttpVersion::get_version(void) const {
	return (_version);
}

void	HttpVersion::set_version(std::string const & version) {
	_parse_version(version);
	return ;
}

///////////////////////////////////////////////// Private member functions

void	HttpVersion::_parse_version(std::string const & version) {
	size_t	i_name = version.find('/');
	if (i_name == std::string::npos)
		throw HttpRequestStatus(400, "Bad Request");
	if (version.substr(0, i_name) != "HTTP")
		throw HttpRequestStatus(400, "Bad Request");
	char	major = version[i_name + 1];
	char	dot = version[i_name + 2];
	char	minor = version[i_name + 3];
	if (!is_DIGIT(major) || dot != '.' || !is_DIGIT(minor))
		throw HttpRequestStatus(400, "Bad Request");
	if ((major == '1' && minor == '0') || (major == '0' && minor == '9'))
		throw HttpRequestStatus(426, "Upgrade Required");
	if (major != '1' && minor != '1')
		throw HttpRequestStatus(505, "HTTP Version Not Supported");

	_version = version;

	return ;
}

///////////////////////////////////////////////// Operator overloads

HttpVersion &	HttpVersion::operator=(HttpVersion const & source) {
	//std::cout << BLACK_ON_WHITE "HttpVersion assigment operator called." RESET << std::endl;
	if (this != &source) {
		_version = source._version;
	}
	return (*this);
}

std::ostream & operator<<(std::ostream & out, HttpVersion const & in) {
	out << "HttpVersion : " << std::endl;
	out << "{" << std::endl;
	out << "\tVersion : " << "'" << in._version << "'." << std::endl;
	out << "};" << std::endl;

	return (out);
}