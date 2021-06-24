/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ftToupperStr.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyukim <hyukim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/25 03:48:14 by hyukim            #+#    #+#             */
/*   Updated: 2021/06/25 04:12:14 by hyukim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

std::string     ftToupperStr(const std::string &str)
{
    std::string ret(str);
    size_t      i;

    i = -1;
    while (++i < str.length())
        ret[i] = ft_toupper(str[i]);
    return (ret);
}
