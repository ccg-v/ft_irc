/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccarrace <ccarrace@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 20:22:33 by ccarrace          #+#    #+#             */
/*   Updated: 2025/02/23 20:33:36 by ccarrace         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ctime>
#include <string>
#include "utils.hpp"

std::string getCurrentDate()
{
    std::time_t now;
    std::tm *ltm;
    char buffer[80];

	now = std::time(0);
	ltm = std::localtime(&now);

    std::strftime(buffer, sizeof(buffer), "%a %b %d %H:%M:%S %Y", ltm);
    return (std::string(buffer));
}
