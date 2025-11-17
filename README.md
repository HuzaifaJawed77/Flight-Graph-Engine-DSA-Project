
# ✈️ FlightGraph Engine – Optimized Air Route Management

This is my **semester project** for the **Data Structures & Algorithms (DSA)** course at **NED University of Engineering & Technology**.  

Developed in **C++ with an SFML-based graphical interface**, the project models an **air transportation network** and allows users to compute the **most cost-efficient flight paths between cities**.  

It applies core **DSA concepts**, including **Binary Search Trees (BST), Graphs (Adjacency Lists), Stacks, and Dijkstra’s Algorithm**, demonstrating practical algorithm design, data structure usage, and graphical visualization in real-world scenarios.



---

 👥 Team Information

**Team Lead:** Huzaifa Jawed (CT-24077) SEC-B  
**Team Members:**  
- Fozan Ahmad Khan (CT-24096) SEC-B  
- Saim Uz Zaman (CT-24078) SEC-B  

---

 📝 Project Description

**FlightGraph Engine – Optimized Air Route Management Using DSA** is a C++ project that simulates a flight network between cities and calculates the most cost-efficient paths.  

The system uses advanced **Data Structures** and **Algorithms**, including:

- **Graph (Adjacency List)** to represent flight connections  
- **Binary Search Tree (BST)** for fast city lookup by unique ID  
- **Stack** for tracking user actions  
- **Dijkstra’s Algorithm** for shortest path computation  

The project demonstrates a practical combination of **data structures, graph theory, and algorithm design** to efficiently plan flight routes in a real-world inspired system.

---

 🎯 Objectives

- Efficiently read and store flight cities (airports)  
- Use BST for fast city lookup  
- Represent flight routes as a weighted directed graph  
- Compute minimum-cost flight paths using Dijkstra’s algorithm  
- Allow users to view cities, routes, direct connections, and optimized paths  
- Maintain a stack-based history of user actions  
- Provide a clean menu-based console interface for academic and practical use

---

 🗂️ System Overview

The system reads two text files:

1. **`cities.txt`** – Contains unique city IDs and names (airports)  
2. **`routes.txt`** – Contains flight routes in the format: `SourceID DestinationID FlightCost`  

After loading data, the system constructs:

- A vector of City objects  
- A BST for city lookup  
- An adjacency list representing flight routes  
- A priority queue for Dijkstra’s algorithm  
- A stack for user action history  

The menu interface allows users to view city lists, flight routes, direct connections, and shortest-cost paths.

---

 📊 Data Structures Used

### 1️⃣ Vector
Stores city objects with:

- City ID  
- Airport name  
- Maps each city to an index for graph traversal  

### 2️⃣ Binary Search Tree (BST)
Enables fast searching of cities by ID. Each node stores:

- City ID  
- City name  
- Vector index  
- Left and right child pointers  

### 3️⃣ Adjacency List (Graph)
Represents directed flight routes with:

- Destination city index  
- Flight cost  

### 4️⃣ Priority Queue (Min-Heap)
Used in Dijkstra’s algorithm for efficiently selecting the next closest unvisited city.

### 5️⃣ Stack
Tracks user actions such as:

- Viewing cities  
- Viewing routes  
- Checking shortest path  
- Viewing direct connections  
- History can be displayed or cleared

---

 🛠️ Methodology

1. **Load Cities:** Read `cities.txt`, store in vector, insert into BST  
2. **Load Flight Routes:** Read `routes.txt`, map IDs using BST, store as adjacency list  
3. **Dijkstra’s Algorithm:** Compute minimum-cost paths with priority queue and parent array  
4. **Reconstruct Route:** Backtrack from destination using parent array to get full path  
5. **User Menu System:** Options include viewing cities, routes, optimized paths, direct connections, and action history

---

 🖥️ User Interface (UI)

- Menu-based console interface  
- Keyboard-controlled, beginner-friendly  
- Displays main menu after each operation until exit  
- Handles invalid inputs gracefully  
- Stores all actions in history stack

**Main Menu Options:**

1. View all cities  
2. View all flight routes  
3. Find optimized flight path  
4. Show direct flight connections  
5. View/Delete action history  
6. Exit program

---

 ✅ Testing and Results

- 20 cities and 23 routes were tested  
- BST structure and adjacency list verified  
- Dijkstra’s algorithm computed shortest paths correctly  
- Direct connections feature functional  
- Stack-based history works for all operations  
- Handles invalid input without crashing

---

 💡 Advantages

- Efficient city lookup using BST  
- Accurate shortest path calculation using Dijkstra  
- Memory-efficient adjacency list for sparse graphs  
- Expandable architecture for future improvements  
- Action history enables tracking and debugging  
- Practical demonstration of real-world routing systems

---

 ⚠️ Limitations

- BST is not self-balancing (affects worst-case performance)  
- Algorithm does not support negative weights  
- Routes are static, not editable at runtime  
- Only direct routes stored; reverse routes must be added manually  

---

 🔮 Future Improvements

- Replace BST with AVL or Red-Black Tree for balanced searches  
- Detect bidirectional routes automatically  
- Implement a route editor for adding/removing flights  
- Integrate airline-specific data (duration, fuel cost, seat availability)  
- Implement A* algorithm for larger datasets  
- Provide automated reports and summaries

---

 🏃 How to Compile & Run

1. Ensure `cities.txt` and `routes.txt` are in the `data/` folder  
2. Open terminal/command prompt in the `src/` folder  
3. Compile & Run
   
```bash
g++ main.cpp ui.cpp -o FlightGraphEngine // COMPILE

./FlightGraphEngine      # Linux/macOS // RUN
FlightGraphEngine.exe    # Windows

