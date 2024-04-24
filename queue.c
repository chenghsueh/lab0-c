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
    if (head) {
        INIT_LIST_HEAD(head);
        return head;
    }
    return NULL;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (head) {
        element_t *entry, *safe;
        list_for_each_entry_safe (entry, safe, head, list) {
            q_release_element(entry);
        }
        free(head);
    }
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }

    element_t *new_element = malloc(sizeof(element_t));
    if (!new_element) {
        return false;
    }

    new_element->value = strdup(s);
    if (!new_element->value) {
        free(new_element);
        return false;
    }

    list_add(&new_element->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    return q_insert_head(head->prev, s);
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }

    element_t *re_element = list_first_entry(head, element_t, list);

    if (sp) {
        strncpy(sp, re_element->value, bufsize - 1);
        *(sp + bufsize - 1) = '\0';
    }
    list_del_init(&re_element->list);

    return re_element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head)
        return NULL;
    return q_remove_head(head->prev->prev, sp, bufsize);
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    int count = 0;
    element_t *node;
    list_for_each_entry (node, head, list) {
        count++;
    }
    return count;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head)) {
        return false;
    }

    struct list_head *front, *back;
    for (front = head->next, back = head->prev;
         front != back && front->next != back;
         front = front->next, back = back->prev) {
    }

    element_t *re_element = list_entry(front, element_t, list);
    list_del(front);
    q_release_element(re_element);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head)) {
        return false;
    }

    element_t *entry, *safe;
    bool is_dup = false;
    list_for_each_entry_safe (entry, safe, head, list) {
        if (&safe->list != head && !strcmp(entry->value, safe->value)) {
            list_del(&entry->list);
            q_release_element(entry);
            is_dup = true;
        } else if (is_dup) {
            list_del(&entry->list);
            q_release_element(entry);
            is_dup = false;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head) {
        return;
    }

    struct list_head *node;
    list_for_each (node, head) {
        if (node->next == head) {
            break;
        }
        list_move(node, node->next);
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head) {
        return;
    }

    struct list_head *entry, *safe;
    list_for_each_safe (entry, safe, head) {
        list_move(entry, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || k <= 1) {
        return;
    }

    int count = k;
    struct list_head *entry, *safe, *tmp = head;
    LIST_HEAD(dummy);
    list_for_each_safe (entry, safe, head) {
        count--;
        if (!count) {
            list_cut_position(&dummy, tmp, entry);
            q_reverse(&dummy);
            list_splice_init(&dummy, tmp);
            count = k;
            tmp = safe->prev;
        }
    }
}

void q_merge_two(struct list_head *head,
                 struct list_head *L1,
                 struct list_head *L2,
                 bool descend)
{
    while (!list_empty(L1) && !list_empty(L2)) {
        if (descend
                ? strcmp(list_entry(L1->next, element_t, list)->value,
                         list_entry(L2->next, element_t, list)->value) >= 0
                : strcmp(list_entry(L1->next, element_t, list)->value,
                         list_entry(L2->next, element_t, list)->value) <= 0) {
            list_move_tail(L1->next, head);
        } else {
            list_move_tail(L2->next, head);
        }
    }

    if (list_empty(L1)) {
        list_splice_tail(L2, head);
    } else {
        list_splice_tail(L1, head);
    }
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }

    struct list_head *slow = head->next, *fast = head->next->next;
    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }

    LIST_HEAD(l1);
    LIST_HEAD(l2);
    list_cut_position(&l1, head, slow);
    list_splice_init(head, &l2);

    q_sort(&l1, descend);
    q_sort(&l2, descend);

    q_merge_two(head, &l1, &l2, descend);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head)) {
        return 0;
    }

    int count = 0;
    struct list_head *cur = head->prev->prev;
    element_t *min = list_entry(head->prev, element_t, list);

    while (cur != head) {
        element_t *cur_e = list_entry(cur, element_t, list);
        struct list_head *prev = cur->prev;

        if (strcmp(cur_e->value, min->value) >= 0) {
            list_del(cur);
            q_release_element(cur_e);
        } else {
            count++;
            min = cur_e;
        }
        cur = prev;
    }

    return count;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head)) {
        return 0;
    }

    int count = 0;
    struct list_head *cur = head->prev->prev;
    element_t *max = list_entry(head->prev, element_t, list);

    while (cur != head) {
        element_t *cur_e = list_entry(cur, element_t, list);
        struct list_head *prev = cur->prev;

        if (strcmp(cur_e->value, max->value) <= 0) {
            list_del(cur);
            q_release_element(cur_e);
        } else {
            count++;
            max = cur_e;
        }
        cur = prev;
    }

    return count;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head)) {
        return 0;
    }

    queue_contex_t *t = list_entry(head->next, queue_contex_t, chain),
                   *cur = NULL;
    int size = t->size;

    if (list_is_singular(head)) {
        return size;
    }

    list_for_each_entry (cur, head, chain) {
        if (cur == t)
            continue;

        list_splice_init(cur->q, t->q);
        size += cur->size;
    }

    q_sort(t->q, descend);

    return size;
}
