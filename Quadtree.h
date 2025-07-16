#ifndef QUADTREE_H
#define QUADTREE_H

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include "ChannelGraph.h"

using namespace std;

struct Vertex {
    int x, y, width, height, level;
    Vertex(int x, int y, int w, int h, int l) : x(x), y(y), width(w), height(h), level(l) {}
};

class QuadtreeNode {
private:
    Vertex region;
    vector<QuadtreeNode*> children;
    vector<pair<int, int>> points;
    vector<int> graphPartitionIDs;
    QuadtreeNode* parent;
    int id;
    void GenerateDot(std::ostream& out) const;
    static int currentID;
 ChannelGraph* graphPtr;
 

public:
    QuadtreeNode(const Vertex& r, ChannelGraph* g);
    ~QuadtreeNode();
 string GetDotRepresentation() const;
void Subdivide(int minW, int minH, int scale);
void InsertPoint( int x, int y);
void SearchPoint( int x, int y,vector<int>& path);
void QuadtreeNode::DeletePoint(int x, int y);
void PathToRoot( int x, int y,vector<int>& path);
vector<string> RectQuery( int x1, int y1, int x2, int y2);
vector<string> NetIntersect( int x1, int y1, int x2, int y2);

    static vector<QuadtreeNode*>& GetLeafNodes();
    static vector<QuadtreeNode*> leafNodes;
    static bool InBoundary(QuadtreeNode* node, int x, int y);

 const Vertex& getRegion() const {
        return region;
    }
   
};

#endif
