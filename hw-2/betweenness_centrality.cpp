#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <stack>
#include <cstdlib>
#include <ctime>
#include <limits.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <omp.h>

using namespace std;


unordered_map<int, vector<int>> erdos_renyi_graph(int n, float p){
    unordered_map<int, vector<int>> graph;

    for (int i = 0; i < n; i++){
        graph.insert(pair<int, vector<int>>(i, vector<int>()));
    }

    for (int i = 0; i < n; i++){
        for (int j = i + 1; j < n; j++){
            if ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) < p){
                graph[i].push_back(j);
                graph[j].push_back(i);
            }
        }
    }
    return graph;
}


void print_graph(unordered_map<int, vector<int>> graph){
    for(auto elem : graph){
        cout << elem.first << " ";
        for (auto i : elem.second){
            cout << i << ",";
        }
        cout << endl;
    }
}

struct sssp_retval{
    unordered_map<int, vector<int>> pred;
    int* sigma;
    stack<int> _s;
};

sssp_retval singe_source_shortes_paths(unordered_map<int, vector<int>>& graph, int s){
    int n = graph.size();

    int* dist = new int[n];
    int* sigma = new int[n];
    unordered_map<int, vector<int>> pred;

    fill_n(dist, n, INT_MAX);
    fill_n(sigma, n, 0);


    for (int i = 0; i < graph.size(); i++){
        pred.insert(pair<int, vector<int>>(i, vector<int>()));
    }

    dist[s] = 0;
    sigma[s] = 1;

    queue<int> _q;
    stack<int> _s;
    _q.push(s);

    while (!_q.empty()){
        int v = _q.front();
        _q.pop();
        _s.push(v);
        for (int w: graph[v]){
            if (dist[w] == INT_MAX){
                dist[w] = dist[v] + 1;
                _q.push(w);
            }
            if (dist[w] == dist[v] + 1){
                sigma[w] += sigma[v];
                pred[w].push_back(v);
            }
        }
    }

    sssp_retval res;
    res.pred = pred;
    res.sigma = sigma;
    res._s = _s;

    delete[] dist;
    return res;
}

void accumulation(int n, int s, sssp_retval& d, double* btw){
    double* delta = new double[n];
    fill_n(delta, n, 0.);

    while (!d._s.empty()){
        int w = d._s.top();
        d._s.pop();

        for (int v: d.pred[w]){
            delta[v] += ((double)d.sigma[v] * (1. + (double)delta[w])) / (double)d.sigma[w];
        }

        if (w != s){
            #pragma omp atomic
            btw[w] += delta[w];
        
        }
    }
    delete[] delta;
}

void betweenness_centrality_serial(unordered_map<int, vector<int>>& graph, double* btw){
    int n = graph.size();

    for (int s = 0; s < n; s++){
        sssp_retval rv = singe_source_shortes_paths(graph, s);
        accumulation(n, s, rv, btw);
    }
}

void betweenness_centrality_parallel(unordered_map<int, vector<int>>& graph, double* btw){
    int n = graph.size();

    #pragma omp parallel for
    for (int s = 0; s < n; s++){
        sssp_retval rv = singe_source_shortes_paths(graph, s);
        accumulation(n, s, rv, btw);
    }
}


bool assert_equal(double* arr1, double* arr2, int n){
    for (int i = 0; i < n; i++){
        if (fabs(arr1[i] - arr2[i]) > 1e-7){
            cout << i << " | " << arr1[i] << ", " << arr2[i] << endl; 
            return false;
        }
    }
    return true;
}

bool test_correct_parallel(unordered_map<int, vector<int>>& graph){
    int n = graph.size();

    double* btw_serial = new double[n];
    double* btw_parallel = new double[n];
    fill_n(btw_serial, n, 0.);
    fill_n(btw_parallel, n, 0.);

    betweenness_centrality_serial(graph, btw_serial);
    betweenness_centrality_parallel(graph, btw_parallel);

    return assert_equal(btw_serial, btw_parallel, n);
}


int main(){
    srand (static_cast <unsigned> (time(0)));

    for (int n = 500; n <= 5000; n += 500){
        float p = (n * 10.) / (n * (n - 1) * 0.5);
        double* btw = new double[n];

        for (int repeat = 0; repeat < 3; repeat += 1){
            auto graph = erdos_renyi_graph(n, p);
            fill_n(btw, n, 0.);

            struct timeval start_s, end_s;
            gettimeofday(&start_s, NULL);
            betweenness_centrality_serial(graph, btw);
            gettimeofday(&end_s, NULL);
            double sec_s = ((end_s.tv_sec  - start_s.tv_sec) * 1000000u +
                             end_s.tv_usec - start_s.tv_usec) / 1.e6;

            cout << "{\"type\": \"serial\", \"n\": " <<  n << ", \"p\": " << p <<  ", \"repeat\": " << repeat <<", \"sec\": " << sec_s << "}" << endl;


            for (int num_threads = 2; num_threads <= 8; num_threads++){
                omp_set_dynamic(0);
                omp_set_num_threads(num_threads);
                fill_n(btw, n, 0.);

                struct timeval start_p, end_p;
                gettimeofday(&start_p, NULL);
                betweenness_centrality_parallel(graph, btw);
                gettimeofday(&end_p, NULL);
                double sec_p = ((end_p.tv_sec  - start_p.tv_sec) * 1000000u +
                                 end_p.tv_usec - start_p.tv_usec) / 1.e6;

                cout << "{\"type\": \"parallel\", \"n\": " <<  n << ", \"p\": " << p <<  ", \"num_threads\": " << num_threads << ", \"repeat\": " << repeat <<", \"sec\": " << sec_p << "}" << endl;
            }
        }
    }
}
