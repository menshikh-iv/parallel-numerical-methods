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
        return;
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
    omp_set_nested(1);
    omp_set_dynamic(1);

    for (int exponent = 8; exponent <= 28; exponent++){
        int n = (1 << exponent);

        for (int repeat = 1; repeat <= 5; repeat++){
            int* arr_canonical = create_random_array(n);
            int* arr_s = new int[n];
            int* arr_p = new int[n];
            for (int i = 0; i < n; i++){
                arr_s[i] = arr_canonical[i];
            }

            int* tmp_s = new int[n];
            int* tmp_p = new int[n];
            for (int i = 0; i < n; i++){
                tmp_s[i] = 0;
            }

            struct timeval start_s, end_s;
            gettimeofday(&start_s, NULL);
            merge_sort_serial(arr_s, n, tmp_s);
            gettimeofday(&end_s, NULL);

            double sec_s = ((end_s.tv_sec  - start_s.tv_sec) * 1000000u +
                             end_s.tv_usec - start_s.tv_usec) / 1.e6;
            
            if (!is_sorted_asc(arr_s, n)){
                cout << "[FAIL-SERIAL] Array is'nt sorted" << endl;
                return 1;
            }

            cout << "{\"type\": \"serial\", \"exponent\": " <<  exponent << ", \"repeat\": " << repeat <<", \"sec\": " << sec_s << "}" << endl;


            for (int num_threads = 2; num_threads <= 8; num_threads++){
                omp_set_num_threads(num_threads);

                for (int i = 0; i < n; i++){
                    tmp_p[i] = 0;
                }

                for (int i = 0; i < n; i++){
                    arr_p[i] = arr_canonical[i];
                }


                struct timeval start_p, end_p;
                gettimeofday(&start_p, NULL);
                merge_sort_parallel(arr_p, n, num_threads, tmp_p);
                gettimeofday(&end_p, NULL);

                double sec_p = ((end_p.tv_sec  - start_p.tv_sec) * 1000000u +
                                 end_p.tv_usec - start_p.tv_usec) / 1.e6;

                if (!is_sorted_asc(arr_p, n)){
                    cout << "[FAIL-PARALLEL] Array is'nt sorted" << endl;
                    return 1;
                }

                cout << "{\"type\": \"parallel\", \"exponent\": " <<  exponent << ", \"num_threads\": " << num_threads <<", \"repeat\": " << repeat <<", \"sec\": " << sec_p << "}" << endl;
            }

            delete[] arr_canonical;
            delete[] arr_s;
            delete[] arr_p;
            delete[] tmp_s;
            delete[] tmp_p;
        }
    }
}
    