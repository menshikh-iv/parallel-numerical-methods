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
	map<int, int> sigma;
	stack<int> _s;
};

sssp_retval singe_source_shortes_paths(map<int, vector<int>> graph, int s){
	map<int, vector<int>> pred;
	map<int, int> dist;
	map<int, int> sigma;

	for (int i = 0; i < graph.size(); i++){
		pred.insert(pair<int, vector<int>>(i, vector<int>()));
		dist.insert(pair<int, int>(i, INT_MAX));
		sigma.insert(pair<int, int>(i, 0));
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
	return res;
}

void accumulation(map<int, vector<int>> graph, int s, sssp_retval d, map<int, float>& btw){
	map<int, float> delta;
	for (int i = 0; i < graph.size(); i++){
		delta.insert(pair<int, float>(i, 0.));
	}

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
}


int main(){
	srand (static_cast <unsigned> (time(0)));

	int n = 200;
	float p = (n * 20.) / (n * (n - 1) * 0.5);

	auto graph = erdos_renyi_graph(n, p);
	//print_graph(graph);

	map<int, float> btw;
	for (int i = 0; i < graph.size(); i++){
		btw.insert(pair<int, int>(i, 0.));
	}

	for (int s = 0; s < graph.size(); s++){
		const sssp_retval& rv = singe_source_shortes_paths(graph, s);
		accumulation(graph, s, rv, btw);
	}
	
	for (auto el: btw){
		cout << el.first << " - " << el.second / 2. << endl;
	}
	return 0;
}
