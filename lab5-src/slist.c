
#include <stdio.h>
#include <stdlib.h>

#include "slist.h"

void
sllist_init(SLList * list)
{
	list->head = NULL;
}

// Add int value to the end of list. The values may be repeated.
void
sllist_add_end( SLList *list, int value )  {
	SLEntry * e;
	SLEntry * temp = (SLEntry *)malloc(value * sizeof(SLEntry));
	if (list->head == NULL) {
		temp ->value = value;
		list->head = temp;
		temp->next = NULL;
	}
	else {
		e = list->head;
		while(e->next != NULL)
			e = e->next;
		temp->value = value;
		e->next = temp;
		temp->next = NULL;
	}
}


// Remove first occurrence of value in the list. It returns 1 if value is found or 0 otherwise.
int sllist_remove(SLList *list, int value) {
	SLEntry * e;
	if (list->head == NULL) 
		return 0;
	e = list->head;
	if (e->value == value) {
		list->head = e->next;
		return 1;
	}
	else {
		while(e->next != NULL) {
			if (e->next->value == value) {
				e->next = e->next->next;
				return 1;
			}
			else ;
			e = e->next;
		}
	}
	return 0;
}


// Removes from the list and frees the nodes of all the items that are min <= value <= max
void sllist_remove_interval(SLList *list, int min, int max) {
	/*SLEntry * e;
	SLEntry * temp;
	if (list->head == NULL)
		exit(0);
	e = list->head;
	while(e != NULL) {
		if (e->value >= min && e->value <= max) {
			temp = e;
			e = e->next;
			free(temp);
		}
	}*/
	SLEntry * e;
	SLEntry * temp;
	e = list->head;
	e -> next;
	while (e->next) {
		
		if ( (list -> head -> value >= min) && (list -> head -> value <= max))  {
			temp = list -> head;
			list -> head = e -> next;
			free(temp);
		}
		temp = e->next;
		if (temp -> value >= min && temp -> value <= max) {
			e->next = temp->next;
			free (temp);
		}
		else e = e -> next;
	}
	}


// It allocates and returns a new list with the intersection of the two lists, that is the 
// list of all the items that are common to the two lists. The items in the intersection are
// unique. 
SLList *
sllist_intersection(SLList *a, SLList *b) {
	/*SLList * ans = (SLList *)malloc (1000 * sizeof(SLList));
	SLEntry * e;
	SLEntry * f;
	SLEntry * g;
	if (a ->head == NULL || b->head == NULL)
		return NULL;
	g = ans->head;
	SLEntry * m;
	m = ans->head;
	for(e = a->head; e != NULL; e = e->next){
		for (f = b->head; f !=NULL; f = f->next) {
			if (e->value == f->value) {
				for (m = ans->head; m!= NULL; m = m->next){
					if (f->value != m->value)
						m = m->next;
				}
				if (m->next == NULL) {
					g->value = f->value;
					g = g->next;
				}
			}
			else ;
		}
	}
	g->next = NULL;
	return ans;*/
	SLList * m = (SLList *)malloc(sizeof(struct SLList));
    sllist_init(m);
    SLEntry * p = a -> head;
    SLEntry * q = b -> head;
    SLEntry * tmp;
    if (a -> head == NULL || b -> head == NULL) {
        return NULL;
    }
    else {
        while (q != NULL) {
            while (p != NULL) {
                if (p ->value == q->value) {
                    sllist_add_end(m, p -> value);
                    break;
                }
                p = p->next;
            }
            p = a -> head;
            q = q->next;
        }
    }
   
    return m;
}

void
sllist_print(SLList *list)
{
	// Move to the end
	SLEntry * e = list->head;

	printf("--- List ---\n");
	while (e!= NULL) {
		printf("val=%d\n", e->value);
		e = e->next;
	}
}

