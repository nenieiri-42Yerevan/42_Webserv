/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vismaily <nenie_iri@mail.ru>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/06 16:38:11 by vismaily          #+#    #+#             */
/*   Updated: 2022/12/05 13:54:56 by vismaily         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# define CONNECTION_CLOSE_SECONDS	10

# include <iostream>
# include <string>
# include <cstring>
# include <cstdlib>
# include <fstream>
# include <sstream>
# include <map>
# include <vector>
# include <algorithm>
# include <unistd.h>
# include <utility>
# include <sys/types.h>
# include <sys/stat.h>
# include <dirent.h>
# include <time.h>

# include "Server.hpp"
# include "Location.hpp"

class	Cgi;

class	Client
{
	private:
		typedef std::string				t_str;
	public:
		Client();
		Client(std::vector<Server> &serverSet, int serverNumber);
		Client(const Client &other);
		Client	&operator=(const Client &rhs);
		~Client();

	public:
		void							setRequest(const std::string &request);
		void							setResponse(const std::string &response);
		bool							getRecvStatus() const;
		bool							getSendStatus() const;
		bool							getCloseStatus();
		const std::string				getResponse();
		const std::string				getResponse(size_t	buff_size);
		const std::string				&getFile() const;
		const std::map<t_str, t_str>	getHeader() const;
		const std::pair<t_str, t_str>	getCgi() const;
		const std::string				&getUploadDir() const;
		const std::string				&getBody() const;
		const std::string				&getServerPort() const;
	private:
		void							parsing();
		int								parsingRequestLine(std::string line);
		void							parsingHeader(std::string line);
		int								receiveInfo();
		int								findServer();
		void							findLocation();
		bool							findReturn();
		bool							findFile(t_str &full_path, \
											t_str::size_type pos);
		void							findLength();
		void							findCgi();
		bool							directListening(t_str &full_path, \
														t_str &rel_path);
		bool							isAllowedMethods();
		void							parsingBody();
		void							readBody();
		void							prepareAnswer();
		bool							readWhole(const std::string &full_path, \
											std::string &readFile) const;
		int								getError(int num);
		void							getErrorMsg(int errNum, \
											const t_str &num, const t_str &msg);
		bool							responseErrorPage(int errNum, \
											std::string &response_body) const;
		void							resetHeader();
	private:
		std::string						_request;
		std::string						_response;
		std::map<t_str, t_str>			_header;
		std::string						_body;
		std::string						_chunkedBody;
		int								_chunkedLen;
		bool							_isRecvFinish;
		bool							_isSendFinish;
		bool							_isErrored;
		std::string						_version;
		int								_isStart;
		int								_isHeader;
		std::string						_lastHeader;
		bool							_isServerFound;
		std::vector<Server>				_serverSet;
		Server							_server;
		std::pair<t_str, Location>		_location;
		bool							_isLocation;
		std::string						_port;
		std::string						_host;
		std::string						_file;
		std::vector<t_str>				_supportedMethods;
		std::string						_errorAllowed;
		std::string						_bodyType;
		unsigned long int				_contentLength;
		bool							_isCgi;
		std::pair<t_str, t_str>			_Cgi;
		std::string						_uploadDir;
		bool							_isClosed;
		time_t							_closeTime;
		std::string						_redirectHeader;
};

#endif
