#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head) {
        return NULL;
    }
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l) {
        return;
    }
    element_t *curr, *next;
    list_for_each_entry_safe (curr, next, l, list) {
        list_del(&curr->list);
        q_release_element(curr);
    }
    free(l);
    return;
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }
    element_t *node = malloc(sizeof(element_t));
    if (!node) {
        return false;
    }
    size_t len = strlen(s);
    node->value = malloc((len + 1) * sizeof(char));
    if (!(node->value)) {
        free(node);
        return false;
    }
    list_add(&node->list, head);
    if (!node->list.next || !node->list.prev) {
        q_release_element(node);
        return false;
    }
    strncpy(node->value, s, len + 1);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }
    element_t *node = malloc(sizeof(element_t));
    if (!node) {
        return false;
    }
    size_t len = strlen(s);
    node->value = malloc((len + 1) * sizeof(char));
    if (!(node->value)) {
        free(node);
        return false;
    }
    list_add_tail(&node->list, head);
    if (!node->list.next || !node->list.prev) {
        q_release_element(node);
        return false;
    }
    strncpy(node->value, s, len + 1);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    element_t *del_node = list_entry(head->next, element_t, list);
    char *del_str = del_node->value;
    list_del_init(head->next);
    if (sp) {
        size_t del_len = strlen(del_str);
        size_t len = (del_len > (bufsize - 1)) ? bufsize - 1 : del_len;
        strncpy(sp, del_str, len);
        *(sp + len) = '\0';
    }
    return del_node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    element_t *del_node = list_entry(head->prev, element_t, list);
    char *del_str = del_node->value;
    list_del_init(head->prev);
    if (sp) {
        size_t del_len = strlen(del_str);
        size_t len = (del_len > (bufsize - 1)) ? bufsize - 1 : del_len;
        strncpy(sp, del_str, len);
        *(sp + len) = '\0';
    }
    return del_node;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head) {
        return 0;
    }
    struct list_head *iterator;
    int size = 0;
    list_for_each (iterator, head) {
        size++;
    }
    return size;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head)) {
        return false;
    }
    struct list_head *right = head->next;
    struct list_head *left = head->prev;
    while (!((left == right) || ((left->prev) == right))) {
        right = right->next;
        left = left->prev;
    }
    list_del(left);
    q_release_element(list_entry(left, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head)) {
        return false;
    }
    struct list_head *reference = head->next, *traverse, *delete;
    while ((reference->next != head) && (reference != head)) {
        traverse = reference->next;
        char *str_ref = list_entry(reference, element_t, list)->value;
        char *str_tra = list_entry(traverse, element_t, list)->value;
        if (!strcmp(str_ref, str_tra)) {
            while (!(traverse == head) && !strcmp(str_ref, str_tra)) {
                traverse = traverse->next;
                str_tra = list_entry(traverse, element_t, list)->value;
            }
            while (reference != traverse) {
                delete = reference;
                reference = reference->next;
                list_del(delete);
                q_release_element(list_entry(delete, element_t, list));
            }
        } else {
            reference = traverse;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head)) {
        return;
    }
    struct list_head *curr, *next;
    list_for_each_safe (curr, next, head) {
        if (next == head) {
            return;
        }
        list_del(curr);
        list_add(curr, next);
        next = next->next->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return;
    }
    struct list_head stack, *curr, *next;
    INIT_LIST_HEAD(&stack);
    list_for_each_safe (curr, next, head) {
        list_del(curr);
        list_add(curr, &stack);
    }
    list_for_each_safe (curr, next, &stack) {
        list_del(curr);
        list_add_tail(curr, head);
    }
    return;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || k <= 1) {
        return;
    }

    struct list_head **TAIL = &(head->next);
    while (!0) {
        // Check if the number of nodes is < k
        struct list_head *HEAD = *TAIL;
        for (int count = 0; count < k; count++) {
            if (*TAIL == head) {
                return;
            }
            TAIL = &(*TAIL)->next;
        }

        // Delete the nodes from the list
        struct list_head stack;
        INIT_LIST_HEAD(&stack);
        for (int count = 0; count < k; count++) {
            struct list_head *delete = HEAD;
            HEAD = HEAD->next;
            list_del(delete);
            list_add(delete, &stack);
        }

        // Insert the list in reverse order
        list_splice(&stack, HEAD->prev);
        TAIL = &HEAD->prev->next;
    }
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || q_size(head) <= 1) {
        return;
    }

    // Find the middle point
    struct list_head *right = head->next;
    struct list_head *left = head->prev;
    while (!((left == right) || ((left->prev) == right))) {
        right = right->next;
        left = left->prev;
    }

    // Break into 2 parts (head and stack)
    struct list_head stack;
    INIT_LIST_HEAD(&stack);
    for (struct list_head *traverse = head->next, *safe = traverse->next;
         traverse != left; traverse = safe, safe = safe->next) {
        list_del(traverse);
        list_add_tail(traverse, &stack);
    }

    q_sort(head, descend);
    q_sort(&stack, descend);

    // Merge two lists
    struct list_head *head_tra = head->next, *stack_del;
    while (head_tra != head && !list_empty(&stack)) {
        char *str_head = list_entry(head_tra, element_t, list)->value;
        char *str_stack = list_entry(stack.next, element_t, list)->value;
        if (descend ? (strcmp(str_head, str_stack) > 0)
                    : (strcmp(str_head, str_stack) < 0)) {
            head_tra = head_tra->next;
        } else {
            stack_del = stack.next;
            list_del(stack_del);
            list_add(stack_del, head_tra->prev);
        }
    }
    while (!list_empty(&stack)) {
        stack_del = stack.next;
        list_del(stack_del);
        list_add(stack_del, head_tra->prev);
    }
    return;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head)) {
        return q_size(head);
    }
    element_t *curr, *next;
    list_for_each_entry_safe (curr, next, head, list) {
        for (struct list_head *traverse = &next->list; traverse != head;
             traverse = traverse->next) {
            if (strcmp(curr->value,
                       list_entry(traverse, element_t, list)->value) > 0) {
                list_del(&curr->list);
                q_release_element(curr);
                break;
            }
        }
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head)) {
        return q_size(head);
    }
    element_t *curr, *next;
    list_for_each_entry_safe (curr, next, head, list) {
        for (struct list_head *traverse = &next->list; traverse != head;
             traverse = traverse->next) {
            if (strcmp(curr->value,
                       list_entry(traverse, element_t, list)->value) < 0) {
                list_del(&curr->list);
                q_release_element(curr);
                break;
            }
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head)) {
        return 0;
    }
    queue_contex_t *first = list_entry(head->next, queue_contex_t, chain);
    if (head->next->next == head) {
        return q_size(first->q);
    }
    queue_contex_t *traverse =
        list_entry((first->chain).next, queue_contex_t, chain);
    while (&traverse->chain != head) {
        struct list_head *first_tra = first->q->next;
        while (!list_empty(traverse->q) && first_tra != first->q) {
            char *first_str = list_entry(first_tra, element_t, list)->value;
            char *traverse_str =
                list_entry(traverse->q->next, element_t, list)->value;
            if (descend ? (strcmp(first_str, traverse_str) > 0)
                        : (strcmp(first_str, traverse_str) < 0)) {
                first_tra = first_tra->next;
            } else {
                struct list_head *delete = traverse->q->next;
                list_del(delete);
                list_add(delete, first_tra->prev);
            }
        }
        while (!list_empty(traverse->q)) {
            struct list_head *delete = traverse->q->next;
            list_del(delete);
            list_add(delete, first_tra->prev);
        }
        traverse = list_entry((traverse->chain).next, queue_contex_t, chain);
    }
    return q_size(first->q);
}
