#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../include/config/Config.hpp"
#include "../include/Types.hpp"
#include "../cookies/Cookie.class.hpp"
#include "../Error/HttpRequestStatus.error.hpp"
#include "../httpRequest/HttpRequest.class.hpp"
#include "../httpRequest/HttpField.class.hpp"
#include "../Error/HttpRequestStatus.error.hpp"


#include <iostream>
#include <string>
#include <sys/socket.h>
#include <vector>
#include <cstdlib>
#include <cctype>
#include <unistd.h>

enum State
{
	READING_FIRST_LINE,
	READING_HEADER,
	READING_BODY,
	READY,
	ERROR,
	WAITING_CGI
};

class Client
{

	private:

		int					_ListenSocket;
		int					_ClientSocket;
		std::string			_buffer;
		std::string			_firstLine;
		std::string			_header;
		std::string			_body;
		State				_state;
		size_t				_clientMaxBodySize;
		size_t				_clientMaxHeaderSize;
		long				_contentLength;
		ServersConfigVector	_ServersConfigOnSocket;
		const ServerConfig	*_serverConfig;
		std::string			_response;
		size_t				_posResponseSent;
		bool				_keepAlive;
		HttpRequest			_request;

		void				setContentLength( void );
		void				setServerConfig(std::vector<HttpField> const & fields);
		void				setCMBSAndCMHS(std::vector<LocationConfig> const &locations);

	public:

		Client( int listenSocket, int clientSocket, ServersConfigVector ServersConfigOnSocket );
		Client( void );
		Client( const Client &other );
		Client &operator=( const Client &other );
		~Client( void );

		State				getState( void );
		std::string			getBuffer( void ) const;
		std::string			getResponse( void ) const;
		std::string			getHeader( void ) const;
		std::string			getBody( void ) const;
		int					getClientSocket( void) const;
		const ServerConfig	&getServerConfig(void) const;
		HttpRequest			getRequest( void ) const;
		const HttpRequest	&getRequestRef(void) const;

		void				setState( State state);
		void				setAppendBuffer(const char *buffer, int n);
		void				setDefaultServerConfig( void );

		void				fillFirstLine( void );
		void				fillHeader( void );
		void				fillBody( void );

		bool				hasServerConfig() const;
		void				setResponse(const std::string &response);
		void				appendResponse(const std::string &chunk);
		bool				sendResponse(void);
		bool				keepAlive(void) const;
		void				reset(void);

};

#endif