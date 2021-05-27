/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memccpy.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunpark <sunpark@student.42.kr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/26 13:31:19 by sunpark           #+#    #+#             */
/*   Updated: 2021/05/27 15:12:27 by hyukim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memccpy(void *dst, const void *src, int c, size_t n)
{
	size_t			locate;
	unsigned char	*s;
	unsigned char	*d;

	if (!dst && !src)
		return (dst);
	locate = 0;
	s = (unsigned char *)src;
	while (locate < n)
	{
		((unsigned char *)dst)[locate] = s[locate];
		if (((unsigned char *)dst)[locate] == (unsigned char)c)
			break ;
		locate++;
	}
	if (locate == n)
		return ((void *)0);
	d = (unsigned char *)dst;
	return (d + locate + 1);
}
