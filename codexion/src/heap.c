/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdaakour <mdaakour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 09:33:34 by mdaakour          #+#    #+#             */
/*   Updated: 2026/06/29 15:04:43 by mdaakour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

void	swap_req(t_request *a, t_request *b)
{
	t_request	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

void	heapify_down(t_heap *heap, int scheduler)
{
	int	i;
	int	left;
	int	right;
	int	smallest;

	i = 0;
	while (2 * i + 1 < heap->size)
	{
		left = 2 * i + 1;
		right = 2 * i + 2;
		smallest = left;
		if (right < heap->size
			&& compare_req(heap->nodes[right],
				heap->nodes[left], scheduler) < 0)
			smallest = right;
		if (compare_req(heap->nodes[smallest],
				heap->nodes[i], scheduler) < 0)
		{
			swap_req(&heap->nodes[i], &heap->nodes[smallest]);
			i = smallest;
		}
		else
			break ;
	}
}

void	heap_pop(t_heap *heap, int scheduler)
{
	if (heap->size <= 0)
		return ;
	heap->size--;
	heap->nodes[0] = heap->nodes[heap->size];
	heapify_down(heap, scheduler);
}

t_request	heap_peek(t_heap *heap)
{
	t_request	req;

	req.coder_id = 0;
	req.sequence = 0;
	req.deadline = 0;
	if (heap->size > 0)
		req = heap->nodes[0];
	return (req);
}

void	rebuild_heap(t_heap *heap, int scheduler)
{
	t_heap	tmp;
	int		i;

	if (init_heap(&tmp, heap->capacity))
		return ;
	i = 0;
	while (i < heap->size)
	{
		heap_push(&tmp, heap->nodes[i], scheduler);
		i++;
	}
	free(heap->nodes);
	heap->nodes = tmp.nodes;
	heap->size = tmp.size;
}
