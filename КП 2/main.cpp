#include <cstring>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <vector>
#include <queue>

using namespace std;

//#pragma pack(1)
//#pragma pack(push)

// Preprocessed graph

const int MIN_NUMB_OF_PARAMETERS = 8;
const int MAX_NUMB_OF_PARAMETERS = 9;

struct cord {
    uint32_t id;
    double wid;
    double lon;
};

bool operator < (const cord& a, const cord& b) {
    return a.id < b.id;
}

struct edge {
    uint32_t from;
    uint32_t to;
};

bool operator < (const edge& a, const edge& b) {
    if (a.from == b.from) {
        return a.to < b.to;
    }
    else {
        return a.from < b.from;
    }
}

void r_and_w_nodes(FILE* f_nodes, FILE* f_out, vector<uint32_t>& ids) {
    vector<cord> cords;
    cord a;
    while (fscanf(f_nodes, "%u%lf%lf", &a.id, &a.wid, &a.lon) == 3) {
        cords.push_back(a);
    }
    sort(cords.begin(), cords.end());
    uint32_t n = cords.size();
    fwrite(&n, sizeof(uint32_t), 1, f_out);
    for (uint32_t i = 0; i < n; i++) {
        fwrite(&cords[i].id, sizeof(uint32_t), 1, f_out);
        fwrite(&cords[i].wid, sizeof(double), 1, f_out);
        fwrite(&cords[i].lon, sizeof(double), 1, f_out);
        ids.push_back(cords[i].id);
    }
}

uint32_t binary_search(const vector<uint32_t>& ids, const uint32_t& node_id) {
    uint32_t l = -1;
    uint32_t r = ids.size();
    while (r - l > 1) {
        uint32_t m = (l + r) / 2;
        if (ids[m] < node_id) {
            l = m;
        }
        else {
            r = m;
        }
    }
    return r;
}

void r_and_w_edges(FILE* f_edges, FILE* f_out, const vector<uint32_t>& ids) {
    uint32_t n;
    uint32_t c;
    edge a;
    vector<edge> edges;
    while (fscanf(f_edges, "%u", &n) > 0) {
        fscanf(f_edges, "%u", &a.from);
        for (uint32_t i = 1; i < n; i++) {
            fscanf(f_edges, "%u", &a.to);
            edges.push_back(a);
            c = a.from;
            a.from = a.to;
            a.to = c;
            edges.push_back(a);
        }
    }
    sort(edges.begin(), edges.end());
    c = 0;
    n = edges.size();
    for (uint32_t i = 0; i < n; i++) {
        a = edges[i];
        fwrite(&a.to, sizeof(uint32_t), 1, f_out);
        i++;
        while (i < n && edges[i].from == a.from) {
            fwrite(&edges[i].to, sizeof(uint32_t), 1, f_out);
            i++;
        }
        i--;
    }
    fwrite(&c, sizeof(uint32_t), 1, f_out);
    //offsets
    uint32_t j = 0;
    n = ids.size();
    c = edges.size();
    for (uint32_t i = 0; i < n; i++) {
        while (j < c && binary_search(ids, edges[j].from) < i) {
            j++;
        }
        fwrite(&j, sizeof(uint32_t), 1, f_out);
    }
}

void preprocess_graph(char* nodes_name, char* edges_name, char* out_name) {
    FILE* f_nodes = fopen(nodes_name, "r");
    if (f_nodes == NULL) {
        fprintf(stderr, "Something wrong with nodes file\n");
    }
    FILE* f_edges = fopen(edges_name, "r");
    if (f_edges == NULL) {
        fprintf(stderr, "Something wrong with edges file\n");
    }
    FILE* f_out = fopen(out_name, "wb");
    if (f_out == NULL) {
        fprintf(stderr, "Something wrong with output file\n");
    }
    vector<uint32_t> ids;
    r_and_w_nodes(f_nodes, f_out, ids);
    r_and_w_edges(f_edges, f_out, ids);
    fclose(f_nodes);
    fclose(f_edges);
    fclose(f_out);
}

// Searching

const uint32_t GEODATA_SIZE = sizeof(uint32_t) + 2 * sizeof(double);

const double EARTH_R = 6371e3;
const double MAX_ANGLE = 180.0;
const double PI = 4 * atan(1);
const double EPS = 1e-6;

double rad(double a) {
    return a * PI / MAX_ANGLE;
}

double geo_dist(const cord& a, const cord& b) {
    double phi_a = rad(a.wid);
    double phi_b = rad(b.wid);
    double delta = rad(a.lon - b.lon);
    double cos_d = sin(phi_a) * sin(phi_b) + cos(phi_a) * cos(phi_b) * cos(delta);
    double d = acos(cos_d);
    if (isnan(d)) {
        return 0;
    }
    return EARTH_R * d;
}

struct euristic {
    uint32_t id;
    double dist;
    double path;

    euristic(const uint32_t& u_id, const cord& u, const cord& v, const double& path_to) {
        id = u_id;
        dist = geo_dist(u, v);
        path = path_to;
    }
};

bool operator < (const euristic& a, const euristic& b) {
    if (abs((a.path + a.dist) - (b.path + b.dist)) > EPS) {
        return b.path + b.dist < a.path + a.dist;
    }
    else if (abs(a.path - b.path) > EPS) {
        return b.path < a.path;
    }
    else {
        return a.id < b.id;
    }
}

void r_cord(FILE* f_graph, cord& a) {
    fread(&a.id, sizeof(uint32_t), 1, f_graph);
    fread(&a.wid, sizeof(double), 1, f_graph);
    fread(&a.lon, sizeof(double), 1, f_graph);
}

void r_cords_vec(FILE* f_graph, vector<uint32_t>& ids) {
    uint32_t n;
    cord a;
    fread(&n, sizeof(uint32_t), 1, f_graph);
    for (uint32_t i = 0; i < n; i++) {
        r_cord(f_graph, a);
        ids.push_back(a.id);
    }
}

uint32_t count_edges(FILE* f_graph) {
    uint32_t m = 0;
    uint32_t id = 1;
    while (id != 0) {
        fread(&id, sizeof(uint32_t), 1, f_graph);
        m++;
    }
    return m - 1;
}

void r_offsets(FILE* f_graph, vector<uint32_t>& offsets) {
    uint32_t n = offsets.size();
    for (uint32_t i = 0; i < n - 1; i++) {
        fread(&offsets[i], sizeof(uint32_t), 1, f_graph);
    }
}

cord get_cord(FILE* f_graph, const uint32_t& node_ind) {
    fseek(f_graph, sizeof(uint32_t) + GEODATA_SIZE * node_ind, 0);
    cord a;
    r_cord(f_graph, a);
    return a;
}

void push_nodes(FILE* f_graph, cord end, uint32_t cur_ind, const vector<uint32_t>& ids, const vector<uint32_t>& offsets, vector<uint32_t>& prev, vector<double>& d, priority_queue<euristic>& q) {
    cord cur_cord = get_cord(f_graph, cur_ind);
    uint32_t cur_offset = offsets[cur_ind];
    vector<uint32_t> available_nodes;
    fseek(f_graph, sizeof(uint32_t) + GEODATA_SIZE * (ids.size()) + cur_offset * sizeof(uint32_t), 0);
    uint32_t a;
    for (uint32_t i = cur_offset; i < offsets[cur_ind + 1]; i++) {
        fread(&a, sizeof(uint32_t), 1, f_graph);
        available_nodes.push_back(a);
    }
    uint32_t n = available_nodes.size();
    for (uint32_t i = 0; i < n; i++) {
        a = available_nodes[i];
        uint32_t a_ind = binary_search(ids, a);
        cord a_cord = get_cord(f_graph, a_ind);
        double cur_next_dist = geo_dist(cur_cord, a_cord);
        if (d[a_ind] < 0 || (abs(d[a_ind] - (d[cur_ind] + cur_next_dist)) > EPS && d[a_ind] > d[cur_ind] + cur_next_dist)) {
            d[a_ind] = d[cur_ind] + cur_next_dist;
            prev[a_ind] = cur_ind;
            q.push(euristic(a, a_cord, end, d[a_ind]));
        }
    }
}

double euristic_find(FILE* f_graph, const vector<uint32_t>& ids, const vector<uint32_t>& offsets, const uint32_t& u, const uint32_t& v, vector<uint32_t>& prev, vector<double>& d) {
    uint32_t u_ind = binary_search(ids, u);
    cord u_cord = get_cord(f_graph, u_ind);
    uint32_t v_ind = binary_search(ids, v);
    cord v_cord = get_cord(f_graph, v_ind);
    d[u_ind] = 0;
    prev[u_ind] = u_ind;
    priority_queue<euristic> q;
    q.push(euristic(u, u_cord, v_cord, 0));
    while (!q.empty()) {
        euristic cur = q.top();
        q.pop();
        uint32_t cur_ind = binary_search(ids, cur.id);
        if (cur.path > d[cur_ind]) {
            continue;
        }
        if (cur.id == v) {
            break;
        }
       // cout << 1 << '\n';
        push_nodes(f_graph, v_cord, cur_ind, ids, offsets, prev, d, q);
    }
    return d[v_ind];
}

void get_path(FILE* f_graph, uint32_t node_id, const vector<uint32_t>& prev, vector<uint32_t>& res) {
    while (prev[node_id] != node_id) {
        cord cur_node = get_cord(f_graph, node_id);
        res.push_back(cur_node.id);
        node_id = prev[node_id];
    }
    cord cur_node = get_cord(f_graph, node_id);
    res.push_back(cur_node.id);
}

void execute_search(FILE* f_in, FILE* f_out, FILE* f_graph, const vector<uint32_t>& ids, const vector<uint32_t>& offsets, bool ops) {
    uint32_t n = ids.size();
    vector<uint32_t> prev(n);
    vector<double> d(n);
    uint32_t u = 0;
    uint32_t v = 0;
    while (fscanf(f_in, "%u%u", &u, &v) == 2) {
        prev.assign(n, 0);
        d.assign(n, -1);
        double ans = euristic_find(f_graph, ids, offsets, u, v, prev, d);
        uint32_t v_ind = binary_search(ids, v);
        if (ops) {
            if (d[v_ind] < 0) {
                fprintf(f_out, "-1 0\n");
            }
            else {
                fprintf(f_out, "%.6lf ", ans);
                vector<uint32_t> path;
                get_path(f_graph, v_ind, prev, path);
                n = path.size();
                fprintf(f_out, "%d ", n);
                for (uint32_t i = n - 1; i > 0; --i) {
                    fprintf(f_out, "%u ", path[i]);
                }
                fprintf(f_out, "%u\n", path[0]);
            }
        }
        else {
            if (d[v_ind] < 0) {
                fprintf(f_out, "-1\n");
            }
            else {
                fprintf(f_out, "%.6lf\n", ans);
            }
        }
    }
}

void search(char* graph_name, char* in_name, char* out_name, bool ops) {
    FILE* f_graph = fopen(graph_name, "rb");
    if (f_graph == NULL) {
        fprintf(stderr, "Something wrong with graph file\n");
    }
    FILE* f_in = fopen(in_name, "r");
    if (f_in == NULL) {
        fprintf(stderr, "Something wrong with input file\n");
    }
    FILE* f_out = fopen(out_name, "w");
    if (f_out == NULL) {
        fprintf(stderr, "Something wrong with output file\n");
    }
    vector<uint32_t> ids;
    r_cords_vec(f_graph, ids);
    uint32_t m = count_edges(f_graph);
    vector<uint32_t> offsets(ids.size() + 1);
    offsets[offsets.size() - 1] = m;
    r_offsets(f_graph, offsets);
    execute_search(f_in, f_out, f_graph, ids, offsets, ops);
    fclose(f_graph);
    fclose(f_in);
    fclose(f_out);
}

// Usage

void usage() {
    cout << "Usage:\n";
    cout << "  Create preprocess graph:\n";
    cout << "    ./prog preprocess --nodes <nodes file>\n";
    cout << "    --edges <edges file>\n";
    cout << "    --output <preprocessed graph>\n";
    cout << "  Searching:\n";
    cout << "    --graph <preprocessed graph>\n";
    cout << "    --input <input file>\n";
    cout << "    --output <output file>\n";
    cout << "    [--full-output]\n";
}

int main(int argc, char* argv[]) {
    if (argc == 2) {
        if (strcmp(argv[1], "--help") == 0) {
            usage();
            return 0;
        }
        else {
            cout << "Error: only command '--help' can not have parameters! Use '--help' for info.\n";
            return 10;
        }
    }
    if (argc < MIN_NUMB_OF_PARAMETERS || argc > MAX_NUMB_OF_PARAMETERS) {
        cout << "Error: wrong number of parameters! Use '--help' for info.\n";
        return 11;
    }
    int k = -1;
    if (strcmp(argv[k + 2], "preprocess") == 0) {
        char* nodes_name;
        char* edges_name;
        char* out_name;
        if (strcmp(argv[k + 3], "--nodes") == 0) {
            nodes_name = argv[k + 4];
        }
        if (strcmp(argv[k + 5], "--edges") == 0) {
            edges_name = argv[k + 6];
        }
        if (strcmp(argv[k + 7], "--output") == 0) {
            out_name = argv[k + 8];
        }
        if (nodes_name == nullptr || edges_name == nullptr || out_name == nullptr || argc == MAX_NUMB_OF_PARAMETERS) {
            cout << "Error: wrong arguments for this command! Use '--help' for info.\n";
            return 20;
        }
        preprocess_graph(nodes_name, edges_name, out_name);
        return 0;
    }
    if (strcmp(argv[k + 2], "search") == 0) {
        char* graph_name;
        char* in_name;
        char* out_name;
        bool ops = false;
        if (strcmp(argv[k + 3], "--graph") == 0) {
            graph_name = argv[k + 4];
        }
        if (strcmp(argv[k + 5], "--input") == 0) {
            in_name = argv[k + 6];
        }
        if (strcmp(argv[k + 7], "--output") == 0) {
            out_name = argv[k + 8];
        }
        if (argc == MAX_NUMB_OF_PARAMETERS && strcmp(argv[k + 9], "--full-output") == 0) {
            ops = true;
        }
        if (graph_name == nullptr || in_name == nullptr || out_name == nullptr || !(ops || argc == MIN_NUMB_OF_PARAMETERS)) {
            cout << "Error: wrong arguments for this command! Use '--help' for info.\n";
            return 30;
        }
        search(graph_name, in_name, out_name, ops);
        return 0;
    }
    cout << "Error: wrong command! Use '--help' for info.\n";
    return 12;
}
