/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_back.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunpark <sunpark@student.42.kr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/01 15:57:38 by sunpark           #+#    #+#             */
/*   Updated: 2021/05/25 15:07:39 by hyukim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstadd_back(t_list **lst, t_list *new_node)
{
	t_list	*now;

	if (*lst == NULL)
	{
		*lst = new_node;
		return ;
	}
	now = *lst;
	while (now->next)
		now = now->next;
	now->next = new_node;
}
