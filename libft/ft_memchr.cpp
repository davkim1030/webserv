/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunpark <sunpark@student.42.kr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/26 19:56:06 by sunpark           #+#    #+#             */
/*   Updated: 2021/05/27 15:14:17 by hyukim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	size_t			locate;
	unsigned char	*find;

	locate = -1;
	find = (unsigned char *)s;
	while (++locate < n)
	{
		if (*find == (unsigned char)c)
			break ;
		find++;
	}
	if (locate == n)
		return (NULL);
	return ((void *)find);
}
