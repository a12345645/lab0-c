#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Create element
 * Allocate space for value and copy the string into it.
 * Return NULL if could not allocate space.
 */
element_t *q_alloc_element(char *s)
{
    element_t *e = malloc(sizeof(element_t));
    if (!e)
        return NULL;

    int len = strlen(s) + 1;
    e->value = malloc(len);
    if (!e->value) {
        free(e);
        return NULL;
    }
    INIT_LIST_HEAD(&e->list);

    strncpy(e->value, s, len);

    return e;
}

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
struct list_head *q_new()
{
    struct list_head *q = malloc(sizeof(struct list_head));
    if (q)
        INIT_LIST_HEAD(q);
    return q;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (l) {
        element_t *e, *safe;

        list_for_each_entry_safe (e, safe, l, list) {
            q_release_element(e);
        }

        free(l);
    }
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *item = q_alloc_element(s);
    if (!item)
        return false;

    list_add(&item->list, head);
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *item = q_alloc_element(s);
    if (!item)
        return false;

    list_add_tail(&item->list, head);
    return true;
}

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *item = list_entry(head->next, element_t, list);
    list_del_init(head->next);
    if (sp) {
        strncpy(sp, item->value, MIN(bufsize - 1, strlen(item->value) + 1));
        sp[bufsize - 1] = '\0';
    }
    return item;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *item = list_entry(head->prev, element_t, list);
    list_del_init(head->prev);
    if (sp) {
        strncpy(sp, item->value, MIN(bufsize - 1, strlen(item->value) + 1));
        sp[bufsize - 1] = '\0';
    }
    return item;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    struct list_head *node;
    int count = 0;
    list_for_each (node, head)
        count++;

    return count;
}

/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return true if successful.
 * Return false if list is NULL or empty.
 */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/

    if (!head || list_empty(head))
        return false;

    struct list_head *mid = head->next, *fast = head->next;

    while (fast != head && fast->next != head) {
        mid = mid->next;
        fast = fast->next->next;
    }

    list_del(mid);
    q_release_element(list_entry(mid, element_t, list));

    return true;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

void list_swap(struct list_head *left, struct list_head *right)
{
    struct list_head *tmp;

    tmp = left->prev;
    tmp->next = right;
    right->prev = tmp;

    left->prev = right;
    left->next = right->next;
    right->next = left;
    left->next->prev = left;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/

    if (!head || list_empty(head))
        return;

    struct list_head *ptr = head->next;
    while (ptr != head && ptr->next != head) {
        list_swap(ptr, ptr->next);
        ptr = ptr->next;
    }
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *current = head->next, *tmp;

    while (current != head) {
        tmp = current->prev;
        current->prev = current->next;
        current->next = tmp;
        current = current->prev;
    }

    tmp = head->next;
    head->next = head->prev;
    head->prev = tmp;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */

struct list_head *merge_sort(struct list_head *head)
{
    if (!head || !head->next)
        return head;

    struct list_head *slow = head, *fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    slow->prev->next = NULL;

    struct list_head *left, *right, *merge, *ptr;
    left = merge_sort(head);
    right = merge_sort(slow);

    element_t *node1, *node2;

    node1 = list_entry(left, element_t, list);
    node2 = list_entry(right, element_t, list);
    if (strcmp(node1->value, node2->value) <= 0) {
        merge = left;
        left = left->next;
    } else {
        merge = right;
        right = right->next;
    }
    ptr = merge;
    while (left && right) {
        node1 = list_entry(left, element_t, list);
        node2 = list_entry(right, element_t, list);

        if (strcmp(node1->value, node2->value) <= 0) {
            ptr->next = left;
            left = left->next;
        } else {
            ptr->next = right;
            right = right->next;
        }
        ptr = ptr->next;
    }
    if (left) {
        ptr->next = left;
    }

    if (right) {
        ptr->next = right;
    }

    return merge;
}

void q_sort(struct list_head *head)
{
    if (!head || !head->next)
        return;

    head->prev->next = NULL;
    head->next = merge_sort(head->next);

    struct list_head *front = head, *behind = head->next;
    while (behind) {
        behind->prev = front;
        front = behind;
        behind = behind->next;
    }

    front->next = head;
    head->prev = front;
}
