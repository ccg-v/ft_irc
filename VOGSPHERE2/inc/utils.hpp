/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erosas-c <erosas-c@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 20:29:36 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/25 22:03:55 by erosas-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

#include <vector>
#include <ctime>
#include <string>
#include <sstream>

std::string					getCurrentDate();
std::vector<std::string>	splitByComma(const std::string &str);
bool						validKey(const std::string &key, size_t min);
std::string 				timeToString(time_t timestamp);
std::string					toLowerStr(const std::string& str);

#endif
