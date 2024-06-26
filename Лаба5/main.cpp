#include <map>
#include <iostream>

using namespace std;

const char TERMINAL = 'z' + 1;
const int ABSOLUTE_END = 1e7;
int endOfPrefix = -1; // конец префикса в 0 индексации
int remainingPart = 0; // пройденный путь
int remainingPass = 0; // пройденный путь на ребре
string pattern; 

class TSuffixNode {
public:
    map<char, TSuffixNode*> mapData;
    TSuffixNode* SuffixLink = nullptr;
    pair<int,int> Key = {-1,-1};
};

TSuffixNode* node = nullptr;  // текущая вершина
pair<int, int> keyNode;  // текущее ребро
char alphaNode;  // текущий ключ ребра
TSuffixNode* suffLink = nullptr; // текущая суффиксная ссылка
int suffNumber = -1;  // позиция буквы, на которую нас перемещает суффиксная ссылка

class TSuffixTree {
public:
    TSuffixTree(const string pattern) { Build(); }
    ~TSuffixTree();
private:
    void Build();
    void AddSuffixes();
    void Print(TSuffixNode*, int);
    void Delete(TSuffixNode*);
private:
    TSuffixNode* Root = nullptr;
};

void TSuffixTree::Delete(TSuffixNode* node) {
    for(auto elem1 = node->mapData.begin(); elem1 != node->mapData.end(); elem1++) {
        auto elem = *elem1;
        Delete(node->mapData[elem.first]);
        delete node->mapData[elem.first];
    }
}

TSuffixTree::~TSuffixTree() {
    Delete(Root);
    Root->mapData.clear();
    endOfPrefix = -1;
    remainingPart = 0;
    remainingPass = 0;
    node = nullptr;
}

void TSuffixTree::Build() {
    for(size_t i = 0; i < pattern.size(); ++i) {
        AddSuffixes();
    }
    Print(Root,endOfPrefix/2);
}

void TSuffixTree::AddSuffixes() {
    if(Root == nullptr){
        Root =  new TSuffixNode();
        suffLink = nullptr;
    }
    endOfPrefix++;
    const char sign = pattern[endOfPrefix]; // текущая буква
    bool creating = false;  // флаг создания разветвления
    bool newNode = true;  // флаг новой буквы
    TSuffixNode* suffLinkNode = nullptr; // переменная для создания суффиксной ссылки
    bool suffTreap = false; // флаг прохода по суффиксной ссылке
    if(node == nullptr){
        node = Root;
    }
    auto elem1 = Root->mapData.find(sign);
    if(elem1 != Root->mapData.end()){      // проход по дереву, если в нём уже есть текущая буква
        if(remainingPart == 0){
            remainingPart++;
            remainingPass++;
            alphaNode = sign;
            keyNode = Root->mapData[sign]->Key;
        } else {
            if(remainingPart == keyNode.second - keyNode.first + 1 || remainingPart == endOfPrefix - keyNode.first){  // полностью прошли ребро
                TSuffixNode* node1 = node->mapData[alphaNode];
                auto elem2 = node1->mapData.find(sign);
                if(elem2 != node1->mapData.end()){     // поиск пути из следующей вершины
                    remainingPass++;
                    remainingPart = 1;
                    keyNode = node1->mapData[sign]->Key;
                    node = node1;
                    suffLink = node->SuffixLink;    // сохранение суффиксной ссылки, которую мы встретили по пути
                    suffNumber = endOfPrefix;
                    alphaNode = sign;
                } else {
                    creating = true;
                }
            } else if(pattern[keyNode.first + remainingPart] != sign){   // на текущем ребре не следуют буква в том месте, где мы на нём стоим
                creating = true;
            } else {     // продолжение прохождения пути
                remainingPart++;
                remainingPass++;
            }
        }
        newNode = false;
    }
    if(newNode && remainingPass){
        creating = true;
    }
    if(creating){
        while(true){
            if(suffTreap){            // перешли по суффиксной ссылке
                auto elem1 = node->mapData.find(pattern[endOfPrefix]);
                if(elem1 != node->mapData.end()){   // если из следующей вершины мы можем продолжить путь
                    remainingPart = 1;
                    remainingPass++;
                    keyNode = node->mapData[pattern[endOfPrefix]]->Key;
                    alphaNode = sign;
                    if(suffLinkNode != nullptr){   // создание суффиксной ссылки
                        suffLinkNode->SuffixLink = node;
                        suffLinkNode = node;
                    }
                    break;
                }
                pair<int, int> two = {endOfPrefix, ABSOLUTE_END};
                node->mapData[pattern[endOfPrefix]] = new TSuffixNode();
                node->mapData[pattern[endOfPrefix]]->Key = two;
            } else if(remainingPart == keyNode.second - keyNode.first + 1 || remainingPart == endOfPrefix - keyNode.first){   //  текущее ребро пройдено
                TSuffixNode *node1 = node->mapData[alphaNode];
                if(node1 == nullptr){
                    node1 = new TSuffixNode();
                }
                auto elem1 = node1->mapData.find(pattern[endOfPrefix]);
                if(elem1 != node1->mapData.end()){   // если из следующей вершины мы можем продолжить путь
                    remainingPart = 1;
                    remainingPass++;
                    keyNode = node1->mapData[pattern[endOfPrefix]]->Key;
                    alphaNode = sign;
                    node = node1;
                    if(suffLinkNode != nullptr){ // создание суффиксной ссылки
                        suffLinkNode->SuffixLink = node;
                        suffLinkNode = node;
                    }
                    break;
                }
                if(node->mapData[alphaNode]->mapData.size() == 0){     //  на следующей вершине нет исходящих рёбер
                    node1->Key.second = endOfPrefix - remainingPass - 1;
                    pair<int, int> one = {endOfPrefix - remainingPass, ABSOLUTE_END};
                    node1->mapData[pattern[endOfPrefix - remainingPass]] = new TSuffixNode();
                    node1->mapData[pattern[endOfPrefix - remainingPass]]->Key = one;
                }
                pair<int, int> two = {endOfPrefix, ABSOLUTE_END};
                node1->mapData[pattern[endOfPrefix]] = new TSuffixNode();
                node1->mapData[pattern[endOfPrefix]]->Key = two;
                node = node1;
            } else {    // стоим на ребре, но не на его конце
                node->mapData[alphaNode]->Key.second = keyNode.first + remainingPart - 1;
                if(node->mapData[alphaNode]->mapData.size() == 0){   // на следующей вершине нет исходящих ребёр
                    pair<int, int> one = {keyNode.first + remainingPart, ABSOLUTE_END};
                    pair<int, int> two = {endOfPrefix, ABSOLUTE_END};
                    TSuffixNode *node1 = node->mapData[alphaNode];
                    keyNode.second = keyNode.first + remainingPart - 1;
                    node1->mapData[pattern[keyNode.first + remainingPart]] = new TSuffixNode();
                    node1->mapData[pattern[keyNode.first + remainingPart]]->Key = one;

                    node1->mapData[pattern[endOfPrefix]] = new TSuffixNode();
                    node1->mapData[pattern[endOfPrefix]]->Key = two;
                    node->mapData[alphaNode]->Key = keyNode;
                    node = node1;
                } else {    //  на следующей вершине рёбра есть
                    TSuffixNode * node1 = new TSuffixNode();
                    pair<int, int> one = {keyNode.first + remainingPart, keyNode.second};
                    pair<int, int> two = {endOfPrefix, ABSOLUTE_END};
                    keyNode.second = keyNode.first + remainingPart - 1;
                    node1->mapData[pattern[keyNode.first + remainingPart]] = node->mapData[alphaNode];
                    node1->mapData[pattern[keyNode.first + remainingPart]]->Key = one;
                    node->mapData[alphaNode] = node1;

                    node1->mapData[pattern[endOfPrefix]] = new TSuffixNode();
                    node1->mapData[pattern[endOfPrefix]]->Key = two;
                    node->mapData[alphaNode]->Key = keyNode;
                    node = node1;
                }
            }
            remainingPass--;
            if(remainingPass < remainingPart){
                remainingPart--;
            }
            if(suffLinkNode == nullptr){       // создание суффиксной ссылки
                suffLinkNode = node;
            } else {
                suffLinkNode->SuffixLink = node;
                suffLinkNode = node;
            }
            if(!remainingPass){   //  пройденный путь закончен
                node = Root;
                suffLink = nullptr;
                if(suffLinkNode != nullptr){
                    suffLinkNode->SuffixLink = node;
                    suffLinkNode = node;
                }
                auto elem1 = node->mapData.find(sign);
                if(elem1 != node->mapData.end()){  // если из корня мы можем шагнуть по текущей букве
                    remainingPart++;
                    remainingPass++;
                    keyNode = node->mapData[sign]->Key;
                    alphaNode = sign;
                }
                break;
                //добавление в node при необходимости
            }
            if(node->SuffixLink == nullptr){  // если нет суффиксной ссылки
                int ind;
                if(suffLink != nullptr){  // если мы до этого встречали суффиксную ссылку, то переходим по ней
                    node = suffLink;
                    ind = suffNumber;
                } else {
                    node = Root;
                    ind = endOfPrefix - remainingPass;
                }
                suffTreap = false;
                bool stop = false;
                while(true){  // проходим новый путь, но на один короче, чем был до этого
                    if(endOfPrefix-ind > (node->mapData[pattern[ind]]->Key.second - node->mapData[pattern[ind]]->Key.first + 1) || endOfPrefix-ind > (endOfPrefix - node->mapData[pattern[ind]]->Key.first + 1)){
                        // переход на новую вершину
                        pair<int,int> a = node->mapData[pattern[ind]]->Key;
                        node = node->mapData[pattern[ind]];
                        ind = ind + (a.second - a.first + 1);
                        suffLink = node->SuffixLink;  // сохранение суффиксной ссылки, которую мы встретили по пути
                        suffNumber = ind;
                        if(ind >= 1e7){
                            break;
                        }
                        continue;
                    } else { // проход оставшейся части ребра
                        suffLink = node->SuffixLink;
                        suffNumber = ind;
                        keyNode = node->mapData[pattern[ind]]->Key;
                        alphaNode = pattern[ind];
                        remainingPart = remainingPass - (ind - (endOfPrefix - remainingPass));
                        if(keyNode.second - keyNode.first + 1 > remainingPart && pattern[keyNode.first + remainingPart] == pattern[endOfPrefix]){ // если на данном ребре можно продолжить путь
                            remainingPass++;
                            remainingPart++;
                        	stop = true; // если мы можем пройти по текущему ребру
                        }
                        break;
                    }
                }
                if(stop){
                    break;
                }
            } else { // переход по суффиксной ссылке
                node = node->SuffixLink;
                suffTreap = true;
                suffLinkNode = nullptr;
                suffLink = nullptr;
            }
        }
    }
    if(newNode){ // создание новой вершины
        Root->mapData[pattern[endOfPrefix]] = new TSuffixNode();
        Root->mapData[pattern[endOfPrefix]]->Key = {endOfPrefix,ABSOLUTE_END};
        node = Root;
    }
}

void TSuffixTree::Print(TSuffixNode* node, int k) {
    auto elem1 = node->mapData.begin();
    auto elem = *elem1;
    pair<int,int> a = elem.second->Key;
    int endOfPrefix;
    if(a.second - a.first + 1 < k){
        endOfPrefix = a.second;
    } else { // оставшаяся часть меньше длины текущего ребра
        endOfPrefix = a.first + k - 1;
    }
    for(int i = a.first; i <= endOfPrefix; i++){
        cout << pattern[i];
    }
    k = k - (endOfPrefix - a.first + 1);
    if(!k){  // напечатали весь паттерн
        cout << '\n';
        return;
    }
    Print(elem.second, k);
}

int main(){
//    ios::sync_with_stdio(false);
//    cin.tie(nullptr); cout.tie(nullptr);
    cin >> pattern;
    pattern = pattern + pattern + TERMINAL;
    TSuffixTree Tree(pattern);
}
