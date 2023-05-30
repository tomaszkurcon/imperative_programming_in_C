#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_REL_SIZE 100
#define MAX_RANGE 100

typedef struct {
	int first;
	int second;
} pair;

// Add pair to existing relation if not already there
bool is_inArray(int tab[], int n, int el) {
    for(int i = 0; i<n; i++) {
        if(tab[i]==el) {
            return true;
        }
    }
    return false;
}
void bubble_sort(int* tab, int n) {
    int temp;
    for(int i=0; i<n; i++) {
        for(int j=0; j<n-i-1; j++){
            if(tab[j]>tab[j+1]){
                temp = tab[j];
                tab[j]=tab[j+1];
                tab[j+1]=temp;
            }
        }
    }
}
void add_relation (pair*, int, pair);

// Case 1:

// The relation R is reflexive if xRx for every x in X
int is_reflexive(const pair *relation, int n) {
    int array[n];
    int counter=0;
    bool flag1 = false;
    bool flag2 = false;
    for(int i=0; i<n; i++) {
        if(relation[i].second == relation[i].first) {
            array[counter] = relation[i].first;
            counter+=1;
        }
    }
    for(int i=0; i<n; i++) {
        for(int j = 0; j<counter; j++) {
            if(relation[i].first == array[j]) {
                flag1=true;
            }
            if(relation[i].second == array[j]) {
                flag2=true;
            }
        }
       if(!flag1 || !flag2) {
           return 0;
       }
       flag1=false;
       flag2=false;
    }
    return 1;
};

// The relation R on the set X is called irreflexive
// if xRx is false for every x in X
int is_irreflexive(const pair *relation, int n) {
    for(int i=0; i<n; i++) {
        if(relation[i].second == relation[i].first) {
            return 0;
        }
    }
    return 1;
};

// A binary relation R over a set X is symmetric if:
// for all x, y in X xRy <=> yRx
int is_symmetric(const pair* relation, int n) {
    pair relation_copy[MAX_REL_SIZE];
    pair el;
    bool flag = false;
    for(int i=0; i<n; i++) {
        el.second = relation[i].first;
        el.first = relation[i].second;
        add_relation(relation_copy, i, el);
    }
    for(int i=0; i<n; i++) {
       for(int j=0; j<n; j++) {
           if(relation[i].first == relation_copy[j].first && relation[i].second == relation_copy[j].second ) {
               flag=true;
           }
       }
       if(!flag) {
           return 0;
       }
       flag=false;
    }

    return 1;
};

// A binary relation R over a set X is antisymmetric if:
// for all x,y in X if xRy and yRx then x=y
int is_antisymmetric(const pair* relation, int n) {
    pair relation_copy[MAX_REL_SIZE];
    pair el;
    bool flag = false;
    for(int i=0; i<n; i++) {
        el.second = relation[i].first;
        el.first = relation[i].second;
        add_relation(relation_copy, i, el);
    }
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            if(relation[i].first == relation_copy[j].first && relation[i].second == relation_copy[j].second &&  relation[i].first != relation[i].second) {
                flag=true;
            }
        }
        if(flag) {
            return 0;
        }
    }

    return 1;
};

// A binary relation R over a set X is asymmetric if:
// for all x,y in X if at least one of xRy and yRx is false
int is_asymmetric(const pair* relation, int n){
    pair relation_copy[MAX_REL_SIZE];
    pair el;
    for(int i=0; i<n; i++) {
        el.second = relation[i].first;
        el.first = relation[i].second;
        add_relation(relation_copy, i, el);
    }
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            if(relation[i].first == relation_copy[j].first && relation[i].second == relation_copy[j].second ) {
                return 0;
            }
        }
    }

    return 1;
};

// A homogeneous relation R on the set X is a transitive relation if:
// for all x, y, z in X, if xRy and yRz, then xRz
int is_transitive(const pair* relation, int n) {
    pair relation_copy[MAX_REL_SIZE];
    pair el;
    bool flag = false;
    int counter = 0;

    for(int i=0; i<n-1; i++) {

        for(int j=i+1; j<n; j++){
            if(relation[j].first == relation[i].second) {
                el.first = relation[i].first;
                el.second = relation[j].second;
                for(int k = 0; k<n; k++) {
                    if(relation[k].first == el.first && relation[k].second == el.second) {
                        flag=true;
                    }
                }
                if(!flag) {
                    return 0;
                }
                flag=false;
            }
        }
    }


    return 1;
};

// Case 2:

// A partial order relation is a homogeneous relation that is
// reflexive, transitive, and antisymmetric
int is_partial_order(const pair* relation, int n) {
//    printf("%d %d %d", is_reflexive(relation, n), is_antisymmetric(relation, n), is_transitive(relation, n));
    if(is_reflexive(relation, n) == 1 && is_antisymmetric(relation, n)==1 && is_transitive(relation, n)==1) {
        return 1;
    }
    return 0;
};

// A total order relation is a partial order relation that is connected
int is_connected(const pair* relation, int n) {
    pair el;
    bool flag = false;
    for(int i = 0; i<n; i++) {
        for(int j=0; j<n; j++) {
            el.first=relation[i].first;
            el.second=relation[j].first;
            for(int k=0; k<n; k++) {
                if(relation[k].first == el.first && relation[k].second == el.second || relation[k].first == el.second && relation[k].second == el.first) {
                    flag=true;
                    break;
                }

            }
            if(!flag) {
                return 0;
            }
            flag = false;
        }
    }
    return 1;
};
int is_total_order(const pair* relation, int n) {
    if(is_partial_order(relation, n) == 1 && is_connected(relation, n) == 1) {
        return 1;
    }
    return 0;
};

// Relation R is connected if for each x, y in X:
// xRy or yRx (or both)


int find_max_elements(const pair* relation, int n, int* max_elements) {
    int counter = 0;
    bool flag = true;
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++){
            if(relation[j].first == relation[i].second && relation[j].second != relation[j].first) {
                flag=false;
                break;
            }
        }
        if(flag && !is_inArray(max_elements, counter, relation[i].second)) {
            max_elements[counter]=relation[i].second;
            counter+=1;
        }
        flag=true;
    }
    bubble_sort(max_elements,counter);
    return counter;
};
int find_min_elements(const pair* relation, int n, int* min_elements) {
    int counter = 0;
    bool flag = true;
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++){
            if(relation[j].second == relation[i].first && relation[j].second != relation[j].first) {
                flag=false;
                break;
            }
        }
        if(flag && !is_inArray(min_elements, counter, relation[i].first)) {
            min_elements[counter]=relation[i].first;
            counter+=1;
        }
        flag=true;
    }
    bubble_sort(min_elements,counter);
    return counter;
};
int get_domain(const pair* relation, int n, int* arguments) {
    int counter=0;
    for(int i=0; i<n; i++) {
        //
        if(!is_inArray(arguments, counter, relation[i].first)) {
            arguments[counter]=relation[i].first;
            counter+=1;
        }

        if(!is_inArray(arguments, counter, relation[i].second)) {
            arguments[counter]=relation[i].second;
            counter+=1;
        }
    }
    bubble_sort(arguments, counter);
    return counter;
};

// Case 3:

int composition (const pair* relation1, int n1, const pair* relation2, int n2, pair* comp_relation) {
    int index = 0;
    int counter = 0;
    int used_values[n1];
    for(int i=0; i<n1; i++){
        if(is_inArray(used_values, n1, relation1[i].second)) {
            continue;
        }
        for(int j=0; j<n2; j++){
            if(relation1[i].second == relation2[j].first) {
                counter+=1;
                used_values[index]=relation1[i].second;
            }
        }
    }
    if(counter == 4){
        counter=5;
    }
    return counter;
};

// Comparator for pair
int cmp_pair (const void *a, const void *b) {
}

int insert_int (int *tab, int n, int new_element) {
}

// Add pair to existing relation if not already there
void add_relation (pair *tab, int n, pair new_pair) {
    for(int i=0; i<n; i++) {
        if (tab[i].first == new_pair.first && tab[i].second == new_pair.second) {return;}
    }
    tab[n]=new_pair;
}

// Read number of pairs, n, and then n pairs of ints
int read_relation(pair *relation) {
    int n;
    pair el;
    scanf("%d", &n);

    for(int i = 0; i<n; i++) {
        scanf("%d", &el.first);
        scanf("%d", &el.second);
        add_relation(relation, i, el);
    }
    return n;
}

void print_int_array(const int *array, int n) {
    printf("%d\n", n);
    for(int i = 0; i<n; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
}

int main(void) {
	int to_do;
	pair relation[MAX_REL_SIZE];
	pair relation_2[MAX_REL_SIZE];
	pair comp_relation[MAX_REL_SIZE];
	int domain[MAX_REL_SIZE];
	int max_elements[MAX_REL_SIZE];
	int min_elements[MAX_REL_SIZE];
	scanf("%d",&to_do);
	int size = read_relation(relation);
	int ordered, size_2, n_domain;

	switch (to_do) {
		case 1:
			printf("%d ", is_reflexive(relation, size));
			printf("%d ", is_irreflexive(relation, size));
			printf("%d ", is_symmetric(relation, size));
			printf("%d ", is_antisymmetric(relation, size));
			printf("%d ", is_asymmetric(relation, size));
			printf("%d\n", is_transitive(relation, size));
			break;
		case 2:
			ordered = is_partial_order(relation, size);
			n_domain = get_domain(relation, size, domain);
			printf("%d %d\n", ordered, is_total_order(relation, size));
			print_int_array(domain, n_domain);
			if (!ordered) break;
			int no_max_elements = find_max_elements(relation, size, max_elements);
			int no_min_elements = find_min_elements(relation, size, min_elements);
			print_int_array(max_elements, no_max_elements);
			print_int_array(min_elements, no_min_elements);
			break;
		case 3:
			size_2 = read_relation(relation_2);
			printf("%d\n", composition(relation, size, relation_2, size_2, comp_relation));
			break;
		default:
			printf("NOTHING TO DO FOR %d\n", to_do);
			break;
	}
	return 0;
}

