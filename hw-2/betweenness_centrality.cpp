#include <iostream>
#include <map>
#include <vector>
#include <queue>
#include <stack>
#include <cstdlib>
#include <ctime>
#include <limits.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>

using namespace std;


map<int, vector<int>> erdos_renyi_graph(int n, float p){
	map<int, vector<int>> graph;

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


void print_graph(map<int, vector<int>> graph){
	for(auto elem : graph){
   		cout << elem.first << " ";
   		for (auto i : elem.second){
   			cout << i << ",";
   		}
   		cout << endl;
	}
}

struct sssp_retval{
	map<int, vector<int>> pred;
	int* sigma;
	stack<int> _s;
};

sssp_retval singe_source_shortes_paths(map<int, vector<int>> graph, int s){
	int n = graph.size();

	int* dist = new int[n];
	int* sigma = new int[n];
	map<int, vector<int>> pred;

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

void accumulation(int n, int s, sssp_retval d, float* btw){
	float* delta = new float[n];
	fill_n(delta, n, 0.);

	while (!d._s.empty()){
		int w = d._s.top();
		d._s.pop();

		for (int v: d.pred[w]){
			delta[v] += ((float)d.sigma[v] * (1. + delta[w])) / (float)d.sigma[w];
		}

		if (w != s){
            btw[w] += delta[w];
		}
	}
	delete[] delta;
}


int main(){
	srand (static_cast <unsigned> (time(0)));

	float sssp_sec = 0.;
	float acc_sec = 0.;

	int n = 1000;
	float p = (n * 10.) / (n * (n - 1) * 0.5);

	auto graph = erdos_renyi_graph(n, p);
	//print_graph(graph);

	float* btw = new float[n];
	fill_n(btw, n, 0.);

	for (int s = 0; s < n; s++){
		struct timeval start_s, end_s;

        gettimeofday(&start_s, NULL);
		sssp_retval rv = singe_source_shortes_paths(graph, s);
		gettimeofday(&end_s, NULL);
		sssp_sec += ((end_s.tv_sec  - start_s.tv_sec) * 1000000u + end_s.tv_usec - start_s.tv_usec);

		gettimeofday(&start_s, NULL);
		accumulation(n, s, rv, btw);
		gettimeofday(&end_s, NULL);
		acc_sec += ((end_s.tv_sec  - start_s.tv_sec) * 1000000u + end_s.tv_usec - start_s.tv_usec);

	}
	/*
	for (auto el: btw){
		cout << el.first << " - " << el.second / 2. << endl;
	}*/

	cout << "sssp_sec: "<< sssp_sec / 1.e6 << " acc_sec: "<< acc_sec / 1.e6 << endl;
	return 0;
}
