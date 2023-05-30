#include <stdio.h>
#include <math.h>

#define RECURS_LEVEL_MAX  10
#define N_MAX             10

// pointer to function of one variable
typedef double (*Func1vFp) (double);

// example functions of one variable
double f_poly(double x) { // polynomial a[0] + a[1]x + ... + a[n]x^n
//    printf("%.5f in poly\n", 2*pow(x,5)-4*pow(x,4)+3,5*pow(x,2)+1,35*x-6,25);
    return 2*pow(x,5)-4*pow(x,4)+3.5*pow(x,2)+1.35*x-6.25;
}

double f_rat(double x) {
    return 1/(pow((x-0.5), 2)+0.01);
}

double f_exp(double x) {
    return 2*x*exp((-1.5*x))-1;
}

double f_trig(double x) {
    return x*tan(x);
}

// Quadratures

// rectangle rule, leftpoint
double quad_rect_left(Func1vFp f1, double a, double b, int n) {
    double sum = 0;
    double length = (b-a)/n;
    for(int i=0; i<n; i++) {
        sum+=length*f1(a+i*length);
    }


    return sum;
}

// rectangle rule, rightpoint
double quad_rect_right(Func1vFp f1, double a, double b, int n) {
    double sum = 0;
    double length = (b-a)/n;
    for(int i=1; i<=n; i++) {
        sum+=length*f1(a+i*length);
    }

    return sum;
}

// rectangle rule, midpoint
double quad_rect_mid(Func1vFp f1, double a, double b, int n) {
    double sum = 0;
    double length = (b-a)/n;
    for(int i=0; i<n; i++) {
        sum+=length*f1(a+length/2);
        a=a+length;
    }
	return sum;
}

// trapezoidal rule
double quad_trap(Func1vFp func, double a, double b, int n) {
    double sum = 0;
    double length = (b-a)/n;
    double fa, fb;
    fa=func(a);
    for(int i=0; i<n; i++) {
        fb=func(a+length);
        sum+=length*(fa+fb)/2;
        fa=fb;
        a+=length;
    }

    return sum;
}

// Simpson's rule
double quad_simpson(Func1vFp f, double a, double b, int n) {
    double sum = 0;
    double length = (b-a)/n;
    double fa, fb;
    fa=f(a);
    for(int i=0; i<n; i++) {
        fb=f(a+length);
        sum+=length*(fa+fb+4*f(a+length/2))/6;
        fa=fb;
        a+=length;
    }
    return sum;
}

// pointer to quadrature function
typedef double (*QuadratureFp) (Func1vFp, double, double, int);

// array of pointers to integrand functions
Func1vFp func_tab[] = { f_poly, f_rat, f_trig, f_exp };

// array of pointers to quadrature functions
QuadratureFp quad_tab[] = {
	quad_rect_left, quad_rect_right, quad_rect_mid, quad_trap, quad_simpson };

// calls 'quad_no' quadrature function for 'fun_no' integrand function
// on interval [a, b] and n subintervals
double quad_select(int fun_no, int quad_no, double a, double b, int n) {
    return quad_tab[quad_no](func_tab[fun_no], a, b, n);
}

// adaptive algorithm
double recurs(Func1vFp f, double a, double b, double S, double delta, QuadratureFp quad, int level) {
    //
    if(level > RECURS_LEVEL_MAX) {
        return NAN;
    }
    double s1 = quad(f,  a, (a+b)/2, 1);
    double s2 = quad(f,  (a+b)/2, b, 1);
    if(fabs(s1+s2-S) < delta) {
        return s1+s2;
    }
    else {
        s1 = recurs(f, a, (a+b)/2, s1, delta/2, quad, level+1);
        s2 = recurs(f, (a+b)/2, b, s2, delta/2, quad, level+1);
        return s1 + s2;
    }
}

// initialization for adaptive algorithm
double init_recurs(Func1vFp f, double a, double b, double delta, QuadratureFp quad) {
    double S = quad(f, a, b, 1);
    return recurs(f, a, b, S, delta, quad, 1);
}

// double integrals

// pointer to function of two variables
typedef double Func2vFp (double, double);

double func2v_2(double x, double y) {
	return 2 - x*x - y*y*y;
}

// sample functions to define the normal domain

double lower_bound2(double x) {
	return 0.7*exp(-2*x*x);
}
double upper_bound2(double x) {
	return sin(10*x);
}

// rectangle rule (leftpoint) - double integral over rectangular domain
double dbl_integr(Func2vFp f, double x1, double x2, int nx, double y1, double y2, int ny) {
   double lengthX = (x2-x1)/nx;
   double lengthY = (y2-y1)/ny;
   double y1_copy = y1;
   double sum = 0;
   for(int i=0; i<nx; i++) {
       y1=y1_copy;
       for(int j=0; j<ny; j++) {
           sum+=f(x1,y1);
           y1+=lengthY;
       }
       x1+=lengthX;
   }
   return lengthX*lengthY*sum;
}



// rectangle rule (midpoint) - double integral over normal domain with respect to the x-axis
double dbl_integr_normal_1(Func2vFp f, double x1, double x2, int nx, double hy,
						   Func1vFp fg, Func1vFp fh) {
   double lengthX = (x2-x1)/nx;
   double sum=0;
   int ny;
   double lengthY;
   for(int i=0; i<nx; i++) {
       double xi = x1+lengthX/2+i*lengthX;
       double startY = fg(xi);
       double endY = fh(xi);
       ny=ceil((endY-startY)/hy);
       lengthY=(endY-startY)/ny;
       for(int j=0; j<ny; j++) {
           double yi = startY+lengthY/2+j*lengthY;
           sum+=lengthX*lengthY*f(xi, yi);
       }

   }
   return sum;
}

// rectangle rule (leftpoint) - double integral over multiple normal
// domains with respect to the x-axis
double dbl_integr_normal_n(Func2vFp f, double x1, double x2, int nx, double y1, double y2,
		int ny, Func1vFp fg, Func1vFp fh)  {

    double lengthX = (x2-x1)/nx;
    double lengthY = (y2-y1)/ny;
    double sum=0;
    double endY;
    double startY;
    for(int i=0; i<nx; i++) {
        if(fh(x1)>fg(x1)) {
            if(y2<fh(x1)) {
                endY=y2;
            } else {
                endY=fh(x1);
            }
            if(y1<fg(x1)) {
                startY=fg(x1);
            } else {
                startY=y1;
            }
            double nj = ceil((endY-startY)/lengthY);
            double lengthY2 = (endY-startY)/nj;
            for(int j=0; j<nj; ++j) {
                sum+=f(x1, startY)*lengthY2*lengthX;
                startY+=lengthY2;
            }

        }
        x1+=lengthX;
        }

    return sum;
}

// multiple quadratures

typedef double (*FuncNvFp)(const double*, int);
typedef int (*BoundNvFp)(const double*, int);

// sample function of three variables
double func3v(const double v[], int n) {
	return v[0] - v[1] + 2*v[2];
}

// sample predicate in 3D
int bound3v(const double v[], int n) { // a cylinder
	return v[0] > 0 && v[0] < 0.5 && v[1]*v[1] + (v[2]-1)*(v[2]-1) < 1;
}

// sample function of n variables
double funcNv(const double v[], int n) {
	double fv = 1.;
	for (int i = 1; i < n; ++i) {
		fv += sin(i*v[i]);
	}
	return fv;
}
// sample n-dimensional predicate (n-dim hypersphere)
int boundNv(const double v[], int n) {
	double r = 0.0;
	for (int i = 0; i < n; ++i) r += (v[i]-1)*(v[i]-1);
	return r <= 1.;
}

// multiple integrals over a cuboid with predicate (if boundary != NULL)
// rectangular rule (rightpoint)
double trpl_quad_rect(FuncNvFp f, double variable_lim[][2], const int tn[], BoundNvFp boundary) {
  double sum =0;
  double lengthX = (variable_lim[0][1]-variable_lim[0][0])/tn[0];
  double lengthY =  (variable_lim[1][1]-variable_lim[1][0])/tn[1];
  double lengthZ =(variable_lim[2][1]-variable_lim[2][0])/tn[2];
  double x = variable_lim[0][0];
  double y = variable_lim[1][0];
  double y_start_copy = y;
  double z = variable_lim[2][0];
    double z_start_copy = z;
    for(int i =0; i<tn[0]; i++) {
        x+=lengthX;
        y=y_start_copy;
        for(int j=0; j<tn[1]; j++) {
            y+=lengthY;
            z=z_start_copy;
            for(int k=0; k<tn[2]; k++) {
                z+=lengthZ;
                double list[3] = {x, y, z};
                if(boundary) {
                    if(boundary(list, 3)) {
                        sum+=f(list,3);

                    }

                } else {
                    sum+=f(list,3);

                }


            }

        }

    }
    return sum*lengthX*lengthY*lengthZ;
}

// multiple integrals over a n-dim hypercuboid with predicate (if boundary != NULL)
// rectangular rule (midpoint)
void recur_quad_rect_mid(double *p_sum, FuncNvFp f, int variable_no, double t_variable[],
         double variable_lim[][2], const int tn[], int level, BoundNvFp boundary) {

    if(variable_no==level) {
        if(boundary) {
            if(boundary(t_variable, variable_no)) {
                //
                (*p_sum)+=f(t_variable, variable_no);
                //
            }
        }
        else {
            (*p_sum)+=f(t_variable, variable_no);
        }
    }
    if(variable_no < level) {
        return;
    }
    if(variable_no>level) {
        double length = (variable_lim[level][1]-variable_lim[level][0])/tn[level];
        t_variable[level]=variable_lim[level][0]+length/2;;
        for(int i=0; i<tn[level]; i++) {

            recur_quad_rect_mid(p_sum, f, variable_no, t_variable, variable_lim, tn, level+1, boundary);
            t_variable[level]+=length;
        }


    }
    if(level==0) {
        for(int i=0; i<variable_no; i++) {
            double length = (variable_lim[i][1]-variable_lim[i][0])/tn[i];
            (*p_sum)*=length;
        }
    }
}

int main(void) {
	int to_do, n, nx, ny, integrand_fun_no, method_no, flag;
	int no_funcs = sizeof(func_tab) / sizeof(Func1vFp);
	int no_quads = sizeof(quad_tab) / sizeof(QuadratureFp);
	double a, b, x1, x2, y1, y2, hy, sum, delta;
	double t_variable[N_MAX], variable_lim[N_MAX][2];
	int tn[N_MAX];

	scanf("%d", &to_do);
	switch (to_do) {
		case 1: // loop over quadratures and integrands
			scanf("%lf %lf %d", &a, &b, &n);
			for(int q = 0; q < no_quads; ++q) {
                for (int f = 0; f < no_funcs; ++f) {
                    printf("%.5f ", quad_select(f, q, a, b, n));
                }
                printf("\n");
            }
			break;
		case 2: // adaptive algorithm
			scanf("%d %d",&integrand_fun_no,&method_no);
			scanf("%lf %lf %lf", &a, &b, &delta);
			printf("%.5f\n", init_recurs(func_tab[integrand_fun_no],a,b,delta,quad_tab[method_no]));
			break;
		case 3: // double integral over a rectangle
			scanf("%lf %lf %d", &x1, &x2, &nx);
			scanf("%lf %lf %d", &y1, &y2, &ny);
			printf("%.5f\n", dbl_integr(func2v_2, x1, x2, nx, y1, y2, ny));
			break;
		case 4: // double integral over normal domain
			scanf("%lf %lf %d", &x1, &x2, &nx);
			scanf("%lf", &hy);
			printf("%.5f\n", dbl_integr_normal_1(func2v_2, x1, x2, nx, hy, lower_bound2, upper_bound2));
			break;
		case 5: // double integral over multiple normal domains
			scanf("%lf %lf %d", &x1, &x2, &nx);
			scanf("%lf %lf %d", &y1, &y2, &ny);
			printf("%.5f\n",dbl_integr_normal_n(func2v_2, x1, x2, nx, y1, y2, ny, lower_bound2, upper_bound2));
			break;
		case 6: // triple integral over a cuboid
			scanf("%lf %lf %d", &variable_lim[0][0], &variable_lim[0][1], tn);
			scanf("%lf %lf %d", &variable_lim[1][0], &variable_lim[1][1], tn+1);
			scanf("%lf %lf %d", &variable_lim[2][0], &variable_lim[2][1], tn+2);
			scanf("%d", &flag);
			printf("%.5f\n", trpl_quad_rect(func3v, variable_lim, tn, flag ? bound3v : NULL));
			break;
		case 7: // multiple integral over hyper-cuboid
			scanf("%d", &n);
			if(n > N_MAX) break;
			for(int i = 0; i < n; ++i) {
				scanf("%lf %lf %d", &variable_lim[i][0], &variable_lim[i][1], tn+i);
			}
			scanf("%d", &flag);
			sum = 0.;
			recur_quad_rect_mid(&sum, funcNv, n, t_variable, variable_lim, tn, 0, flag ? boundNv : NULL);

//            for(int i=0; i<n; i++) {
//                double length = (variable_lim[i][1]-variable_lim[i][0])/tn[i];
//                (sum)*=length;
//            }
			printf("%.5f\n", sum);
			break;
		default:
			printf("Nothing to do for %d\n", to_do);
			break;
	}
	return 0;
}

