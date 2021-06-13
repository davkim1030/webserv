/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_get_time.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyukim <hyukim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/10 16:03:52 by hyukim            #+#    #+#             */
/*   Updated: 2021/06/10 16:05:51 by hyukim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <sys/time.h>

unsigned long   ft_get_time(void)
{
    struct timeval  tv;

    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}
