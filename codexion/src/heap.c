#include "../include/codexion.h"

int init_heap(t_heap *heap, int capacity) {
	// we create the array of requests
	heap->nodes = (t_request *)malloc(sizeof(t_request) * capacity);

	if (!heap->nodes) {
		return 1;
	}

	heap->size = 0;
	heap->capacity = capacity;

	return 0;
}

// return -1 means request a should take the dongle first
int compare_req(t_request a, t_request b, int scheduler) {
	if (scheduler == EDF) {
		if (a.deadline < b.deadline)
			return -1;
		if (a.deadline > b.deadline)
			return 1;
	}

	if (a.sequence < b.sequence)
		return -1;
	if (a.sequence > b.sequence)
		return 1;

	return 0;
}

void heap_push(t_heap *heap, t_request req, int scheduler) {
	// if the number of requests in the heap is more than the maximum capacity exit the function
	if (heap->size >= heap->capacity)
		return;

	int i = heap->size;
	heap->nodes[i] = req;
	heap->size++;

	// here we are using a heap and it is a binary tree but we are arranged it in a regular array
	// the node having the lowest value should be at the beggining of the array
	// to get the parent in the heap we use this formula (i - 1) / 2
	while (i > 0)
    {
        int parent = (i - 1) / 2;

		// swap
        if (compare_req(heap->nodes[i], heap->nodes[parent], scheduler) < 0)
        {
            t_request tmp = heap->nodes[i];
            heap->nodes[i] = heap->nodes[parent];
            heap->nodes[parent] = tmp;
            i = parent;
        }
        else
			break;
    }
}

// to remove the last node in the array
void heap_pop(t_heap *heap, int scheduler) {
	// if the heap is empty exit the function
    if (heap->size <= 0)
		return;

    heap->size--;
    heap->nodes[0] = heap->nodes[heap->size];

    int i = 0;
    while (2 * i + 1 < heap->size)
    {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smallest = left;

        if (right < heap->size && compare_req(heap->nodes[right], heap->nodes[left], scheduler) < 0)
            smallest = right;

        if (compare_req(heap->nodes[smallest], heap->nodes[i], scheduler) < 0)
        {
            t_request tmp = heap->nodes[i];
            heap->nodes[i] = heap->nodes[smallest];
            heap->nodes[smallest] = tmp;
            i = smallest;
        }

        else
			break;
    }
}

// will return the node with the lowest priority
t_request heap_peek(t_heap *heap) {
    if (heap->size > 0)
		return heap->nodes[0];

    t_request null_req = {0, 0, 0};

    return null_req;
}

// to remove a specific node
void heap_remove(t_heap *heap, int coder_id, int scheduler) {
    int i = 0;
    while (i < heap->size && heap->nodes[i].coder_id != coder_id)
		i++;

    if (i == heap->size)
		return;

    heap->nodes[i] = heap->nodes[heap->size - 1];
    heap->size--;

    t_heap tmp_heap;
    init_heap(&tmp_heap, heap->capacity);

    for (int j = 0; j < heap->size; j++)
        heap_push(&tmp_heap, heap->nodes[j], scheduler);

    free(heap->nodes);
    *heap = tmp_heap;
}
