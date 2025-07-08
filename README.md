Quadtree Visualization with Channel Graph Integration
This project implements a Quadtree-based spatial partitioning system in C++ with Boost Graph integration to represent channel partitions. It provides command-line functionality for spatial queries like point insertion, deletion, region search, and net/line intersection with partitioned areas.

 Project Structure
```text
quadtree-visualization-cpp/
           │
           ├── main.cpp               # CLI interface for interacting with Quadtree
           ├── Quadtree.h             # Structures and class declarations
           ├── Quadtree.cpp           # Quadtree logic and query implementations
           ├── ChannelGraph.h/.cpp    # Boost-based graph for partitions
           ├── main.exe               # Compiled executable (optional)
           └── .vscode/               # VS Code config (optional)
# How It Works
 Quadtree
A 2D spatial data structure that recursively subdivides the area into four quadrants.

Each node stores points or further subdivides until the region size becomes ≤ 25×25 or is uniform (e.g., all free or all obstacle).

 Channel Graph
Boost Graph stores partitions (Partition nodes) with spatial boundaries.

Each Quadtree leaf node stores the names of partitions overlapping its region.


 ## Available Commands
All commands are executed via terminal through main.cpp:

Command	Description
subdivide x1 y1 x2 y2	Subdivides the region into a Quadtree
insertPoint x y	Inserts a point into the Quadtree
searchPoint x y	Searches the region containing the point
deletePoint x y	Deletes the point (if in a leaf node)
pathtoroot x y	Prints path from the leaf node to root
makechannelgraph	Builds the partition graph before subdivision
netIntersect x1 y1 x2 y2	Returns all partitions intersected by a line
rectQuery x1 y1 x2 y2	Queries all partitions within a rectangle

##Tech Stack
C++17

1.Boost Graph Library
2.Command-Line Interface
3.Optional: VS Code for development.

BETTER UNDERSTANDING:
![image](https://github.com/user-attachments/assets/a5fcd8b3-dc1c-4e25-ac3a-753bc96c267c)

Author
**Kashish**
B.Tech Student, Netaji Subhas University of Technology (NSUT)

