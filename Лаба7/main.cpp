#include <iostream>
#include <vector>
#include <stack>

using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr); cout.tie(nullptr);
    int n, m;
    cin >> n >> m;
    int ans = 0;
    vector<int> matrix1(m);
    vector<int> matrix2(m);
    char a;
    for (int i = 0;i < n;i++) {
        fill(matrix2.begin(), matrix2.end(), 0);
        stack<pair<int, int>> s; 
        for (int j = 0;j < m;j++) {
            cin >> a;
            if (a - '0' == 1) {
                continue;
            }
            else if (i == 0) {
                matrix2[j] = 1;
            }
            else {   // высчитываем колонну подряд идущих нулей
                matrix2[j] = matrix1[j] + 1;
            }
        }
        s.push({ 0, 0 }); // заполняем стек для гистограммы первым элементом
        int high;
        int width;
        int tmp;
        for (int j = 0;j < m;j++) {
            if (matrix2[j] > (s.top()).first) {  // продолжаем рост гастограммы
                s.push({ matrix2[j], j });
            }
            else if (matrix2[j] < (s.top()).first) {  // гистограмма понизит максимальную высоту
                high = 1e3;
                width = 0;
                while (matrix2[j] < (s.top()).first) {
                    high = min(high, (s.top()).first);
                    width = j - (s.top()).second;
                    tmp = (s.top()).second;
                    ans = max(ans, high * width);
                    s.pop();
                }
                if (matrix2[j] > (s.top()).first) {
                    s.push({ matrix2[j], tmp });
                }
            }
            else {  // гистограмма сохраняет прошлую высоту
                continue;
            }

        }
        high = 1e3;
        width = 0;
        while (!s.empty()) {  // выводим оставшиеся элементы гистограммы
            high = min(high, (s.top()).first);
            width = m - (s.top()).second;
            ans = max(ans, high * width);
            s.pop();
        }
        swap(matrix1, matrix2);
    }
    cout << ans;
}
