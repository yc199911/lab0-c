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
    if (head == NULL)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *entry;
    element_t *safe;

    list_for_each_entry_safe (entry, safe, l, list) {
        list_del(&entry->list);
        q_release_element(entry);
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *new_node = malloc(sizeof(element_t));
    if (!new_node)
        return false;
    INIT_LIST_HEAD(&new_node->list);
    // 分配足夠的記憶體來容納字串 s，包括結尾的空字符 '\0'
    char *new_char = malloc((strlen(s) + 1) * sizeof(char));
    if (!new_char) {
        free(new_node);
        return false;
    }
    // 複製字串 s 的內容到新分配的記憶體中
    strncpy(new_char, s, strlen(s));
    new_char[strlen(s)] = '\0';
    new_node->value = new_char;
    list_add(&new_node->list, head);
    return true;
}


/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *new_node = malloc(sizeof(element_t));
    if (!new_node)
        return false;
    INIT_LIST_HEAD(&new_node->list);
    // 分配足夠的記憶體來容納字串 s，包括結尾的空字符 '\0'
    size_t len = strlen(s);
    char *new_char = malloc((len + 1) * sizeof(char));
    if (!new_char) {
        free(new_node);
        return false;
    }
    // 複製字串 s 的內容到新分配的記憶體中
    strncpy(new_char, s, len);
    new_char[len] = '\0';  // 確保結尾為 '\0'
    new_node->value = new_char;
    list_add_tail(&new_node->list, head);
    return true;
}


/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *node = list_first_entry(head, element_t, list);

    if (sp) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = 0;
    }
    list_del_init(&node->list);
    return node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *node = list_last_entry(head, element_t, list);

    if (sp && bufsize > 0) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = 0;
    }
    list_del_init(&node->list);
    return node;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;
    int count = 0;
    struct list_head *node;
    list_for_each (node, head)
        count++;
    return count;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || !head->next)
        return false;

    struct list_head *slow = head->next;
    struct list_head *fast = head->next;

    while (fast->next != head && fast != head) {
        slow = slow->next;
        fast = fast->next->next;
    }
    list_del(slow);
    element_t *del = list_entry(slow, element_t, list);
    q_release_element(del);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return false;

    element_t *entry = NULL;
    element_t *temp = malloc(sizeof(element_t));
    struct list_head *pos, *next;

    list_for_each_safe (pos, next, head) {
        entry = list_entry(pos, element_t, list);
        list_for_each_entry (temp, head, list) {
            if (temp != entry && temp->value == entry->value) {
                list_del(&entry->list);
                free(entry);
                break;
            }
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *first;
    struct list_head *second;
    list_for_each_safe (first, second, head) {
        if (first == head || second == head)
            break;
        list_move(second, first);
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *node;
    struct list_head *safe;
    list_for_each_safe (node, safe, head) {
        node->next = node->prev;
        node->prev = safe;
    }
    node->next = node->prev;
    node->prev = safe;
    return;
}
/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head))
        return;

    struct list_head *curr;
    struct list_head *safe;
    struct list_head temp;
    struct list_head *begin = head;
    INIT_LIST_HEAD(&temp);
    int count = 0;
    list_for_each_safe (curr, safe, head) {
        count++;
        if (k == count) {
            list_cut_position(&temp, begin, curr);
            q_reverse(&temp);
            list_splice_init(&temp, begin);
            count = 0;
            begin = safe->prev;
        }
    }
    return;
}

void mergeTwoLists(struct list_head *L1, struct list_head *L2)
{
    if (!L1 || !L2)
        return;
    struct list_head *Lnode = L1->next, *Rnode = L2->next;
    while (Lnode != L1->next && Rnode != L2->next) {
        if (strcmp(list_entry(Lnode, element_t, list)->value,
                   list_entry(Rnode, element_t, list)->value) < 0) {
            Lnode = Lnode->next;
        } else {
            list_move(Rnode, Lnode->prev);
            Rnode = Rnode->next;
        }
    }
    if (q_size(L2) != 0) {
        list_splice_tail(L2, L1);
    }
}
/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *slow = head->next;
    for (struct list_head *fast = head->next;
         fast != head && (fast->next) != head; fast = fast->next->next)
        slow = slow->next;
    struct list_head *mid = slow;
    LIST_HEAD(right);
    list_cut_position(&right, head, mid->prev);

    q_sort(head, 1);
    q_sort(&right, 1);
    mergeTwoLists(head, &right);
}



/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    element_t *first;
    element_t *second;
    list_for_each_entry_safe (first, second, head, list) {
        if (second->value > first->value)
            list_del(&first->list);
    }
    return q_size(head);
}


/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    element_t *first;
    element_t *second;
    list_for_each_entry_safe (first, second, head, list) {
        if (second->value < first->value)
            list_del(&first->list);
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head)
        return 0;
    queue_contex_t *queue_head = list_entry(head->next, queue_contex_t, chain);
    for (struct list_head *curr = head->next->next; curr != head;
         curr = curr->next) {
        queue_contex_t *queue = list_entry(curr, queue_contex_t, chain);
        mergeTwoLists(queue_head->q, queue->q);
        INIT_LIST_HEAD(queue->q);
        queue->size = 0;
    }
    return queue_head->size;
}