/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 20:22:33 by ccarrace          #+#    #+#             */
/*   Updated: 2025/02/24 16:58:34 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ctime>
#include <string>
#include "utils.hpp"

/* 
 *	getCurrentDate()
 *
 *	std::localtime() converts a time_t value (which represents seconds since the 
 *	(Unix epoch: January 1, 1970, 00:00:00 UTC) into a tm structure representing 
 *	local time.
 *	After this, `ltm` will contain broken-down time fields:
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
std::string getCurrentDate()
{
    std::time_t now;
    std::tm *ltm;
    char buffer[80];

	now = std::time(0);
	ltm = std::localtime(&now);

    // std::strftime(buffer, sizeof(buffer), "%a %b %d %H:%M:%S %Y", ltm);
	// std::strftime(buffer, sizeof(buffer), "%c", ltm);
	std::strftime(buffer, sizeof(buffer), "%A, %b %d %Y, %H:%M:%S", ltm);
    return (std::string(buffer));
}
