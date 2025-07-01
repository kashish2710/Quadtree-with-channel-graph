#ifndef CHANNEL_GRAPH_H
#define CHANNEL_GRAPH_H

#include <string>
#include <vector>
#include <boost/graph/adjacency_list.hpp>

using namespace std;

struct Partition {
    string name;
    int x1, y1, x2, y2;
};

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, Partition> ChannelGraph;

bool isHorizontallyTouching(const Partition& a, const Partition& b);

void makeChannelGraph(ChannelGraph& graph, const vector<Partition>& partitions);

#endif
