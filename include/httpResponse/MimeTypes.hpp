#ifndef MIMETYPES_HPP
# define MIMETYPES_HPP

#include <string>
#include <map>

class MimeTypes
{
	public:

		static std::string getType(const std::string& path);

	private:

		MimeTypes();
		~MimeTypes();

		static std::map<std::string, std::string> _types;
		static void _init();

};

#endif