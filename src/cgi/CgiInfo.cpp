#include "../../include/cgi/CgiInfo.hpp"

// Canonique

CgiInfo::CgiInfo() :
	_clientFd(-1),
	_pipeOutFd(-1),
	_pipeInFd(-1),
	_pid(-1),
	_buffer(),
	_header(),
	_headersSent(false),
	_headerOk(false),
	_cgiStartTime(0),
	_CGIBody(),
	_posPipeInOffset(0)
{}

CgiInfo::CgiInfo(int clientFd, int pipeOutFd, int pipeInFd, pid_t pid) :
	_clientFd(clientFd),
	_pipeOutFd(pipeOutFd),
	_pipeInFd(pipeInFd),
	_pid(pid),
	_buffer(),
	_header(),
	_headersSent(false),
	_headerOk(false),
	_cgiStartTime(0),
	_CGIBody(),
	_posPipeInOffset(0)
{}

CgiInfo::CgiInfo(const CgiInfo &other)
{
	*this = other;
}

CgiInfo &CgiInfo::operator=(const CgiInfo &other)
{
	if (this != &other)
	{
		_clientFd = other._clientFd;
		_pipeOutFd = other._pipeOutFd;
		_pipeInFd = other._pipeInFd;
		_pid = other._pid;

		_buffer = other._buffer;
		_header = other._header;

		_headersSent = other._headersSent;
		_headerOk = other._headerOk;

		_cgiStartTime = other._cgiStartTime;

		_CGIBody = other._CGIBody;
		_posPipeInOffset = other._posPipeInOffset;
	}
	return *this;
}

CgiInfo::~CgiInfo()
{}

// Getters

int CgiInfo::getClientFd() const
{
	return _clientFd;
}

int CgiInfo::getPipeOutFd() const
{
	return _pipeOutFd;
}

int CgiInfo::getPipeInFd() const
{
	return _pipeInFd;
}

pid_t CgiInfo::getPid() const
{
	return _pid;
}

const std::string &CgiInfo::getBuffer() const
{
	return _buffer;
}

const std::string &CgiInfo::getHeader() const
{
	return _header;
}

const bool &CgiInfo::getHeaderOk() const
{
	return _headerOk;
}

const bool &CgiInfo::getHeadersSent() const
{
	return _headersSent;
}

long CgiInfo::getCgiStartTime(void) const
{
	return _cgiStartTime;
}

// stdin CGI

const std::string &CgiInfo::getCGIBody(void) const
{
	return _CGIBody;
}

size_t CgiInfo::getPosPipeInOffset(void) const
{
	return _posPipeInOffset;
}

// Setters / helpers

void CgiInfo::setClientFd(int fd)
{
	_clientFd = fd;
}

void CgiInfo::setPipeOutFd(int fd)
{
	_pipeOutFd = fd;
}

void CgiInfo::setPipeInFd(int fd)
{
	_pipeInFd = fd;
}

void CgiInfo::setPid(pid_t pid)
{
	_pid = pid;
}

void CgiInfo::appendToBuffer(const char *chunk)
{
	_buffer.append(chunk);
}

void CgiInfo::appendToHeader(const char *buf, size_t len)
{
	_header.append(buf, len);
}

void CgiInfo::setHeadersSent(const bool &state)
{
	_headersSent = state;
}

void CgiInfo::setHeaderOk(const bool &state)
{
	_headerOk = state;
}

void CgiInfo::eraseHeader(size_t n)
{
	if (n >= _header.size())
		_header.clear();
	else
		_header.erase(0, n);
}

void CgiInfo::setCgiStartTime(long startTimeStamp)
{
	_cgiStartTime = startTimeStamp;
}

// stdin CGI helpers

void CgiInfo::
setCGIBody(const std::string &body)
{
	_CGIBody = body;
	_posPipeInOffset = 0;
}

void CgiInfo::addPosPipeInOffset(size_t n)
{
	_posPipeInOffset += n;
}
