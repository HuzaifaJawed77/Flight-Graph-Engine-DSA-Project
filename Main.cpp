#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <limits>
#include <queue>
#include <utility>

using namespace std;

const int MAX_CITIES = 50;
const int INF = numeric_limits<int>::max() / 4;

// --------------- ACTION HISTORY STACK -----------------
stack<string> actionHistory;

void addHistory(const string &event) {
    actionHistory.push(event);
}

void showHistory() {
    cout << "\nAction History (Stack):\n";
    if (actionHistory.empty()) {
        cout << "No actions recorded.\n";
        return;
    }
    stack<string> temp = actionHistory;
    while (!temp.empty()) {
        cout << "- " << temp.top() << "\n";
        temp.pop();
    }
}

void deleteHistory() {
    while (!actionHistory.empty()) {
        actionHistory.pop();
    }
    cout << "History cleared successfully.\n";
}

// ------------------- City Class -------------------
class City {
public:
    int id;
    string name;

    City(int id, const string &name) : id(id), name(name) {}
};

// ------------------- BST Node Class -------------------
class BSTNode {
public:
    int cityId;
    string name;
    int index;
    BSTNode* left;
    BSTNode* right;

    BSTNode(int id, const string &name, int idx)
        : cityId(id), name(name), index(idx), left(nullptr), right(nullptr) {}
};

// ------------------- BST Class -------------------
class CityBST {
private:
    BSTNode* root;

    BSTNode* insert(BSTNode* node, int id, const string &name, int idx) {
        if (!node) return new BSTNode(id, name, idx);
        if (id < node->cityId) node->left = insert(node->left, id, name, idx);
        else if (id > node->cityId) node->right = insert(node->right, id, name, idx);
        return node;
    }

    BSTNode* search(BSTNode* node, int id) {
        if (!node) return nullptr;
        if (node->cityId == id) return node;
        if (id < node->cityId) return search(node->left, id);
        return search(node->right, id);
    }

public:
    CityBST() : root(nullptr) {}

    void build(const vector<City> &cities) {
        for (size_t i = 0; i < cities.size(); i++)
            root = insert(root, cities[i].id, cities[i].name, i);
    }

    BSTNode* find(int id) { return search(root, id); }
};

// ------------------- Edge structure -------------------
struct Edge {
    int to;
    int cost;   // CHANGED from weight ? cost of flight
};

// ------------------- Global Variables -------------------
vector<City> cities;
vector<Edge> adj[MAX_CITIES];

// ------------------- Loading Data -------------------
void loadCities(const string &filename = "cities.txt") {
    ifstream fin(filename);
    if (!fin.is_open()) { cout << "Error opening file.\n"; return; }

    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        int id; ss >> id;
        string name; getline(ss, name);
        if (!name.empty() && name[0]==' ') name.erase(0,1);
        if ((int)cities.size() >= MAX_CITIES) break;
        cities.push_back(City(id,name));
    }
    fin.close();
    cout << "Loaded " << cities.size() << " cities.\n";
}

void loadRoutes(CityBST &cityBST, const string &filename = "routes.txt") {
    if (cities.empty()) return;

    ifstream fin(filename);
    if (!fin.is_open()) { cout << "No routes loaded.\n"; return; }

    int srcId, dstId, cost, count = 0;
    while (fin >> srcId >> dstId >> cost) {
        BSTNode* uNode = cityBST.find(srcId);
        BSTNode* vNode = cityBST.find(dstId);
        if (!uNode || !vNode) continue;

        // Add the forward edge
        adj[uNode->index].push_back({vNode->index, cost});
        // Add the reverse edge (undirected)
        adj[vNode->index].push_back({uNode->index, cost});

        count++;
    }
    fin.close();
    cout << "Loaded " << count << " routes (undirected).\n";
}

// ------------------- Print Functions -------------------
void printCities() {
    cout << "\nCities:\n";
    for (auto &c : cities)
        cout << c.id << " - " << c.name << "\n";
    addHistory("Viewed all cities");
}

void printRoutes() {
    cout << "\nFlight Routes:\n";
    for (size_t u=0; u<cities.size(); u++) {
        for (auto &e : adj[u])
            cout << cities[u].name << " -> " << cities[e.to].name 
                 << " : Cost = " << e.cost << "\n";
    }
    addHistory("Viewed all routes");
}

// ------------------- Dijkstra -------------------
pair<vector<int>, vector<int>> dijkstra(int srcIndex) {
    int n = cities.size();
    vector<int> cost(n, INF), parent(n, -1);
    typedef pair<int,int> P;
    priority_queue<P, vector<P>, greater<P>> pq;
    cost[srcIndex] = 0;
    pq.push({0, srcIndex});

    while(!pq.empty()) {
        auto [c, u] = pq.top(); pq.pop();
        if(c != cost[u]) continue;
        for(auto &edge : adj[u]) {
            int v = edge.to;
            int w = edge.cost;
            if(cost[u] + w < cost[v]) {
                cost[v] = cost[u] + w;
                parent[v] = u;
                pq.push({cost[v], v});
            }
        }
    }
    return {cost, parent};
}

vector<int> reconstructPath(int targetIndex, const vector<int> &parent) {
    vector<int> path; 
    stack<int> s;

    int cur = targetIndex;
    while(cur != -1) { 
        s.push(cur); 
        cur = parent[cur]; 
    }
    while(!s.empty()) { 
        path.push_back(s.top()); 
        s.pop(); 
    }
    return path;
}

// ------------------- Shortest Path -------------------
void findAndPrintShortestPath(CityBST &cityBST, int sourceID, int destID) {
    BSTNode* sNode = cityBST.find(sourceID);
    BSTNode* tNode = cityBST.find(destID);

    if(!sNode){ cout<<"Source not found.\n"; return; }
    if(!tNode){ cout<<"Destination not found.\n"; return; }

    auto [cost, parent] = dijkstra(sNode->index);

    if(cost[tNode->index] >= INF){ 
        cout<<"No flight path exists.\n"; 
        return; 
    }

    vector<int> path = reconstructPath(tNode->index, parent);
    cout << "Cheapest flight path:\n";
    for(size_t i=0; i<path.size(); i++){
        cout << cities[path[i]].name;
        if(i+1 < path.size()) cout << " -> ";
    }
    cout << "\nTotal cost: " << cost[tNode->index] << "\n";

    addHistory("Found cheapest flight path from " + sNode->name + " to " + tNode->name);
}

// ------------------- Direct Connections -------------------
void showDirectConnections(CityBST &cityBST, int startID) {
    BSTNode* startNode = cityBST.find(startID);
    if(!startNode){ cout << "City ID not found.\n"; return; }

    addHistory("Viewed direct connections of " + startNode->name);

    cout << "Direct flights available from " << startNode->name << ": ";
    if(adj[startNode->index].empty()){ 
        cout << "None"; 
    }
    else {
        for(auto &e : adj[startNode->index])
            cout << cities[e.to].name << " ";
    }
    cout << endl;
}

// ------------------- Main -------------------
int main() {
    loadCities("cities.txt");

    if(cities.empty()){ 
        cout<<"No cities loaded.\n"; 
        return 0; 
    }

    CityBST cityBST;
    cityBST.build(cities);

    loadRoutes(cityBST, "routes.txt");

    int choice;
    while(true) {
        cout<<"\n--- Main Menu ---\n";
        cout<<"1. View all cities\n";
        cout<<"2. View all flight routes\n";
        cout<<"3. Find cheapest flight path\n";
        cout<<"4. Show direct flight connections\n";
        cout<<"5. History\n";
        cout<<"6. Exit\n";
        cout<<"Enter choice: ";

        if(!(cin >> choice)){
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(),'\n');
            cout<<"Invalid input.\n";
            continue;
        }

        if(choice==1) printCities();
        else if(choice==2) printRoutes();
        else if(choice==3){
            int src,dst; 
            cout<<"Source city ID: "; cin>>src; 
            cout<<"Destination city ID: "; cin>>dst;
            findAndPrintShortestPath(cityBST,src,dst);
        }
        else if(choice==4){
            int src; 
            cout<<"Enter city ID: "; cin>>src;
            showDirectConnections(cityBST, src);
        }
        else if(choice==5){

            int hChoice;
            cout << "\n--- History Menu ---\n";
            cout << "1. View History\n";
            cout << "2. Delete History\n";
            cout << "Enter choice: ";
            cin >> hChoice;

            if(hChoice == 1) showHistory();
            else if(hChoice == 2) deleteHistory();
            else cout << "Invalid option.\n";
        }
        else if(choice==6){ 
            cout<<"Exiting.\n"; 
            break; 
        }
        else cout<<"Unknown choice.\n";
    }

    return 0;
}