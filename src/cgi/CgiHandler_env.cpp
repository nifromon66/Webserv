#include "../../include/cgi/CgiHandler.hpp"

void	CgiHandler::addEnv(std::vector<std::string> &env, const std::string &key, const std::string &value) const
{
	env.push_back(key + "=" + value);
}

void	CgiHandler::appendBaseEnv(std::vector<std::string> &env, const HttpRequest &req, const std::string &scriptPath) const
{
	addEnv(env, "GATEWAY_INTERFACE", "CGI/1.1");
	addEnv(env, "SERVER_PROTOCOL", "HTTP/1.1");
	addEnv(env, "REQUEST_METHOD", req.get_method().get_method());
	addEnv(env, "SCRIPT_FILENAME", scriptPath);
	addEnv(env, "SCRIPT_NAME", req.get_target().get_path());
	addEnv(env, "REQUEST_URI", req.get_target().get_path());
	addEnv(env, "REDIRECT_STATUS", "200");
}

void	CgiHandler::appendQueryAndPath(std::vector<std::string> &env, const HttpRequest &req) const
{
	addEnv(env, "QUERY_STRING", req.get_target().get_query());
	addEnv(env, "PATH_INFO", req.get_target().get_path());
}

std::string	CgiHandler::extractHostName(const std::string &host) const
{
	size_t	pos;

	pos = host.find(':');
	if (pos == std::string::npos)
		return (host);
	return (host.substr(0, pos));
}

std::string	CgiHandler::extractHostPort(const std::string &host) const
{
	size_t	pos;

	pos = host.find(':');
	if (pos == std::string::npos)
		return ("80");
	return (host.substr(pos + 1));
}

std::string	CgiHandler::getHeaderValue(const HttpRequest &req,
		const std::string &name) const
{
	const std::vector<HttpField>	&fields = req.get_header().get_header();
	size_t							i;

	i = 0;
	while (i < fields.size())
	{
		if (fields[i].get_name() == name)
		{
			if (!fields[i].get_values().empty())
				return (fields[i].get_value(0));
			return ("");
		}
		i++;
	}
	return ("");
}

void	CgiHandler::appendServerEnv(std::vector<std::string> &env, const HttpRequest &req) const
{
	std::string	host;

	host = getHeaderValue(req, "Host");
	if (host.empty())
	{
		addEnv(env, "SERVER_NAME", "localhost");
		addEnv(env, "SERVER_PORT", "80");
		return ;
	}
	addEnv(env, "SERVER_NAME", extractHostName(host));
	addEnv(env, "SERVER_PORT", extractHostPort(host));
	addEnv(env, "HTTP_HOST", host);
}

void	CgiHandler::appendContentEnv(std::vector<std::string> &env, const HttpRequest &req) const
{
	std::string	method;
	std::string	ContentType;
	std::string	Cookies;
	size_t		len;

	Cookies = getHeaderValue(req, "Cookie");
	addEnv(env, "HTTP_COOKIE", Cookies);
	method = req.get_method().get_method();
	if (method != "POST")
	{
		addEnv(env, "CONTENT_LENGTH", "0");
		return ;
	}
	ContentType = getHeaderValue(req, "Content-Type");
	if (ContentType.empty())
		ContentType = "application/x-www-form-urlencoded";
	len = req.get_body().get_body().size();
	addEnv(env, "CONTENT_TYPE", ContentType);
	addEnv(env, "CONTENT_LENGTH", FileUtils::sizeToString(len));
}

std::vector<std::string>	CgiHandler::buildEnv(const Client &client, const std::string &scriptPath, const LocationConfig &loc)
{
	(void)loc;
	std::vector<std::string>	env;
	const HttpRequest			&req = client.getRequest();

	appendBaseEnv(env, req, scriptPath);
	appendQueryAndPath(env, req);
	appendServerEnv(env, req);
	appendContentEnv(env, req);
	return (env);
}