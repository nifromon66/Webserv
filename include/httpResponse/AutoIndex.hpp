#ifndef AUTOINDEX_HPP
# define AUTOINDEX_HPP

#include <sstream>
#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdexcept>
#include <string>

class AutoIndex
{

	public:

		static std::string buildAutoIndexPage(const std::string& physicalPath, const std::string& requestUri);

	private:

		AutoIndex();
		~AutoIndex();
		AutoIndex(const AutoIndex& other);
		AutoIndex& operator=(const AutoIndex& other);

};

#endif