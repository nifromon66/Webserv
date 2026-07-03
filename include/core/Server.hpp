#ifndef SERVER_HPP
#define SERVER_HPP

#include "../config/Config.hpp"
#include "../core/Epoll.hpp"
#include "../core/EventLoop.hpp"

#include <vector>
#include <map>

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <stdexcept>
#include <csignal>			// std::signal (Ctrl-C → SIGINT)
#include <cstring>			// std::strerror
#include <sys/socket.h>		// socket, setsockopt, bind, listen
#include <netinet/in.h>		// sockaddr_in, htons, INADDR_*
#include <arpa/inet.h>		// htonl
#include <unistd.h>			// close, read, write, etc.
#include <sstream>			// std::stringstream


class Server
{

	private:

		std::vector<int>				_listenSockets;			// stocke le FD des sockets d'ecoutes
		Epoll							_epoll;
		ServersConfigMap				_serversConfigBySocket;		// stocke les configs servers associés à un socket d'écoute
		std::map<std::string, int>		_checkIPPortExist;

		bool							openAllSockets(const std::vector<ServerConfig> &servers);
		bool							openSocket(const ListenEndpoint &ep, const ServerConfig &serverConf);
		bool							makeAddr(const ListenEndpoint &ep, sockaddr_in &addr) const;
		void							closeAllSockets(void);

		void							errorMsgAndQuit(const std::string &msg, const ListenEndpoint &ep, int fd);
		void							registerListenSockets(void);

	public:

		Server();
		// Important de ne pas copier _epfd (Epoll) qui est une ressource système non duplicable
		// Server(const Server &other);
		// Server &operator=(const Server &other);
		~Server();

		void							initSockets( const Config &conf );
		void							run();

};

#endif
