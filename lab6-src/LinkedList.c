
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
		printf("%s", e->userName);
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
void llist_add(LinkedList * list, const char * userName, const char * password) {
	// Create new node
	ListNode * n = (ListNode *) malloc(sizeof(ListNode));
	n->userName = strdup(userName);
	n->password = strdup(password);
	
	// Add at the beginning of the list
	n->next = list->head;
	list->head = n;
}

void llist_add_room(LinkedList * list, const char * userName, int roomNumber) {
	// Create new node
	ListNode * n = (ListNode *) malloc(sizeof(ListNode));
	n->userName = strdup(userName);
	n->roomNumber = roomNumber;
	
	// Add at the beginning of the list
	n->next = list->head;
	list->head = n;
}

//
// Returns 1 if the value exists in the list.
// Returns 0 if the valur does not exist in the list. 
//
int llist_exists_name(LinkedList * list, const char * userName) {
	ListNode * e;
	e = list->head;
	while(e != NULL) {
		if (strcmp(e->userName, userName) == 0)
			return 1;
		else
			e = e->next;
	}
	return 0;
}

const char * llist_password(LinkedList * list, const char * userName) {    //return corresponding password
	ListNode * e;
	e = list->head;
	while(e != NULL) {
		if (strcmp(e->userName, userName) == 0)
			return e->password;
		else
			e = e->next;
	}
	return NULL;
}

int llist_roomNumber(LinkedList * list, const char * userName) {    //return corresponding roomNumber
	ListNode * e;
	e = list->head;
	while(e != NULL) {
		if (strcmp(e->userName, userName) == 0)
			return e->roomNumber;
		else
			e = e->next;
	}
	return -2;
}

void llist_sort(LinkedList * list) {
	ListNode * p;
	p = list->head;
	ListNode * q;
	q = p->next;
	const char * tempName;
	const char * tempPassword;
	for (p = list->head; p != NULL; p = p->next) {
		for (q = p->next; q != NULL; q = q->next) {
			if (strcmp(p->userName,q->userName) > 0) {
				tempName = strdup(q->userName);
				q->userName = strdup(p->userName);
				p->userName = strdup(tempName);

				tempPassword = strdup(q->password);
				q->password = strdup(p->password);
				p->password = strdup(tempPassword);
			}
		}
	}
}

void llist_sort_room(LinkedList * list) {
	ListNode * p;
	p = list->head;
	ListNode * q;
	q = p->next;
	const char * tempName;
	int tempRoomNumber;
	for (p = list->head; p != NULL; p = p->next) {
		for (q = p->next; q != NULL; q = q->next) {
			if (strcmp(p->userName,q->userName) > 0) {
				tempName = strdup(q->userName);
				q->userName = strdup(p->userName);
				p->userName = strdup(tempName);

				tempRoomNumber = q->roomNumber;
				q->roomNumber = p->roomNumber;
				p->roomNumber = tempRoomNumber;
			}
		}
	}
}

//
// It removes the entry with that value in the list.
//
int llist_remove(LinkedList * list, const char * userName) {
	ListNode * e;
	e = list->head;
	if (strcmp(e->userName, userName) == 0) {
		list->head = e->next;
		return 1;
	}
	while(e->next != NULL) {
		if (strcmp(e->next->userName, userName) == 0) {
			e->next = e->next->next;
			//free(e);
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
int llist_get_ith(LinkedList * list, int ith, const char * userName) {
	ListNode * e;
	e = list->head;
	int count = 0;
	while(e != NULL) {
		if (count == ith) {
			//strcpy(*userName, e->userName);
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
			fprintf(fd, "%s %s\n", e->userName, e->password);
			e = e->next;
		}
		fclose(fd);
	}
	return 0;
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
