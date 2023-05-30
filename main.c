#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

#define N 52
int rand_from_interval(int a, int b) {
    if (a > b) return INT_MIN;
    if (b - a > RAND_MAX ) return INT_MAX;
    if (a == b) return a;
    return rand()%(b-a+1)+a;
}

void swap (int array[], int i, int k) {
    int buffor = array[i];
    array[i]=array[k];
    array[k]=buffor;
}


void rand_permutation(int n, int array[]) {
    int k;
    for(int i =0; i<n; i++) {
        array[i]=i;
    }
    for(int i=0; i<n-1; i++){
        k= rand_from_interval(i, n-1);
        swap(array, i, k);
    }
}


//CBUFF

int taliaA[N];
int taliaB[N];
int wojna[N];
int outA = 0, lenA = 0, outB=0, lenB=0, outW=0, lenW=0;


int cbuff_push(int cli_nr, char player) {
    if(player=='A') {
        int indeks = (outA+lenA)%52;
        lenA++;
        taliaA[indeks]=cli_nr;
        return 1;
    }
    else if(player=='B') {
        int indeks = (outB+lenB)%52;
        lenB++;
        taliaB[indeks]=cli_nr;
        return 1;
    }
    else {
        int indeks = (outW+lenW)%52;
        lenW++;
        wojna[indeks]=cli_nr;
        return 1;
    }

}

int cbuff_pop(char player) {
    if(player=='A') {
        lenA--;
        outA=(outA+1)%52;
    }
    else if(player=='B') {
        lenB--;
        outB=(outB+1)%52;
    }
    else {
        lenW--;
        outW=(outW+1)%52;
    }
}



void cbuff_print(int len, int out, int cbuff[]) {
    for(int i=0; i <len; i++) {
        printf("%d ", cbuff[(out+i)%52]);
    }
}

int main() {
    int to_do, seed, conflict_number;
    int n=52;
    int array[N];
    int counter = 0;
    scanf("%d %d %d", &seed, &to_do, &conflict_number);
    srand((unsigned) seed); // set the seed
    rand_permutation(52, array);
    for(int i=0; i<26; i++ ) {
        cbuff_push(array[i], 'A');
    }
    for(int i=26; i<52; i++) {
        cbuff_push(array[i], 'B');
    }
    switch(to_do) {
        case 0:
            while(counter<conflict_number && lenA>0 && lenB > 0) {
                if(taliaA[outA]/4 > taliaB[outB]/4) {
                    counter++;
                    int card_A=taliaA[outA];
                    cbuff_pop('A');
                    cbuff_push(card_A, 'A');
                    cbuff_push(taliaB[outB], 'A');
                    cbuff_pop('B');
                }
                else if(taliaA[outA]/4 < taliaB[outB]/4) {
                    counter++;
                    int card_B=taliaB[outB];
                    cbuff_pop('B');
                    cbuff_push(card_B, 'B');
                    cbuff_push(taliaA[outA], 'B');
                    cbuff_pop('A');
                }
                else {
                    bool isCover = false;
                    while(!(!isCover && taliaA[outA]/4 != taliaB[outB]/4 || lenA==0 || lenB == 0 || counter>=conflict_number)) {
                        if(isCover) {
                            isCover=false;
                        }
                        else {
                            isCover=true;
                            counter++;
                        }
                        cbuff_push(taliaA[outA], 'W');
                        cbuff_push(taliaB[outB], 'W');
                        cbuff_pop('A');
                        cbuff_pop('B');
                    }

                    if(!(counter<=conflict_number)) {
                        for(int i=0; i<lenW; i=i+2) {
                            cbuff_push(wojna[(outW+i)%52], 'A');
                        }
                        for(int i=1; i<lenW; i=i+2) {
                            cbuff_push(wojna[(outW+i)%52], 'B');
                        }
                        printf("%d %d %d", 0, lenA, lenB);
                        return 0;
                    }
                    if (lenA==0 || lenB == 0) {
                        for(int i=0; i<lenW; i=i+2) {
                            cbuff_push(wojna[(outW+i)%52], 'A');
                        }
                        for(int i=1; i<lenW; i=i+2) {
                            cbuff_push(wojna[(outW+i)%52], 'B');
                        }
                        printf("%d %d %d", 1, lenA, lenB);
                        return 0;

                    }
                    cbuff_push(taliaA[outA], 'W');
                    cbuff_push(taliaB[outB], 'W');
                    counter++;
                    if(taliaA[outA]/4 > taliaB[outB]/4) {
                        cbuff_pop('A');
                        cbuff_pop('B');
                        for(int i=0; i<lenW; i=i+2) {
                            cbuff_push(wojna[(outW+i)%52], 'A');
                        }
                        for(int i=1; i<lenW; i=i+2) {
                            cbuff_push(wojna[(outW+i)%52], 'A');
                        }
                    }
                    else if(taliaA[outA]/4 < taliaB[outB]/4) {
                        cbuff_pop('A');
                        cbuff_pop('B');
                        for(int i=1; i<lenW; i=i+2) {
                            cbuff_push(wojna[(outW+i)%52], 'B');
                        }
                        for(int i=0; i<lenW; i=i+2) {
                            cbuff_push(wojna[(outW+i)%52], 'B');
                        }
                    }
                    while(lenW != 0) {
                        cbuff_pop('W');
                    }
                }
            }
            if(counter>=conflict_number) {
                printf("%d %d %d", 0, lenA, lenB);
                return 0;
            }
            if(lenB==0) {
                printf("%d %d", 2, counter);

            } else if(lenA==0) {
                printf("%d\n", 3);
                cbuff_print(lenB, outB, taliaB);
            }
            break;
        case 1:
            while(counter<=conflict_number && lenA>0 && lenB > 0) {
                if (taliaA[outA] / 4 > taliaB[outB] / 4) {
                    int card_A = taliaA[outA];
                    cbuff_pop('A');
                    cbuff_push(card_A, 'A');
                    cbuff_push(taliaB[outB], 'A');
                    cbuff_pop('B');
                    counter++;
                } else if (taliaA[outA] / 4 < taliaB[outB] / 4) {
                    int card_B = taliaB[outB];
                    cbuff_pop('B');
                    cbuff_push(card_B, 'B');
                    cbuff_push(taliaA[outA], 'B');
                    cbuff_pop('A');
                    counter++;
                } else {
                    int card_A = taliaA[outA];
                    int card_B = taliaB[outB];
                    cbuff_pop('A');
                    cbuff_pop('B');
                    cbuff_push(card_A, 'A');
                    cbuff_push(card_B, 'B');
                    counter++;
                }
            }
            if(counter>=conflict_number) {
                printf("%d %d %d", 0, lenA, lenB);
                return 0;
            }
            if(lenB==0) {
                printf("%d %d", 2, counter);

            } else if(lenA==0) {
                printf("%d\n", 3);
                cbuff_print(lenB, outB, taliaB);
            }

            break;
    }
    return 0;
}
