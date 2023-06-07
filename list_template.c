#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 1024

struct List;
typedef void (*DataFp)(void*);
typedef void (*ConstDataFp)(const void*);
typedef  int (*CompareDataFp)(const void*, const void*);

typedef struct ListElement {
	struct ListElement *next;
	void *data;
} ListElement;

typedef struct {
	ListElement *head;
	ListElement *tail;
	ConstDataFp dump_data;
	DataFp free_data;
	CompareDataFp compare_data;
	DataFp modify_data;
} List;

void *safe_malloc(size_t size) {
	void *ptr = malloc(size);
	if(ptr) return ptr;
	printf("malloc error\n");
	exit(EXIT_FAILURE);
}

void *safe_strdup(const char *string) {
	void *ptr = strdup(string);
	if(ptr) return ptr;
	printf("strdup error\n");
	exit(EXIT_FAILURE);
}

// --- generic functions --- for any data type

void init_list(List *p_list, ConstDataFp dump_data, DataFp free_data,
		CompareDataFp compare_data, DataFp modify_data) {
//    ListElement *head = safe_malloc(sizeof(ListElement));
//    ListElement *tail = safe_malloc(sizeof(ListElement));
//    head->next=tail;
//    tail->next=NULL;
    p_list->head = NULL;
    p_list->tail = NULL;
    p_list->compare_data = compare_data;
    p_list->dump_data = dump_data;
    p_list->free_data = free_data;
    p_list->modify_data = modify_data;
}

// Print elements of the list
void dump_list(const List* p_list) {
    ListElement *curr = p_list->head;
    while(curr != NULL) {
        p_list->dump_data(curr->data);
        curr=curr->next;
    }
}

// Print elements of the list if comparable to data
void dump_list_if(List *p_list, void *data) {

    ListElement *curr = p_list->head;
    while(curr != NULL) {
        if(p_list->compare_data(curr->data, data) > 0) {
            p_list->dump_data(curr->data);

        }
        curr=curr->next;
    }

}

// Free element pointed by data using free_data() function
void free_element(DataFp free_data, ListElement *to_delete) {
    free_data(to_delete);
}

// Free all elements of the list
void free_list(List* p_list) {
	p_list->tail = NULL;
}

// Push element at the beginning of the list
void push_front(List *p_list, void *data){
    ListElement *el = safe_malloc(sizeof(ListElement));
    el->data = data;
    if(p_list->head == NULL) {
        p_list->head = el;
        return;
    }
    if(p_list->tail == NULL) {
        p_list->tail = p_list->head;
        p_list->head = el;
        p_list->tail->next = NULL;
        p_list->head->next=p_list->tail;
        return;
    }
    el->next = p_list->head;
    p_list->head = el;


//     el->data = data;
//    int *test = (int*)data;
//    printf("%d", *test);
//     el->next = p_list->head;
//     p_list->head = el;

}

// Push element at the end of the list
void push_back(List *p_list, void *data) {

    ListElement *el = safe_malloc(sizeof(ListElement));
    el->data = data;
    el->next = NULL;
    if(p_list->head == NULL) {
        el->next=NULL;
        p_list->head = el;
        return;
    }
    if(p_list->tail == NULL) {
        p_list->tail = el;
        p_list->head->next = p_list->tail;
        return;
    }
    p_list->tail->next = el;
    p_list->tail = el;
}

// Remove the first element
void pop_front(List *p_list) {
    p_list->head = p_list->head->next;
}

// Reverse the list
void reverse(List *p_list) {
    ListElement *curr = p_list->head;
    ListElement  *prev = NULL;
    ListElement  *next = NULL;
    p_list->tail = p_list->head;
    while(curr != NULL) {
        next = curr->next;
        curr->next = prev;
        prev=curr;
        curr=next;
    }
    p_list->head = prev;

}

// find element in sorted list after which to insert given element
ListElement* find_insertion_point(const List *p_list, ListElement *p_element) {
    ListElement *p = p_list->head;
    ListElement *prev = NULL;

    while( p != NULL ){
        if( p_list->compare_data(p->data, p_element->data) > 0 ){
            break;
        }
        prev = p;
        p = p->next;
    }

    return prev;
}

// Insert element after 'previous'
void push_after(List *p_list, void *data, ListElement *previous) {
    ListElement *newElement= safe_malloc(sizeof(ListElement));
    newElement->data = data;
    newElement->next = previous->next;
    previous->next = newElement;
}

// Insert element preserving order
void insert_in_order(List *p_list, void *p_data, CompareDataFp cmp) {
    ListElement *newElement = (ListElement*) safe_malloc(sizeof(ListElement));
    newElement->data = p_data;
    newElement->next = NULL;
    ListElement *previous = find_insertion_point(p_list, newElement);

    if (previous == NULL) {
        newElement->next = p_list->head;
        newElement->next = p_list->head;
        p_list->head = newElement;
    } else {
        if(cmp) {
            if(cmp(previous->data,p_data) == 0) {
                if(p_list->modify_data != NULL) {
                    p_list->modify_data(previous->data);
                }

                return;
            }
        }
        push_after(p_list, p_data, previous);
    }
}

// -----------------------------------------------------------
// --- type-specific definitions

// int element

void dump_int(const void *d) {
    const int* value = (const int*)d;
    printf("%d ", *value);
}
int cmp_int(const void *a, const void *b) {
    const int* l1 = (const void*)a;
    const int* l2 = (const void*)b;

    if(*l1<*l2) return -1;
    if(*l1==*l2) return 0;
    if(*l1>*l2) return 1;
}
void free_int(void *d) {
    free(d);
}



int *create_data_int(int v) {
    int *ptr = safe_malloc(sizeof(int));
    *ptr = v;

    return ptr;
}

// Word element

typedef struct DataWord {
	char *word;
	int counter;
} DataWord;

void dump_word (const void *d) {
    const DataWord *word = (const DataWord *)d;

    printf("%s ", word->word);
}

void free_word(void *d) {
    free(d);
}

int cmp_word_alphabet(const void *a, const void *b) {
    const DataWord *word1 = (const DataWord*)a;
    const DataWord *word2 = (const DataWord*)b;
    return strcmp(word1->word, word2->word);

}

int cmp_word_counter(const void *a, const void *b) {
    const DataWord *word1 = (const DataWord*)a;
    const DataWord *word2 = (const DataWord*)b;
//    printf("%d %d \n", word1->counter, word2->counter);
    if(word1->counter == word2->counter) {
        return 1;
    }
    return -1;
}
void dump_word_lowercase (const void *d) {
    const DataWord *word = (const DataWord *)d;
    printf("%s ", word->word);
}
void modify_word(void *p) {
     DataWord *word = (DataWord*)p;
     word->counter++;
}

void *create_data_word(const char *string, int counter) {\
    char* word_ptr = safe_malloc((strlen(string) + 1) * sizeof(char));
    strcpy(word_ptr, string);

    DataWord *struct_ptr = safe_malloc(sizeof(DataWord));
    struct_ptr->counter = counter;
    struct_ptr->word = word_ptr;

    return struct_ptr;


}


// read text, parse it to words, and insert those words to the list.
// Order of insertions is given by the last parameter of type CompareDataFp.
// (comparator function address). If this address is not NULL the element is
// inserted according to the comparator. Otherwise, read order is preserved.
void stream_to_list(List *p_list, FILE *stream, CompareDataFp cmp) {
    char text[BUFFER_SIZE];
    char word_separators[] = " \n\t\r\v\f.,?!:;-";
    char * buff;
    while (fgets(text, BUFFER_SIZE, stream) != NULL) {
//        if (strcmp(text, "q\n") == 0) {
//            break;
//        }
        buff = strtok( text, word_separators );
        while( buff != NULL )
        {
            DataWord *word = create_data_word(buff, 1);
            if(cmp) {
                for (int i = 0; i < strlen(word->word); i++)
                {
                    word->word[i] = tolower(word->word[i]);
                }
                insert_in_order(p_list, word, cmp);
            }
            else {
                push_back(p_list, word);
            }

            buff = strtok( NULL, word_separators );
        }
    }
}
// test integer list
void list_test(List *p_list, int n) {
	char op;
	int v;
	for (int i = 0; i < n; ++i) {
		scanf(" %c", &op);
		switch (op) {
			case 'f':
				scanf("%d", &v);
				push_front(p_list, create_data_int(v));
				break;
			case 'b':
				scanf("%d", &v);
				push_back(p_list, create_data_int(v));
				break;
			case 'd':
				pop_front(p_list);
				break;
			case 'r':
				reverse(p_list);
				break;
			case 'i':
				scanf("%d", &v);
				insert_in_order(p_list, create_data_int(v), cmp_int);
				break;
			default:
				printf("No such operation: %c\n", op);
				break;
		}
	}
}

int main(void) {
	int to_do, n;
	List list;

	scanf ("%d", &to_do);
	switch (to_do) {
		case 1: // test integer list
			scanf("%d",&n);
			init_list(&list, dump_int, free_int, cmp_int, NULL);
			list_test(&list, n);
			dump_list(&list);
			free_list(&list);
			break;
		case 2: // read words from text, insert into list, and print
			init_list(&list, dump_word, free_word, NULL, NULL);
			stream_to_list(&list, stdin, NULL);
			dump_list(&list);
			free_list(&list);
			break;
		case 3: // read words, insert into list alphabetically, print words encountered n times
			scanf("%d",&n);
			init_list(&list, dump_word_lowercase, free_word, cmp_word_alphabet, modify_word);
			stream_to_list(&list, stdin, cmp_word_alphabet);
			list.compare_data = cmp_word_counter;
			DataWord data = { NULL, n };
			dump_list_if(&list, &data);
			free_list(&list);
			break;
		default:
			printf("NOTHING TO DO FOR %d\n", to_do);
			break;
	}
	return 0;
}

