#include <iostream>
#include <vector>
#include <queue>

using namespace std;

struct point {
	int x;
	int y;
	int func_g;
	int func_f;
};

int func_f(point a, int x2, int y2) {
	return abs(a.x - x2) + abs(a.y - y2) + a.func_g;
}

int c1[4] = { -1, 1, 0, 0 };
int c2[4] = { 0, 0, -1, 1 };

int find(int x1, int y1, int x2, int y2, vector<vector<char>>& v) {
	deque<point> d;
	vector<vector<bool>> used(v.size(), vector<bool>(v[0].size()));
	point a;
	a.func_g = 1;
	for (int i = 0;i < 4;i++) {
		a.x = x1 + c1[i];
		a.y = y1 + c2[i];
		if (a.x >= 0 && a.x <= v.size() - 1 && a.y >= 0 && a.y <= v[0].size() - 1) {
			if (d.empty() || func_f(a, x2, y2) <= (d.front()).func_f) {
				d.push_front({ a.x, a.y, 1, func_f(a, x2, y2) });
			}
			else {
				d.push_back({ a.x, a.y, 1, func_f(a, x2, y2) });
			}
		}
	}
	int ans = -1;
	int x, y;
	while (!d.empty()) {
		a = d.front();
		d.pop_front();
		if (v[a.x][a.y] == '#' || used[a.x][a.y]) {
			continue;
		}
		if (a.x == x2 && a.y == y2) {
			ans = a.func_g;
			break;
		}
		used[a.x][a.y] = true;
		x = a.x;
		y = a.y;
		for (int i = 0;i < 4;i++) {
			a.x = x + c1[i];
			a.y = y + c2[i];
			if (a.x >= 0 && a.x <= v.size() - 1 && a.y >= 0 && a.y <= v[0].size() - 1 && !used[a.x][a.y]) {
				if (d.empty() || func_f(a, x2, y2) <= (d.front()).func_f) {
					d.push_front({ a.x, a.y, a.func_g + 1, func_f(a, x2, y2) });
				}
				else {
					d.push_back({ a.x, a.y, a.func_g + 1, func_f(a, x2, y2) });
				}
			}
		}
	}
	return ans;
}

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr); cout.tie(nullptr);
	int n, m;
	cin >> n >> m;
	vector<vector<char>> v(n, vector<char>(m));
	for (int i = 0;i < n;i++) {
		for (int j = 0;j < m;j++) {
			cin >> v[i][j];
		}
	}
	int q;
	cin >> q;
	int x1, y1, x2, y2;
	while (q--) {
		cin >> x1 >> y1 >> x2 >> y2;
		if (x1 == x2 && y1 == y2) {
			cout << 0 << '\n';
		}
		else {
			x1--;
			x2--;
			y1--;
			y2--;
			cout << find(x1, y1, x2, y2, v) << '\n';
		}
	}
}
