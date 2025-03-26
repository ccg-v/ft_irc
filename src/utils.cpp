/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erosas-c <erosas-c@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 20:22:33 by ccarrace          #+#    #+#             */
/*   Updated: 2025/03/26 20:17:08 by erosas-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"
#include <algorithm>	// std::transform
#include <iostream>

std::string getCurrentDate() // [1]
{
    std::time_t now;
    std::tm *ltm;
    char buffer[80];

	now = std::time(0);
	ltm = std::localtime(&now);

	std::strftime(buffer, sizeof(buffer), "%A, %b %d %Y, %H:%M:%S", ltm);
    return (std::string(buffer));
}

std::vector<std::string>	splitByComma(const std::string &str)
{
    std::vector<std::string> result;
    std::istringstream iss(str);
    std::string token;

    while (std::getline(iss, token, ','))   // Extract until ','
	{  
        result.push_back(token);
    }
    return result;
}

bool	validKey(const std::string &key, size_t min)
{
	std::string allowedChars = "$%&'()*+-.@"; // Allowed special chars
	
	if (key.length() > 32 || key.length() < min)
		return false ;
	for (size_t i = 0; i < key.length(); ++i)
	{
        char c = key[i];
        if (!std::isalnum(c) && allowedChars.find(c) == std::string::npos)
            return false;  // Invalid character found
    }
    return true;
}

std::string timeToString(time_t timestamp)
{
    std::stringstream ss;
    ss << timestamp;
    return ss.str();
}

std::string toLowerStr(const std::string& str)
{
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), static_cast<int(*)(int)>(std::tolower));
    return lowerStr;
}

/*  [1]
 *	getCurrentDate()
 *
 *	std::localtime() converts a time_t value (which represents seconds since the 
 *	(Unix epoch: January 1, 1970, 00:00:00 UTC) into a tm structure representing 
 *	local time.
 *	After conversion, `ltm` will contain broken-down time fields:
 *
 *		tm_year	|	Years since 1900					|	124 (for 2024)
 *		-------	| -------------------------------------	| -------------------
 *		tm_mon	|	Month (0 = January, 11 = December)	|	1 (for February)
 *		tm_mday	|	Day of the month (1-31)				|	21
 *		tm_hour	|	Hours (0-23)						|	16
 *		tm_min	|	Minutes (0-59)						|	35
 *		tm_sec	|	Seconds (0-59)						|	42
 *
 *	The `strftime()` function writes the formatted time string into a character 
 *	buffer. The size 80 is a convention used to ensure the buffer is large enough 
 *	to hold most common date-time formats.
 *	See `man strftime()` for formatting parameters.
 */