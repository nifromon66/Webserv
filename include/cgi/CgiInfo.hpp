#ifndef CGI_INFO_HPP
#define CGI_INFO_HPP

#include <string>
#include <unistd.h>

class CgiInfo
{
	private:
		int					_clientFd;
		int					_pipeOutFd;
		int					_pipeInFd;
		pid_t				_pid;
		std::string			_buffer;
		std::string			_header;
		bool				_headersSent;
		bool				_headerOk;
		long				_cgiStartTime;
		std::string			_CGIBody;
		size_t				_posPipeInOffset;

	public:
		// Canonique
		CgiInfo();
		CgiInfo(int clientFd, int pipeFd,int inFd, pid_t pid);
		CgiInfo(const CgiInfo &other);
		CgiInfo &operator=(const CgiInfo &other);
		~CgiInfo();

		// Getters
		int					getClientFd( void ) const;
		int					getPipeOutFd( void ) const;
		int					getPipeInFd( void ) const;
		pid_t				getPid( void ) const;
		const std::string	&getBuffer( void ) const;
		const bool			&getHeaderOk( void ) const;
		const bool			&getHeadersSent( void ) const;
		const std::string	&getHeader( void ) const;
		long				getCgiStartTime( void ) const;

		const std::string	&getCGIBody( void ) const;
		size_t				getPosPipeInOffset( void ) const;

		// Setters / helpers
		void				setClientFd(int fd);
		void				setPipeOutFd(int fd);
		void				setPipeInFd(int fd);
		void				setPid(pid_t pid);
		void				appendToBuffer(const char *chunk);
		void				appendToHeader(const char *buf, size_t len);
		void				setHeadersSent(const bool &state);
		void				setHeaderOk(const bool &state);
		void				eraseHeader(size_t n);
		void				setCgiStartTime(long startTimeStamp);
		void				setCGIBody(const std::string &body);
		void				addPosPipeInOffset(size_t n);

};

#endif