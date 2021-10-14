/*
 * Declaration of a generic undirected list. Utilized for the implementation
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

#ifndef __LIST_H
#define __LIST_H

#include <stdbool.h>

// ========== Public data types ============
// List type.
typedef struct list list;

// List position type.
typedef struct cell *list_pos;

// Type definition for de-allocator function, e.g. free().
typedef void (*free_function)(void *);

// ======= Data structure interface =======
/**
 * list_empty() - Create an empty list.
 * @free_func: Pointer to a function (or NULL) to be called to deallocate
 *             memory on kill.
 *
 * Returns: A pointer to the new list, or NULL on error.
 */
list *list_empty(free_function free_func);

/**
 * list_is_empty() - Check if a list is empty.
 * @l: List to check.
 *
 * Returns: True if the list is empty, otherwise false.
 */
bool list_is_empty(const list *l);

/**
 * list_length() - Get the current length of a list.
 * @l: List to inspect.
 *
 * Returns: The number of elements contained in the list.
 */
int list_length(list *l);

/**
 * list_first() - Return the first position of a list, i.e. the position of the
 *                first element in the list.
 * @l: List to inspect.list_is_end
 *
 * Returns: The first position in the given list.
 */
list_pos list_first(const list *l);

/**
 * list_end() - Return the last position of a list, i.e. the position after the
 *               last element in the list.
 * @l: List to inspect.
 *
 * Returns: The last position in the given list.
 */
list_pos list_end(const list *l);

/**
 * list_next() - Return the next position in a list.
 * @l: List to inspect.
 * @p: Any valid position except the last in the list.
 *
 * Returns: The position in the list after the given position.
 *          Returns NULL on invalid action, i.e. when p is the last position.
 */
list_pos list_next(const list *l, const list_pos p);

/**
 * list_previous() - Return the previous position in a list.
 * @l: List to inspect.
 * @p: Any valid position except the first in the list.
 *
 * Returns: The position in the list before the given position.
 *          Returns NULL on invalid action, i.e. when p is the first position.
 */
list_pos list_previous(const list *l, const list_pos p);

/**
 * list_index() - Get the position of an element with a certain index in a list.
 * @l:   List to inspect.
 * @idx: The index of the list element we are searching for.
 *
 * Returns: Pointer to the element, or NULL on error.
 */
list_pos list_index(const list *l, int idx);

/**
 * list_inspect() - Get the value of the element at a given position in a list.
 * @l: List to inspect.
 * @p: Any valid position in the list, except the last.
 *
 * Returns: The value at the given position as a void pointer. NOTE: Returns
            NULL if the given position is the last position of the list.
 */
void *list_inspect(const list *l, const list_pos p);

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
list_pos list_insert(list *l, void *v, const list_pos p);

/**
 * list_append() - Insert a new element at the end of the list.
 * @l: List to manipulate.
 * @v: Pointer to the value to be inserted into the list.
 *
 * Returns: 0 if the element could successfully be appended into the list.
 */
int list_append(list *l, void *v);

/**
 * list_pop() - Inspect and remove an element at the beginning of the list. Can 
 *              only be used if the freeing function is not set (i.e. set to 
 *              NULL), otherwise memory allocated for the element value will be 
 *              returned when the list element is removed.  
 * @l: List to manipulate.
 *
 * Returns: Pointer to the value of the first element, or NULL if list is empty.  
 */
void *list_pop(list *l);

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
list_pos list_remove(list *l, const list_pos p);

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
void list_kill(list *l);

#endif
