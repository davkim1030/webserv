/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ftLog.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyukim <hyukim@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/23 05:58:38 by hyukim            #+#    #+#             */
/*   Updated: 2021/06/23 06:07:35 by hyukim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

const int WIDTH = 80;

void	ftLog(int value)
{
	char 	*tmp = ft_itoa(value);
	ftLog(tmp);
	free(tmp);
}

void	ftLog(std::string key, int value)
{
	char 	*tmp = ft_itoa(value);
	ftLog(key, tmp);
	free(tmp);
}

void	ftLog(std::string key, std::string value)
{
	std::string	pKey;
	int			keyLength = static_cast<int>(key.length());

	if (keyLength < WIDTH)
	{
		if (keyLength % 2 == 0)
		{
			for (int i = 0; i < (WIDTH - keyLength) / 2; i++)
				pKey += "-";
			pKey += key;
			for (int i = 0; i < (WIDTH - keyLength) / 2; i++)
				pKey += "-";
		}
		else
		{
			for (int i = 0; i < (WIDTH - keyLength) / 2; i++)
				pKey += "-";
			pKey += key;
			for (int i = 0; i < (WIDTH - keyLength) / 2 + 1; i++)
				pKey += "-";
		}
	}
	std::cout << pKey << std::endl;
	std::cout << value << std::endl;
	std::cout << pKey << std::endl;
}

void	ftLog(std::string value)
{
	std::string	pKey;
	int			keyLength = static_cast<int>(value.length());

	if (keyLength < WIDTH)
	{
		if (keyLength % 2 == 0)
		{
			for (int i = 0; i < (WIDTH - keyLength) / 2; i++)
				pKey += ">";
			pKey += value;
			for (int i = 0; i < (WIDTH - keyLength) / 2; i++)
				pKey += "<";
		}
		else
		{
			for (int i = 0; i < (WIDTH - keyLength) / 2; i++)
				pKey += ">";
			pKey += value;
			for (int i = 0; i < (WIDTH - keyLength) / 2 + 1; i++)
				pKey += "<";
		}
	}
	std::cout << pKey << std::endl;
}
