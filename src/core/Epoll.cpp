#include "../../include/core/Epoll.hpp"

Epoll::Epoll()
{
	_epfd = ::epoll_create(1);
	if (_epfd == -1)
		throw std::runtime_error(std::string("epoll_create failed. "));
}

Epoll::~Epoll()
{
	if (_epfd != -1)
		::close(_epfd);
}

int	Epoll::getEpfd() const{
	return (_epfd);
}

// Ajoute un fd à l'instance epoll avec les événements spécifiés
// ajoute le fd à la liste de surveillance epoll
void	Epoll::add(int fd, uint32_t events)
{
	struct epoll_event ev;
	ev.events  = events;
	ev.data.fd = fd;
	if (::epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev) == -1)
		throw std::runtime_error(std::string("epoll_ctl(ADD) failed. "));
}

void	Epoll::mod(int fd, uint32_t events)
{
	struct epoll_event ev;
	ev.events  = events;
	ev.data.fd = fd;
	if (::epoll_ctl(_epfd, EPOLL_CTL_MOD, fd, &ev) == -1)
		throw std::runtime_error(std::string("epoll_ctl(MOD) failed. "));
}

void	Epoll::del(int fd)
{
	if (::epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL) == -1)
		std::cerr << "⚠️ epoll_ctl(DEL) failed: " << std::strerror(errno) << std::endl;
}
