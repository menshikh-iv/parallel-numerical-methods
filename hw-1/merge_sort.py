from random import random
from time import time


def merge_sort(arr):
    result = []
    arr_len = len(arr)

    if arr_len < 2:
        return arr

    mid_idx = arr_len // 2
    l_array = merge_sort(arr[:mid_idx])
    r_array = merge_sort(arr[mid_idx:])

    l_idx, r_idx = 0, 0
    while l_idx < len(l_array) and r_idx < len(r_array):
        if l_array[l_idx] < r_array[r_idx]:
            result.append(l_array[l_idx])
            l_idx += 1
        else:
            result.append(r_array[r_idx])
            r_idx += 1

    if l_idx == len(l_array):
        result.extend(r_array[r_idx:])
    else:
        result.extend(l_array[l_idx:])

    return result


def measurement(n_size, n_iters=3):
    arr = [random() for _ in range(n_size)]
    ts, ts_orig = [], []
    for _ in range(n_iters):
        start = time()
        result = merge_sort(arr)
        ts.append(time() - start)

        start = time()
        orig = sorted(arr)
        ts_orig.append(time() - start)
        assert result == orig

    mean_time = float(sum(ts)) / n_iters
    orig_mean_time = float(sum(ts_orig)) / n_iters

    print("n_size={}, mean_time={:.4f}, orig_time={:.4f}".format(n_size, mean_time, orig_mean_time))


measurement(1000)
measurement(10000)
measurement(100000)
measurement(1000000)
