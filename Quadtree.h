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

    static void subdivide(QuadtreeNode* node, int minW, int minH, int scale, const ChannelGraph& graph);
    static void insertPoint(QuadtreeNode* root, int px, int py);
    static void searchPoint(QuadtreeNode* root, int px, int py);
    static void deletePoint(QuadtreeNode*& root, int px, int py);
    static void pathtoroot(QuadtreeNode* root, int px, int py);
    static vector<string> rectQuery(QuadtreeNode* root, int x1, int y1, int x2, int y2, const ChannelGraph& graph);
    static vector<string> netIntersect(QuadtreeNode* root, int x1, int y1, int x2, int y2, const ChannelGraph& graph);

    static vector<QuadtreeNode*>& getLeafNodes();
    static void printPathFromLeafToRoot(QuadtreeNode* node);
    static bool inBoundary(QuadtreeNode* node, int px, int py);

    friend class Helper;
};

#endif
