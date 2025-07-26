#ifndef QUADTREE_H
#define QUADTREE_H
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include "ChannelGraph.h"
using namespace std;

// Represents a rectangular region in 2D space along with its level in the quadtree
struct Vertex {
    int x, y, width, height, level;
    Vertex(int x, int y, int w, int h, int l) : x(x), y(y), width(w), height(h), level(l) {}
};
//points on line
struct Point{
    int x ,y;
};
class QuadtreeNode {
private:
     Vertex region; // Current region this node represents
    vector<QuadtreeNode*> children; // Child nodes (4 children in case of subdivision)
    vector<pair<int, int>> points; // Points stored in this node (only in leaves)
    vector<int> graphPartitionIDs; // Partition IDs from the ChannelGraph overlapping this node
    QuadtreeNode* parent; // Pointer to the parent node
    int id; // Unique ID for the node
    static int currentID; // Static counter for assigning unique IDs
    ChannelGraph* graphPtr; // Pointer to the associated channel graph
    vector<QuadtreeNode*> leafNodes; // Stores all leaf nodes for querying etc.
    void GenerateDot(std::ostream& out) const; // Used for visualization (Graphviz DOT format)


public:
   // Constructor: Initializes the node with its region and associated ChannelGraph
    QuadtreeNode(const Vertex& r, ChannelGraph* g);

    // Destructor: Frees all child nodes
    ~QuadtreeNode();

    // Returns a string in DOT format representing the tree rooted at this node
    string GetDotRepresentation() const;

    // Subdivides the current node into 4 children recursively if region size > minW and minH
    void Subdivide(int minW, int minH, int scale);

    // Inserts a point (x, y) into the appropriate leaf node
    void InsertPoint(int x, int y);

    // Searches for the point and stores path of node IDs from root to that point
    void SearchPoint(int x, int y, vector<int>& path);

    // Deletes a point (x, y) if present in a leaf node
    void DeletePoint(int x, int y);

    // Finds the path from a leaf node (containing x, y) up to the root
    void PathToRoot(int x, int y, vector<int>& path);

    // Returns list of partition names that intersect the rectangle defined by (x1,y1)-(x2,y2)
    vector<string> RectQuery(int x1, int y1, int x2, int y2);

    // Returns list of partition names that a line segment (x1,y1)-(x2,y2) intersects
    vector<string> NetIntersect(int x1, int y1, int x2, int y2);

    // Returns a reference to the current list of leaf nodes (modifiable)
    vector<QuadtreeNode*>& GetLeafNodes();

    // Returns a const reference to the leaf nodes (non-modifiable)
    const vector<QuadtreeNode*>& GetLeafNodes() const;

    // Checks if a point (x,y) lies within this node's boundary
    bool InBoundary(int x, int y);

    // Returns the region (rectangle) that this node represents
    const Vertex& getRegion() const {
        return region;
    }
 const vector<QuadtreeNode*>& GetChildren() const { return children; }

};

#endif
