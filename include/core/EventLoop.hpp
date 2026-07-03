#ifndef EVENT_LOOP_HPP
#define EVENT_LOOP_HPP

#include "../core/Epoll.hpp"
#include "../core/Client.hpp"
#include "../httpResponse/FileUtils.hpp"
#include "../include/Types.hpp"
#include "../cgi/CgiHandler.hpp"

#include "../httpResponse/HttpResponse.hpp"
#include "../cookies//Cookie.class.hpp"
#include "../cookies/Session.class.hpp"
#include "../Utils/HttpSyntax.utils.hpp"
#include "../httpResponse/RequestRouter.hpp"
#include "../httpRequest/HttpRequest.class.hpp"
#include "../httpRequest/HttpHeader.class.hpp"
#include "../Error/HttpRequestStatus.error.hpp"

#include <map>
#include <vector>

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <stdexcept>
#include <csignal>

class RequestRouter;

class EventLoop
{
	private:

		Epoll						&_epoll;
		std::vector<int>			&_listenSockets;
		std::map<int, Client>		_clients;
		ServersConfigMap			&_serversConfigBySocket;
		int							_error;
		std::string					_statusError;
		CgiHandler					_cgiHandler;

		std::vector<Session>		_Sessions;


		bool						isInListenSockets(int fd) const;
		void						acceptNewClients(int listenFd);
		void						closeClient(int clientFd);

		void						parseRequest(Client &client);
		bool						handleCgi(Client &client, RequestRouter &router, int eventFd);

		void						handleLogin(Client &client, HttpResponse &response, int eventFd);
		void						handleLogout(Client &client, HttpResponse &response, int eventFd);
		void						handle_Q_and_A(Client &client, HttpResponse &response, int eventFd);

		bool						ValidateInput(std::string const & input, size_t minLen, size_t maxLen);
		bool						findCookie(Client &client, std::string const & CookieName);
		int							findSession(std::string const & session_id);
		std::string					extractSessionId(Client &client);
		std::string					extractUsername(std::string const & body);
		std::string					extractPassword(std::string const & body);

		int							verifyNewSession(std::string const & username, std::string const & password);

		void						handleResponse(int eventFd);
		void						handleClient(int eventFd);

	public:

		EventLoop(Epoll &epoll, std::vector<int> &listens, ServersConfigMap &serversConfigBySocket);
		~EventLoop( void );
		// pour les fct en comm ci dessous cf -> Epoll.hpp
		// EventLoop( void );
		// EventLoop(const EventLoop &other);
		// EventLoop &operator=(const EventLoop &other);


		void						run();

};

#endif