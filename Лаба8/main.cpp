#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

double eps = 1;

void CalculateMachineEps() {
    while (1 + eps > 1) {
        eps = eps / 2;
    }
}

vector<double> Difference(vector<double> v, vector<double> v1, int tmp) { // приведение к ортогональному виду
    double coef = v[tmp] / v1[tmp];
    for (int i = tmp; i < v.size();i++) {
        v[i] = v[i] - v1[i] * coef;
    }
    return v;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr); cout.tie(nullptr);
    int n, m;
    CalculateMachineEps();
    cin >> n >> m;
    vector<vector<double>> input(n, vector<double>(m));
    vector<pair<int, int>> price(n);
    for (int i = 0;i < n;i++) {
        for (int j = 0;j < m;j++) {
            cin >> input[i][j];
        }
        cin >> price[i].first;
        price[i].second = i;
    }
    sort(price.begin(), price.end());
    vector<int> discharge(m, -1);  // ступени ортогональной матрицы
    vector<double> v(m);
    int tmp;
    vector<vector<double>> preAns;
    for (int i = 0;i < n;i++) {  // берём самые дешёвые строки
        v = input[price[i].second];
        tmp = -1;
        for (int j = 0;j < m;j++) {
            if (v[j] > eps) {
                tmp = j;
                break;
            }
        }
        if (tmp == -1) {  // вся строка нулевая
            price[i] = { 1e3, 1e3 };
        }
        else if (discharge[tmp] == -1) {
            discharge[tmp] = preAns.size();
            preAns.push_back(v);
        }
        else {
            while (tmp != -1 && discharge[tmp] != -1) {
                v = Difference(v, preAns[discharge[tmp]], tmp);
                tmp = -1;
                for (int j = 0;j < m;j++) {
                    if (v[j] > eps || v[j] < -eps) {
                        tmp = j;
                        break;
                    }
                }
            }
            if (tmp == -1) {  // строка линейно зависима от тех, которые уже находятся в матрице
                price[i] = { 1e3, 1e3 };
            }
            else {  // подходящая строка
                discharge[tmp] = preAns.size();
                preAns.push_back(v);
            }
        }
        if (preAns.size() == m) { // квадратная матрица заполнена
            break;
        }
    }
    if (preAns.size() != m) {
        cout << -1;
    }
    else {
        vector<int> ans;
        sort(price.begin(), price.end());
        for (int i = 0;i < m;i++) {  // выписываем номера
            ans.push_back(price[i].second + 1);
        }
        sort(ans.begin(), ans.end());
        for (int i = 0;i < m;i++) {
            cout << ans[i] << ' ';
        }
    }
}
