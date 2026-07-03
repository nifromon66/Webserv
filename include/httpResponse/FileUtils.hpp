#ifndef FILEUTILS_HPP
#define FILEUTILS_HPP

#include "../Error/HttpRequestStatus.error.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <fcntl.h>

class FileUtils
{

	public:

		static bool				isDirectory(const std::string& path);
		static bool				pathExists(const std::string& path);
		static std::string		readFile(const std::string& path);
		static bool				deleteFile(const std::string& path);

		static std::string		intToString(int n);
		static std::string		sizeToString(size_t n);

		static int				setFDNonBloquing(int fd);

};

#endif