/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vismaily <nenie_iri@mail.ru>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/29 16:36:35 by vismaily          #+#    #+#             */
/*   Updated: 2022/11/08 15:54:24 by vismaily         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <vector>
# include <map>
# include <utility>
# include <string>
# include <cstring>
# include <cctype>
# include <algorithm>

# include <iostream>

class	Server
{
	private:
		typedef std::string				t_str;

	public:
		Server(t_str &body);
		Server(const Server &other);
		Server	&operator=(const Server &rhs);
		~Server();
	private:
		Server();

	public:
		const std::vector<t_str>		&getServerName() const;
		const std::map<t_str, t_str>	&getListen() const;
		const std::string				&getRoot() const;
	private:
		void					setServerName(t_str &value);
		void					setListen(t_str &addr, t_str &port);
		void					setRoot(const t_str &root);
	private:
		void					parsingRoot(t_str &value);
		void					parsingListen(t_str &value);
		bool					isValidPort(t_str port) const;
		bool					isValidIP(t_str addr) const;
		void					setFildes(const t_str &name, t_str &value);
		void					parsingLocation(t_str &body, t_str::size_type \
									value_begin, t_str::size_type value_end);
		void					setDefaults();
		void					parsingBody(t_str &body);
	private:
		std::vector<t_str>		_directiveList;
		std::vector<t_str>		_serverName;
		std::map<t_str, t_str>	_listen;
		std::string				_root;
};

#endif
