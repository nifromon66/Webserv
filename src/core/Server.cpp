#include "../../include/core/Server.hpp"

Server::Server() :
	_listenSockets(),
	_epoll(),
	_serversConfigBySocket(),
	_checkIPPortExist()
{}

Server::~Server()
{
	closeAllSockets();
}

void	Server::initSockets( const Config &conf )
{
	const std::vector<ServerConfig> &servers = conf.servers();
	bool ok = openAllSockets(servers);

	// Message d'état - pas oblige de le garder
	if (!ok)
		std::cerr << "⚠️  Some sockets failed to open, continuing with valid ones." << std::endl;
	else
		std::cout << "✅ All sockets opened successfully." << std::endl;
}

bool Server::openAllSockets(const std::vector<ServerConfig> &servers)
{
	bool ok = true;

	for (size_t i_svrIndex = 0; i_svrIndex < servers.size(); ++i_svrIndex)
	{
		const std::vector<ListenEndpoint> &ls = servers[i_svrIndex].listens;
		for (size_t i_LEIndex = 0; i_LEIndex < ls.size(); ++i_LEIndex)
		{
			if (!openSocket(ls[i_LEIndex], servers[i_svrIndex]))
				ok = false;
		}
	}
	return (ok);
}

void	Server::errorMsgAndQuit(const std::string &msg, const ListenEndpoint &ep, int socket)
{
	std::cerr << "❌ Listening on "
			  << ep.host << ":" << ep.port
			  << " failed. Error : " << msg << std::endl;
	if (socket != -1)
		close(socket);
}

bool Server::openSocket(const ListenEndpoint &ep, const ServerConfig &serverConf)
{

	std::ostringstream oss;
	oss << ep.host << ":" << ep.port;
	std::string key = oss.str();

	std::map<std::string, int>::iterator it = _checkIPPortExist.find(key);		// on vérifie si une socket écoute déjà sur cette ip:port
	if(it != _checkIPPortExist.end())
	{
		std::cout << "❌ Server already listening on " << ep.host << ":" << ep.port << std::endl;
		int socket = it->second;
		_serversConfigBySocket[socket].push_back(&serverConf);					// on ajoute la config server à la socket déjà existante
		return (true);
	}

	int socket = -1;

	try
	{
		socket = ::socket(AF_INET, SOCK_STREAM, 0);
		if (socket < 0)
			throw std::runtime_error("socket() failed.");

		int opt = 1;
		if (::setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
			throw std::runtime_error("setsockopt() failed.");

		sockaddr_in addr;
		if (!makeAddr(ep, addr))
			throw std::runtime_error("invalid address format.");

		if (::bind(socket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
			throw std::runtime_error("bind() failed. Socket is already listening on this adress/port");

		if (::listen(socket, 128) < 0)
			throw std::runtime_error("listen() failed.");

		FileUtils::setFDNonBloquing(socket);
		_checkIPPortExist[key] = socket;
		_listenSockets.push_back(socket);
		_serversConfigBySocket[socket].push_back(&serverConf);

		std::cout << "✅ Listening on " << ep.host << ":" << ep.port << std::endl;
		return (true);
	}
	catch (const std::exception &e)
	{
		errorMsgAndQuit(e.what(), ep, socket);
		return (false);
	}
}

bool Server::makeAddr(const ListenEndpoint &ep, sockaddr_in &addr) const
{
	addr.sin_family = AF_INET;
	addr.sin_port = htons(ep.port);

	if (ep.host == "0.0.0.0" || ep.host == "*")
	{
		addr.sin_addr.s_addr = INADDR_ANY;
		return (true);
	}

	int octs[4];
	std::stringstream ss(ep.host);
	std::string part;
	int i = 0;

	while (std::getline(ss, part, '.') && i < 4)
		octs[i++] = std::atoi(part.c_str());

	addr.sin_addr.s_addr = htonl((octs[0] << 24)
								| (octs[1] << 16)
								| (octs[2] << 8)
								| octs[3]);

	return (true);
}

void Server::closeAllSockets(void)
{
	for (size_t i = 0; i < _listenSockets.size(); ++i)
	{
		if (_listenSockets[i] >= 0)
			::close(_listenSockets[i]);
	}
	_listenSockets.clear();

	std::cout << "\n✅ All sockets closed.\n";
}

void Server::registerListenSockets(void)
{
	for (size_t i = 0; i < _listenSockets.size(); ++i)
	{
		int socket = _listenSockets[i];
		_epoll.add(socket, EPOLLIN);						// on dit à epoll : “surveille ce socket en lecture”
		std::cout << "📡 Registered listen socket=" << socket << " in epoll\n";
	}
}

void Server::run(void)
{
	registerListenSockets();
	EventLoop loop(_epoll, _listenSockets, _serversConfigBySocket);
	loop.run();
}
