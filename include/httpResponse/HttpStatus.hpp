#ifndef HTTPSTATUS_HPP
#define HTTPSTATUS_HPP

#include <string>

class HttpStatus
{

	public:

		static std::string getStatusText(int code);

};

#endif