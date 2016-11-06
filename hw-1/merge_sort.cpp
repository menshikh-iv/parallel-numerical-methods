#include <iostream>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>

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



void merge_arrays(int* arr, int size, int* tmp){
    int l_idx = 0;
    int r_idx = size / 2;
    int res_idx = 0;
    
    int i1 = 0;
    int i2 = size / 2;
    int it = 0;

    while(l_idx < size/2 && r_idx < size) {
        if (arr[l_idx] < arr[r_idx]) {
            tmp[res_idx++] = arr[l_idx++];
        }
        else {
            tmp[res_idx++] = arr[r_idx++];
        }
    }

    while (l_idx < size/2) {
        tmp[res_idx++] = arr[l_idx++];
    }
    while (r_idx < size) {
        tmp[res_idx++] = arr[r_idx++];
    }

    for (int i = 0; i < size; i++){
        arr[i] = tmp[i];
    }
}

void merge_sort_serial(int* arr, int size, int* tmp){
    if (size < 2){
        return;
    }
    if (size == 2){
        if (arr[0] > arr[1]){
            int curr = arr[0];
            arr[0] = arr[1];
            arr[1] = curr;
        }
    }

    int mid_idx = size / 2;
    merge_sort_serial(arr, mid_idx, tmp);
    merge_sort_serial(arr + mid_idx, size - mid_idx, tmp);

    merge_arrays(arr, size, tmp);
}


void merge_sort_parallel(int* arr, int size, int threads, int* tmp){
    if (size <= 2){
        merge_sort_serial(arr, size, tmp);
        return;
    }

    int mid_idx = size / 2;
    int* l_arr;
    int* r_arr;

    if ( threads == 1) {
        merge_sort_serial(arr, size, tmp);
    }
    else{
        #pragma omp parallel sections
        {
            #pragma omp section
            merge_sort_parallel(arr, mid_idx, threads/2, tmp);

            #pragma omp section
            merge_sort_parallel(arr + mid_idx, size - mid_idx, threads - threads/2, tmp + mid_idx);
        }
    }

    merge_arrays(arr, size, tmp);
}




int main(){
    int n = 10000000;
    int* arr_1 = create_random_array(n);
    int* arr_2 = new int[n];
    for (int i = 0; i < n; i++){
        arr_2[i] = arr_1[i];
    }

    int* tmp_1 = new int[n];
    int* tmp_2 = new int[n];
    for (int i = 0; i < n; i++){
        tmp_1[i] = tmp_2[i] = 0;
    }

    struct timeval start_s, end_s;
    gettimeofday(&start_s, NULL);
    merge_sort_serial(arr_1, n, tmp_1);
    gettimeofday(&end_s, NULL);

    double sec_s = ((end_s.tv_sec  - start_s.tv_sec) * 1000000u +
                     end_s.tv_usec - start_s.tv_usec) / 1.e6;
    if (!is_sorted_asc(arr_1, n)){
        cout << "[FAIL] Array is'nt sorted" << endl;
    }

    cout << "Serial version with n=" << n << " time=" << sec_s << endl;
    

    int num_threads;
    #pragma omp parallel
    {
        #pragma omp master
        {
            num_threads = omp_get_num_threads();
        }
    }
    omp_set_nested(1);

    
    
    struct timeval start_p, end_p;
    gettimeofday(&start_p, NULL);
    merge_sort_parallel(arr_2, n, num_threads, tmp_2);
    gettimeofday(&end_p, NULL);
    if (!is_sorted_asc(arr_2, n)){
        cout << "[FAIL] Array is'nt sorted" << endl;
    }

    double sec_p = ((end_p.tv_sec  - start_p.tv_sec) * 1000000u +
                     end_p.tv_usec - start_p.tv_usec) / 1.e6;

    cout << "Parallel version with n=" << n << " n_treads=" << num_threads << " time=" << sec_p << endl;
    
    return 0;
}
