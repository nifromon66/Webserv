#include "../../include/config/Location.hpp"

LocationConfig::LocationConfig() :
	path(""),
	root(""),
	autoindex(false),
	client_max_body_size(static_cast<size_t>(-1)),
	client_max_header_size(8 * 1024),                   //8 Ko par défaut
	upload_dir(""),
	return_code(0)
{
	this->allowed_methods.push_back("GET");             // GET initialise par défaut
	this->redirection = std::make_pair(0, "");          // 0 signifie pas de redirection
}

LocationConfig::LocationConfig(const LocationConfig &copy) :
	path(copy.path),
	root(copy.root),
	allowed_methods(copy.allowed_methods),
	autoindex(copy.autoindex),
	index_files(copy.index_files),
	client_max_body_size(copy.client_max_body_size),
	client_max_header_size(copy.client_max_header_size),
	redirection(copy.redirection),
	upload_dir(copy.upload_dir),
	cgi_executors(copy.cgi_executors),
	error_pages(copy.error_pages)
{
	this->return_code = copy.return_code;
	this->return_url = copy.return_url;
}

LocationConfig &LocationConfig ::operator=(const LocationConfig &other)
{
	if (this != &other)
	{
		this->path = other.path;
		this->root = other.root;
		this->allowed_methods = other.allowed_methods;
		this->autoindex = other.autoindex;
		this->index_files = other.index_files;
		this->client_max_body_size = other.client_max_body_size;
		this->client_max_header_size = other.client_max_header_size;
		this->redirection = other.redirection;
		this->upload_dir = other.upload_dir;
		this->cgi_executors = other.cgi_executors;
		this->error_pages = other.error_pages;
	}
	return *this;
}


LocationConfig::~LocationConfig()
{}

