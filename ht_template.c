#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define BUFFER_SIZE 1024
#define MAX_RATE 4
#define MEMORY_ALLOCATION_ERROR  (-1)

typedef union {
    int int_data;
    char char_data;
    // ... other primitive types used
    void *ptr_data;
} data_union;

typedef struct ht_element {
    struct ht_element *next;
    data_union data;
} ht_element;

typedef void (*DataFp)(data_union);
typedef void (*DataPFp)(data_union*);
typedef int (*CompareDataFp)(data_union, data_union);
typedef size_t (*HashFp)(data_union, size_t);
typedef data_union (*CreateDataFp)(void*);

typedef struct {
    size_t size;
    size_t no_elements;
    ht_element *ht;
    DataFp dump_data;
    CreateDataFp create_data;
    DataFp free_data;
    CompareDataFp compare_data;
    HashFp hash_function;
    DataPFp modify_data;
} hash_table;

typedef struct DataWord {
    char *word;
    int counter;
} DataWord;
// ---------------------- functions to implement

// initialize table fields
void init_ht(hash_table *p_table, size_t size, DataFp dump_data, CreateDataFp create_data,
             DataFp free_data, CompareDataFp compare_data, HashFp hash_function, DataPFp modify_data) {

    p_table->ht = (ht_element*)malloc(size * sizeof(ht_element));
    p_table->size = size;
    p_table->no_elements = 0;
    p_table->dump_data = dump_data;
    p_table->create_data = create_data;
    p_table->free_data = free_data;
    p_table->compare_data = compare_data;
    p_table->hash_function = hash_function;
    p_table->modify_data = modify_data;
    for (size_t i = 0; i < size; i++) {
        p_table->ht[i].next = NULL;
    }
}

// print elements of the list with hash n
void dump_list(const hash_table* p_table, size_t n) {
    ht_element* list_head = p_table->ht[n].next;
    while(list_head != NULL) {
        p_table->dump_data(list_head->data);
        list_head=list_head->next;
    }
//    for(int i = 0; i<p_table->size; i++) {
//        ht_element* list_head = p_table->ht[i].next;
//        while(list_head != NULL) {
//            p_table->dump_data(list_head->data);
//            list_head=list_head->next;
//        }
//    }
}

// Free element pointed by data_union using free_data() function
void free_element(DataFp free_data, ht_element *to_delete) {\
    if(free_data != NULL) {
        free_data(to_delete->data);
    }
    free(to_delete);
}

// free all elements from the table (and the table itself)
void free_table(hash_table* p_table) {
    for(int i = 0; i<p_table->size; i++) {
        ht_element* list_head = p_table->ht[i].next;
        while(list_head != NULL) {
            ht_element* next = list_head->next;
            free_element(p_table->free_data,list_head);

            list_head=next;
        }
    }
    free(p_table->ht);
    p_table->size = 0;
    p_table->no_elements = 0;
    p_table->dump_data = NULL;
    p_table->create_data = NULL;
    p_table->free_data = NULL;
    p_table->compare_data = NULL;
    p_table->hash_function = NULL;
    p_table->modify_data = NULL;
}

// calculate hash function for integer k
size_t hash_base(int k, size_t size) {
    static const double c = 0.618033988; // (sqrt(5.) – 1) / 2.;
    double tmp = k * c;
    return (size_t)floor((double)size * (tmp - floor(tmp)));
}

void rehash(hash_table *p_table) {
    size_t new_size = 2*p_table->size;
    ht_element* new_ht = (ht_element*) malloc(new_size*sizeof(ht_element));
    for (size_t i = 0; i < new_size; i++) {
        new_ht[i].next = NULL;
    }
    for (size_t i = 0; i < p_table->size; i++) {
        ht_element* list_head = p_table->ht[i].next;
        while (list_head != NULL) {
            ht_element* next_element = list_head->next;
            size_t new_index = p_table->hash_function(list_head->data, new_size);
            list_head->next = new_ht[new_index].next;
            new_ht[new_index].next = list_head;
            list_head = next_element;
        }
    }
    free(p_table->ht);
    p_table->ht = new_ht;
    p_table->size = new_size;
}

// find element; return pointer to previous
ht_element *find_previous(hash_table *p_table, data_union data) {
    size_t n = p_table->hash_function(data, p_table->size);
    ht_element* list_head = p_table->ht[n].next;
    ht_element* prev= &p_table->ht[n];
    while(list_head != NULL) {
        if(p_table->compare_data(list_head->data, data)==0) {
            return prev->next=prev->next->next;

        }
        prev = list_head;
        list_head=list_head->next;
    }
}

// return pointer to element with given value
ht_element *get_element(hash_table *p_table, data_union *data) {
    size_t n = p_table->hash_function(*data, p_table->size);
    ht_element* el = p_table->ht[n].next;
    while(el!=NULL) {
        if(p_table->compare_data(el->data, *data)==0) {
            return el;
        }
        el = el->next;
    }
    return NULL;
}

// insert element
void insert_element(hash_table *p_table, data_union *data) {

    size_t n = p_table->hash_function(*data, p_table->size);
    ht_element* list_head = p_table->ht[n].next;
    while(list_head != NULL) {
        if(p_table->compare_data(list_head->data, *data)==0) {
            if(p_table->modify_data != NULL) {
                p_table->modify_data(&list_head->data.ptr_data);
            }

            return;
        }
        list_head=list_head->next;
    }
    ht_element *new_element = malloc(sizeof(ht_element));
    new_element->data = *data;
    new_element->next = p_table->ht[n].next;
    p_table->ht[n].next = new_element;
    p_table->no_elements++;
    if(p_table->no_elements / p_table->size > MAX_RATE) {
        rehash(p_table);
    }
}


// remove element
void remove_element(hash_table *p_table, data_union data) {
    size_t n = p_table->hash_function(data, p_table->size);
    ht_element* list_head = p_table->ht[n].next;
    ht_element* prev= &p_table->ht[n];
    while(list_head != NULL) {
        if(p_table->compare_data(list_head->data, data)==0) {
            prev->next=prev->next->next;
            p_table->no_elements--;
            free_element(p_table->free_data,list_head);
            return;
        }
        prev = list_head;
        list_head=list_head->next;
    }
}

// type-specific definitions

// int element

size_t hash_int(data_union data, size_t size) {
    return hash_base(data.int_data, size);
}

void dump_int(data_union data) {
    printf("%d ", data.int_data);
}

int cmp_int(data_union a, data_union b) {
    if(a.int_data < b.int_data) {
        return 1;
    } else if (a.int_data > b.int_data) {
        return -1;
    }
    return 0;
}

data_union create_int(void* value) {
    int val;
    scanf(" %d", &val);
    data_union* data = malloc(sizeof(data_union));
    data->int_data = val;
    if(value != NULL) {
        value = data;
    }
    return *data;



}
// char element
size_t hash_char(data_union data, size_t size) {
    return hash_base((int)data.char_data, size);
}

void dump_char(data_union data) {
    printf("%c ", data.char_data);
}

int cmp_char(data_union a, data_union b) {
    if(a.char_data == b.char_data) {
        return 0;
    }
    return -1;
}

data_union create_char(void* value) {
    char c;
    scanf(" %c", &c);
    data_union* data = malloc(sizeof(data_union));
    data->char_data = c;
    if(value!=NULL) {
        value=data;
    }
    return *data;
}

// Word element



void dump_word(data_union data) {
    DataWord *word = (DataWord*) data.ptr_data;
    printf("%s %d ", word->word, word->counter);
}

void free_word(data_union data) {
    free(data.ptr_data);
}

int cmp_word(data_union a, data_union b) {
    DataWord *word_1 = (DataWord*) a.ptr_data;
    DataWord *word_2 = (DataWord*) b.ptr_data;
//    printf("%s ", *word_1);
//    printf("%s \n", *word_2);
    return strcmp(word_1->word, word_2->word);
}

size_t hash_word(data_union data, size_t size) {
    int s = 0;
    DataWord *dw = (DataWord*)data.ptr_data;
    char *p = dw->word;
    while (*p) {
        s += *p++;
    }
    return hash_base(s, size);
}

void modify_word(data_union *data) {
    DataWord *word = (DataWord*)data->ptr_data;
    word->counter++;

}

data_union create_data_word(void *value) {
    data_union *word = malloc(sizeof(data_union));
    DataWord *data = malloc(sizeof(DataWord));
    data->word = strdup(value);
    data->counter = 1;
    word->ptr_data = data;
    return *word;
}

// read text, parse it to words, and insert these words to the hashtable
void stream_to_ht(hash_table *p_table, FILE *stream) {
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
            data_union word = p_table->create_data(buff);
            DataWord *data = (DataWord*)word.ptr_data;
            for (int i = 0; i < strlen(data->word); i++)
            {
                data->word[i] = tolower(data->word[i]);
            }
            insert_element(p_table, &word);


            buff = strtok( NULL, word_separators );
        }
    }
}

// test primitive type list
void test_ht(hash_table *p_table, int n) {
    char op;
    data_union data;
    for (int i = 0; i < n; ++i) {
        scanf(" %c", &op);
//		p_table->create_data(&data);
        data = p_table->create_data(NULL); // should give the same result as the line above
        switch (op) {
            case 'r':
                remove_element(p_table, data);
                break;
            case 'i':
                insert_element(p_table, &data);
                break;
            default:
                printf("No such operation: %c\n", op);
                break;
        }
    }
}

int main(void) {
    int to_do, n;
    size_t index;
    hash_table table;
    char buffer[BUFFER_SIZE];
    data_union data;

    scanf ("%d", &to_do);
    switch (to_do) {
        case 1: // test integer hash table
            scanf("%d %zu", &n, &index);
            init_ht(&table, 4, dump_int, create_int, NULL, cmp_int, hash_int, NULL);
            test_ht(&table, n);
            printf ("%zu\n", table.size);
            dump_list(&table, index);
            break;
        case 2: // test char hash table
            scanf("%d %zu", &n, &index);
            init_ht(&table, 4, dump_char, create_char, NULL, cmp_char, hash_char, NULL);
            test_ht(&table, n);
            printf ("%zu\n", table.size);
            dump_list(&table, index);
            break;
        case 3: // read words from text, insert into hash table, and print
            scanf("%s", buffer);
            init_ht(&table, 8, dump_word, create_data_word, free_word, cmp_word, hash_word, modify_word);
            stream_to_ht(&table, stdin);
            printf ("%zu\n", table.size);
            data = table.create_data(buffer);
            ht_element *e = get_element(&table, &data);
            if (e) table.dump_data(e->data);
            if (table.free_data) table.free_data(data);
            break;
        default:
            printf("NOTHING TO DO FOR %d\n", to_do);
            break;
    }
    free_table(&table);

    return 0;
}

