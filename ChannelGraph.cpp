#include "ChannelGraph.h"

bool isHorizontallyTouching(const Partition& a, const Partition& b) {
    bool verticalOverlap = max(a.y1, b.y1) < min(a.y2, b.y2);
    return (a.x2 == b.x1 || a.x1 == b.x2) && verticalOverlap;
}

void makeChannelGraph(ChannelGraph& graph, const vector<Partition>& partitions) {
    graph.clear();
    for (const auto& p : partitions) {
        boost::add_vertex(p, graph);
    }

    size_t n = partitions.size();
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            if (isHorizontallyTouching(partitions[i], partitions[j])) {
                boost::add_edge(i, j, graph);
            }
        }
    }
}
