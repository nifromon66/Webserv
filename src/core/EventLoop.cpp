#include "../../include/core/EventLoop.hpp"

/* ----------------------------- Gestion ctrl c ----------------------------- */

static volatile sig_atomic_t g_stop = 0;

static void on_sigint(int)
{
	g_stop = 1;
}
/* -------------------------------------------------------------------------- */

EventLoop::EventLoop(Epoll &epoll, std::vector<int> &listens, ServersConfigMap &serversConfigBySocket) :
	_epoll(epoll),
	_listenSockets(listens),
	_clients(),
	_serversConfigBySocket(serversConfigBySocket),
	_error(0),
	_statusError(""),
	_cgiHandler(_epoll, _clients)
{}

EventLoop::~EventLoop( void )
{}

// Vérifie si un fd est un listen socket
bool EventLoop::isInListenSockets(int fd) const
{
	for (size_t i = 0; i < _listenSockets.size(); ++i)
		if (_listenSockets[i] == fd)
			return (true);
	return (false);
}

// Accepte les nouveaux clients d'un listen socket
void EventLoop::acceptNewClients(int listenFd)
{
	int clientFd = ::accept(listenFd, NULL, NULL);
	if (clientFd == -1)											// sort de la boucle quand il n'y a plus de clients a accepter
		return;

	try
	{
		FileUtils::setFDNonBloquing(clientFd);
		_epoll.add(clientFd, EPOLLIN);

		ServersConfigMap::iterator it = _serversConfigBySocket.find(listenFd);
		if (it == _serversConfigBySocket.end())
			throw std::runtime_error("listenFd not found in server config map");
		_clients.insert(std::make_pair(clientFd, Client(listenFd, clientFd, it->second)));

		std::cout << "👤 Client accepté (fd=" << clientFd
				<< ") via port d’écoute fd=" << listenFd << "\n";			// a supprimer plus tard
	}
	catch (const std::exception &e)
	{
		std::cerr << "❌ New client setup failed: " << e.what() << std::endl;
		::close(clientFd);
	}
}

void EventLoop::closeClient(int clientFd)
{
	std::cout << "👤 Client supprimé (fd=" << clientFd << ")" << std::endl;

	_epoll.del(clientFd);						// Stoppe epoll  qui surveille ce fd
	::close(clientFd);							// Ferme le FD(socket)
	_clients.erase(clientFd);					// Supprime le client de la map
}

void	EventLoop::handleResponse(int eventFd)
{
	//std::cout << "DEBUG: Response = \n" << _clients[eventFd].getResponse() << std::endl;

	bool done = _clients[eventFd].sendResponse();

	// Encore des données à envoyer → on ne fait rien
	if (!done)
		return;
	if (_clients[eventFd].keepAlive())
	{
		_epoll.mod(eventFd, EPOLLIN);
		_clients[eventFd].reset();
	}
	else
		closeClient(eventFd);
}

void	EventLoop::parseRequest(Client &client)
{
	try {
		if (client.getState() == READING_FIRST_LINE)
			client.fillFirstLine();
		if (client.getState() == READING_HEADER)
			client.fillHeader();
		if (client.getState() == READING_BODY)
			client.fillBody();
	}
	catch (const HttpRequestStatus& e) {
		std::cerr << RED << e << RESET << std::endl;
		_error = e.getCode();
		_statusError = e.getMessage();
		client.setState(ERROR);

	}
	if (_error == 0 && client.getState() == READY)
		_error = 200;
	if (client.getState() == ERROR)
	{
		if (!client.hasServerConfig())
			client.setDefaultServerConfig();
	}
}

bool	EventLoop::handleCgi(Client &client, RequestRouter &router, int eventFd)
{
	const LocationConfig& loc = router.resolveLocation(client.getRequest().get_target().get_path(), client.getServerConfig());

	if (router.isCgiMatch(client.getRequest().get_target().get_path(), loc.path) && _error == 200)
	{
		std::cout << "🔍 CGI détecté pour ce clientFd = " << eventFd << std::endl;
		try
		{
			router.checkMethodAndSize(client.getRequest(), loc);
			_cgiHandler.createAndLaunchCGI(eventFd, client, loc);
			return (true);
		}
		catch (const HttpRequestStatus& e) {
			std::cerr << RED << e << RESET << std::endl;
			_error = e.getCode();
			_statusError = e.getMessage();
			client.setState(ERROR);
		}
	}
	return (false);
}

std::string	EventLoop::extractUsername(std::string const & body) {
	size_t	i_username = body.find('=', 0) + 1;
	size_t	i_and = body.find('&', i_username);
	return (body.substr(i_username, (i_and - i_username)));
}

std::string	EventLoop::extractPassword(std::string const & body) {
	size_t	i_and = body.find('&', 0) + 1;
	size_t	i_password = body.find('=', i_and) + 1;
	return (body.substr(i_password));
}

bool	EventLoop::ValidateInput(std::string const & input, size_t minLen, size_t maxLen) {
	if (input.length() < minLen || input.length() > maxLen)
		return (false);
	for (size_t i = 0; i < input.length(); ++i) {
		char	c = input[i];
		if (!is_ALPHA(c) && !is_DIGIT(c) && (c != '_' || c != '-'))
			return (false);
	}
	return (true);
}

int	EventLoop::verifyNewSession(std::string const & username, std::string const & password) {

	if (!ValidateInput(username, 4, 21)) {
		std::cout << "\t❌️ Login attempt failed: Username Syntax incorrect." << std::endl;
		return (-1);
	}

	if (!ValidateInput(password, 8, 42)) {
		std::cout << "\t❌️ Login attempt failed: Password Syntax incorrect." << std::endl;
		return (-2);
	}

	for (size_t i = 0; i < _Sessions.size(); ++i) {
		if (_Sessions[i].getUsername() == username) {
			if (_Sessions[i].getLog()) {
				std::cout << "\t❌️ Login attempt failed: User already logged in." << std::endl;
				return (-3);
			}
			if (!_Sessions[i].passwordVerification(password)) {
				std::cout << "\t❌️ Login attempt failed: Incorrect password." << std::endl;
				return (-4);
			}
			else {
				std::cout << "\t✅ Login attempt succesful." << std::endl;
				return (i);
			}
		}
	}

	std::cout << "\t⚠️ User not registered yet." << std::endl;
	return (-5);
}

void	EventLoop::handleLogin(Client &client, HttpResponse &response, int eventFd) {
	if (client.getRequest().get_target().get_path() == "/login"
		&& client.getRequest().get_method().get_method() == "POST")
	{
		std::cout << "🔍 Login attempt detected for this clientFd = " << eventFd << std::endl;

		std::string	username = extractUsername(client.getBody());
		std::string	password = extractPassword(client.getBody());
		int	res = verifyNewSession(username, password);

		if (res == -1) {
			response.setStatusCode(303);
			response.setHeader("Location", "/login/loginFailed_UsernameSyntaxIncorrect.html");
			return ;
		}
		else if (res == -2) {
			response.setStatusCode(303);
			response.setHeader("Location", "/login/loginFailed_PasswordSyntaxIncorrect.html");
			return ;
		}
		if (res == -3) {
			response.setStatusCode(303);
			response.setHeader("Location", "/login/loginFailed_UserLogged.html");
			return ;
		}
		else if (res == -4) {
			response.setStatusCode(303);
			response.setHeader("Location", "/login/loginFailed_PasswordIncorrect.html");
			return ;
		}
		else if (res == -5) {
			std::cout << "⌛ Creating new session..." << std::endl;
			_Sessions.push_back(Session(username, password));
			response.setHeader("Set-Cookie", _Sessions.back().getSessionId().serializeCookie());
			std::cout << "✅ New Session created successfully." << std::endl;
			_Sessions.back().login();
			std::cout << "☑️ New User logged in." << std::endl;
			response.setStatusCode(303);
			response.setHeader("Location", "/login/loginSuccess_New.html");
			return ;
		}
		else if (res >= 0) {
			_Sessions[res].login();
			response.setHeader("Set-Cookie", _Sessions[res].getSessionId().serializeCookie());
			std::cout << "☑️ User logged in." << std::endl;
			response.setStatusCode(303);
			response.setHeader("Location", "/login/loginSuccess_Old.html");
			return ;
		}
		return ;
	}
	return ;
}

bool	EventLoop::findCookie(Client &client, std::string const & CookieName) {
	HttpHeader	tmp(client.getRequest().get_header());
	if (tmp.find_field("Cookie") == -1)
		return (false);

	std::vector<std::string> Cookies(tmp.get_field(tmp.find_field("Cookie")).get_values());
	for (size_t i = 0; i < Cookies.size(); ++i) {
		if (Cookies[i].find(CookieName) < Cookies[i].find("="))
			return (true);
	}
	return (false);
}

std::string	EventLoop::extractSessionId(Client &client) {
	HttpHeader	tmp(client.getRequest().get_header());

	std::vector<std::string> Cookies(tmp.get_field(tmp.find_field("Cookie")).get_values());
	for (size_t i = 0; i < Cookies.size(); ++i) {
		if (Cookies[i].find("session_id") < Cookies[i].find("=")) {
			return (Cookies[i].substr(Cookies[i].find("=") + 1, 64));
		}
	}
	return ("");
}

int	EventLoop::findSession(std::string const & session_id) {
	for (size_t i = 0; i < _Sessions.size(); ++i) {
		if (_Sessions[i].SessionIdVerification(session_id))
			return (i);
	}
	return (-1);
}

void	EventLoop::handleLogout(Client &client, HttpResponse &response, int eventFd) {
	if (client.getRequest().get_target().get_path() == "/logout"
		&& client.getRequest().get_method().get_method() == "POST")
	{
		std::cout << "🔍 Logout attempt detected for this clientFd = " << eventFd << std::endl;

		if (!findCookie(client, "session_id")) {
			std::cout << "\t❌️ Logout attempt failed: Not logged in." << std::endl;
			response.setStatusCode(303);
			response.setHeader("Location", "/logout/logoutFailed.html");
			return ;
		}
		else if (findCookie(client, "session_id")) {
			int	res = findSession(extractSessionId(client));

			if (res == -1) {
				std::cout << "\t❌️ Logout attempt failed: Not logged in." << std::endl;
				response.setStatusCode(303);
				response.setHeader("Location", "/logout/logoutFailed.html");
				return ;
			}
			else if (res >= 0) {
				_Sessions[res].logout();
				response.setHeader("Set-Cookie", _Sessions[res].getSessionId().serializeCookieClear());
				std::cout << "☑️ User logged out successfully." << std::endl;
				response.setStatusCode(303);
				response.setHeader("Location", "/logout/logoutSuccess.html");
				return ;
			}
		}
	}

}

void	EventLoop::handle_Q_and_A(Client &client, HttpResponse &response, int eventFd) {
	if (client.getRequest().get_target().get_path() == "/Q&A"
		&& client.getRequest().get_method().get_method() == "POST")
	{
		std::cout << "🔍 Access attempt to Q&A detected for this clientFd = " << eventFd << std::endl;

		if (!findCookie(client, "session_id")) {
			std::cout << "\t❌️ Access to Q&A not granted: Not logged in." << std::endl;
			response.setStatusCode(303);
			response.setHeader("Location", "/Q&A/Q&A_Access_not_granted.html");
			return ;
		}
		else if (findCookie(client, "session_id")) {
			int	res = findSession(extractSessionId(client));

			if (res == -1) {
				std::cout << "\t❌️ Access to Q&A not granted: Not logged in." << std::endl;
				response.setStatusCode(303);
				response.setHeader("Location", "/Q&A/Q&A_Access_not_granted.html");
				return ;
			}
			else if (res >= 0) {
				std::cout << "☑️ Access to Q&A granted to user." << std::endl;
				response.setStatusCode(303);
				response.setHeader("Location", "/Q&A/Q&A_Access_granted.html");
				return ;
			}
		}
	}
}

void	EventLoop::handleClient(int eventFd)
{
	_error = 0;
	Client	&client = _clients[eventFd];
	char	buffer[4096];

	if (client.getState() == WAITING_CGI)
		return;
	int		n = recv(client.getClientSocket(), buffer, sizeof(buffer) - 1, 0);
	//std::cout << "DEBUG:n = " << n << std::endl;
	if (n <= 0)
	{
		closeClient(eventFd);
		return;
	}
	client.setAppendBuffer(buffer, n);
	//std::cout << "DEBUG:BUFFER =\n" << client.getBuffer() << std::endl;
	parseRequest(client);
	//std::cout << "DEBUG:state client = " << client.getState() << std::endl;
	//std::cout << "DEBUG:error = " << _error << std::endl;

	if ((client.getState() == READY || client.getState() == ERROR) && !g_stop)
	{
		RequestRouter router;

		if (handleCgi(client, router, eventFd)) {
			return;
		}

		HttpResponse response = router.handle(client.getRequest(), client.getServerConfig(), _error);

		handleLogin(client, response, eventFd);
		handleLogout(client, response, eventFd);
		handle_Q_and_A(client, response, eventFd);

		client.setResponse(response.buildResponse());
		_epoll.mod(eventFd, EPOLLIN | EPOLLOUT);
	}
}

void	EventLoop::run()
{
	std::signal(SIGINT, on_sigint);
	std::cout << "Serveur en écoute (Ctrl-C pour arrêter)" << std::endl;

	epoll_event events[MAX_EVENTS];

	while (!g_stop)
	{
		_cgiHandler.checkTimeouts();
		for (std::size_t i = 0; i < _Sessions.size(); ++i) {
			_Sessions[i].checkExpiry();
		}
		int n_events = ::epoll_wait(_epoll.getEpfd(), events, MAX_EVENTS, 1);
		if (n_events <= 0)
		{
			if (g_stop)
				break;		// SIGINT (Ctrl-C)
			continue;		// autre signal (ex: SIGCHLD) → relance sans fermer
		}
		for (int i = 0; i < n_events && !g_stop; ++i)
		{
			int eventFd = events[i].data.fd;
			if (events[i].events & EPOLLOUT)
			{
				if (_cgiHandler.isCgiPipeInFd(eventFd))
					_cgiHandler.writeCGIInput(eventFd);
				else
					handleResponse(eventFd);
				continue;
			}
			if (isInListenSockets(eventFd))
			{
				acceptNewClients(eventFd);
				continue;
			}
			else if(_cgiHandler.isCgiFd(eventFd))
			{
				//std::cout << "\n[LOOP] eventFd=" << eventFd << " is CGI fd -> readOutput()\n";
				_cgiHandler.readOutput(eventFd);
				continue;
			}
			else if (events[i].events & EPOLLIN)
				handleClient(eventFd);
		}
	}
}
