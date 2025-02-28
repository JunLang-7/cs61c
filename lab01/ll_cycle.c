#include <stddef.h>
#include "ll_cycle.h"

int ll_has_cycle(node *head) {
    /* your code here */
    node* tortoise = head;
    node* hare = head;
    while (tortoise && hare) {
        hare = hare->next;
        if (hare == NULL) {
            return 0;
        }
        hare = hare->next;
        tortoise = tortoise->next;
        if (hare == NULL || tortoise == NULL) {
            return 0; 
        }
        if (hare == tortoise) {
            return 1;
        }
    }
    return 0;
}
