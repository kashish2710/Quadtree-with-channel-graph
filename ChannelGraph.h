#ifndef CHANNEL_GRAPH_H
#define CHANNEL_GRAPH_H

#include <string>
#include <vector>
#include <boost/graph/adjacency_list.hpp>

using namespace std;

// Structure to represent a Partition (channel block) with coordinates
struct Partition {
    string name; // Unique name/ID of the partition
    int x1, y1;  // Top-left corner
    int x2, y2;  // Bottom-right corner
};

// Define the ChannelGraph type using Boost adjacency_list.
// Each node in the graph is a Partition, and edges are undirected.
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, Partition> ChannelGraph;

//  To check if two partitions touch horizontally (i.e., side-by-side and overlapping in Y)
bool isHorizontallyTouching(const Partition& a, const Partition& b);

// Function to construct the channel graph by connecting horizontally touching partitions.
void makeChannelGraph(ChannelGraph& graph, const vector<Partition>& partitions);

#endif
