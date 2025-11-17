// main.cpp
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <limits>
#include <cmath>
#include <unordered_map>

using namespace std;

struct City {
    int id;
    string name;
    sf::Vector2f position;
};

struct Edge {
    int from; // index in cities vector
    int to;
    int weight;
};

vector<City> cities;
vector<Edge> edges;
vector<vector<pair<int,int>>> adj; // adjacency list (to, weight)

const float NODE_RADIUS = 25.f;
const float SELECT_SCALE = 1.25f;
const float WINDOW_W = 1000.f;
const float WINDOW_H = 800.f;
const int FONT_SIZE_LABEL = 14;
const int FONT_SIZE_NODE = 16;
const int PATH_MARKER_RADIUS = 10;

// ---- File loaders ----
void loadCities(const string &filename = "cities.txt") {
    cities.clear();
    ifstream fin(filename);
    if (!fin.is_open()) {
        cout << "Cannot open " << filename << " — create a file with lines: <id> <name>\n";
        return;
    }
    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        int id;
        string name;
        ss >> id;
        getline(ss, name);
        if (!name.empty() && name[0] == ' ') name.erase(0,1);
        if (name.empty()) name = "City" + to_string(id);
        cities.push_back({id, name, {0.f,0.f}});
    }
    fin.close();
}

void loadRoutes(const string &filename = "routes.txt") {
    edges.clear();
    adj.clear();
    if (cities.empty()) return;
    adj.assign((int)cities.size(), {});
    ifstream fin(filename);
    if (!fin.is_open()) {
        cout << "Cannot open " << filename << " — create a file with lines: <srcId> <dstId> <weight>\n";
        return;
    }
    unordered_map<int,int> idToIndex;
    for (int i=0;i<(int)cities.size();++i) idToIndex[cities[i].id] = i;

    int srcId, dstId, w;
    while (fin >> srcId >> dstId >> w) {
        if (idToIndex.find(srcId)==idToIndex.end() || idToIndex.find(dstId)==idToIndex.end()) continue;
        int u = idToIndex[srcId];
        int v = idToIndex[dstId];
        edges.push_back({u,v,w});
        adj[u].push_back({v,w});
        adj[v].push_back({u,w}); // treat undirected by default
    }
    fin.close();
}

// ---- Random map-like layout ----
void computeCircleLayout(float margin = 50.f) {
    int n = cities.size();
    if(n == 0) return;

    static bool seeded = false;
    if(!seeded){ srand((unsigned int)time(nullptr)); seeded = true; }

    vector<sf::Vector2f> positions;
    for(int i=0;i<n;i++){
        bool placed = false;
        int attempts = 0;
        while(!placed && attempts < 1000){
            attempts++;
            float x = margin + (rand() % (int)(WINDOW_W - 2*margin));
            float y = margin + (rand() % (int)(WINDOW_H - 2*margin));

            bool ok = true;
            for(auto &pos : positions){
                float dx = x - pos.x;
                float dy = y - pos.y;
                if(sqrt(dx*dx + dy*dy) < NODE_RADIUS*2.5f){ ok = false; break; }
            }

            if(ok){ cities[i].position = {x,y}; positions.push_back({x,y}); placed = true; }
        }
        if(!placed){
            cities[i].position = {margin + i*NODE_RADIUS*3.f, margin + i*NODE_RADIUS*3.f};
        }
    }
}

// ---- Dijkstra shortest path (indices) ----
vector<int> dijkstra_index(int src, int dest) {
    int n = cities.size();
    if (src<0 || dest<0 || src>=n || dest>=n) return {};
    const int INF = numeric_limits<int>::max() / 4;
    vector<int> dist(n, INF), parent(n, -1);
    using P = pair<int,int>;
    priority_queue<P, vector<P>, greater<P>> pq;
    dist[src] = 0;
    pq.push({0, src});
    while(!pq.empty()){
        auto [d,u] = pq.top(); pq.pop();
        if (d != dist[u]) continue;
        if (u==dest) break;
        for(auto [v,w] : adj[u]){
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                parent[v] = u;
                pq.push({dist[v], v});
            }
        }
    }
    if (dist[dest] >= INF) return {};
    vector<int> path;
    int cur = dest;
    while(cur != -1){
        path.push_back(cur);
        cur = parent[cur];
    }
    reverse(path.begin(), path.end());
    return path;
}

// ---- Utilities ----
int findCityAtPosition(const sf::Vector2f &mousePos) {
    for (int i=0;i<(int)cities.size();++i){
        sf::Vector2f center = cities[i].position + sf::Vector2f(NODE_RADIUS, NODE_RADIUS);
        float dx = mousePos.x - center.x;
        float dy = mousePos.y - center.y;
        if (sqrt(dx*dx + dy*dy) <= NODE_RADIUS + 4.f) return i;
    }
    return -1;
}

vector<float> computeSegmentLengths(const vector<int> &path) {
    vector<float> lens;
    if (path.size()<2) return lens;
    for (size_t i=0;i+1<path.size();++i){
        sf::Vector2f p1 = cities[path[i]].position + sf::Vector2f(NODE_RADIUS, NODE_RADIUS);
        sf::Vector2f p2 = cities[path[i+1]].position + sf::Vector2f(NODE_RADIUS, NODE_RADIUS);
        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;
        lens.push_back(sqrt(dx*dx + dy*dy));
    }
    return lens;
}

int main() {
    loadCities("cities.txt");
    loadRoutes("routes.txt");

    if (cities.empty()) {
        cout << "No cities loaded. Create cities.txt and routes.txt next to the exe.\n";
        return 0;
    }

    computeCircleLayout();

    sf::RenderWindow window(sf::VideoMode((unsigned)WINDOW_W,(unsigned)WINDOW_H), "Cities Graph");
    window.setFramerateLimit(60);

    vector<sf::CircleShape> cityShapes;
    cityShapes.reserve(cities.size());
    for (auto &c: cities) {
        sf::CircleShape s(NODE_RADIUS);
        s.setOrigin(0.f,0.f);
        s.setPosition(c.position);
        s.setFillColor(sf::Color(173,216,230));
        s.setOutlineThickness(2.f);
        s.setOutlineColor(sf::Color::Black);
        cityShapes.push_back(s);
    }

    sf::Font font;
    bool fontLoaded = font.loadFromFile("C:/Windows/Fonts/arial.ttf");

    int selectedSource = -1;
    int selectedDest = -1;

    vector<int> shortestPath;
    vector<float> segmentLengths;
    float pathTotalLength = 0.f;
    float pathTravelSpeed = 180.f;
    float pathTravelled = 0.f;
    sf::Clock animClock;
    bool pathAnimating = false;
    float highlightPulse = 0.f;

    auto drawEdge = [&](sf::RenderWindow &win, const Edge &e, sf::Color col, float thickness=2.f) {
        sf::Vector2f p1 = cities[e.from].position + sf::Vector2f(NODE_RADIUS, NODE_RADIUS);
        sf::Vector2f p2 = cities[e.to].position + sf::Vector2f(NODE_RADIUS, NODE_RADIUS);
        sf::Vertex line[] = { sf::Vertex(p1, col), sf::Vertex(p2, col) };
        win.draw(line, 2, sf::Lines);
    };

    while(window.isOpen()){
        sf::Event event;
        while(window.pollEvent(event)){
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mp = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                int idx = findCityAtPosition(mp);
                if (idx != -1) {
                    if (event.mouseButton.button == sf::Mouse::Left) selectedSource = idx;
                    else if (event.mouseButton.button == sf::Mouse::Right) selectedDest = idx;
                }
                if (event.mouseButton.button == sf::Mouse::Middle) selectedSource = selectedDest = -1;
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::S) {
                    int srcIdx = (selectedSource!=-1)?selectedSource:0;
                    int dstIdx = (selectedDest!=-1)?selectedDest:(int)cities.size()-1;
                    shortestPath = dijkstra_index(srcIdx, dstIdx);
                    if(!shortestPath.empty()){
                        segmentLengths = computeSegmentLengths(shortestPath);
                        pathTotalLength = 0.f;
                        for(float L: segmentLengths) pathTotalLength += L;
                        pathTravelled = 0.f;
                        animClock.restart();
                        pathAnimating = true;
                    }
                }
                if (event.key.code == sf::Keyboard::R) {
                    computeCircleLayout();
                    for (int i=0;i<(int)cityShapes.size();++i) cityShapes[i].setPosition(cities[i].position);
                }
            }
        }

        float dt = animClock.getElapsedTime().asSeconds();
        if (pathAnimating) {
            pathTravelled = dt * pathTravelSpeed;
            if (pathTravelled >= pathTotalLength) pathTravelled = pathTotalLength;
            highlightPulse = fabs(sin(dt*3.0f));
        } else highlightPulse = 0.f;

        window.clear(sf::Color::White);

        // Draw normal edges with weights
        for (auto &e: edges) {
            drawEdge(window, e, sf::Color::Black, 2.f);

            sf::Vector2f p1 = cities[e.from].position + sf::Vector2f(NODE_RADIUS, NODE_RADIUS);
            sf::Vector2f p2 = cities[e.to].position + sf::Vector2f(NODE_RADIUS, NODE_RADIUS);
            sf::Vector2f mid = (p1 + p2) * 0.5f;

            sf::Vector2f dir = p2 - p1;
            float length = sqrt(dir.x*dir.x + dir.y*dir.y);
            if(length != 0.f){
                sf::Vector2f offset(-dir.y/length * 15.f, dir.x/length * 15.f);
                mid += offset;
            }

            if(fontLoaded){
                sf::Text wt(to_string(e.weight), font, FONT_SIZE_LABEL);
                wt.setFillColor(sf::Color::Red);
                wt.setOrigin(wt.getLocalBounds().width/2.f, wt.getLocalBounds().height/2.f);
                wt.setPosition(mid);
                window.draw(wt);
            }
        }

        // ---- Progressive edge highlighting (dark green thick) ----
        if(!shortestPath.empty()){
            float remaining=pathTravelled;
            size_t segIndex=0;
            while(segIndex<segmentLengths.size() && remaining>segmentLengths[segIndex]){
                remaining -= segmentLengths[segIndex];
                segIndex++;
            }

            for(size_t i=0;i+1<shortestPath.size();++i){
                sf::Vector2f A = cities[shortestPath[i]].position + sf::Vector2f(NODE_RADIUS, NODE_RADIUS);
                sf::Vector2f B = cities[shortestPath[i+1]].position + sf::Vector2f(NODE_RADIUS, NODE_RADIUS);

                sf::Color col;
                float thickness = 2.f;
                if(i < segIndex){ 
                    col = sf::Color(0,150,0); // dark green
                    thickness = 4.f;
                }
                else if(i == segIndex){
                    float t = (segmentLengths[i]==0)?0:(remaining/segmentLengths[i]);
                    col = sf::Color(0,(sf::Uint8)(150*t),0);
                    thickness = 4.f;
                } 
                else col = sf::Color::Black;

                sf::Vector2f dir = B - A;
                float len = sqrt(dir.x*dir.x + dir.y*dir.y);
                if(len != 0.f){
                    sf::Vector2f unit = dir / len;
                    sf::Vector2f normal(-unit.y, unit.x);
                    sf::Vertex quad[4] = {
                        {A + normal*thickness/2.f, col},
                        {B + normal*thickness/2.f, col},
                        {B - normal*thickness/2.f, col},
                        {A - normal*thickness/2.f, col}
                    };
                    window.draw(quad, 4, sf::Quads);
                }
            }

            // Draw yellow marker
            sf::Vector2f pos;
            if(segIndex>=segmentLengths.size()) pos=cities[shortestPath.back()].position+sf::Vector2f(NODE_RADIUS,NODE_RADIUS);
            else{
                sf::Vector2f A=cities[shortestPath[segIndex]].position+sf::Vector2f(NODE_RADIUS,NODE_RADIUS);
                sf::Vector2f B=cities[shortestPath[segIndex+1]].position+sf::Vector2f(NODE_RADIUS,NODE_RADIUS);
                float t = (segmentLengths[segIndex]==0)?0:(remaining/segmentLengths[segIndex]);
                pos = A + (B-A)*t;
            }
            sf::CircleShape marker(PATH_MARKER_RADIUS);
            marker.setOrigin(PATH_MARKER_RADIUS,PATH_MARKER_RADIUS);
            marker.setPosition(pos);
            marker.setFillColor(sf::Color::Yellow);
            window.draw(marker);
        }

        // Draw nodes
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        int hoverIdx = findCityAtPosition(mousePos);
        for(int i=0;i<(int)cities.size();++i){
            auto &shape = cityShapes[i];
            shape.setFillColor(sf::Color(173,216,230));
            float scale = 1.f;
            if(i==selectedSource||i==selectedDest) scale=SELECT_SCALE;
            if(i==hoverIdx) scale=max(scale,1.08f);
            shape.setScale(scale,scale);
            if(i==selectedSource) shape.setOutlineColor(sf::Color::Blue);
            else if(i==selectedDest) shape.setOutlineColor(sf::Color::Red);
            else shape.setOutlineColor(sf::Color::Black);
            shape.setOrigin(NODE_RADIUS,NODE_RADIUS);
            sf::Vector2f center=cities[i].position+sf::Vector2f(NODE_RADIUS,NODE_RADIUS);
            shape.setPosition(center);
            window.draw(shape);

            if(fontLoaded){
                sf::Text tName(cities[i].name,font,FONT_SIZE_NODE);
                tName.setFillColor(sf::Color::Black);
                tName.setPosition(center+sf::Vector2f(NODE_RADIUS*0.8f,-8.f));
                window.draw(tName);

                sf::Text tId(to_string(cities[i].id),font,12);
                tId.setFillColor(sf::Color::Black);
                tId.setPosition(center-sf::Vector2f(6.f,6.f));
                window.draw(tId);
            }
        }

        window.display();
    }

    return 0;
}