/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 20:29:36 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/21 22:48:37 by ccarrace         ###   ########.fr       */
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
bool						validKey(const std::string &key);
std::string 				timeToString(time_t timestamp);
// bool						isOnlySpaces(const std::string &str);

#endif
