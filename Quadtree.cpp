#include "Quadtree.h"

int QuadtreeNode::currentID = 1;
vector<QuadtreeNode*> QuadtreeNode::leafNodes;

QuadtreeNode::QuadtreeNode(Vertex r) : region(r), parent(nullptr), id(currentID++) {}

QuadtreeNode::~QuadtreeNode() {
    for (auto* ch : children) delete ch;
}

bool QuadtreeNode::inBoundary(QuadtreeNode* n, int px, int py) {
    return (px >= n->region.x && px < n->region.x + n->region.width &&
            py >= n->region.y && py < n->region.y + n->region.height);
}

void QuadtreeNode::subdivide(QuadtreeNode* node, int minW, int minH, int k, const ChannelGraph& graph) {
    int w = node->region.width, h = node->region.height;
    if (w <= minW && h <= minH) {
        for (auto vp : boost::make_iterator_range(vertices(graph))) {
            const Partition& p = graph[vp];
            bool overlaps = !(p.x2 <= node->region.x || p.x1 >= node->region.x + w ||
                              p.y2 <= node->region.y || p.y1 >= node->region.y + h);
            if (overlaps) {
                node->graphPartitionIDs.push_back(vp);
            }
        }
        leafNodes.push_back(node);
        return;
    }

    int subW = (w + k - 1) / k, subH = (h + k - 1) / k;
    int x0 = node->region.x, y0 = node->region.y, lvl = node->region.level + 1;

    for (int i = 0; i < k; ++i) {
        for (int j = 0; j < k; ++j) {
            int nx = x0 + i * subW, ny = y0 + j * subH;
            int aw = min(subW, x0 + w - nx), ah = min(subH, y0 + h - ny);
            if (aw > 0 && ah > 0) {
                auto* child = new QuadtreeNode(Vertex(nx, ny, aw, ah, lvl));
                child->parent = node;
                node->children.push_back(child);
            }
        }
    }

    for (auto* ch : node->children)
        subdivide(ch, minW, minH, k, graph);
}

void QuadtreeNode::insertPoint(QuadtreeNode* root, int px, int py) {
    if (!inBoundary(root, px, py)) {
        cout << "Point (" << px << "," << py << ") is outside the root boundary. Cannot insert.\n";
        return;
    }

    if (root->children.empty()) {
        root->points.emplace_back(px, py);
        cout << "Inserted at node " << root->id << "\n";
        return;
    }

    bool inserted = false;
    for (auto* ch : root->children) {
        if (inBoundary(ch, px, py)) {
            insertPoint(ch, px, py);
            inserted = true;
            break;
        }
    }

    if (!inserted) {
        cout << "Point (" << px << "," << py << ") is outside all current leaf nodes. Cannot insert.\n";
    }
}


void QuadtreeNode::searchPoint(QuadtreeNode* root, int px, int py) {
    if (!inBoundary(root, px, py)) {
        cout << "Point (" << px << "," << py << ") is outside the root boundary.\n";
        return;
    }

    if (root->children.empty()) {
        cout << "Node ID " << root->id << " region (" << root->region.x << "," << root->region.y
             << ") " << root->region.width << "x" << root->region.height << "\nPartitions: ";
        for (int pid : root->graphPartitionIDs)
            cout << pid << " ";
        cout << "\nPath: ";
        printPathFromLeafToRoot(root);
        cout << "\n";
        return;
    }

    bool found = false;
    for (auto* ch : root->children) {
        if (inBoundary(ch, px, py)) {
            searchPoint(ch, px, py);  // just recurse once in correct child
            found = true;
            break;
        }
    }

    if (!found) {
        cout << "Point (" << px << "," << py << ") is outside all current leaf nodes.\n";
    }
}

void QuadtreeNode::printPathFromLeafToRoot(QuadtreeNode* n) {
    vector<int> path;
    while (n) {
        path.push_back(n->id);
        n = n->parent;
    }
    reverse(path.begin(), path.end());
    for (int id : path) cout << id << " ";
}

void QuadtreeNode::deletePoint(QuadtreeNode*& root, int px, int py) {
    if (!inBoundary(root, px, py)) return;

    if (root->children.empty()) {
        auto it = find(leafNodes.begin(), leafNodes.end(), root);
        if (it != leafNodes.end()) leafNodes.erase(it);

        if (root->parent) {
            auto& siblings = root->parent->children;
            siblings.erase(remove(siblings.begin(), siblings.end(), root), siblings.end());
        }

        delete root;
        root = nullptr;
        cout << "Deleted leaf node containing (" << px << "," << py << ")\n";
        return;
    }

    bool found = false;
    for (auto*& ch : root->children) {
        if (ch && inBoundary(ch, px, py)) {
            deletePoint(ch, px, py);
            found = true;
            return;
        }
    }

    if (!found) {
        cout << "No leaf node contains point (" << px << "," << py << "). Cannot delete.\n";
    }
}


void QuadtreeNode::pathtoroot(QuadtreeNode* root, int px, int py) {
    searchPoint(root, px, py);
}

vector<QuadtreeNode*>& QuadtreeNode::getLeafNodes() {
    return leafNodes;
}

vector<string> QuadtreeNode::rectQuery(QuadtreeNode* root, int x1, int y1, int x2, int y2, const ChannelGraph& graph) {
    unordered_set<string> unique;
    if (!root) return {};

    int rx1 = min(x1, x2), ry1 = min(y1, y2);
    int rx2 = max(x1, x2), ry2 = max(y1, y2);

    if (root->region.x + root->region.width <= rx1 || root->region.x >= rx2 ||
        root->region.y + root->region.height <= ry1 || root->region.y >= ry2)
        return {};

    if (root->children.empty()) {
        for (int pid : root->graphPartitionIDs) {
            const auto& p = graph[pid];
            bool ox = max(rx1, p.x1) < min(rx2, p.x2);
            bool oy = max(ry1, p.y1) < min(ry2, p.y2);
            if (ox && oy) unique.insert(p.name);
        }
    } else {
        for (auto* ch : root->children) {
            auto sub = rectQuery(ch, x1, y1, x2, y2, graph);
            unique.insert(sub.begin(), sub.end());
        }
    }

    return vector<string>(unique.begin(), unique.end());
}

vector<string> QuadtreeNode::netIntersect(QuadtreeNode* root, int x1, int y1, int x2, int y2, const ChannelGraph& graph) {
    auto inside = [](int x, int y, const Partition& p) {
        return x >= p.x1 && x <= p.x2 && y >= p.y1 && y <= p.y2;
    };
    auto intersects = [&](const Partition& p) {
        return inside(x1, y1, p) || inside(x2, y2, p) ||
               !(max(x1, x2) < p.x1 || min(x1, x2) > p.x2 || max(y1, y2) < p.y1 || min(y1, y2) > p.y2);
    };

    unordered_set<string> result;
    for (auto* n : leafNodes) {
        if (!inBoundary(n, x1, y1) && !inBoundary(n, x2, y2)) continue;
        for (int pid : n->graphPartitionIDs) {
            if (intersects(graph[pid])) result.insert(graph[pid].name);
        }
    }
    return vector<string>(result.begin(), result.end());
}
