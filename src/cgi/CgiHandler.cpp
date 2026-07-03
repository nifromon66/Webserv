#include "../../include/cgi/CgiHandler.hpp"
#include "../../include/httpResponse/RequestRouter.hpp"

CgiHandler::CgiHandler(Epoll &epoll, std::map<int, Client> &clients) :
	_epoll(epoll),
	_clients(clients),
	_cgiInfos()
{}

CgiHandler::~CgiHandler()
{}

bool CgiHandler::isCgiFd(int fd) const
{
	return (_cgiInfos.find(fd) != _cgiInfos.end());
}

pid_t CgiHandler::forkCgi(int pipeIn[2], int pipeOut[2])
{
	pid_t pid = ::fork();
	if (pid == -1)
	{
		::close(pipeIn[0]);
		::close(pipeIn[1]);
		::close(pipeOut[0]);
		::close(pipeOut[1]);
		throw HttpRequestStatus(500, "Internal Server Error");
	}
	return (pid);
}

void CgiHandler::createAndLaunchCGI(int clientFd, Client &client, const LocationConfig &loc)
{
	int pipeIn[2];
	int pipeOut[2];

	if (::pipe(pipeIn) == -1 || ::pipe(pipeOut) == -1)
		throw HttpRequestStatus(500, "Internal Server Error");

	pid_t pid = forkCgi(pipeIn, pipeOut);
	if (pid == 0)
	{
		setupChildAndExec(pipeIn, pipeOut, client, loc);
		_exit(1); // sécurité si jamais
	}

	setupParentAfterFork(pid, clientFd, client, pipeIn, pipeOut);
}

void CgiHandler::setupChildAndExec(int pipeIn[2], int pipeOut[2], Client &client, const LocationConfig &loc)
{
	// redirection stdin/stdout
	if (::dup2(pipeIn[0], STDIN_FILENO) == -1 || ::dup2(pipeOut[1], STDOUT_FILENO) == -1)
		_exit(1);

	::close(pipeIn[1]);
	::close(pipeOut[0]);

	std::string scriptPath = fillScriptPath(client, loc);
	std::string executorPath = fillExecutorPath(client, loc);

	std::vector<char*> argv;
	argv.push_back(const_cast<char*>(executorPath.c_str()));
	if (executorPath != scriptPath)
		argv.push_back(const_cast<char*>(scriptPath.c_str()));
	argv.push_back(NULL);

	std::vector<std::string> envStrings = buildEnv(client, scriptPath, loc);

	std::vector<char*> envp;
	for (size_t i = 0; i < envStrings.size(); ++i)
		envp.push_back(const_cast<char*>(envStrings[i].c_str()));
	envp.push_back(NULL);

	::execve(executorPath.c_str(), &argv[0], &envp[0]);

	_exit(1); // execve a échoué
}

static long	getNowMs(void)
{
	struct timespec	ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ts.tv_sec * 1000L + ts.tv_nsec / 1000000L);
}

void CgiHandler::sendCgiError(int clientFd, int statusCode)
{
	Client &client = _clients[clientFd];

	if (!client.hasServerConfig())
		client.setDefaultServerConfig();

	HttpResponse resp = RequestRouter().buildErrorResponse(statusCode, client.getServerConfig());

	client.setResponse(resp.buildResponse());
	_epoll.mod(clientFd, EPOLLOUT);
}

void	CgiHandler::checkTimeouts(void)
{
	const long	now = getNowMs();
	const long	timeoutMs = 3 * 1000;									// (3s pour test)

	std::map<int, CgiInfo>::iterator it = _cgiInfos.begin();
	while (it != _cgiInfos.end())
	{
		std::map<int, CgiInfo>::iterator cgiFd = it++;
		CgiInfo &info = cgiFd->second;

		if (info.getCgiStartTime() == 0 || now - info.getCgiStartTime() <= timeoutMs)
			continue;

		::kill(info.getPid(), SIGKILL);
		int	status = 0;
		::waitpid(info.getPid(), &status, WNOHANG);

		_epoll.del(cgiFd->first);
		::close(cgiFd->first);
		sendCgiError(info.getClientFd(), 504);

		int inFd = info.getPipeInFd();
		if (inFd != -1)
		{
			_epoll.del(inFd);
			::close(inFd);
			info.setPipeInFd(-1);
		}

		_cgiInfos.erase(cgiFd);
	}
}

void CgiHandler::setupParentAfterFork(pid_t pid, int clientFd, Client &client,
									int pipeIn[2], int pipeOut[2])
{
	::close(pipeIn[0]);   // parent écrit dans stdin CGI
	::close(pipeOut[1]);  // parent lit stdout CGI

	FileUtils::setFDNonBloquing(pipeIn[1]);
	FileUtils::setFDNonBloquing(pipeOut[0]);

	const HttpRequest &request = client.getRequest();

	CgiInfo info(clientFd, pipeOut[0], pipeIn[1], pid);

	if (request.get_method().get_method() == "POST")
		info.setCGIBody(request.get_body().get_body());

	_cgiInfos[pipeOut[0]] = info;
	_cgiInfos[pipeOut[0]].setCgiStartTime(getNowMs());

	// stdout CGI -> lecture
	_epoll.add(pipeOut[0], EPOLLIN);

	// stdin CGI -> écriture uniquement si on a un body à envoyer
	if (!_cgiInfos[pipeOut[0]].getCGIBody().empty())
		_epoll.add(pipeIn[1], EPOLLOUT);
	else
	{
		// pas de body => on ferme stdin tout de suite (EOF)
		::close(pipeIn[1]);
		_cgiInfos[pipeOut[0]].setPipeInFd(-1);
	}

	client.setState(WAITING_CGI);
}


std::string CgiHandler::fillScriptPath(const Client &client, const LocationConfig &loc)
{
	// ex: "/cgi-bin/test.py"
	std::string uriPath = client.getRequest().get_target().get_path();
	std::string relative;

	if (uriPath.compare(0, loc.path.size(), loc.path) == 0)
		relative = uriPath.substr(loc.path.size());
	else
		relative = uriPath;

	// Construire : loc.root + "/" + relative en gérant proprement les "/" en double
	std::string root = loc.root;
	if (root.empty())
		root = ".";																// par sécurité

	bool rootEndsWithSlash     = (!root.empty() && root[root.size() - 1] == '/');
	bool relativeStartsWithSlash = (!relative.empty() && relative[0] == '/');

	if (rootEndsWithSlash && relativeStartsWithSlash)
		return (root + relative.substr(1));
	else if (!rootEndsWithSlash && !relativeStartsWithSlash)
		return (root + "/" + relative);
	else
		return (root + relative);
}

static std::string getExtension(const std::string &path)
{
	size_t	slashPos = path.find_last_of('/');
	size_t	dotPos = path.find_last_of('.');

	if (dotPos == std::string::npos)
		return ("");

	if (slashPos != std::string::npos && dotPos < slashPos)		// le '.' est dans un dossier (ex: ".hidden/dir")
		return ("");

	return (path.substr(dotPos));		// ex: ".py"
}

std::string CgiHandler::fillExecutorPath(const Client &client, const LocationConfig &loc)
{
	std::string uriPath = client.getRequest().get_target().get_path();
	std::string ext = getExtension(uriPath);

	// 1) Si une entrée existe dans loc.cgi_executors pour cette extension → on l'utilise
	if (!ext.empty())
	{
		std::map<std::string, std::string>::const_iterator it = loc.cgi_executors.find(ext);
		if (it != loc.cgi_executors.end())
			return (it->second);				// ex: "/usr/bin/python3"
	}

	// 2) Sinon, on considère que le script est directement exécutable
	//    (binaire ou script avec shebang).
	// Dans ce cas, on retourne directement le chemin du script.
	std::string scriptPath = fillScriptPath(client, loc);
	return (scriptPath);
}

// transforme n de size_t à hexadécimal
static std::string toHex(size_t n)
{
	std::ostringstream oss;
	oss << std::hex << n;
	return (oss.str());
}

// construit la string à envoyer du morceau de réponse connu
static std::string makeChunk(const char *data, size_t len)
{
	std::string out;
	out += toHex(len);
	out += "\r\n";
	out.append(data, len);
	out += "\r\n";
	return (out);
}

// trouver la fin du header (\r\n\r\n ou \n\n)
static bool findHeaderEnd(const std::string &s, size_t &endPos, size_t &offset)
{
	size_t pos;

	pos = s.find("\r\n\r\n");
	if (pos != std::string::npos)
	{
		endPos = pos;
		offset = 4;
		return (true);
	}
	pos = s.find("\n\n");
	if (pos != std::string::npos)
	{
		endPos = pos;
		offset = 2;
		return (true);
	}
	return (false);
}

// retourne le status code
static int parseCgiStatus(const std::string &hdr)
{
	size_t pos = hdr.find("Status:");
	if (pos == std::string::npos)
		return (200);
	pos += 7;
	while (pos < hdr.size() && (hdr[pos] == ' ' || hdr[pos] == '\t'))
		pos++;
	int code = std::atoi(hdr.c_str() + pos);
	if (code <= 0)
		return (200);
	return (code);
}

// prépare le header CGI à l'envoi
void CgiHandler::sendHeaders(int clientFd, CgiInfo &info, int statusCode)
{
	if (info.getHeadersSent() == true)
		return;

	std::ostringstream resp;
	resp << "HTTP/1.1 " << statusCode << " " << HttpStatus::getStatusText(statusCode) << "\r\n"
		 << "Transfer-Encoding: chunked\r\n"
		 << "Content-Type: text/html; charset=utf-8\r\n"
		 << "Connection: close\r\n"
		 << "Server: Webserv/1.0 (42)\r\n"
		 << "\r\n";

	_clients[clientFd].appendResponse(resp.str());
	_epoll.mod(clientFd, EPOLLOUT);
	info.setHeadersSent(true);
}

// prépare le morceau de CGI retourné par le script à l'envoi
void CgiHandler::pushChunkBodyCGI(int clientFd, const char *data, size_t len)
{
	if (len == 0)
		return;

	_clients[clientFd].appendResponse(makeChunk(data, len));
	_epoll.mod(clientFd, EPOLLOUT);
}

// Accumule buf  dans _header jusqu'à reception de tout le header
// Envoi au client du buffer et du morceau de body déjà présent dans buf
void CgiHandler::handleCgiHeaders(int clientFd, CgiInfo &info, const char *buf, size_t len)
{
	size_t endPos;
	size_t offset;

	info.appendToHeader(buf, len);

	if (!findHeaderEnd(info.getHeader(), endPos, offset))
		return ;
	info.setHeaderOk(true);

	if (info.getHeadersSent() == false)
	{
		int statusCode = parseCgiStatus(info.getHeader());
		sendHeaders(clientFd, info, statusCode);
	}

	size_t bodyStart = endPos + offset;
	if (bodyStart < info.getHeader().size())
	{
		std::string beginOfBody = info.getHeader().substr(bodyStart);
		if (!beginOfBody.empty())
			pushChunkBodyCGI(clientFd, beginOfBody.c_str(), beginOfBody.size());
	}

	info.eraseHeader(info.getHeader().size());
	return ;
}

bool CgiHandler::ckeckLocationBla(Client &client)
{
	std::string path = client.getRequestRef().get_target().get_path();

	//std::cout << "DEBUG path loc bla:= [" << path << "]" << std::endl;

	if (path.size() < 4)
		return (false);
	return (path.compare(path.size() - 4, 4, ".bla") == 0);
}

static void	sendEmpty200(Client &client, Epoll &epoll, int clientFd)
{
	client.setResponse(
		"HTTP/1.1 200 OK\r\n"
		"Content-Length: 0\r\n"
		"Connection: close\r\n\r\n"
	);
	epoll.mod(clientFd, EPOLLOUT);
}

bool	CgiHandler::handleCgiEarlyExit(std::map<int, CgiInfo>::iterator it, int clientFd, int status)
{
	Client	&client = _clients[clientFd];
	bool	exitedNormally = WIFEXITED(status);
	bool	locationBla = ckeckLocationBla(client);

	/* Exception .bla : CGI vide -> 200 */
	if (locationBla && !it->second.getHeaderOk() && it->second.getBuffer().empty())
	{
		sendEmpty200(client, _epoll, clientFd);
		_cgiInfos.erase(it);
		return (true);
	}

	/* CGI pas terminé correctement */
	if (!exitedNormally)
	{
		sendCgiError(clientFd, 502);
		_cgiInfos.erase(it);
		return (true);
	}

	/* Headers invalides */
	if (!it->second.getHeaderOk())
	{
		sendCgiError(clientFd, 502);
		_cgiInfos.erase(it);
		return (true);
	}

	return (false);
}

void CgiHandler::finishCgi(std::map<int, CgiInfo>::iterator it, int cgiFd)
{
	int		clientFd = it->second.getClientFd();
	pid_t	pid = it->second.getPid();
	int		status = 0;
	pid_t	ret;

	ret = ::waitpid(pid, &status, WNOHANG);
	if (ret == 0)
		return;

	_epoll.del(cgiFd);
	::close(cgiFd);

	if (ret < 0)
	{
		sendCgiError(clientFd, 502);
		_cgiInfos.erase(it);
		return;
	}

	if (handleCgiEarlyExit(it, clientFd, status))
		return;

	if (it->second.getHeadersSent())
	{
		_clients[clientFd].appendResponse("0\r\n\r\n");
		_epoll.mod(clientFd, EPOLLOUT);
	}
	_cgiInfos.erase(it);
}

void CgiHandler::readOutput(int cgiFd)
{
	std::map<int, CgiInfo>::iterator	it = _cgiInfos.find(cgiFd);
	if (it == _cgiInfos.end())
		return;

	CgiInfo	&info = it->second;
	int		clientFd = info.getClientFd();

	char buf[4096];
	ssize_t n = ::read(cgiFd, buf, sizeof(buf));
	//std::cout << "DEBUG READ OUTPUT: n = " << n << std::endl;
	if (n > 0)
	{
		if (info.getHeaderOk() == false)
		{
			handleCgiHeaders(clientFd, info, buf, static_cast<size_t>(n));
			return;
		}
		pushChunkBodyCGI(clientFd, buf, static_cast<size_t>(n));
	}
	else if (n == 0)
		finishCgi(it, cgiFd);
	else // n < 0
	{
		sendCgiError(clientFd, 502);
		_epoll.del(cgiFd);
		::close(cgiFd);

		int PipeInFd = info.getPipeInFd();
		if (PipeInFd != -1)
		{
			_epoll.del(PipeInFd);
			::close(PipeInFd);
			info.setPipeInFd(-1);
		}
		_cgiInfos.erase(it);
	}
}

bool CgiHandler::isCgiPipeInFd(int fd) const
{
	std::map<int, CgiInfo>::const_iterator it = _cgiInfos.begin();
	while (it != _cgiInfos.end())
	{
		if (it->second.getPipeInFd() == fd)
			return true;
		++it;
	}
	return false;
}

CgiInfo *CgiHandler::findInfoOfPipeInFd(int fd)
{
	std::map<int, CgiInfo>::iterator it = _cgiInfos.begin();
	while (it != _cgiInfos.end())
	{
		if (it->second.getPipeInFd() == fd)
			return &it->second;
		++it;
	}
	return NULL;
}

void CgiHandler::writeCGIInput(int pipeInFd)
{
	CgiInfo *info = findInfoOfPipeInFd(pipeInFd);
	if (!info)
		return;

	const std::string &body = info->getCGIBody();
	size_t offset = info->getPosPipeInOffset();

	// Plus rien à envoyer -> fermer stdin CGI => EOF côté script
	if (offset >= body.size())
	{
		_epoll.del(pipeInFd);
		::close(pipeInFd);
		info->setPipeInFd(-1);
		return;
	}

	// 1 seule write par event EPOLLOUT
	ssize_t w = ::write(pipeInFd, body.c_str() + offset, body.size() - offset);
	if (w <= 0)
	{
		// version simple/robuste : on stoppe l'input en cas de souci
		_epoll.del(pipeInFd);
		::close(pipeInFd);
		info->setPipeInFd(-1);
		return;
	}

	info->addPosPipeInOffset(static_cast<size_t>(w));

	// Si tout est envoyé -> fermer stdin
	if (info->getPosPipeInOffset() >= info->getCGIBody().size())
	{
		_epoll.del(pipeInFd);
		::close(pipeInFd);
		info->setPipeInFd(-1);
	}
}
