/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akatfi <akatfi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 11:13:54 by akatfi            #+#    #+#             */
/*   Updated: 2024/01/21 12:31:33 by akatfi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigFile.hpp"

ConfigFile::ConfigFile(const std::string& FileName)
{
    config.open(FileName.c_str());
    if (config.is_open())
        throw ("120");
}

void    ConfigFile::parceConfig()
{
    
}