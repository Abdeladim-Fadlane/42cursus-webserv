/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configfile.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akatfi <akatfi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 11:14:04 by akatfi            #+#    #+#             */
/*   Updated: 2024/01/21 12:31:49 by akatfi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_FILE_HPP
#define CONFIG_FILE_HPP
#include <fstream>

class ConfigFile
{
    private:
        std::fstream config;
    public:
        ConfiigFile(const std::string& FileName);
        void    parceConfig();
};

#endif