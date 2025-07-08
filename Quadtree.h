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

    static int currentID;
    static vector<QuadtreeNode*> leafNodes;

public:
    QuadtreeNode(Vertex r);
    ~QuadtreeNode();

void Subdivide(int minW, int minH, int scale, const ChannelGraph& graph);
void InsertPoint( int x, int y);
void SearchPoint( int x, int y,vector<int>& path, const ChannelGraph& graph);
void QuadtreeNode::DeletePoint(int x, int y);
void PathToRoot( int x, int y,vector<int>& path, const ChannelGraph& graph);
vector<string> RectQuery( int x1, int y1, int x2, int y2, const ChannelGraph& graph);
vector<string> NetIntersect( int x1, int y1, int x2, int y2, const ChannelGraph& graph);

    static vector<QuadtreeNode*>& GetLeafNodes();

    static bool InBoundary(QuadtreeNode* node, int x, int y);

   
};

#endif
