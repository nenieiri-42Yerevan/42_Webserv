/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vismaily <nenie_iri@mail.ru>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/09 12:21:31 by vismaily          #+#    #+#             */
/*   Updated: 2022/11/09 16:22:25 by vismaily         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <map>
# include <vector>
# include <string>
# include <algorithm>

# include <iostream>

class	Location
{
	private:
		typedef std::string				t_str;

	public:
		Location(t_str &body);
		Location(const Location &other);
		Location	&operator=(const Location &rhs);
		~Location();
	private:
		Location();

	private:
		void							setRoot(const t_str &root);
		void							setLocation(t_str &value);
		void							setFildes(const t_str &name, t_str &value);
	private:
		void							parsingRoot(t_str &value);
		void							parsingLocation(t_str &body, \
											t_str::size_type &value_begin, \
											t_str::size_type &value_end);
		void							parsingBody(t_str &body);
	private:
		std::vector<t_str>				_directiveList;
		std::string						_root;
		std::map<t_str, Location>		_location;
};

#endif