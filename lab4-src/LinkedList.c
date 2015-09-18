
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "LinkedList.h"

//
// Initialize a linked list
//
void llist_init(LinkedList * list)
{
	list->head = NULL;
}

//
// It prints the elements in the list in the form:
// 4, 6, 2, 3, 8,7
//
void llist_print(LinkedList * list) {
	
	ListNode * e;

	if (list->head == NULL) {
		printf("{EMPTY}\n");
		return;
	}

	printf("{");

	e = list->head;
	while (e != NULL) {
		printf("%d", e->value);
		e = e->next;
		if (e!=NULL) {
			printf(", ");
		}
	}
	printf("}\n");
}

//
// Appends a new node with this value at the beginning of the list
//
void llist_add(LinkedList * list, int value) {
	// Create new node
	ListNode * n = (ListNode *) malloc(sizeof(ListNode));
	n->value = value;
	
	// Add at the beginning of the list
	n->next = list->head;
	list->head = n;
}

//
// Returns true if the value exists in the list.
//
int llist_exists(LinkedList * list, int value) {
	ListNode * e;
	e = list->head;
	while(e != NULL) {
		if (e->value == value)
			return 1;
		else
			e = e->next;
	}
	return 0;
}

//
// It removes the entry with that value in the list.
//
int llist_remove(LinkedList * list, int value) {
	ListNode * e;
	e = list->head;
	while(e->next != NULL) {
		if (e->next->value == value) {
			e->next = e->next->next;
			return 1;
		}
		else 
			e = e->next;
	}
	return 0;
}

//
// It stores in *value the value that correspond to the ith entry.
// It returns 1 if success or 0 if there is no ith entry.
//
int llist_get_ith(LinkedList * list, int ith, int * value) {
	ListNode * e;
	e = list->head;
	int count = 0;
	while(e != NULL) {
		if (count == ith) {
			*value = e->value;
			return 1;
		}
		e = e->next;
		count++;
	}
	return 0;
}

//
// It removes the ith entry from the list.
// It returns 1 if success or 0 if there is no ith entry.
//
int llist_remove_ith(LinkedList * list, int ith) {
	ListNode * e;
	e = list->head;
	int count = 0;
	while(e->next != NULL) {
		e = e->next;
		count++;
		if (count == ith - 1) {
			e->next = e->next->next;
			return 1;
		}
	}
	return 0;
}

//
// It returns the number of elements in the list.
//
int llist_number_elements(LinkedList * list) {
	ListNode * e;
	e = list->head;
	int count = 0;
	while(e != NULL) {
		count++;
		e = e->next;
	}
	return count;
}


//
// It saves the list in a file called file_name. The format of the
// file is as follows:
//
// value1\n
// value2\n
// ...
//
int llist_save(LinkedList * list, char * file_name) {
	FILE * fd = fopen(file_name, "w+");
	ListNode * e;
	e = list->head;
	if(fd == NULL) {
		return 0;
	}
	else {
		while(e != NULL) {
			fprintf(fd, "%i\n", e->value);
			e = e->next;
		}
		fclose(fd);
	}
	return 0;
}

//
// It reads the list from the file_name indicated. If the list already has entries, 
// it will clear the entries.
//
int llist_read(LinkedList * list, char * file_name) {
	FILE * fd = fopen(file_name, "r");
	int len = 0;
	int i, ch;
	char word[10];
	if(fd == NULL) {
		return 0;
	}
	while(1) {
		if (ch == EOF) {
			break; 
		}
		for (i = len; i < 10; i++) {
			word[i] = '\0';
		}
		ch = fgetc(fd);
		if (ch != ' ' && ch != '\n' && ch != EOF && ch != '\t') {
			word[len++] = ch;
		}
		else if (len != 0) {
			llist_add(list, atoi(word));
			len = 0;
		}
	}
	fclose(fd);
	return 1;
}



//
// It sorts the list. The parameter ascending determines if the
// order si ascending (1) or descending(0).
//
void llist_sort(LinkedList * list, int ascending) {
	ListNode * p;
	p = list->head;
	ListNode * q;
	q = p->next;
	int temp;
	if (ascending == 1) {
		for (p = list->head; p != NULL; p = p->next) {
			for (q = p->next; q != NULL; q = q->next) {
				if (p->value > q->value) {
					temp = q->value;
					q->value = p->value;
					p->value = temp;
				}
			}
		}
	}
	else if (ascending == 0) {
		for (p = list->head; p != NULL; p = p->next) {
			for (q = p->next; q != NULL; q = q->next) {
				if (p->value < q->value) {
					temp = q->value;
					q->value = p->value;
					p->value = temp;
				}
			}
		}
	}
}

//
// It removes the first entry in the list and puts value in *value.
// It also frees memory allocated for the node
//
int llist_remove_first(LinkedList * list, int * value) {
	ListNode * e;
	e = list->head;
	if (e != NULL) {
		*value = list->head->value;
		list->head = e->next;
		return 1;
	}
	return 0;
}

//
// It removes the last entry in the list and puts value in *value/
// It also frees memory allocated for node.
//
int llist_remove_last(LinkedList * list, int *value) {
	ListNode * e;
	e = list->head;
	if(e == NULL) {
		free(e);
		list->head = NULL;
		return 0;
	}
	while(e->next->next != NULL) {
		e = e->next;
	}
	*value = e->next->value;
	e->next = NULL;
	return 1;
}

//
// Insert a value at the beginning of the list.
// There is no check if the value exists. The entry is added
// at the beginning of the list.
//
void llist_insert_first(LinkedList * list, int value) {
	ListNode * n = (ListNode *) malloc(sizeof(ListNode));
	n->value = value;
	n->next = list->head;
	list->head = n;
}

//
// Insert a value at the end of the list.
// There is no check if the name already exists. The entry is added
// at the end of the list.
//
void llist_insert_last(LinkedList * list, int value) {
	ListNode * e;
	ListNode * temp = (ListNode *)malloc(sizeof(struct ListNode));
	temp->value = value;
	temp->next = NULL;
	if (list->head == NULL) {
		list->head = temp;
	}
	else {
		e = list->head;
		while(e->next != NULL) {
			e = e->next;
		}
		e->next = temp;
	}
}

//
// Clear all elements in the list and free the nodes
//
void llist_clear(LinkedList *list){
	ListNode * e;
	e = list->head;
	ListNode * temp;
	while(e != NULL) {
		temp = e;
		e = e->next;
		free(temp);
	}
	list->head = NULL;
}
