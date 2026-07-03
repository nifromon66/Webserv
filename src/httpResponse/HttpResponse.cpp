#include "../../include/httpResponse/HttpResponse.hpp"

HttpResponse::HttpResponse() :
	_statusCode(200),
	_httpVersion("HTTP/1.1") 
{
	_setDefaultHeaders();
}

HttpResponse::HttpResponse(const HttpResponse &copy)
{
	*this = copy;
}

HttpResponse &HttpResponse::operator=(const HttpResponse &other)
{
	if (this != &other)
	{
		_statusCode = other._statusCode;
		_headers = other._headers;
		_body = other._body;
		_httpVersion = other._httpVersion;
	}
	return *this;
}

HttpResponse::~HttpResponse()
{}

void HttpResponse::setStatusCode(int code)
{
	_statusCode = code;
}

void HttpResponse::setHeader(const std::string& key, const std::string& value) 
{
	_headers[key] = value;
}

void HttpResponse::setBody(const std::string& content) 
{
	_body = content;
	setContentLength(_body.length());
}

void HttpResponse::setContentLength(size_t length) 
{
	std::stringstream ss;
	ss << length;
	setHeader("Content-Length", ss.str());
}

void HttpResponse::_setDefaultHeaders(void) 
{
	setHeader("Server", "Webserv/1.0 (42)");
	setHeader("Connection", "close");
	// setHeader("Date", getTimeStamp());
}

std::string HttpResponse::buildResponse() const 
{
	std::stringstream response;

	// 1. Ligne de Statut (Ex: HTTP/1.1 200 OK)
	response << _httpVersion << " " << _statusCode << " " << HttpStatus::getStatusText(_statusCode) << "\r\n"; // Nécessite une map de codes/textes

	// 2. Headers
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
		response << it->first << ": " << it->second << "\r\n";

	// 3. Séparateur de Headers et Corps
	response << "\r\n";

	// 4. Corps (Body)
	response << _body;

	return response.str();
}

int HttpResponse::getStatusCode() const
{
	return _statusCode;
}

const std::string& HttpResponse::getBody() const 
{
	return _body;
}

// Récupérer la valeur d'un en-tête HTTP spécifique (par exemple, la valeur associée à la clé "Content-Type" ou "Content-Length") à partir de la collection d'en-têtes stockée dans l'objet HttpResponse.
std::string HttpResponse::getHeader(const std::string& key) const 
{
	std::map<std::string, std::string>::const_iterator it = _headers.find(key);

	if (it != _headers.end()) 
		return it->second; // la valeur associée à cette clé (ex: "text/html" pour "Content-Type")
	return ""; // Retourner une chaîne vide si l'en-tête n'est pas trouvé
}

const std::string& HttpResponse::getHttpVersion() const 
{ 
	return _httpVersion; 
}
