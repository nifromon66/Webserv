#include "../../include/core/Client.hpp"

Client::Client( void ) :
	_ListenSocket(-1),
	_ClientSocket(-1),
	_buffer(""),
	_header(""),
	_body(""),
	_state(READING_FIRST_LINE),
	_clientMaxBodySize(0),
	_clientMaxHeaderSize(0),
	_contentLength(-1),
	_serverConfig(NULL),
	_response(""),
	_posResponseSent(0),
	_keepAlive(false),
	_request()
{}

Client::Client(int listenSocket, int clientSocket, ServersConfigVector ServersConfigOnSocket ) :
	_ListenSocket(listenSocket),
	_ClientSocket(clientSocket),
	_buffer(""),
	_firstLine(""),
	_header(""),
	_body(""),
	_state(READING_FIRST_LINE),
	_clientMaxBodySize(0),
	_clientMaxHeaderSize(0),
	_contentLength(-1),
	_ServersConfigOnSocket(ServersConfigOnSocket),
	_serverConfig(NULL),
	_response(""),
	_posResponseSent(0),
	_keepAlive(false),
	_request()
{}

Client::Client( const Client &other ) :
	_ListenSocket(other._ListenSocket),
	_ClientSocket(other._ClientSocket),
	_buffer(other._buffer),
	_firstLine(other._firstLine),
	_header(other._header),
	_body(other._body),
	_state(other._state),
	_clientMaxBodySize(other._clientMaxBodySize),
	_clientMaxHeaderSize(other._clientMaxHeaderSize),
	_contentLength(other._contentLength),
	_ServersConfigOnSocket(other._ServersConfigOnSocket),
	_serverConfig(other._serverConfig),
	_response(other._response),
	_posResponseSent(other._posResponseSent),
	_keepAlive(other._keepAlive),
	_request(other._request)
{}

Client &Client::operator=( const Client &other )
{
	if (this != &other)
	{
		_ListenSocket = other._ListenSocket;
		_ClientSocket = other._ClientSocket;
		_buffer = other._buffer;
		_firstLine = other._firstLine;
		_header = other._header;
		_body = other._body;
		_state = other._state;
		_clientMaxBodySize = other._clientMaxBodySize;
		_clientMaxHeaderSize = other._clientMaxHeaderSize;
		_contentLength = other._contentLength;
		_ServersConfigOnSocket = other._ServersConfigOnSocket;
		_serverConfig = other._serverConfig;
		_response = other._response;
		_posResponseSent = other._posResponseSent;
		_keepAlive = other._keepAlive;
		_request = other._request;

	}
	return (*this);
}

Client::~Client( void )
{}

std::string	Client::getHeader( void ) const
{
	return (_header);
}

std::string	Client::getBuffer( void ) const
{
	return (_buffer);
}

void	Client::setState( State state)
{
	_state = state;
}

void	Client::setAppendBuffer(const char *buffer, int n)
{
	_buffer.append(buffer, n);
}

std::string	Client::getBody( void ) const
{
	return (_body);
}

State	Client::getState( void )
{
	return (_state);
}

std::string	Client::getResponse( void ) const
{
	return (_response);
}

int	Client::getClientSocket( void ) const
{
	return(_ClientSocket);
}

const ServerConfig	&Client::getServerConfig(void) const
{
	return (*_serverConfig);
}

HttpRequest	Client::getRequest( void ) const
{
	return (_request);
}

const HttpRequest &Client::getRequestRef(void) const
{
	return _request;
}

bool Client::hasServerConfig() const
{
	return (_serverConfig != NULL);
}

void Client::setDefaultServerConfig( void )
{
	if (_ServersConfigOnSocket.empty())
	{
		_serverConfig = NULL;
		return;
	}
	_serverConfig = _ServersConfigOnSocket[0];
}

void	Client::setResponse(const std::string &response)
{
	_response = response;
	_posResponseSent = 0;
	_keepAlive = keepAlive();
}

void	Client::appendResponse(const std::string &chunk)
{
	_response.append(chunk);
}

bool	Client::sendResponse(void)
{
	ssize_t	n;
	size_t	responsePartUnset;

	if (_response.empty())
		return (true);
	if (_posResponseSent >= _response.size())
		return (true);

	responsePartUnset = _response.size() - _posResponseSent;
	n = ::send(_ClientSocket, _response.c_str() + _posResponseSent, responsePartUnset, 0);
	if (n <= 0)
		return (false);

	_posResponseSent += static_cast<size_t>(n);
	if (_posResponseSent >= _response.size())
		return (true);
	return (false);
}

bool	Client::keepAlive(void) const
{
	if (_header.find("Connection: close") != std::string::npos
		|| _header.find("connection: close") != std::string::npos)
		return (false);
	return (true);
}

void	Client::reset(void)
{
	_buffer.clear();
	_header.clear();
	_body.clear();

	_state = READING_FIRST_LINE;
	_contentLength = -1;

	_response.clear();
	_posResponseSent = 0;
	_keepAlive = false;
}

static std::string trimWhitespace( const std::string &s )
{
	size_t start = 0;
	while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start])))
		start++;

	size_t end = s.size();
	while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1])))
		end--;

	return s.substr(start, end - start);
}

void	Client::setContentLength( void )
{
	std::vector<HttpField> const &fields = _request.get_header().get_header();
	std::string contentLengthValue;
	// Recherche du header "Content-Length"
	for (size_t i = 0; i < fields.size(); ++i)
	{
		if (fields[i].get_name() == "Content-Length")
		{
			if (!fields[i].get_values().empty())
				contentLengthValue = fields[i].get_value(0);	// récupère la 1ère valeur
			break;												// même si c'est vide → on sort, car on a trouvé le champ
		}
	}
	if (contentLengthValue.empty())
	{
		_contentLength = 0;
		return ;
	}

	contentLengthValue = trimWhitespace(contentLengthValue);
	char *end = NULL;
	long len = std::strtol(contentLengthValue.c_str(), &end, 10);
	if (end == contentLengthValue.c_str() || *end != '\0' || len < 0)
	{
		_state = ERROR;
		throw HttpRequestStatus(400, "Bad Request");
	}
	_contentLength = len;
}

void	Client::fillFirstLine( void )
{
	if (_state == READING_FIRST_LINE)
	{
		size_t posEndFirstLine = _buffer.find("\r\n");
		if (posEndFirstLine == std::string::npos)
			return ;
		_firstLine = _buffer.substr(0, posEndFirstLine + 2);
		//std:: cout << "DEBUG:first line = \n" << _firstLine << std::endl;

		_buffer.erase(0, posEndFirstLine + 2);
		_request.set_first_line(_firstLine);
		_state = READING_HEADER;
	}
}

void	Client::setServerConfig(std::vector<HttpField> const & fields)
{
	if (_ServersConfigOnSocket.empty())
	{
		_serverConfig = NULL;
		return;
	}
	std::string	hostValueInHeader;
	for (size_t i = 0; i < fields.size(); i++)
	{
		if (fields[i].get_name() == "Host")
		{
			if (!fields[i].get_values().empty())
				hostValueInHeader = fields[i].get_value(0);
			break;
		}
	}
	size_t colonPos = hostValueInHeader.find(':');
	if (colonPos != std::string::npos)
		hostValueInHeader = hostValueInHeader.substr(0, colonPos);
	for (size_t i = 0; i < _ServersConfigOnSocket.size(); i++)
	{
		const ServerConfig *serverConfig = _ServersConfigOnSocket[i];
		for (size_t j = 0; j < serverConfig->server_names.size(); j++)
		{
			if (hostValueInHeader == serverConfig->server_names[j])
			{
				_serverConfig = serverConfig;										// assigne la config selon le nom du hostname
				return;
			}
		}
	}
	_serverConfig = _ServersConfigOnSocket[0];									// prend la première config par défaut si le hostname ne trouve pas de server_name correspondant
}

void	Client::setCMBSAndCMHS(std::vector<LocationConfig> const &locations)
{
	std::string	uri = _request.get_target().get_path();
	LocationConfig const	*uriLocationConfig = NULL;

	for (size_t i = 0; i < locations.size(); i++)
	{
		const LocationConfig	&loc = locations[i];
		if (uri.compare(0, loc.path.length(), loc.path) == 0)					// compare le début de l'URI avec la totalité  du path de la localisation testée
			uriLocationConfig = &loc;
	}
	if (uriLocationConfig != NULL)
	{
		if (uriLocationConfig->client_max_body_size > 0)
			_clientMaxBodySize = uriLocationConfig->client_max_body_size;
		else
			_clientMaxBodySize = _serverConfig-> clientMaxBodySize;
		if (uriLocationConfig->client_max_header_size > 0)
			_clientMaxHeaderSize = uriLocationConfig->client_max_header_size;
		else
			_clientMaxHeaderSize = _serverConfig-> clientMaxHeaderSize;

	}
	else
	{
		_clientMaxBodySize = static_cast<size_t>(_serverConfig->clientMaxBodySize);
		_clientMaxHeaderSize = static_cast<size_t>(_serverConfig->clientMaxHeaderSize);
	}
	//std::cout << "DEBUG:CMBS = " << _clientMaxBodySize
	//		  << " | CMHS = " << _clientMaxHeaderSize << std::endl;
}

void	Client::fillHeader( void )
{
	if (_state == READING_HEADER)
	{
		size_t posEndHeader = _buffer.find("\r\n\r\n");
		if (posEndHeader == std::string::npos)
			return ;
		_header = _buffer.substr(0, posEndHeader + 4);
		//std:: cout << "DEBUG:_header = \n" << _header << std::endl;
		_buffer.erase(0, posEndHeader + 4);

		_request.set_header(_header);

		if (_request.get_method().get_method() == "POST")
		{
			setContentLength();
			_state = READING_BODY;
		}
		else
			_state = READY;
		setServerConfig(_request.get_header().get_header());
		setCMBSAndCMHS(_serverConfig->locations);
		if (_header.size() > _clientMaxHeaderSize)
		{
			_state = ERROR;
			throw HttpRequestStatus(431, "Request Header Fields Too Large");
		}
	}
}

void	Client::fillBody( void )
{
	if (_state == READING_BODY)
	{
		if (_contentLength <= 0)
		{
			_request.set_body("");
			_state = READY;
			return ;
		}
		if (_buffer.length() > 0)
			_body.append(_buffer);
		if (_body.size() > _clientMaxBodySize)
		{
			_buffer.clear();
			_state = ERROR;
			throw HttpRequestStatus(413, "Content Too Large");
		}
		if (_body.size() < static_cast<size_t>(_contentLength))
			_state = READING_BODY;
		else
		{
			_request.set_body(_body);
			_state = READY;
		}
		_buffer.clear();
	}
}
