#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include "../httpResponse/HttpStatus.hpp"

#include <map>
#include <vector>

#include <string>
#include <sstream>
#include <iostream>

class HttpResponse
{
	public:
		HttpResponse();
		HttpResponse(const HttpResponse &copy);
		HttpResponse &operator=(const HttpResponse &other);
		~HttpResponse();

	// --- Méthodes de Construction ---
		void setStatusCode(int code);
		void setHeader(const std::string& key, const std::string& value);
		void setBody(const std::string& content);
		void setContentLength(size_t length);

		// --- Méthode d'Envoi ---
		std::string buildResponse() const; 

		int getStatusCode() const;
		const std::string& getBody() const;
		const std::map<std::string, std::string>& getHeaders() const { return _headers; }
		const std::string& getHttpVersion() const;
		std::string getHeader(const std::string& key) const;
		
	private:
		int									_statusCode;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		std::string							_httpVersion;
		
		void _setDefaultHeaders(void);};

#endif