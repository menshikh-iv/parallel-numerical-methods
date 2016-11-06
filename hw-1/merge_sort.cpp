#include <iostream>
#include <stdlib.h>

using namespace std;



void shuffle(int* arr, int size){
	if (size < 2){
		return;
	}

    srand(time(NULL));
    for (int i = 0; i < size - 1; i++){
      int j = i + rand() / (RAND_MAX / (size - i) + 1);
      int t = arr[j];

      arr[j] = arr[i];
      arr[i] = t;
    }
}


int* create_random_array(int size){
	int* arr = new int[size];
	for (int i=0; i<size; i++){
		arr[i] = i;
	}

	shuffle(arr, size);
	return arr;
}

bool is_sorted_asc(int* arr, int size){
	if (size < 2){
		return true;
	}

	int prev = arr[0];
	for (int i=1; i<size; i++){
		if (arr[i] < prev){
			return false;
		}
		prev = arr[i];

	}
	return true;
}



int* merge_arrays(int* l_arr, int* r_arr, int l_size, int r_size){
	int* result = new int[l_size + r_size];
	
	int result_idx, l_idx, r_idx;
	result_idx = l_idx = r_idx = 0;

	while ((l_idx < l_size) && (r_idx < r_size)){
		if (l_arr[l_idx] < r_arr[r_idx]){
			result[result_idx++] = l_arr[l_idx++];
		}
		else{
			result[result_idx++] = r_arr[r_idx++];
		}
	}

	while (l_idx < l_size){
		result[result_idx++] = l_arr[l_idx++];
	}

	while (r_idx < r_size){
		result[result_idx++] = r_arr[r_idx++];
	}

	return result;
}

int* merge_sort_serial(int* arr, int size){
	if (size < 2){
		return arr;
	}

	int mid_idx = size / 2;
	// cout << "size:"<<size <<" mid_idx:" <<mid_idx<<endl;
	int* l_arr = merge_sort_serial(arr, mid_idx);
	int* r_arr = merge_sort_serial(arr + mid_idx, size - mid_idx);

	return merge_arrays(l_arr, r_arr, mid_idx, size - mid_idx);
}


int main(){/*
	int* array = new int[5];
	array[3] = 10;
	array[2] = 23;
	array[1] = 25;
	array[0] = -100;
	array[4] = -5;*/
	int n=20;
	int* arr = create_random_array(n);
	for (int i =0; i<n; i++){
		cout << arr[i] << ",";
	}
	cout << endl;

	int* res = merge_sort_serial(arr, n);
	if (!is_sorted_asc(res, n)){
		cout << "[FAIL] Array is'nt sorted" << endl;
	}
	for (int i =0; i<n; i++){
		cout << res[i] << ",";
	}
	cout << endl;
	return 0;
}