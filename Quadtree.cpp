#include "Quadtree.h"
//initialization
int QuadtreeNode::currentID = 1;
vector<QuadtreeNode*> QuadtreeNode::leafNodes;

//constructor
QuadtreeNode::QuadtreeNode(Vertex r) : region(r), parent(nullptr), id(currentID++) {}
//destructor
QuadtreeNode::~QuadtreeNode() {
    for (auto* ch : children) delete ch;
}
//in boundary function
bool QuadtreeNode::inBoundary(QuadtreeNode* n, int px, int py) {
    return (px >= n->region.x && px < n->region.x + n->region.width &&
            py >= n->region.y && py < n->region.y + n->region.height);
}
//subdivide-API
void QuadtreeNode::Subdivide(int minW, int minH, int scale, const ChannelGraph& graph) {
    int w = region.width, h = region.height;
    if (w <= minW && h <= minH) {
        for (auto vp : boost::make_iterator_range(vertices(graph))) {
            const Partition& p = graph[vp];
            bool overlaps = !(p.x2 <= region.x || p.x1 >= region.x + w ||
                              p.y2 <= region.y || p.y1 >= region.y + h);
            if (overlaps) {
                graphPartitionIDs.push_back(vp);
            }
        }
        leafNodes.push_back(this);
        return;
    }

    int subW = (w + scale - 1) / scale;
    int subH = (h + scale - 1) / scale;
    int x0 = region.x, y0 = region.y, lvl = region.level + 1;

    for (int i = 0; i < scale; ++i) {
        for (int j = 0; j < scale; ++j) {
            int nx = x0 + i * subW, ny = y0 + j * subH;
            int aw = std::min(subW, x0 + w - nx);
            int ah = std::min(subH, y0 + h - ny);
            if (aw > 0 && ah > 0) {
                auto* child = new QuadtreeNode(Vertex(nx, ny, aw, ah, lvl));
                child->parent = this;
                children.push_back(child);
            }
        }
    }

    for (auto* ch : children) {
        ch->Subdivide(minW, minH, scale, graph);  // recursive call on child
    }
}
//insertPoint-API
void QuadtreeNode::InsertPoint(int px, int py) {
    if (!inBoundary(this, px, py)) {
        cout << "Point (" << px << "," << py << ") is outside the root boundary. Cannot insert.\n";
        return;
    }

    if (children.empty()) {
        if (inBoundary(this, px, py)) {
            points.emplace_back(px, py);
            cout << "Inserted at node " << id << "\n";
        } else {
            cout << "Point (" << px << "," << py << ") is outside all current leaf nodes. Cannot insert.\n";
        }
        return;
    }

    bool inserted = false;
    for (auto* ch : children) {
        if (ch && inBoundary(ch, px, py)) {
            ch->InsertPoint(px, py);
            inserted = true;
            break;
        }
    }

    if (!inserted) {
        cout << "Point (" << px << "," << py << ") is outside all current leaf nodes. Cannot insert.\n";
    }
}




//searchPoint-API
void QuadtreeNode::SearchPoint( int px, int py) {
    if (!inBoundary(this, px, py)) {
        cout << "Point (" << px << "," << py << ") is outside the this boundary.\n";
        return;
    }

    if (this->children.empty()) {
        cout << "Node ID " << this->id << " region (" << this->region.x << "," << this->region.y
             << ") " << this->region.width << "x" << this->region.height << "\nPartitions: ";
        for (int pid : this->graphPartitionIDs)
            cout << pid << " ";
        cout << "\nPath: ";
        printPathFromLeafToRoot(this);
        cout << "\n";
        return;
    }

    bool found = false;
   for (auto* ch : this->children) {
    if (ch && inBoundary(ch, px, py)) { 
        ch->SearchPoint( px, py);
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

//deletepoint-API
void QuadtreeNode::DeletePoint(QuadtreeNode*& root, int px, int py) {
    
    if (!inBoundary(root, px, py)) {
    cout<<"points does not lie inside tree";
    return;
    }

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
            DeletePoint(ch, px, py);
            found = true;
            return;
        }
    }

    if (!found) {
        cout << "No leaf node contains point (" << px << "," << py << "). Cannot delete.\n";
    }
}

//pathtoroot-API
void QuadtreeNode::PathToRoot( int px, int py) {
    SearchPoint( px, py);
}

vector<QuadtreeNode*>& QuadtreeNode::getLeafNodes() {
    return leafNodes;
}


//rectQuery-API
vector<string> QuadtreeNode::RectQuery(QuadtreeNode* root, int x1, int y1, int x2, int y2, const ChannelGraph& graph) {
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
            auto sub = RectQuery(ch, x1, y1, x2, y2, graph);
            unique.insert(sub.begin(), sub.end());
        }
    }

    return vector<string>(unique.begin(), unique.end());
}
                //netIntersect-API
vector<string> QuadtreeNode::NetIntersect(QuadtreeNode* root, int x1, int y1, int x2, int y2, const ChannelGraph& graph) {
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
