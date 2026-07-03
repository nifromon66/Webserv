NAME = webserv

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -I include
INCLUDES =	-I include \
			-I Request/include/Class -I Request/include/Error -I Request/include/Utils \
			-I Cookies/include

SRC = src/core/main.cpp \
		src/config/Config.cpp \
		src/core/Client.cpp \
		src/core/Server.cpp \
		src/config/ConfigParser.cpp \
		src/config/Location.cpp \
		src/core/Epoll.cpp \
		src/core/EventLoop.cpp \
		src/httpResponse/RequestRouter.cpp \
		src/httpResponse/HttpResponse.cpp \
		src/httpResponse/FileUtils.cpp \
		src/httpResponse/MimeTypes.cpp \
		src/httpResponse/AutoIndex.cpp \
		src/httpResponse/HttpStatus.cpp \
		src/cgi/CgiHandler.cpp \
		src/cgi/CgiHandler_env.cpp \
		src/cgi/CgiInfo.cpp \
		src/httpRequest/HttpBody.class.cpp \
		src/httpRequest/HttpField.class.cpp \
		src/httpRequest/HttpHeader.class.cpp \
		src/httpRequest/HttpMethod.class.cpp \
		src/httpRequest/HttpRequest.class.cpp \
		src/httpRequest/HttpTarget.class.cpp \
		src/httpRequest/HttpVersion.class.cpp \
		src/Error/HttpRequestStatus.error.cpp \
		src/Utils/HttpNormalize.utils.cpp \
		src/Utils/HttpSyntax.utils.cpp \
		src/cookies/Cookie.class.cpp \
		src/cookies/Session.class.cpp

OBJ_DIR = obj
OBJ = $(SRC:%.cpp=$(OBJ_DIR)/%.o)

SESSION_PHP_DIR = www/cgi-bin/sessions

all: ${NAME}

$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
	@echo "[✓] Compilation successful!"

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR) $(SESSION_PHP_DIR)
	@echo "[i] Object files cleaned."
	@echo "[i] PHP sessions cleaned."


fclean: clean
	@rm -f ${NAME}
	@echo "[i] Binary removed. Full clean done."

re: fclean all

.PHONY: all clean fclean re