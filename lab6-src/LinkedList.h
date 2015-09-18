
// Data structs for a list of ints

struct ListNode {
	const char * userName;
    const char * password;
    int roomNumber;
	struct ListNode * next;
};

typedef struct ListNode ListNode;

struct LinkedList {
	ListNode * head;
};

typedef struct LinkedList LinkedList;

void llist_init(LinkedList * list);
void llist_print(LinkedList * list);
void llist_add(LinkedList * list, const char * userName, const char * password);
void llist_add_room(LinkedList * list, const char * userName, int roomNumber);
int llist_exists_name(LinkedList * list, const char * userName);
const char * llist_password(LinkedList * list, const char * userName);
int llist_roomNumber(LinkedList * list, const char * userName);
int llist_remove(LinkedList * list, const char * userName);
int llist_get_ith(LinkedList * list, int ith, const char * userName);
int llist_remove_ith(LinkedList * list, int ith);
int llist_number_elements(LinkedList * list);
int llist_save(LinkedList * list, char * file_name);
int llist_read(LinkedList * list, char * file_name);
void llist_sort(LinkedList * list);
void llist_sort_room(LinkedList * list);
void llist_clear(LinkedList *list);

int llist_remove_first(LinkedList * list, int * value);
int llist_remove_last(LinkedList * list, int * value);
void llist_insert_first(LinkedList * list, int value);
void llist_insert_last(LinkedList * list, int value);
