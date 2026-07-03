# 42-Webserv

*This project has been created as part of the 42 curriculum by [sapupier], [badal-la], and [nifromon].*

---

## 📝 Project Overview

**Webserv** is a 42 curriculum project designed to develop a webserver in C++98 similar to Nginx or Apache.

The project challenges you to implement an authentic webserver following the HTTP protocol capable of handling multiple clients simultaneously without blocking.

### Features

The server in itself must be robust and capable of the following features :
- **Configuration:** The server follow to the letter the settings in the configuration file passed to the server as an argument. It can contain basic settings such as listening ports to deeper ones such as locations, methods allowed or max body size, etc ... (It was greatly inspired by the one found in Nginx).
- **Autoindex:** Generates directory listings when index files are missing.
- **Compatibility:** The server is compatible with standard browser such as Chrome or Firefox.
- **I/O multiplexing operations:** Efficiently handle all I/O operations between clients and server without blocking, it must manage those connections via **epoll** using only 1 poll (or equivalent).
- **Multi-ports & Multi-hosts:** Is capable of listening on various and multiple ports, while also being able to handle multiple server blocks.
- **HTTP Protocol [1.1]:** The server must follow the HTTP protocol of the version chosen for:
    - **HTTP Request:** The request sent by the client is parsed troughfully and will not let invalid request, such as error in the protocol or request not allowed by the server to be treated. But alternatives must be proposed to the client if possible.
    - **Methods:** The server must at least handle those methods.
        - **GET:** To fetch a content from the server if authorized.
        - **POST:** To add a content to the server if authorized.
        - **DELETE:** To delete a content from the server if authorized.
    - **HTTP Response:** After the request of the client is treated, the server must generate an adequate response following the same HTTP Protocol and giving accurate response status code.
- **Web Site:** The server will come with a web site which in our case will serve as an exemple of our work but also as a testing ground with the followings:
    - **Static File Serving:** Serves HTML, CSS, images and other static assets.
    - **Basic testing:** Methods, default error pages, MIME types etc ...
- **CGI (Common Gateway Interface):** The server must support multiple dynamic contents and services in multiple programming languages (Python, PHP, C, C++ etc...) using CGI, a method by which external programms are executable.
- **Cookies:** The server must be able to use and manage cookies for different purposes but mainly for client session management to implement memory for conveniance services such as client preferences, access to limited areas of the web site, personal data storage etc ...

---

## 🚀 Instructions

### Compilation

Use the provided `Makefile`:

```bash
make    	# Compiles the project and produces executable `./webserv`
make clean	# Removes object files and php sessions
make fclean	# Removes object files, php sessions and executable `./webserv`
make re		# Rebuild the project entirely
```

### Execution

Use the compiled executable `./webserv` with an apropriate `.conf` file.

```bash
./webserv <configuration_name.conf>
```

**Example:**

```bash
./webserv config/good/default.conf
```

The Program will start the server and display the following informations at first :
- The opened listen sockets.
- The listening ports.

And will later act as your eyes and ears as to what is happening on the server.

What is important are the listening ports which will help you navigate to have access to the site on your chosen browser:

http://localhost:[port]/

**Example:**

http://localhost:8080/

You will then be able to continue the testing with the web site helps.

### Tests required on the terminal

There are 3 set of tests involved in the testing which must be done on the terminal and are accessible in the `/tests/` directory at the root of the repo.

- **Basic:**
```bash
./tests/testWebserv.sh
./tests/ubuntu_tester http://localhost:8080
```

- **Curl:**
```bash
./tests/test_curl.sh
```

> [!WARNING]
> Siege tests are unavaible on the 42 desktops and must be done on a personal laptop.
- **Siege:**
```bash
./tests/siege.sh
```

### Closing

When you are finished, you can stop the server with (Ctrl+C), which will send a signal to the server.

---

## 📚 Resources

### Documentation and References

#### Norms

- **URI:**
	- [URI-RFC3986](./doc/URI/URI-rfc3986.pdf)
- **HTTP Protocol:**
	- **Informational:**
		- [HTTP_Cache_Control-RFC5861](./doc/HTTP_Protocol/Informational/HTTP_Cache-Control-rfc5861.pdf)
		- [HTTP_Version-RFC2145](./doc/HTTP_Protocol/Informational/HTTP_Version-rfc2145.pdf)
	- **Standard:**
		- [HTTP_1.1-RFC9110](./doc/HTTP_Protocol/Standard/HTTP_1.1-rfc9110.pdf)
		- [HTTP_1.1-RFC9111](./doc/HTTP_Protocol/Standard/HTTP_1.1-rfc9111.pdf)
		- [HTTP_1.1-RFC9112](./doc/HTTP_Protocol/Standard/HTTP_1.1-rfc9112.pdf)
- **Cookies:**
	- [HTTP_State_Management-RFC6265](./doc/Cookies/HTTP_State_Management-rfc6265.pdf)

#### Links

- **HTTP :** https://developer.mozilla.org/en-US/docs/Web/HTTP
- **CORS (Cross-Origin Ressource sharing) :** 'https://fetch.spec.whatwg.org/'
- **HTML Living Standard :** 'https://html.spec.whatwg.org/'
- **Beej's Guide to Network Programming :** https://beej.us/guide/bgnet/
- **Nginx Documentation :** https://nginx.org/en/docs/

#### AI

In accordance with the project guidelines, Artificial Intelligence tools (specifically ChatGPT/Gemini) were used during the development of this project for the following tasks:

- **Concept Clarification:** Used to help understand complex topics and clarify certain formulations or vocabularies.
- **Code Structure:** Used to help us start with a good base with suggestions as for our project tree.
- **Debugging and Optimizations:** Used to help us understand error logs or specific bugs in the process of making the project.
- **Testing:** Used to help generate tests to ensure the robustness of the server.
- **Web Site:** As we are unfamiliar with languages such as HTML, CSS, JAVASCRIPT, PYTHON or PHP. It helped us generate scripts when we use them like for the design and organization of the web page or the scripts executed on CGI.

> [!NOTE]
> Aside from that, all the code used were personally written and understood by each team member.

---
