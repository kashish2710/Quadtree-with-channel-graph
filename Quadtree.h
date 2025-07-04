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
   void InsertPoint( int px, int py);
   void SearchPoint( int px, int py);
    static void DeletePoint(QuadtreeNode*& root, int px, int py);
    void PathToRoot( int px, int py);
    static vector<string> RectQuery(QuadtreeNode* root, int x1, int y1, int x2, int y2, const ChannelGraph& graph);
    static vector<string> NetIntersect(QuadtreeNode* root, int x1, int y1, int x2, int y2, const ChannelGraph& graph);

    static vector<QuadtreeNode*>& getLeafNodes();
    static void printPathFromLeafToRoot(QuadtreeNode* node);
    static bool inBoundary(QuadtreeNode* node, int px, int py);

   
};

#endif
