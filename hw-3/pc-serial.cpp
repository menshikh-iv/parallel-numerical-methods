#include <iostream>
using namespace std;

double  f(double* x, int idx){
	bool cond = ((idx % 2) == 1);
	double sign = cond ? -1: 1;
	int i1, i2;
	i1 = idx;
	i2 = cond ? (idx - 1): (idx + 1);
	double fst = sign * x[i1] * x[i2];
	double snd = (-1) * sign * x[idx] + 1. / (x[(idx + 6) % 10] * x[(idx + 6) % 10] + 
                                       		  x[(idx + 7) % 10] * x[(idx + 7) % 10] + 1.);

	return fst + snd;
}

int main(){
	int a, b;
	float h, x;
	a = 0;
	b = 100;
	h = 0.01;

	int n = (b - a) / h;
	double y_c_curr[10];
	double y_c_prev[10];
	double y_p_next[10];

	x = a;
	cout << x << "\t";
	for (int i = 0; i < 10; i++){
		y_c_prev[i] = 1 + 0.1 * i;
		y_c_curr[i] = y_c_prev[i] + h * f(y_c_prev, i);
    	cout << y_c_prev[i] << "\t";
	}
	cout << endl;
	
	x += h;
	cout << x << "\t";
	for (int i = 0; i < 10; i++){
		cout << y_c_curr[i] << "\t";
	}
	cout << endl;


	x += h;
	for (int iteration = 0; iteration < n; iteration++){
		cout << x << "\t";
		for (int i = 0; i < 10; i++){
        	y_p_next[i] = y_c_curr[i] + h * 0.5 * (3 * f(y_c_curr, i) - f(y_c_prev, i));
        	y_c_curr[i] = y_c_curr[i] + h * 0.5 * (f(y_c_curr, i) + f(y_p_next, i));
        	cout << y_c_curr[i] << "\t";
		}
		cout << endl;
		x += h;
	}
}