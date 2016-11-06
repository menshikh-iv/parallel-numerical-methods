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
    int* l_arr = merge_sort_serial(arr, mid_idx);
    int* r_arr = merge_sort_serial(arr + mid_idx, size - mid_idx);

    return merge_arrays(l_arr, r_arr, mid_idx, size - mid_idx);
}


int* merge_sort_parallel(int* arr, int size, int threads){
    if (size < 2){
        return arr;
    }

    int mid_idx = size / 2;
    int* l_arr;
    int* r_arr;

    if ( threads == 1) {
        l_arr = merge_sort_serial(arr, mid_idx);
        r_arr = merge_sort_serial(arr + mid_idx, size - mid_idx);
    }
    else{
        #pragma omp parallel sections
        {
            #pragma omp section
            l_arr = merge_sort_parallel(arr, mid_idx, threads/2);

            #pragma omp section
            r_arr = merge_sort_parallel(arr + mid_idx, size - mid_idx, threads - threads/2);
        }
    }

    return merge_arrays(l_arr, r_arr, mid_idx, size - mid_idx);
}




int main(){
    int n = 10000;
    int* arr = create_random_array(n);

    struct timeval start_s, end_s;
    gettimeofday(&start_s, NULL);
    int* res_s = merge_sort_serial(arr, n);
    gettimeofday(&end_s, NULL);

    double sec_s = ((end_s.tv_sec  - start_s.tv_sec) * 1000000u +
                     end_s.tv_usec - start_s.tv_usec) / 1.e6;
    if (!is_sorted_asc(res_s, n)){
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
    
    struct timeval start_p, end_p;
    gettimeofday(&start_p, NULL);
    int* res_p = merge_sort_parallel(arr, n, num_threads);
    gettimeofday(&end_p, NULL);
    if (!is_sorted_asc(res_p, n)){
        cout << "[FAIL] Array is'nt sorted" << endl;
    }

    double sec_p = ((end_p.tv_sec  - start_p.tv_sec) * 1000000u +
                     end_p.tv_usec - start_p.tv_usec) / 1.e6;

    cout << "Parallel version with n=" << n << " n_treads=" << num_threads << " time=" << sec_p << endl;
    return 0;
}
