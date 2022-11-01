/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vismaily <nenie_iri@mail.ru>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/29 16:42:16 by vismaily          #+#    #+#             */
/*   Updated: 2022/11/01 18:22:42 by vismaily         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/*=====================================*/
/*     Constructors and Destructor     */
/*=====================================*/

Server::Server()
{
}

Server::Server(str_t &body)
{
	str_t	tmp;

	tmp = body.substr(1, body.length() - 2);
	if (tmp == "" || tmp.find_first_not_of(" \t\v\r\n\f") == str_t::npos)
		throw std::logic_error("Error: Config file: directive 'server' is empty.");
	_directive_list.push_back("server_name");
	_directive_list.push_back("listen");
	parsingBody(tmp);
}

Server::Server(const Server &other)
{
	this->_serverName = other._serverName;
}

Server	&Server::operator=(const Server &rhs)
{
	if (this != &rhs)
	{
		this->_serverName = rhs._serverName;
	}
	return (*this);
}

Server::~Server()
{
}

/*=====================================*/
/*          Setter and Getters         */
/*=====================================*/

void	Server::setServerName(str_t &value)
{
	char	*token;

	token = std::strtok(&value[0], " \t\v\r\n\f");
	while (token != NULL)
	{
		this->_serverName.push_back(token);
		token = std::strtok(NULL, " \t\v\r\n\f");
	}
}

const std::vector<std::string>	&Server::getServerName() const
{
	return (this->_serverName);
}

/*=====================================*/
/*       Other Member Functions        */
/*=====================================*/

void	Server::setFildes(const str_t &name, str_t &value)
{
	if (name.compare("server_name") == 0)
		this->setServerName(value);
//	else if (name.compare("listen") == 0)
//		setListen(value);
}

void	Server::parsingValue(str_t &body, str_t::size_type value_begin, \
							 str_t::size_type value_end)
{
	str_t::size_type	count = 1;

	value_end = body.find_first_of("{}", value_begin + 1);
	if (value_end == str_t::npos || body[value_end] != '{')
		throw std::runtime_error("Error: Config file: directive " \
								 "'location' has no opening '{'.");
	value_end = body.find_first_of("{}", value_end + 1);
	while (value_end != str_t::npos && count != 0)
	{
		if (body[value_end] == '{')
			++count;
		else
			--count;
		if (count != 0)
			value_end = body.find_first_of("{}", value_end + 1);
	}
	if (value_end == str_t::npos)
		throw std::runtime_error("Error: Config file: directive " \
								 "'location' has no closing '}'.");
	++value_end;
}

void	Server::parsingBody(str_t &body)
{
	str_t::size_type	name_begin;
	str_t::size_type	name_end;
	str_t				name;
	str_t::size_type	value_begin;
	str_t::size_type	value_end;
	str_t				value;

	while (body != "")
	{
		name_begin = body.find_first_not_of(" \t\v\r\n\f");
		if (name_begin == str_t::npos)
			break ;
		name_end = body.find_first_of(" \t\v\r\n\f", name_begin);
		if (name_end == str_t::npos)
			throw std::logic_error("Error: Config file: after directive name " \
								   "must be at least one whitespce and " \
								   "after that must be its value.");
		name = body.substr(name_begin, name_end - name_begin);
		value_begin = body.find_first_not_of(" \t\v\r\n\f", name_end);
		if (value_begin == str_t::npos)
			throw std::logic_error("Error: Config file: directive '" + name + \
									"' doesn't have a value.");
		if (std::find(_directive_list.begin(), _directive_list.end(), name) == \
													_directive_list.end())
			throw std::logic_error("Error: Config file: directive name '" \
				   					+ name + "' unknown.");
		if (body.compare(name_begin, 8, "location") != 0)
		{
			value_end = body.find_first_of(';', value_begin);
			if (value_end == str_t::npos)
				throw std::logic_error("Error: Config file: value of "
					"directive '" + name + "' must end with ';' symbol.");
		}
		else
			parsingValue(body, value_begin, value_end);
		value = body.substr(value_begin, value_end - value_begin);
		this->setFildes(name, value);
		body.erase(0, value_end + 1);
	}
}
