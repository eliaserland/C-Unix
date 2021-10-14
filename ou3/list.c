/*
 * Implementation of a generic undirected list. Utilized for the implementation
 * of OU3 for C programming and Unix, Umea University Autumn 2021.
 *
 * Heavily inspired of the undirected list implementations provided in the 
 * "Datastructures and algorithms" course at the Department of Computing
 * Science, Umea University. Some modifications, additional methods and error
 * checking has been added compared to the original work of previous authors.
 *
 * Author: Elias Olofsson (tfy17eon@cs.umu.se)
 *
 * Based on earlier code by:
 *         Niclas Borlin (niclas@cs.umu.se)
 *         Adam Dahlgren Lindstrom (dali@cs.umu.se)
 *         Lars Karlsson (larsk@cs.umu.se)
 *
 * Version information:
 *   2021-10-15: v1.0, first public version.
 */

#include <stdio.h>
#include <stdlib.h>
#include "list.h"

// ============== Internal data types ==============
struct cell {
	struct cell *next; // Pointer to next list element.
	struct cell *prev; // Pointer to previous list element.
	void *value;       // Pointer to the value of the list element.
};

struct list {
	struct cell *top;        // Pointer to list top.
	struct cell *bottom;     // Pointer to list bottom.
	int length;              // Length of list.
	free_function free_func; // Function to deallocate list values.
};

// ======= Internal function implementations =======
/**
 * list_empty() - Create an empty list.
 * @free_func: Pointer to a function (or NULL) to be called to deallocate
 *             memory on kill.
 *
 * Returns: A pointer to the new list, or NULL on error.
 */
list *list_empty(free_function free_func)
{
	// Allocate memory for the list structure.
	list *l = calloc(1, sizeof(list));
	if (l == NULL) {
		perror("calloc");
		return NULL;
	}
	l->top = NULL;
	l->bottom = NULL;

	do {
		// Allocate memory for the list top.
		l->top = calloc(1, sizeof(struct cell));
		if (l->top == NULL) {
			perror("calloc");
			
			return NULL;
		}
		// Allocate memory for the list bottom.
		l->bottom = calloc(1, sizeof(struct cell));
		if (l->bottom == NULL) {
			
		}	
		// Set consistent links between border elements.
		l->top->next = l->bottom;
		l->bottom->prev = l->top;
		
		// Set the length of the list.
		l->length = 0;
		
		// Store the free function.
		l->free_func = free_func;
		 
		return l;

	} while(0);
	
	// Error handling.
	perror("calloc");
	free(l->top);
	free(l->bottom);
	free(l);
	return NULL;
	
}

/**
 * list_is_empty() - Check if a list is empty.
 * @l: List to check.
 *
 * Returns: True if the list is empty, otherwise false.
 */
bool list_is_empty(const list *l)
{
	return l->top->next == l->bottom;
}

/**
 * list_length() - Get
{
	for (int i = 0; i < data->num_jobs; i+
 * Returns: The number of elements contained in the list.
 */
int list_length(list *l)
{
	return l->length;
}

/**
 * list_first() - Return the first position of a list, i.e. the position of the
 *                first element in the list.
 * @l: List to inspect.list_is_end
 *
 * Returns: The first position in the given list.
 */
list_pos list_first(const list *l)
{
	return l->top->next;
}

/**
 * list_end() - Return the last position of a list, i.e. the position after the
 *               last element in the list.
 * @l: List to inspect.
 *
 * Returns: The last position in the given list.
 */
list_pos list_end(const list *l)
{
	return l->bottom;
}

/**
 * list_next() - Return the next position in a list.
 * @l: List to inspect.
 * @p: Any valid position except the last in the list.
 *
 * Returns: The position in the list after the given position.
 *          Returns NULL on invalid action, i.e. when p is the last position.
 */
list_pos list_next(const list *l, const list_pos p)
{
	if (p == list_end(l)) {
		fprintf(stderr, "list_next: Warning: Trying to navigate past end of list.\n");
		return NULL;
	}
	return p->next;
}

/**
 * list_previous() - Return the previous position in a list.
 * @l: List to inspect.
 * @p: Any valid position except the first in the list.
 *
 * Returns: The position in the list before the given position.
 *          Returns NULL on invalid action, i.e. when p is the first position.
 */
list_pos list_previous(const list *l, const list_pos p)
{
	if (p == list_first(l)) {
		fprintf(stderr, "list_previous: Warning: Trying to navigate past beginning of list.\n");
		return NULL;
	}
	return p->prev;
}

/**
 * list_index() - Get the position of an element with a certain index in a list.
 * @l:   List to inspect.
 * @idx: The index of the list element we are searching for.
 *
 * Returns: Pointer to the element, or NULL on error.
 */
list_pos list_index(const list *l, int idx)
{
	if (list_is_empty(l)) {
		fprintf(stderr, "list_index: Warning: List is empty.\n");
		return NULL;
	}
	list_pos p = list_first(l);
	for (int i = 0; i < idx; i++) {
		p = list_next(l, p);
		if (p == NULL) {
			return NULL;
		}
	}
	return p;
}

/**
 * list_inspect() - Get the value of the element at a given position in a list.
 * @l: List to inspect.
 * @p: Any valid position in the list, except the last.
 *
 * Returns: The value at the given position as a void pointer. NOTE: Returns
            NULL if the given position is the last position of the list.
 */
void *list_inspect(const list *l, const list_pos p)
{
	if (p == list_end(l)) {
		fprintf(stderr,"list_inspect: Warning: Trying to inspect position at end of list.\n");
		return NULL;
	}
	return p->value;
}

/**
 * list_insert() - Insert a new element with a given value into a list.
 * @l: List to manipulate.
 * @v: Value (pointer) to be inserted into the list.
 * @p: Position in the list before which the value should be inserted.
 *
 * Creates a new element and inserts it into the list before p.
 * Stores data in the new element.
 *
 * Returns: The position of the newly created element.
 */
list_pos list_insert(list *l, void *v, const list_pos p)
{
	// Create new element.
	list_pos new_pos = calloc(1, sizeof(struct cell));
	if (new_pos == NULL) {
		perror("calloc");
		return NULL;
	}

	// Set value.
	new_pos->value = v;

	// Set links.
	new_pos->next = p;
	new_pos->prev = p->prev;
	p->prev = new_pos;
	new_pos->prev->next = new_pos;
	
	// Update length.
	l->length++;
	
	// Return the position of the new cell.
	return p;
}

/**
 * list_append() - Insert a new element at the end of the list.
 * @l: List to manipulate.
 * @v: Pointer to the value to be inserted into the list.
 *
 * Returns: 0 if the element could successfully be appended into the list.
 */
int list_append(list *l, void *v)
{
	if (list_insert(l, v, list_end(l)) == NULL) {
		return 1;
	}
	return 0;
}

/**
 * list_pop() - Inspect and remove an element at the beginning of the list. Can 
 *              only be used if the freeing function is not set (i.e. set to 
 *              NULL), otherwise memory allocated for the element value will be 
 *              returned when the list element is removed.  
 * @l: List to manipulate.
 *
 * Returns: Pointer to the value of the first element, or NULL if list is empty.  
 */
void *list_pop(list *l) 
{
	if (list_is_empty(l)) {
		return NULL;
	} else {
		void *v = list_inspect(l, list_first(l));
		list_remove(l, list_first(l));
		return v;
	}
}

/**
 * list_remove() - Remove an element from a list.
 * @l: List to manipulate.
 * @p: Position in the list of the element to remove.
 *
 * Removes the element at position p from the list. If a free_func
 * was registered at list creation, calls it to deallocate the memory
 * held by the element value.
 *
 * Returns: The position after the removed element.
 */
list_pos list_remove(list *l, const list_pos p)
{
	// Remember return position.
	list_pos next_pos = p->next;
	
	// Link past this element.
	p->prev->next = p->next;
	p->next->prev = p->prev;

	// Call free_func if registered.
	if (l->free_func != NULL) {
		// Free any user-allocated memory for the value.
		l->free_func(p->value);
	}
	// Free the memory allocated to the cell itself.
	free(p);

	// Update list length;
	l->length--;
	
	// Return the position of the next element.
	return next_pos;
}

/**
 * list_kill() - Destroy a given list.
 * @l: List to destroy.
 *
 * Return all dynamic memory used by the list and its elements. If a
 * free_func was registered at list creation, also calls it for each
 * element to free any user-allocated memory occupied by the element values.
 *
 * Returns: Nothing.
 */
void list_kill(list *l)
{
	if (l == NULL) {
		return;
	}
	// Use public functions to traverse the list.
	// Start with the first element (will be defined even for an empty list).
	list_pos p = list_first(l);

	// Remove first element until list is empty.
	while (!list_is_empty(l)) {
		p = list_remove(l, p);
	}

	// Free the head and the list itself.
	free(l->top);
	free(l->bottom);
	free(l);
}
