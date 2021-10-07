#include <stdio.h>
#include <stdlib.h>
#include "list.h"


// ======= Internal data types =======

struct cell {
	struct cell *next; 	// Pointer to next list element.
	void *value;        // Pointer to the value of the list element.
};

struct list {
	struct cell *head;  		// Pointer to list head.
	struct cell *last;			// Pointer to last element of the list.
	int length;            		// Length of list.
	free_function free_func;	// Function to deallocate list values.
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
	
	// Allocate memory for the list head.
	l->head = calloc(1, sizeof(struct cell));
	if (l->head == NULL) {
		perror("calloc");
		free(l);
		return NULL;
	}
	l->head->next  = NULL;    // No elements in the list so far.
	l->last = l->head;        // Set pointer to the last element.
	l->length = 0;            // Set the length of the list.
	l->free_func = free_func; // Store the free function.
	return l;
}

/**
 * list_is_empty() - Check if a list is empty.
 * @l: List to check.
 *
 * Returns: True if the list is empty, otherwise false.
 */
bool list_is_empty(const list *l)
{
	return l->head->next == NULL;
}

/**
 * list_length() - Get the current length of a list.
 * @l: List to inspect.
 *
 * Returns: The number of elements contained in the list.
 */
int list_length(list *l) 
{
	return l->length;
}
	
/**
 * list_is_end() - Check if a given position is at the end of a list.
 * @l: List to inspect.
 * @p: Any valid position in the list.
 *
 * Returns: True if p is at the end of the list.
 */
bool list_is_end(const list *l, const list_pos p)
{
	return p->next == NULL;
}

/**
 * list_first() - Return the first position of a list, i.e. the position of the 
 *                first element in the list.
 * @l: List to inspect.
 *
 * Returns: The first position in the given list.
 */
list_pos list_first(const list *l)
{
	return l->head;
}

/**
 * list_last() - Return the last position of a list, i.e. the position of the 
 *               last element in the list.
 * @l: List to inspect.
 *
 * Returns: The last position in the given list.
 */
list_pos list_last(const list *l)
{
	return l->last;
}

/**
 * list_next() - Return the next position in a list.
 * @l: List to inspect.
 * @p: Any valid position except the last in the list.
 *
 * Returns: The position in the list after the given position.
 *          Returns NULL if the given position is at the end of the list.
 */
list_pos list_next(const list *l, const list_pos p)
{
	if (list_is_end(l, p)) {
		fprintf(stderr, "list_next: Warning: Trying to navigate past end of list.\n");
		return NULL;
	}
	return p->next;
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
	if (list_is_end(l, p)) {
		fprintf(stderr,"list_inspect: Warning: Trying to inspect position at end of list.\n");
		return NULL;
	}
	return p->next->value;
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
	list_pos new_pos=calloc(1, sizeof(struct cell));
	if (new_pos == NULL) {
		perror("calloc");
		return NULL;
	}
	
	// Set value.
	new_pos->value = v;

	// Set links.
	new_pos->next = p->next;
	p->next = new_pos;
	
	// Update position of last element.
	if (!list_is_end(l, list_next(l, list_last(l)))  ) {
		l->last = list_next(l, list_last(l));
	}
	// Update length.
	l->length++;
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
	list_pos p;
	if (list_is_empty(l)) {
		p = list_first(l);
	} else {
		p = list_next(l, list_last(l));
	}
	if (list_insert(l, v, p) == NULL) {
		return 1;	
	}
	return 0;
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
	// Cell to remove.
	list_pos c = p->next;

	// Link past cell to remove.
	p->next = c->next;

	// Call free_func if registered.
	if (l->free_func != NULL) {
		// Free any user-allocated memory for the value.
		l->free_func(c->value);
	}
	// Free the memory allocated to the cell itself.
	free(c);

	// Update pointer to last element.
	list_pos q = list_first(l);
	if (!list_is_empty(l)) {
		while (!list_is_end(l, list_next(l, q))) {
			q = list_next(l, q);
		}
	}
	l->last = q;
	
	// Update list length;
	l->length--;
	// Return the position of the next element.
	return p;
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
	// Use public functions to traverse the list.

	// Start with the first element (will be defined even for an empty list).
	list_pos p = list_first(l);

	// Remove first element until list is empty.
	while(!list_is_empty(l)) {
		p = list_remove(l, p);
	}

	// Free the head and the list itself.
	free(l->head);
	free(l);    
}


