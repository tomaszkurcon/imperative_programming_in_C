#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#define TAB_SIZE  1000
#define BUF_SIZE  1000

int get(int cols, int row, int col, const int *A) {
}

void set(int cols, int row, int col, int *A, int value) {
}

void prod_mat(int rowsA, int colsA, int colsB, int *A, int *B, int *AB) {
    int indeks = 0;
    for(int i=0; i<rowsA; i++) {
        int sum = 0;
        for(int j=0; j<colsB; j++) {
            while(indeks<colsA) {
                sum+=(*(A+i*colsA+indeks))*(*(B+j+indeks*colsB));
                indeks+=1;
                //
            }
            *(AB+i*rowsA+j) = sum;
            //
            sum=0;
            indeks=0;
        }

    }
}

void read_mat(int rows, int cols, int *t) {
    for(int i=0; i<rows; i++) {
        for(int j=0; j<cols; j++) {
            scanf("%d", &t[i*cols+j]);
//            printf("%d \n", t[i*cols+j]);
        }
    }

}

void print_mat(int rows, int cols, int *t) {
    for(int i=0; i<rows; i++) {
        for(int j=0; j<cols; j++) {
            printf("%d ", t[i*cols+j]);
        }
        printf("\n");
    }
}

int read_char_lines(char *array[]) {
    char c_buf[BUF_SIZE];
    int counter = 0;
    while(fgets(c_buf, BUF_SIZE, stdin) != NULL) {
        if (c_buf[0] == '\n') {
            return counter;
        }

        array[counter] = malloc(BUF_SIZE * sizeof(char));
        strcpy(array[counter], c_buf);
        counter++;
    }
    return counter;
}

void write_char_line(char *array[], int n) {
    printf("%s", array[n]);
}

void delete_lines(char *array[], int n) {

}

int read_int_lines_cont(int *ptr_array[]) {
    char c_buf[BUF_SIZE];
    int counter = 0;
    int j=0;
    int first;
    while(fgets(c_buf, BUF_SIZE, stdin) != NULL) {

        if (c_buf[0] == '\n') {
            return counter;
        }
        first = 1;

        char* liczba_str = strtok(c_buf, " ");
        while (liczba_str != NULL) {

            ptr_array[0][j] = atoi(liczba_str);
            if(first == 1 && counter > 0) {
                ptr_array[counter] =  &ptr_array[0][j];
                first = 0;
            }
            liczba_str = strtok(NULL, " ");
            j++;
        }

        counter++;
    }
    return counter;

}

void write_int_line_cont(int *ptr_array[], int n) {

    int i = 0;
    while (ptr_array[n]+i<ptr_array[n+1]) {
        printf("%d ", *(ptr_array[n]+i));
        i++;
    }

}


typedef struct {
    int *values;
    int len;
    double average;
} line_type;

int read_int_lines(line_type lines_array[]) {
    char c_buf[BUF_SIZE];
    int counter = 0;
    int j=0;
    int amount = 0;
    double sum = 0;
    int *values[TAB_SIZE];

    while(fgets(c_buf, BUF_SIZE, stdin) != NULL) {
        if (c_buf[0] == '\n') {
            return counter;
        }
        line_type line;

        char* liczba_str = strtok(c_buf, " ");
        values[counter] = malloc(BUF_SIZE * sizeof(int));
        while (liczba_str != NULL) {
            amount++;
            sum+=atoi(liczba_str);
            values[counter][j] = atoi(liczba_str);

            liczba_str = strtok(NULL, " ");
            j++;
        }

        line.average = sum/amount;
        line.len = amount;
        line.values = values[counter];
        lines_array[counter] = line;
        amount = 0;
        sum = 0;
        j=0;
        counter++;
    }
    return counter;
}

void write_int_line(line_type lines_array[], int n) {
    int i = 0;
    while (lines_array[n].len > i) {
        printf("%d ", *(lines_array[n].values+i));
        i++;
    }

    printf("\n");
    printf("%.2f", lines_array[n].average);
}

void delete_int_lines(line_type array[], int line_count) {
}

int cmp (const void *a, const void *b) {
}
int comparator(const void *x, const void*y) {
    const line_type line_a = *(const line_type *)x;
    const line_type line_b = *(const line_type *)y;
    if(line_a.average > line_b.average) {
        return 1;
    } else {
        return -1;
    }
}
void sort_by_average(line_type lines_array[], int line_count) {
    qsort(lines_array, line_count, sizeof(line_type), comparator);

}

typedef struct {
    int r, c, v;
} triplet;

int read_sparse(triplet *triplet_array, int n_triplets) {
    for(int i=0; i<n_triplets; i++) {
        triplet triple;
        scanf("%d %d %d",   &triple.r , &triple.c, &triple.v);
        triplet_array[i]=triple;
    }
    return n_triplets;
}
int cmp_triplets(const void *x, const void*y) {
    const triplet triplet_a = *(const triplet *)x;
    const triplet triplet_b = *(const triplet *)y;
    if(triplet_a.r > triplet_b.r) {
        return 1;
    } else if (triplet_a.r == triplet_b.r) {
        if(triplet_a.c > triplet_b.c) {
            return 1;
        } else {
            return -1;
        }
    } else {
        return -1;
    }
}

void make_CSR(triplet *triplet_array, int n_triplets, int rows, int *V, int *C, int *R) {
    qsort(triplet_array, n_triplets, sizeof(triplet), cmp_triplets);
    int tab[rows+1];
    for(int i=0; i<rows+1; i++) {
        tab[i]=0;
    }
    for(int i=0; i<n_triplets; i++) {

        V[i]=triplet_array[i].v;
        C[i]=triplet_array[i].c;
        tab[triplet_array[i].r]++;

    }
    int sum =0;
    R[0]=0;
    for(int i=1; i<rows+1; i++) {
        sum=0;
        for(int j=0; j<i; j++) {
            sum+=tab[j];
        }
        R[i]=sum;
    }


}

void multiply_by_vector(int rows, const int *V, const int *C, const int *R, const int *x, int *y) {
    for(int i=0; i<rows; i++) {
        y[i]=0;
        for(int j=R[i]; j<R[i+1]; j++) {
            y[i]+=V[j]*x[C[j]];
        }
    }
}

void read_vector(int *v, int n) {
    for(int i=0; i<n; i++) {
        scanf("%d", &v[i]);
    }
}

void write_vector(int *v, int n) {
    for(int i=0; i<n; i++) {
        printf("%d ", v[i]);
    }
    printf("\n");
}

int read_int() {
    char c_buf[BUF_SIZE];
    fgets(c_buf, BUF_SIZE, stdin);
    return (int)strtol(c_buf, NULL, 10);
}

int main(void) {
    int tab[5][3];
    int to_do = read_int();

    int A[TAB_SIZE], B[TAB_SIZE], AB[TAB_SIZE];
    int n, lines_counter, rowsA, colsA, rowsB, colsB;
    int rows, cols, n_triplets;
    char *char_lines_array[TAB_SIZE] = { NULL };
    int continuous_array[TAB_SIZE];
    int *ptr_array[TAB_SIZE];
    triplet triplet_array[TAB_SIZE];
    int V[TAB_SIZE], C[TAB_SIZE], R[TAB_SIZE];
    int x[TAB_SIZE], y[TAB_SIZE];
    line_type int_lines_array[TAB_SIZE];

    switch (to_do) {
        case 1:
            scanf("%d %d", &rowsA, &colsA);
            read_mat(rowsA, colsA, A);
            scanf("%d %d", &rowsB, &colsB);
            read_mat(rowsB, colsB, B);
            prod_mat(rowsA, colsA, colsB, A, B, AB);
            print_mat(rowsA, colsB, AB);
            break;
        case 2:
            n = read_int() - 1; // we count from 1 :)
            ptr_array[0] = continuous_array;
            read_int_lines_cont(ptr_array);
            write_int_line_cont(ptr_array, n);
            break;
        case 3:
            n = read_int() - 1;
            read_char_lines(char_lines_array);
            write_char_line(char_lines_array, n);
            delete_lines(char_lines_array, n);
            break;
        case 4:
            n = read_int() - 1;
            lines_counter = read_int_lines(int_lines_array);
            sort_by_average(int_lines_array, lines_counter);
            write_int_line(int_lines_array, n);
            delete_int_lines(int_lines_array, lines_counter);
            break;
        case 5:
            scanf("%d %d %d", &rows, &cols, &n_triplets);
            n_triplets = read_sparse(triplet_array, n_triplets);
            read_vector(x, cols);
            make_CSR(triplet_array, n_triplets, rows, V, C, R);
            multiply_by_vector(rows, V, C, R, x, y);
            write_vector(V, n_triplets);
            write_vector(C, n_triplets);
            write_vector(R, rows + 1);
            write_vector(y, rows);
            break;
        default:
            printf("NOTHING TO DO FOR %d\n", to_do);
            break;
    }
    return 0;
}