#ifndef HTTPTARGET_CLASS_HPP
# define HTTPTARGET_CLASS_HPP

# include <iostream>
# include <string>
# include <vector>
# include <exception>

# include "../Macro/Colors.macro.hpp"
# include "../Macro/HttpTarget.macro.hpp"
# include "../Utils/HttpNormalize.utils.hpp"
#include "../Error/HttpRequestStatus.error.hpp"

class	HttpTarget {
	public:
										HttpTarget();
										HttpTarget(std::string const & target);
										HttpTarget(HttpTarget const & source);
										~HttpTarget();

			std::string const &			get_form(void) const;
			std::string const &			get_scheme(void) const;
			std::string const &			get_host(void) const;
			std::string	const &			get_port(void) const;
			std::string const &			get_path(void) const;
			std::string const &			get_query(void) const;
			std::string const &			get_fragment(void) const;
			std::string const &			get_target(void) const;

			void						set_target(std::string const & target);

			HttpTarget &				operator=(HttpTarget const & source);

	private:
			std::string					_form;
			std::string					_scheme;
			std::string					_host;
			std::string					_port;
			std::string					_path;
			std::string					_query;
			std::string					_fragment;
			std::string					_target;

			void						_determine_form(void);

			void						_verif_scheme(void);
			void						_verif_host(void);
			void						_verif_port(void);
			void						_verif_path(void);
			void						_verif_query(void);
			void						_verif_fragment(void);
			void						_parse_target(std::string const & target);

			void						_shape_target(void);

			friend std::ostream &		operator<<(std::ostream & out, HttpTarget const & in);
};

#endif