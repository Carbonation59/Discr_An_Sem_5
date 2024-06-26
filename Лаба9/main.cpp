#include <iostream>
#include <vector>
#include <queue>

using namespace std;

int inf = 1e9 + 1;

int Bfs(vector<vector<int>>& edges) {
	queue<int> q;
	vector<int> path(edges[0].size(), -1);  // значение i-того элемента показывает, из какой вершины попали в вершину i
	vector<int> path_weight(edges[0].size()); // значение i-того элемента показывает, сколько потока может пройти в вершину i
	q.push(0);
	int vertex;
	while (!q.empty()) {
		vertex = q.front();
		if (vertex == edges[0].size() - 1) {  // дошли до стока. Берём самый короткий путь (а в обходе в ширину эо самый первый найденный)
			break;
		}
		q.pop();
		for (int i = 0;i < edges[vertex].size();i++) {   // проходимся по всем вершинам из списка вмежности для  вершины vertex
			if (edges[vertex][i] != 0) {  // берём только те, в которые можем попасть
				if (path[i] == -1) {   // если мы здесь ещё не были, заходим сюда
					q.push(i);
					path[i] = vertex;
					path_weight[i] = edges[vertex][i];
				}
			}
		}
	}
	if (q.empty()) {  // не дошли до стока (путей в него не осталось)
		return -1;
	}
	int tmp = inf;
	while (vertex != 0) {  // находим минимальную пропускную способность в нашем пути
		tmp = min(tmp, path_weight[vertex]);
		vertex = path[vertex];
	}
	vertex = edges[0].size() - 1;
	while (vertex != 0) {  // отнимаем пропускную способности у ребёр из пути и прибавляем её же обратным
		edges[path[vertex]][vertex] = edges[path[vertex]][vertex] - tmp;
		edges[vertex][path[vertex]] = edges[vertex][path[vertex]] + tmp;
		vertex = path[vertex];
	}
	return tmp;
}

int main() {
	int n, m;
	cin >> n >> m;
	vector<vector<int>> edges(n, vector<int>(n));
	int a, b;
	for (int i = 0;i < m;i++) {
		cin >> a >> b;
		a--;
		b--;
		cin >> edges[a][b];  // заполняем матрицу смежности весами
	}
	long long ans = 0;
	long long tmp = Bfs(edges);  // каждый раз поиском в ширину ищем кратчайший по длине рёбер путь
	while (tmp != -1) {
		ans = ans + tmp;
		tmp = Bfs(edges);  // прибавляем поток, который мы пропустили по найденному пути
	}
	cout << ans;
}
