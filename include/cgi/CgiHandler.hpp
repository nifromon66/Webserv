#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include "../core/Epoll.hpp"
#include "../core/Client.hpp"
#include "../cgi/CgiInfo.hpp"
#include "../httpResponse/FileUtils.hpp"
#include "../httpResponse/HttpStatus.hpp"
#include "../Error/HttpRequestStatus.error.hpp"

#include <map>
#include <vector>
#include <string>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdexcept>
#include <signal.h>
#include <time.h>
#include <sstream>
#include <cstdlib>

class CgiHandler
{
	private:

		Epoll						&_epoll;
		std::map<int, Client>		&_clients;
		std::map<int, CgiInfo>		_cgiInfos;		// key = CgiFd soit pipeOut[0] (fd d'ecoute coté parent de ce que l'enfant lui transmet)


		pid_t						forkCgi(int pipeIn[2], int pipeOut[2]);
		std::string					fillScriptPath(const Client &client, const LocationConfig &loc);
		std::string					fillExecutorPath(const Client &client, const LocationConfig &loc);
		void						setupChildAndExec(int pipeIn[2], int pipeOut[2], Client &client, const LocationConfig &loc);
		void						sendCgiError(int clientFd, int statusCode);
		void						setupParentAfterFork(pid_t pid, int clientFd, Client &client, int pipeIn[2], int pipeOut[2]);

		void						sendHeaders(int clientFd, CgiInfo &info, int statusCode);
		void						pushChunkBodyCGI(int clientFd, const char *data, size_t len);
		void						handleCgiHeaders(int clientFd, CgiInfo &info, const char *buf, size_t len);
		
		bool						ckeckLocationBla(Client &client);
		bool						handleCgiEarlyExit(std::map<int, CgiInfo>::iterator it, int clientFd, int status);
		void						finishCgi(std::map<int, CgiInfo>::iterator it, int cgiFd);

		void						addEnv(std::vector<std::string> &env, const std::string &key, const std::string &value) const;
		void						appendBaseEnv(std::vector<std::string> &env, const HttpRequest &req, const std::string &scriptPath) const;
		void						appendQueryAndPath(std::vector<std::string> &env, const HttpRequest &req) const;
		std::string					extractHostName(const std::string &host) const;
		std::string					extractHostPort(const std::string &host) const;
		std::string					getHeaderValue(const HttpRequest &req, const std::string &name) const;
		void						appendServerEnv(std::vector<std::string> &env, const HttpRequest &req) const;
		void						appendContentEnv(std::vector<std::string> &env, const HttpRequest &req) const;
		std::vector<std::string>	buildEnv(const Client &client, const std::string &scriptPath, const LocationConfig &loc);


	public:

		CgiHandler(Epoll &epoll, std::map<int, Client> &clients);
		~CgiHandler();

		bool						isCgiFd(int fd) const;
		void						createAndLaunchCGI(int clientFd, Client &client, const LocationConfig &loc);
		void						readOutput(int cgiFd);
		void						checkTimeouts(void);

		bool						isCgiPipeInFd(int fd) const;
		CgiInfo						*findInfoOfPipeInFd(int fd);
		void						writeCGIInput(int inFd);

};

#endif