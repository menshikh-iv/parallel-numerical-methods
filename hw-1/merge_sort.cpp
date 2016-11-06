#include <iostream>
#include <stdlib.h>

using namespace std;


int* merge_sort_serial(int* arr, int size, int offset=0){
	if (size < 2){
		return arr;
	}

	int* result = new int[size];

	int mid_idx = size / 2;

	int l_len = mid_idx;
	int r_len = size - mid_idx;

	cout << "mid_idx:" << mid_idx << " l_len:" << l_len << " r_len:" << r_len <<endl;
	int* l_array = merge_sort_serial(arr, l_len, 0);
	cout << "Left ok" << endl;
	int* r_array = merge_sort_serial(arr, r_len, mid_idx);
	cout << "Right ok" << endl;

	for (int i =0; i< l_len; i++){
		cout << l_array[i] << ",";
	}
	cout << endl << "----" << endl;

	for (int i =0; i< r_len - mid_idx; i++){
		cout << r_array[i] << ",";
	}
	cout << endl << "----" << endl;

	int l_idx = 0;
	int r_idx = 0;

	int result_idx = 0;

	while ((l_idx < l_len) && (r_idx < r_len)){
		if (l_array[l_idx] < r_array[r_idx]){
            result[result_idx] = l_array[l_idx];
            l_idx += 1;
        }
        else{
            result[result_idx] = r_array[r_idx];
            r_idx += 1;
        }
        result_idx += 1;
	}
    /*
	if (l_idx == l_len){
		for (int idx = r_idx; idx < r_len; idx++){
			result[result_idx] = r_array[idx];
			result_idx += 1;
		}
	}
    else {
        for (int idx = l_idx; idx < l_len; idx++){
			result[result_idx] = l_array[idx];
			result_idx += 1;
		}
    }*/
    return result;
}


int main(){
	int* array = new int[5];
	array[3] = 10;
	array[2] = 23;
	array[1] = 25;
	array[0] = -100;
	array[4] = -200;
	auto res = merge_sort_serial(array, 5);
	for (int i =0; i< 5; i++){
		cout << res[i] << ",";
	}
	cout << endl;
	/*
	srand (time(NULL));
	cout << "###" << rand() % 100 + 1;*/

	return 0;
}