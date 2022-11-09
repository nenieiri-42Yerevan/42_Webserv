/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vismaily <nenie_iri@mail.ru>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/29 16:42:16 by vismaily          #+#    #+#             */
/*   Updated: 2022/11/09 12:29:02 by vismaily         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/*=====================================*/
/*     Constructors and Destructor     */
/*=====================================*/

Server::Server()
{
}

Server::Server(t_str &body)
{
	t_str	tmp;

	tmp = body.substr(1, body.length() - 2);
	if (tmp == "" || tmp.find_first_not_of(" \t\v\r\n\f") == t_str::npos)
		throw std::logic_error("Error: Config file: directive 'server' is empty.");
	_directiveList.push_back("server_name");
	_directiveList.push_back("listen");
	_directiveList.push_back("root");
	_directiveList.push_back("location");
	parsingBody(tmp);
}

Server::Server(const Server &other)
{
	this->_directiveList = other._directiveList;
	this->_serverName = other._serverName;
	this->_listen = other._listen;
	this->_root = other._root;
}

Server	&Server::operator=(const Server &rhs)
{
	if (this != &rhs)
	{
		this->_directiveList = rhs._directiveList;
		this->_serverName = rhs._serverName;
		this->_listen = rhs._listen;
		this->_root = rhs._root;
	}
	return (*this);
}

Server::~Server()
{
}

/*=====================================*/
/*          Setter and Getters         */
/*=====================================*/

const std::vector<std::string>	&Server::getServerName() const
{
	return (this->_serverName);
}

const std::map<std::string, std::string>	&Server::getListen() const
{
	return (this->_listen);
}

const std::string	&Server::getRoot() const
{
	return (this->_root);
}

const std::map<std::string, Location>	&Server::getLocation() const
{
	return (this->_location);
}

void	Server::setLocation(t_str &value)
{
	std::string::size_type	pos;
	std::string::size_type	pos2;
	std::string				name;
	std::string				inner;

	pos = value.find_first_of("{");
	name = value.substr(0, pos);
	pos2 = name.find_last_not_of(" \t\v\r\n\f");
	if (pos2 == std::string::npos)
		throw std::runtime_error("Error: Location is not valid.");
	++pos2;
	name = name.substr(0, pos2);
	++pos;
	inner = value.substr(pos, value.length() - 1 - pos);
	this->_location.insert(std::make_pair(name, Location(inner)));
}

void	Server::setServerName(t_str &value)
{
	char	*token;

	token = std::strtok(&value[0], " \t\v\r\n\f");
	while (token != NULL)
	{
		this->_serverName.push_back(token);
		token = std::strtok(NULL, " \t\v\r\n\f");
	}
}

void	Server::setListen(t_str &addr, t_str &port)
{
	if (isValidIP(addr) == 0)
	{
		this->_serverName.push_back(addr);
		addr = "*";
	}
	this->_listen.insert(std::make_pair(addr, port));
}

void	Server::setRoot(const t_str &root)
{
	this->_root = root;
}

/*=====================================*/
/*       Other Member Functions        */
/*=====================================*/

void	Server::parsingRoot(t_str &value)
{
	std::string::size_type	pos;

	pos = value.find_last_not_of(" \t\v\r\n\f");
	if (pos == std::string::npos)
		throw std::runtime_error("Error: Root is not valid.");
	++pos;
	value = value.substr(0, pos);
	setRoot(value);
}

void	Server::parsingListen(t_str &value)
{
	t_str::size_type	colon_pos;
	t_str::size_type	addr_end;
	t_str				addr;
	t_str::size_type	port_begin;
	t_str::size_type	port_end;
	t_str				port;

	colon_pos = value.find(':');
	if (colon_pos != t_str::npos)
	{
		if (colon_pos == 0)
		{
			addr = "*";
			port_begin = value.find_first_not_of(" \t\v\r\n\f", 1);
			port_end = value.find_last_not_of(" \t\v\r\n\f") + 1;
		}
		else
		{
			addr_end = value.find_last_not_of(" :\t\v\r\n\f", colon_pos) + 1;
			addr = value.substr(0, addr_end);
			port_begin = value.find_first_not_of(" \t\v\r\n\f", colon_pos + 1);
			port_end = value.find_last_not_of(" \t\v\r\n\f") + 1;
		}
		port = value.substr(port_begin, port_end - port_begin);
		if (this->isValidPort(port) == false)
			throw std::runtime_error("Error: Port is not valid.");
		port_begin = port.find_first_not_of('0');
		if (port_begin == t_str::npos)
			throw std::runtime_error("Error: Port is not valid.");
		port = port.substr(port_begin, port.length() - port_begin);
	}
	else
	{
		value = value.substr(0, value.find_last_not_of(" \t\v\r\n\f") + 1);
		if (this->isValidPort(value) == true)
		{
			port = value;
			port_begin = port.find_first_not_of('0');
			if (port_begin == t_str::npos)
				throw std::runtime_error("Error: Port is not valid.");
			port = port.substr(port_begin, port.length() - port_begin);
			addr = "*";
		}
		else
		{
			addr = value;
			port = "80";
		}
	}
	this->setListen(addr, port);
}

bool	Server::isValidPort(t_str port) const
{
	t_str::size_type	begin;

	begin = port.find_first_not_of('0');
	if (begin == t_str::npos)
		return (0);
	port = port.substr(begin, port.length() - begin);
	for(t_str::iterator	it = port.begin(); it < port.end(); ++it)
	{
		if (std::isdigit(*it) == 0)
			return (0);
	}
	if (port.size() > 5)
		return (0);
	if (std::atoi(port.c_str()) > 65536)
		return (0);
	return (1);
}

bool	Server::isValidIP(t_str addr) const
{
	t_str::size_type	count;
	char				*token;
	t_str				token_str;
	t_str				spaces = " \t\v\r\n\f";

	count = 0;
	if (addr == "*")
		return (1);
	if (addr[0] == '.')
		return (0);
	for(t_str::iterator	it = addr.begin(); it < addr.end(); ++it)
	{
		if (*it == '.')
			++count;
		else if (spaces.find(*it) != t_str::npos)
			throw std::runtime_error("Error: listen syntax is wrong.");
	}
	if (count != 3)
		return (0);
	token = std::strtok(&addr[0], ".");
	while (token != NULL)
	{
		token_str = token;
		if (token_str.length() > 3)
			return (0);
		for (t_str::iterator it = token_str.begin(); it < token_str.end(); ++it)
		{
			if (std::isdigit(*it) == 0)
				return (0);
		}
		if (std::atoi(token_str.c_str()) > 255)
			return (0);
		token = std::strtok(NULL, ".");
	}
	return (1);
}

void	Server::setFildes(const t_str &name, t_str &value)
{
	if (name.compare("server_name") == 0)
		this->setServerName(value);
	else if (name.compare("listen") == 0)
		this->parsingListen(value);
	else if (name.compare("root") == 0)
		this->parsingRoot(value);
	else if (name.compare("location") == 0)
		this->setLocation(value);
}

void	Server::parsingLocation(t_str &body, t_str::size_type &value_begin, \
							 t_str::size_type &value_end)
{
	t_str::size_type	count = 1;

	value_end = body.find_first_of("{}", value_begin);
	if (value_end == t_str::npos || body[value_end] == '}')
		throw std::runtime_error("Error: Config file: directive " \
								 "'location' has no opening '{'.");
	value_end = body.find_first_of("{}", value_end + 1);
	while (value_end != t_str::npos && count != 0)
	{
		if (body[value_end] == '{')
			++count;
		else
			--count;
		if (count != 0)
			value_end = body.find_first_of("{}", value_end + 1);
	}
	if (value_end == t_str::npos)
		throw std::runtime_error("Error: Config file: directive " \
								 "'location' has no closing '}'.");
	++value_end;
}

void	Server::setDefaults()
{
	if (_serverName.size() == 0)
		_serverName.push_back("");
}

void	Server::parsingBody(t_str &body)
{
	t_str::size_type	name_begin;
	t_str::size_type	name_end;
	t_str				name;
	t_str::size_type	value_begin;
	t_str::size_type	value_end;
	t_str				value;

	while (body != "")
	{
		name_begin = body.find_first_not_of(" \t\v\r\n\f");
		if (name_begin == t_str::npos)
			break ;
		name_end = body.find_first_of(" \t\v\r\n\f", name_begin);
		if (name_end == t_str::npos)
			throw std::logic_error("Error: Config file: after directive name " \
								   "must be at least one whitespce and " \
								   "after that must be its value.");
		name = body.substr(name_begin, name_end - name_begin);
		if (std::find(_directiveList.begin(), _directiveList.end(), name) == \
													_directiveList.end())
			throw std::logic_error("Error: Config file: directive name '" \
				   					+ name + "' unknown.");
		value_begin = body.find_first_not_of(" \t\v\r\n\f", name_end);
		if (value_begin == t_str::npos)
			throw std::logic_error("Error: Config file: directive '" + name + \
									"' doesn't have a value.");
		if (body.compare(name_begin, 8, "location") != 0)
		{
			value_end = body.find_first_of(';', value_begin);
			if (value_end == t_str::npos)
				throw std::logic_error("Error: Config file: value of "
					"directive '" + name + "' must end with ';' symbol.");
		}
		else
			parsingLocation(body, value_begin, value_end);
		value = body.substr(value_begin, value_end - value_begin);
		this->setFildes(name, value);
		if (body.compare(name_begin, 8, "location") != 0)
			body.erase(0, value_end + 1);
		else
			body.erase(0, value_end);
	}
	this->setDefaults();
}
