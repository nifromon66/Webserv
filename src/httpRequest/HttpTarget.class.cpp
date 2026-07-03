#include "../../include/httpRequest/HttpTarget.class.hpp"

///////////////////////////////////////////////// Constructor / Destructor

HttpTarget::HttpTarget() {
	//std::cout << BLACK_ON_GREEN "HttpTarget default constructor called." RESET << std::endl;
	return ;
}

HttpTarget::HttpTarget(std::string const & target) {
	//std::cout << BLACK_ON_PURPLE "HttpTarget parametric constructor called." RESET << std::endl;
	_parse_target(target);
	return ;
}

HttpTarget::HttpTarget(HttpTarget const & source) {
	//std::cout << BLACK_ON_CYAN "HttpTarget copy constructor called." RESET << std::endl;
	*this = source;
	return ;
}

HttpTarget::~HttpTarget() {
	//std::cout << BLACK_ON_RED "HttpTarget default destructor called." RESET << std::endl;
	return ;
}

///////////////////////////////////////////////// Public member functions

std::string const &	HttpTarget::get_form(void) const {
	return (_form);
}

std::string const & HttpTarget::get_scheme(void) const {
	return (_scheme);
}

std::string const & HttpTarget::get_host(void) const {
	return (_host);
}

std::string const &	HttpTarget::get_port(void) const {
	return (_port);
}

std::string const &	HttpTarget::get_path(void) const {
	return (_path);
}

std::string const &	HttpTarget::get_query(void) const {
	return (_query);
}

std::string const &	HttpTarget::get_fragment(void) const {
	return (_fragment);
}

std::string const &	HttpTarget::get_target(void) const {
	return (_target);
}

void	HttpTarget::set_target(std::string const & target) {
	_target = target;
	normalize_percent_encoding(_target);
	_parse_target(_target);
	return ;
}

///////////////////////////////////////////////// Private member functions

void	HttpTarget::_determine_form(void) {
	if (!_form.empty())
		return ;
	if (_scheme.empty() && _host.empty() && _port.empty() && !_path.empty() && _fragment.empty())
		_form = ORIGIN_FORM;
	else if (!_scheme.empty() && !_host.empty() && !_path.empty())
		_form = ABSOLUTE_FORM;
	else if (_scheme.empty() && !_host.empty() && !_port.empty() && _path.empty() && _query.empty() && _fragment.empty())
		_form = AUTHORITY_FORM;
	return ;
}

void	HttpTarget::_verif_scheme(void) {
	normalize_scheme(_scheme);
	if (_scheme != "http")
		throw HttpRequestStatus(421, "Misdirected Request");

	return ;
}

void	HttpTarget::_verif_host(void) {
	normalize_host(_host);
	if (_host.length() > 253)
		throw HttpRequestStatus(414, "URI Too Long");

	for (size_t i = 0; i < _host.length(); ++i) {
		size_t	dot = _host.find('.', i);
		if (dot != std::string::npos) {
			if (_host.substr(i, (dot - i)).length() > 63)
				throw HttpRequestStatus(414, "URI Too Long");
		}
		else if (dot == std::string::npos) {
			if (_host.substr(i).length() > 63)
				throw HttpRequestStatus(414, "URI Too Long");
			break ;
		}
		i += dot - i;
	}

	for (size_t i = 0; i < _host.length(); ++i) {
		if (!((_host[i] > 64 && _host[i] < 91) || (_host[i] > 96 && _host[i] < 123) || (_host[i] > 47 && _host[i] < 58) || _host[i] == '.' || _host[i] == '-'))
			throw HttpRequestStatus(400, "Bad Request");
		if (_host[i] == '-' && (i == 0 || _host[i - 1] == '.' || _host[i + 1] == '.' || _host[i + 1] == '\0'))
			throw HttpRequestStatus(400, "Bad Request");
	}

	return ;
}

void	HttpTarget::_verif_port(void) {
	if (_port != "80" && _port != "8080")
		throw HttpRequestStatus(400, "Bad Request");

	return ;
}

void	HttpTarget::_verif_path(void) {
	return ;
}

void	HttpTarget::_verif_query(void) {
	return ;
}

void	HttpTarget::_verif_fragment(void) {
	return ;
}

void	HttpTarget::_parse_target(std::string const & target) {

	if (target == "*") {
		_form = ASTERISK_FORM;
		return ;
	}

	size_t	i_scheme = target.find(':');
	_scheme = target.substr(0, i_scheme);
	i_scheme++;

	size_t	i_authority = target.find("//", i_scheme);
	if (i_authority != std::string::npos) {
		i_authority += 2;

		size_t	i_user_info = target.find("@", i_authority);
		if (i_user_info != std::string::npos) {
			size_t	i_user_password = (target.substr(i_authority, (i_user_info - i_authority)).find(':')) + i_authority;
			if (i_user_password != std::string::npos)
				i_user_password++;
			i_authority = i_user_info + 1;
		}

		size_t	i_path = target.find('/', i_authority);
		if (i_path == std::string::npos)
			throw HttpRequestStatus(400, "Bad Request");
		size_t	i_port = (target.substr(i_authority, (i_path - i_authority)).find(':')) + i_authority;
		if (i_port != std::string::npos) {
			_host = target.substr(i_authority, (i_port - i_authority));
			i_port++;
			_port = target.substr(i_port, (i_path - i_port));
		}
		else
			_host = target.substr(i_authority, (i_path - i_authority));

		size_t	i_query = target.find('?', i_path);
		size_t	i_fragment = target.find('#', i_path);
		if (i_query != std::string::npos && i_fragment != std::string::npos) {
			_path = target.substr(i_path, (i_query - i_path));
			i_query++;
			_query = target.substr(i_query, (i_fragment - i_query));
			i_fragment++;
			_fragment = target.substr(i_fragment);
		}
		else if (i_query != std::string::npos && i_fragment == std::string::npos) {
			_path = target.substr(i_path, (i_query - i_path));
			i_query++;
			_query = target.substr(i_query);
		}
		else if (i_query == std::string::npos && i_fragment != std::string::npos) {
			_path = target.substr(i_path, (i_fragment - i_path));
			i_fragment++;
			_fragment = target.substr(i_fragment);
		}
		else
			_path = target.substr(i_path);

		_determine_form();
		_shape_target();
		return ;
	}

	size_t	i_path = target.find('/', i_scheme);
	if (i_path == std::string::npos)
		throw HttpRequestStatus(400, "Bad Request");

	size_t	i_query = target.find('?', i_path);
	size_t	i_fragment = target.find('#', i_path);
	if (i_query != std::string::npos && i_fragment != std::string::npos) {
		_path = target.substr(i_path, (i_query - i_path));
		i_query++;
		_query = target.substr(i_query, (i_fragment - i_query));
		i_fragment++;
		_fragment = target.substr(i_fragment);
	}
	else if (i_query != std::string::npos && i_fragment == std::string::npos) {
		_path = target.substr(i_path, (i_query - i_path));
		i_query++;
		_query = target.substr(i_query);
	}
	else if (i_query == std::string::npos && i_fragment != std::string::npos) {
		_path = target.substr(i_path, (i_fragment - i_path));
		i_fragment++;
		_fragment = target.substr(i_fragment);
	}
	else
		_path = target.substr(i_path);

	_determine_form();
	_shape_target();

	return ;
}

void	HttpTarget::_shape_target(void) {
	if (_scheme.length() != 0)
		_target = _scheme + ":";
	if (_host.length() != 0 || _port.length() != 0) {
		_target += "//";
		if (_host.length() != 0)
			_target += _host;
		if (_port.length() != 0)
			_target += ":" + _port;
	}
	if (_path.length() != 0)
		_target += _path;
	if (_query.length() != 0)
		_target += "?" + _query;
	if (_fragment.length() != 0)
		_target += "#" + _fragment;

	return ;
}

///////////////////////////////////////////////// Operator overloads

HttpTarget &	HttpTarget::operator=(HttpTarget const & source) {
	//std::cout << BLACK_ON_WHITE "HttpTarget assigment operator called." RESET << std::endl;
	if (this != &source) {
		_form = source._form;
		_scheme = source._scheme;
		_host = source._host;
		_port = source._port;
		_path = source._path;
		_query = source._query;
		_fragment = source._fragment;
		_target = source._target;
	}
	return (*this);
}

std::ostream & operator<<(std::ostream & out, HttpTarget const & in) {

	out << "HttpTarget : " << std::endl;
	out << "{" << std::endl;
	out << "\tForm : " << "'" << in._form << "'." << std::endl;
	out << "\tScheme : " << "'" << in._scheme << "'." << std::endl;
	out << "\tHost : " << "'" << in._host << "'." << std::endl;
	out << "\tPort : " << "'" << in._port << "'." << std::endl;
	out << "\tPath : " << "'" << in._path << "'." << std::endl;
	out << "\tQuery : " << "'" << in._query << "'." << std::endl;
	out << "\tFragment : " << "'" << in._fragment << "'." << std::endl;
	out << "\tTarget : " << "'" << in._target << "'." << std::endl;
	out << "};" << std::endl;

	return (out);
}