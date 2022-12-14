/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vismaily <nenie_iri@mail.ru>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/29 16:42:16 by vismaily          #+#    #+#             */
/*   Updated: 2022/12/08 19:20:07 by vismaily         ###   ########.fr       */
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
	_directiveList.push_back("index");
	_directiveList.push_back("autoindex");
	_directiveList.push_back("error_page");
	_directiveList.push_back("client_max_body_size");
	_directiveList.push_back("allow_methods");
	_directiveList.push_back("location");
	_directiveList.push_back("cgi");
	_directiveList.push_back("upload_dir");
	_directiveList.push_back("return");
	this->_autoindex = false;
	this->_clientMaxBodySize = 0;
	parsingBody(tmp);
}

Server::Server(const Server &other)
{
	this->_directiveList = other._directiveList;
	this->_serverName = other._serverName;
	this->_listen = other._listen;
	this->_location = other._location;
	this->_root = other._root;
	this->_index = other._index;
	this->_autoindex = other._autoindex;
	this->_errorPage = other._errorPage;
	this->_location = other._location;
	this->_clientMaxBodySize = other._clientMaxBodySize;
	this->_allowedMethods = other._allowedMethods;
	this->_cgi = other._cgi;
	this->_uploadDir = other._uploadDir;
	this->_return = other._return;
}

Server	&Server::operator=(const Server &rhs)
{
	if (this != &rhs)
	{
		this->_directiveList = rhs._directiveList;
		this->_serverName = rhs._serverName;
		this->_listen = rhs._listen;
		this->_location = rhs._location;
		this->_root = rhs._root;
		this->_index = rhs._index;
		this->_autoindex = rhs._autoindex;
		this->_errorPage = rhs._errorPage;
		this->_clientMaxBodySize = rhs._clientMaxBodySize;
		this->_allowedMethods = rhs._allowedMethods;
		this->_cgi = rhs._cgi;
		this->_uploadDir = rhs._uploadDir;
		this->_return = rhs._return;
	}
	return (*this);
}

Server::~Server()
{
}

/*=====================================*/
/*               Getters               */
/*=====================================*/

const std::vector<std::string>	&Server::getServerName() const
{
	return (this->_serverName);
}

const std::multimap<std::string, std::string>	&Server::getListen() const
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

const std::vector<std::string>	&Server::getIndex() const
{
	return (this->_index);
}

bool	Server::getAutoindex() const
{
	return (this->_autoindex);
}

const std::map<int, std::string>	&Server::getErrorPage() const
{
	return (this->_errorPage);
}

unsigned long int	Server::getClientMaxBodySize() const
{
	return (this->_clientMaxBodySize);
}

const std::vector<std::string>	&Server::getAllowedMethods() const
{
	return (this->_allowedMethods);
}

const std::map<std::string, std::string>	&Server::getCgi() const
{
	return (this->_cgi);
}

const std::string	&Server::getUploadDir() const
{
	return (this->_uploadDir);
}

const std::string	&Server::getReturn() const
{
	return (this->_return);
}

/*=====================================*/
/*               Setters               */
/*=====================================*/

void	Server::setServerName(t_str &value)
{
	char	*token;

	token = std::strtok(&value[0], " \t\v\r\n\f");
	if (token == NULL)
		throw std::runtime_error("Error: Config file: Directive "
								 "value of server_name is empty.");
	while (token != NULL)
	{
		this->_serverName.push_back(token);
		token = std::strtok(NULL, " \t\v\r\n\f");
	}
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
	if (name[0] != '/')
		throw std::runtime_error("Error: Location must start with '/' symbol.");
	if (name[name.length() - 1] != '/')
		name += "/";
	this->_location.insert(std::make_pair(name, Location(inner)));
}

void	Server::setListen(t_str &addr, t_str &port)
{
	if (isValidIP(addr) == 0)
	{
		this->_serverName.push_back(addr);
		addr = "*";
	}
	std::multimap<t_str, t_str>::iterator	it1 = _listen.lower_bound(addr);
	std::multimap<t_str, t_str>::iterator	it2 = _listen.upper_bound(addr);
	while (it1 != it2)
	{
		if (it1->first == addr && it1->second == port)
			throw std::runtime_error("Error: duplicate listen values.");
		++it1;
	}
	this->_listen.insert(std::make_pair(addr, port));
}

void	Server::setRoot(t_str &value)
{
	std::string::size_type	pos;
	std::string				path;

	pos = value.find_last_not_of(" \t\v\r\n\f");
	if (pos == std::string::npos)
		throw std::runtime_error("Error: root value is empty.");
	++pos;
	value = value.substr(0, pos);
	if (value.find_first_of(" \t\v\r\n\f") != std::string::npos)
		throw std::runtime_error("Error: root isn't valid "
								 "(there are whitespaces).");
	if (value.compare(0, 4, "www/") != 0)
		throw std::runtime_error("Error: root must start with 'www/' path.");
	if (access(value.c_str(), F_OK) != 0)
		throw std::runtime_error("Error: root dir does not found.");
	this->_root = value;
}

void	Server::setIndex(t_str &value)
{
	char	*token;

	token = std::strtok(&value[0], " \t\v\r\n\f");
	if (token == NULL)
		throw std::runtime_error("Error: Config file: Directive "
								 "value of index is empty.");
	while (token != NULL)
	{
		this->_index.push_back(token);
		token = std::strtok(NULL, " \t\v\r\n\f");
	}
}

void	Server::setAutoindex(t_str &value)
{
	std::string::size_type	pos;

	pos = value.find_last_not_of(" \t\v\r\n\f");
	if (pos == std::string::npos)
		throw std::runtime_error("Error: autoindex value is empty.");
	++pos;
	value = value.substr(0, pos);
	if (value == "on")
		this->_autoindex = true;
	else if (value == "off")
		this->_autoindex = false;
	else
		throw std::runtime_error("Error: autoindex value can only be "
								 "'on' or 'off'.");
}

void	Server::setErrorPage(t_str &value)
{
	t_str::size_type	pos;
	char				*token;
	t_str				page;
	int					error_number;

	pos = value.find_last_not_of(" \t\v\r\n\f");
	if (pos == std::string::npos)
		throw std::runtime_error("Error: Config file: Directive "
								 "value of error_page is empty.");
	value = value.substr(0, ++pos);
	pos = value.find_last_of(" \t\v\r\n\f");
	if (pos == std::string::npos)
		throw std::runtime_error("Error: Config file: Directive value of "
								 "error_page must contain an error number"
								 "and after that an error page uri.");
	++pos;
	page = value.substr(pos, value.length() - pos);
	value = value.substr(0, pos - 1);
	token = std::strtok(&value[0], " \t\v\r\n\f");
	while (token != NULL)
	{
		error_number = std::atoi(token);
		if (!(error_number >= 300 && error_number <= 599))
			throw std::runtime_error("Error: Config file: Directive "
									 "value of error_page must be between "
									 "'300' and '599' range.");
		this->_errorPage.insert(std::make_pair(error_number, page));
		token = std::strtok(NULL, " \t\v\r\n\f");
	}
}

void	Server::setClientMaxBodySize(t_str &value)
{
	std::string::size_type	pos;
	int						coefficient;

	pos = value.find_last_not_of(" \t\v\r\n\f");
	if (pos == std::string::npos)
		throw std::runtime_error("Error: client_max_body_size value is empty.");
	++pos;
	value = value.substr(0, pos);
	if (value.back() == 'k' || value.back() == 'K')
	{
		coefficient = 1024;
		value.pop_back();
	}
	else if (value.back() == 'm' || value.back() == 'M')
	{
		coefficient = 1024 * 1024;
		value.pop_back();
	}
	else if (value.back() >= '0' && value.back() <= '9')
		coefficient = 1;
	else
		throw std::runtime_error("Error: client_max_body_size is not valid.");
	pos = value.find_last_not_of(" \t\v\r\n\f");
	if (pos == std::string::npos)
		throw std::runtime_error("Error: client_max_body_size value is empty.");
	++pos;
	value = value.substr(0, pos);
	if (value == "")
		throw std::runtime_error("Error: client_max_body_size is not valid.");
	for (std::string::iterator it = value.begin(); it != value.end(); ++it)
	{
		if (!(*it >= '0' && *it <= '9'))
			throw std::runtime_error("Error: client_max_body_size is not valid.");
	}
	this->_clientMaxBodySize = coefficient * std::atol(value.c_str());
}

void	Server::setAllowedMethods(t_str &value)
{
	char	*token;

	token = std::strtok(&value[0], " \t\v\r\n\f");
	if (token == NULL)
		throw std::runtime_error("Error: Config file: Directive "
								 "value of allow_methods is empty.");
	while (token != NULL)
	{
		if (std::strncmp(token, "GET", std::strlen(token)) != 0 && \
			std::strncmp(token, "POST", std::strlen(token)) != 0 && \
			std::strncmp(token, "DELETE", std::strlen(token)) != 0)
			throw std::runtime_error("Error: The values of allow_methods must" \
									 " be one of these 'GET' 'POST' 'DELETE'.");
		this->_allowedMethods.push_back(token);
		token = std::strtok(NULL, " \t\v\r\n\f");
	}
}

void	Server::setCgi(t_str &value)
{
	t_str::size_type	pos;
	t_str				uri;

	pos = value.find_last_not_of(" \t\v\r\n\f");
	if (pos == std::string::npos)
		throw std::runtime_error("Error: Config file: Directive "
								 "value of cgi is empty.");
	value = value.substr(0, ++pos);
	pos = value.find_last_of(" \t\v\r\n\f");
	if (pos == std::string::npos)
		throw std::runtime_error("Error: Config file: Directive value of "
								 "cgi must contain a cgi extension"
								 "and after that the cgi uri.");
	++pos;
	uri = value.substr(pos, value.length() - pos);
	value = value.substr(0, pos - 1);
	pos = value.find_last_not_of(" \t\v\r\n\f");
	value = value.substr(0, ++pos);
	if (value.find(" \t\v\r\n\f") != std::string::npos)
		throw std::runtime_error("Error: Config file: Directive "
								 "value of cgi is not valid.");
	if (value != "php")
	{
		if (BONUS == 0)
			throw std::runtime_error("Error: Config file: Directive "
									 "value of cgi support only 'php'.");
		else if (value != "py")
			throw std::runtime_error("Error: Config file: Directive "
									 "value of cgi support 'php' and/or 'py'.");
	}
	if (access(uri.c_str(), F_OK) != 0)
		throw std::runtime_error("Error: cgi file does not found.");
	this->_cgi.insert(std::make_pair(value, uri));
}

void	Server::setUploadDir(t_str &value)
{
	std::string::size_type	pos;

	pos = value.find_last_not_of(" \t\v\r\n\f");
	if (pos == std::string::npos)
		throw std::runtime_error("Error: upload_dir value is empty.");
	++pos;
	value = value.substr(0, pos);
	if (value.find_first_of(" \t\v\r\n\f") != std::string::npos)
		throw std::runtime_error("Error: upload_dir isn't valid "
								 "(there are whitespaces).");
	if (value.compare(0, 4, "www/") != 0)
		throw std::runtime_error("Error: upload_dir must start with 'www/' path.");
	if (access(value.c_str(), F_OK) != 0)
		throw std::runtime_error("Error: upload_dir value does not exist.");
	this->_uploadDir = value;
}

void	Server::setReturn(t_str &value)
{
	std::string::size_type	pos;

	pos = value.find_last_not_of(" \t\v\r\n\f");
	if (pos == std::string::npos)
		throw std::runtime_error("Error: return directive value is empty.");
	++pos;
	value = value.substr(0, pos);
	if (value.find_first_of(" \t\v\r\n\f") != std::string::npos)
		throw std::runtime_error("Error: return directive isn't valid "
								 "(there are whitespaces).");
	if (value[0] != '/' && value.compare(0, 7, "http://") != 0 && \
			value.compare(0, 8, "https://") != 0)
		throw std::runtime_error("Error: return directive must start with " \
								"one of this 'http://', 'https://', '/'.");
	this->_return = value;
}

void	Server::setFildes(const t_str &name, t_str &value)
{
	if (name.compare("server_name") == 0)
		this->setServerName(value);
	else if (name.compare("listen") == 0)
		this->parsingListen(value);
	else if (name.compare("root") == 0)
		this->setRoot(value);
	else if (name.compare("location") == 0)
		this->setLocation(value);
	else if (name.compare("index") == 0)
		this->setIndex(value);
	else if (name.compare("autoindex") == 0)
		this->setAutoindex(value);
	else if (name.compare("error_page") == 0)
		this->setErrorPage(value);
	else if (name.compare("client_max_body_size") == 0)
		this->setClientMaxBodySize(value);
	else if (name.compare("allow_methods") == 0)
		this->setAllowedMethods(value);
	else if (name.compare("cgi") == 0)
		this->setCgi(value);
	else if (name.compare("upload_dir") == 0)
		this->setUploadDir(value);
	else if (name.compare("return") == 0)
		this->setReturn(value);
}

void	Server::setDefaults()
{
	std::string::size_type	pos;
	std::string				path;
	std::string				value("www/");
	char					*isPathOk;

	if (this->_root == "")
	{
		isPathOk = std::getenv("_");
		if (isPathOk == nullptr)
			throw std::runtime_error("Error: env variable '$_' does not found.");
		path = static_cast<std::string>(isPathOk);
		pos = path.find_last_of("/") + 1;
		if (pos == std::string::npos)
			throw std::runtime_error("Error: not found '/' in env variable '$_'");
		path = path.substr(0, pos);
		value = path + value;
		if (access(value.c_str(), F_OK) != 0)
			throw std::runtime_error("Error: root dir does not found.");
		this->_root = value;
	}
	if (this->_allowedMethods.size() == 0)
	{
		this->_allowedMethods.push_back("GET");
		this->_allowedMethods.push_back("POST");
		this->_allowedMethods.push_back("DELETE");
	}
}

/*=====================================*/
/*       Other Member Functions        */
/*=====================================*/

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
	std::map<t_str, Location>::iterator	it;
	for (it = this->_location.begin(); it != this->_location.end(); ++it)
		it->second.inherit(this->_root, \
							this->_index, \
							this->_autoindex, \
							this->_clientMaxBodySize, \
							this->_allowedMethods, \
							this->_cgi, \
							this->_uploadDir, \
							it->first);
}
