#ifndef EPOLL_HPP
#define EPOLL_HPP

#include <sys/epoll.h>
#include <unistd.h>
#include <stdexcept>
#include <cstring>
#include <cerrno>
#include <iostream>

#define MAX_EVENTS 64

class Epoll
{

	private:

		int		_epfd;

	public:

		Epoll();
		// Important de ne pas copier _epfd (Epoll) qui est une ressource système non duplicable
		// Epoll(const Epoll &other);
		// Epoll &operator=(const Epoll &other);
		~Epoll();

		void	add(int fd, uint32_t events);
		void	mod(int fd, uint32_t events);
		void	del(int fd);

		int		getEpfd() const;

};

#endif
