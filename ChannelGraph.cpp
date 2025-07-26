#include "ChannelGraph.h"

// Function to check if two partitions are horizontally touching
// Conditions:
// - They must be adjacent horizontally 
// - They must overlap vertically (i.e., share some y-range)
bool isHorizontallyTouching(const Partition& a, const Partition& b) {
    bool verticalOverlap = max(a.y1, b.y1) < min(a.y2, b.y2); // checks if Y ranges overlap
    return (a.x2 == b.x1 || a.x1 == b.x2) && verticalOverlap; // checks if horizontally adjacent
}

// Function to construct the channel graph
void makeChannelGraph(ChannelGraph& graph, const vector<Partition>& partitions) {
    graph.clear(); // Clear any existing graph structure

    // Add all partitions as vertices to the graph
    for (const auto& p : partitions) {
        boost::add_vertex(p, graph);
    }

    // Connect partitions that are horizontally touching
    size_t n = partitions.size();
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            if (isHorizontallyTouching(partitions[i], partitions[j])) {
                boost::add_edge(i, j, graph); // Add undirected edge
            }
        }
    }
}
