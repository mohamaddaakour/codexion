/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_helper.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdaakour <mdaakour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 09:52:32 by mdaakour          #+#    #+#             */
/*   Updated: 2026/06/29 09:54:52 by mdaakour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

void	heap_remove(t_heap *heap, int coder_id, int scheduler)
{
	int	i;

	i = 0;
	while (i < heap->size)
	{
		if (heap->nodes[i].coder_id == coder_id)
			break ;
		i++;
	}
	if (i == heap->size)
		return ;
	heap->nodes[i] = heap->nodes[heap->size - 1];
	heap->size--;
	rebuild_heap(heap, scheduler);
}

int	compare_req(t_request a, t_request b, int scheduler)
{
	if (scheduler == EDF)
	{
		if (a.deadline < b.deadline)
			return (-1);
		if (a.deadline > b.deadline)
			return (1);
	}
	if (a.sequence < b.sequence)
		return (-1);
	if (a.sequence > b.sequence)
		return (1);
	return (0);
}

int	init_heap(t_heap *heap, int capacity)
{
	heap->nodes = malloc(sizeof(t_request) * capacity);
	if (!heap->nodes)
		return (1);
	heap->size = 0;
	heap->capacity = capacity;
	return (0);
}

void	heap_push(t_heap *heap, t_request req, int scheduler)
{
	int	i;
	int	parent;

	if (heap->size >= heap->capacity)
		return ;
	i = heap->size++;
	heap->nodes[i] = req;
	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (compare_req(heap->nodes[i], heap->nodes[parent], scheduler) < 0)
		{
			swap_req(&heap->nodes[i], &heap->nodes[parent]);
			i = parent;
		}
		else
			break ;
	}
}
