#include <stdio.h>
#include <math.h>

#define SIZE 40

void read_vector(double x[], int n) {
	for(int i = 0; i < n; ++i) {
		scanf("%lf", x++);
	}
}

void print_vector(double x[], int n) {
	for(int i = 0; i < n; ++i) {
		printf("%.4f ", x[i]);
	}
	printf("\n");
}

void read_mat(double A[][SIZE], int m, int n) {
	for(int i = 0; i < m; ++i) {
		for(int j = 0; j < n; ++j) {
			scanf("%lf", &A[i][j]);
		}
	}
}

void print_mat(double A[][SIZE], int m, int n) {
	for(int i = 0; i < m; ++i) {
		for(int j = 0; j < n; ++j) {
			printf("%.4f ", A[i][j]);
		}
		printf("\n");
	}
}

// 1. Calculate matrix product, AB = A X B
// A[m][p], B[p][n], AB[m][n]
void mat_product(double A[][SIZE], double B[][SIZE], double AB[][SIZE], int m, int p, int n) {
    double element = 0;
    int indeks=0;
    for(int i = 0; i<m; i++){
        for(int j=0; j<n; j++) {
            while(indeks<=m) {
                element=element+A[i][indeks]*B[indeks][j];
                indeks+=1;
            }
            AB[i][j]=element;
            element=0;
            indeks=0;
        }
    }
}


// 2. Matrix triangulation and determinant calculation - simplified version
// (no rows' swaps). If A[i][i] == 0, function returns NAN.
// Function may change A matrix elements.
double gauss_simplified(double A[][SIZE], int n) {
    double product = 0;
    for(int i=0; i<n-1; i++) {
        if(A[i][i]==0) {
            return NAN;
        }

        for(int j=i+1; j<n; j++) {
            product=A[j][i]/A[i][i];
            for(int k=0; k<n; k++) {
                A[j][k]=A[j][k]-product*A[i][k];
            }
        }
    }
    double determinant = 1;
    for(int i=0; i<n; i++) {
        determinant=determinant*A[i][i];
    }
    return determinant;
}

void backward_substitution_index(double A[][SIZE], const int indices[], double x[], double b[], int n) {

    double to_substract = 0;
    for(int i = n-1; i>=0; i--) {
        for(int j = i+1; j<n; j++) {
            to_substract+=A[indices[i]][j]*x[j];
        }
        x[i]=(b[indices[i]]-to_substract)/A[indices[i]][i];
        to_substract=0;
    }
}
void swap(int index_perm[], int index1, int index2){
    int temp = index_perm[index1];
    index_perm[index1]=index_perm[index2];
    index_perm[index2]=temp;

}
// 3. Matrix triangulation, determinant calculation, and Ax = b solving - extended version
// (Swap the rows so that the row with the largest, leftmost nonzero entry is on top. While
// swapping the rows use index vector - do not copy entire rows.)
// If max A[i][i] < eps, function returns 0.
// If det != 0 && b != NULL && x != NULL then vector x should contain solution of Ax = b.
void swap_rows(double A[][SIZE], int index_perm[], int i, int n, int *mark) {
    for(int w=i+1; w<n; w++) {
        if(fabs(A[index_perm[w]][i])> fabs(A[index_perm[i]][i])) {
            swap(index_perm, w, i);
            (*mark)*=-1;
        }
    }

}
double gauss(double A[][SIZE], double b[], double x[], const int n, const double eps) {
    int index_perm[n];
    double product = 0;
    int mark =1 ;
    for(int i =0; i<n; i++) {
        index_perm[i]=i;
    }
    for(int i=0; i<n-1; i++) {
            swap_rows(A, index_perm, i, n, &mark);
            if(fabs(A[index_perm[i]][i])<eps) {
                return 0;
            }
        for(int j=i+1; j<n; j++) {
            product=A[index_perm[j]][i]/A[index_perm[i]][i];
            b[index_perm[j]] = b[index_perm[j]]-product*b[index_perm[i]];
            for(int k=i; k<n; k++) {
                A[index_perm[j]][k]=A[index_perm[j]][k]-product*A[index_perm[i]][k];

            }
        }

    }

    backward_substitution_index(A, index_perm, x,b, n);
    double determinant = 1;
    for(int i=0; i<n; i++) {
        determinant=determinant*A[index_perm[i]][i];
    }
    return (double)mark*determinant;

}

// 4. Returns the determinant; B contains the inverse of A (if det(A) != 0)
// If max A[i][i] < eps, function returns 0.
double matrix_inv(double A[][SIZE], double B[][SIZE], int n, double eps) {
    double divisors[n];
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            if(i==j) {
                B[i][j]=1;
            }
            else {
                B[i][j]=0;
            }

        }
    }
    int index_perm[n];
    double product = 0;
    int mark = 1;
    for (int i = 0; i < n; i++) {
        index_perm[i] = i;
    }
    for (int i = 0; i < n - 1; i++) {
        swap_rows(A, index_perm, i, n, &mark);
        if (fabs(A[index_perm[i]][i]) < eps) {
            return 0;
        }
        for (int j = i + 1; j < n; j++) {
            product = A[index_perm[j]][i] / A[index_perm[i]][i];
            for (int k = 0; k < n; k++) {
                A[index_perm[j]][k] = A[index_perm[j]][k] - product * A[index_perm[i]][k];
                B[index_perm[j]][k] = B[index_perm[j]][k] - product * B[index_perm[i]][k];

            }

        }

    }
    for(int i = n-1; i>0; i--) {
        for(int j=i-1; j>=0; j--) {
            //
            product=A[index_perm[j]][i]/A[index_perm[i]][i];
            for(int k = n-1; k>=0; k--) {
                //
                A[index_perm[j]][k]=A[index_perm[j]][k]-product*A[index_perm[i]][k];
                B[index_perm[j]][k] = B[index_perm[j]][k] - product * B[index_perm[i]][k];

            }

        }
    }


    for(int i =0; i<n; i++) {
        for(int j=0; j<n; j++) {
            B[index_perm[i]][j]=B[index_perm[i]][j]/A[index_perm[i]][i];
        }
    }
    double D[SIZE][SIZE];
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            D[i][j]=B[i][j];
        }
    }
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            B[i][j]=D[index_perm[i]][j];
        }
    }
    double determinant = 1;
    for(int i=0; i<n; i++) {
        determinant=determinant*A[index_perm[i]][i];
    }
    return (double)mark*determinant;
}

int main(void) {

	double A[SIZE][SIZE], B[SIZE][SIZE], C[SIZE][SIZE];
	double b[SIZE], x[SIZE], det, eps = 1.e-13;

	int to_do;
	int m, n, p;

	scanf ("%d", &to_do);

	switch (to_do) {
		case 1:
			scanf("%d %d %d", &m, &p, &n);
			read_mat(A, m, p);
			read_mat(B, p, n);
			mat_product(A, B, C, m, p, n);
			print_mat(C, m, n);
			break;
		case 2:
			scanf("%d", &n);
			read_mat(A, n, n);
			printf("%.4f\n", gauss_simplified(A, n));
			break;
		case 3:
			scanf("%d", &n);
			read_mat(A,n, n);
			read_vector(b, n);
			det = gauss(A, b, x, n, eps);
			printf("%.4f\n", det);
			if (det) print_vector(x, n);
			break;
		case 4:
			scanf("%d", &n);
			read_mat(A, n, n);
			det = matrix_inv(A, B, n, eps);
			printf("%.4f\n", det);
			if (det) print_mat(B, n, n);
			break;
		default:
			printf("NOTHING TO DO FOR %d\n", to_do);
			break;
	}
	return 0;
}

