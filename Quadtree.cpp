#include "Quadtree.h"
#include <sstream>

// ======================= DOT Visualization ==========================

// Generates DOT format string to visualize the quadtree structure
std::string QuadtreeNode::GetDotRepresentation() const {
    std::ostringstream out;
    out << "digraph Quadtree {\n"
        << "  node [shape=box];\n";
    GenerateDot(out);  // Recursively generate structure
    out << "}\n";
    return out.str();
}

// Recursive helper to format each node and its children in DOT
void QuadtreeNode::GenerateDot(std::ostream& out) const {
    out << "  " << id
        << " [label=\"ID: " << id
        << "\\n(" << region.x << ',' << region.y
        << ")-(" << region.x + region.width << ',' << region.y + region.height << ")\"];\n";

    for (auto* child : children) {
        child->GenerateDot(out);                      
        out << "  " << id << " -> " << child->id << ";\n";  
    }
}

// Initialize static ID counter
int QuadtreeNode::currentID = 1;

// =============== Accessors for leaf nodes ==================

// Returns modifiable reference to leaf nodes
vector<QuadtreeNode*>& QuadtreeNode::GetLeafNodes() {
    return leafNodes;
}

// Returns read-only reference to leaf nodes
const vector<QuadtreeNode*>& QuadtreeNode::GetLeafNodes() const {
    return leafNodes;
}

// ================= Constructor/Destructor ===================

// Creates a node with specified region and graph reference
QuadtreeNode::QuadtreeNode(const Vertex& r, ChannelGraph* g)
    : region(r), graphPtr(g), parent(nullptr), id(currentID++) {}

// Recursively deletes all child nodes
QuadtreeNode::~QuadtreeNode() {
    for (auto* ch : children) delete ch;
}

// ================= Point-in-boundary check ===================

// Returns true if point (x, y) lies within this node's region
bool QuadtreeNode::InBoundary(int x, int y) {
    return (x >= region.x && x <= region.x + region.width &&
            y >= region.y && y <= region.y + region.height);
}

// =================== Subdivision =============================

// Recursively subdivides this node into smaller children
void QuadtreeNode::Subdivide(int minW, int minH, int scale) {
    auto& graph = *graphPtr;

    int w = region.width, h = region.height;

    // Base case: small enough to stop subdividing
    if (w <= minW && h <= minH) {
        for (auto vertexID : boost::make_iterator_range(vertices(graph))) {
            const Partition& partition = graph[vertexID];
            bool overlaps = !(partition.x2 <= region.x || partition.x1 >= region.x + w ||
                              partition.y2 <= region.y || partition.y1 >= region.y + h);
            if (overlaps) {
                graphPartitionIDs.push_back(vertexID);
            }
        }

        // Add this leaf to the root's leaf list
        QuadtreeNode* root = this;
        while (root->parent) root = root->parent;
        root->leafNodes.push_back(this);
        return;
    }

    // Compute dimensions of child nodes
    int subWidth = (w + scale - 1) / scale;
    int subHeight = (h + scale - 1) / scale;

    int parentX = region.x;
    int parentY = region.y;
    int nextLevel = region.level + 1;

    // Create and add children
    for (int i = 0; i < scale; ++i) {
        for (int j = 0; j < scale; ++j) {
            int childX = parentX + i * subWidth;
            int childY = parentY + j * subHeight;

            int actualWidth = std::min(subWidth, parentX + w - childX);
            int actualHeight = std::min(subHeight, parentY + h - childY);

            if (actualWidth > 0 && actualHeight > 0) {
                auto* childNode = new QuadtreeNode(Vertex(childX, childY, actualWidth, actualHeight, nextLevel), graphPtr);
                childNode->parent = this;
                children.push_back(childNode);
            }
        }
    }

    // Recurse further
    for (auto* child : children) {
        child->Subdivide(minW, minH, scale);
    }
}

// ================ Insert Point ==============================
// Inserts a point into the quadtree, placing it in the correct leaf
void QuadtreeNode::InsertPoint(int x, int y) {
    if (!this->InBoundary(x, y)) {
        cout << "Point (" << x << "," << y << ") is outside the root boundary. Cannot insert.\n";
        return;
    }

    if (children.empty()) {
        points.emplace_back(x, y);
        cout << "Inserted at node " << id << "\n";
        return;
    }
    for (auto* ch : children) {
        if (ch && ch->InBoundary(x, y)) {
            ch->InsertPoint(x, y);
            return;
        }
    }
    cout << "Point (" << x << "," << y << ") is outside all current leaf nodes. Cannot insert.\n";
}

// ================ Search Point ==============================
// Finds the node containing a point and prints the path followed
void QuadtreeNode::SearchPoint(int x, int y, std::vector<int>& path) {
    if (!this->InBoundary(x, y)) {
        cout << "Point (" << x << "," << y << ") is outside this boundary.\n";
        return;
    }
    path.push_back(this->id);
    // If this is a leaf node
    if (this->children.empty()) {
        cout << "Node ID " << id << " region (" << region.x << "," << region.y << ") "
             << region.width << "x" << region.height << "\nPartitions: ";
        bool foundPartition = false;
        for (int pid : graphPartitionIDs) {
            const Partition& p = (*graphPtr)[pid];
            if (x >= p.x1 && x <= p.x2 && y >= p.y1 && y <= p.y2) {
                cout << p.name << " ";
                foundPartition = true;
            }
        }
        if (!foundPartition) cout << "None";

        cout << "\nPath: ";
        for (size_t i = 0; i < path.size(); ++i)
            cout << path[i] << (i != path.size() - 1 ? " -> " : "");
        cout << "\n";
        return;
    }
    // If not a leaf, search children
    for (auto* ch : children) {
        if (ch && ch->InBoundary(x, y)) {
            ch->SearchPoint(x, y, path);
            return;
        }
    }
    // Fallback: No child covers the point, so print this parent node as the last valid region
    cout << "No leaf node contains (" << x << "," << y << "). Nearest region:\n";
    cout << "Node ID " << id << " region (" << region.x << "," << region.y << ") "
         << region.width << "x" << region.height << "\n";

    cout << "Path: ";
    for (size_t i = 0; i < path.size(); ++i)
        cout << path[i] << (i != path.size() - 1 ? " -> " : "");
    cout << "\n";
}
// =================== Delete Point ============================
// Removes a point and deletes the corresponding leaf node
void QuadtreeNode::DeletePoint(int x, int y) {
    if (!this->InBoundary(x, y)) {
        cout << "Point (" << x << "," << y << ") does not lie inside the tree.\n";
        return;
    }

    if (this->children.empty()) {
        auto it = find(leafNodes.begin(), leafNodes.end(), this);
        if (it != leafNodes.end()) leafNodes.erase(it);

        if (this->parent) {
            auto& siblings = this->parent->children;
            siblings.erase(remove(siblings.begin(), siblings.end(), this), siblings.end());
        }

        cout << "Deleted leaf node containing (" << x << "," << y << ")\n";
        delete this;
        return;
    }
    for (auto*& ch : children) {
        if (ch && ch->InBoundary(x, y)) {
            ch->DeletePoint(x, y);
            return;
        }
    }

    cout << "No leaf node contains point (" << x << "," << y << "). Cannot delete.\n";
}
// ================ Path To Root ==============================
// Records path from a given point to the root using SearchPoint
void QuadtreeNode::PathToRoot(int x, int y, vector<int>& path) {
    SearchPoint(x, y, path);
}
// ================ Rectangle Query ==========================
// Returns names of partitions intersecting a given rectangle
vector<string> QuadtreeNode::RectQuery( int topLeftX, int topLeftY, int bottomRightX, int bottomRightY) {
     auto& graph = *graphPtr;
    unordered_set<string> partitionsInRectangle;
    if (!this) return {};

    int rectangle_top_left_X = min(topLeftX,bottomRightX);
    int rectangle_top_left_Y = min(topLeftY,bottomRightY);
    int rectangle_bottom_right_X = max(topLeftX,bottomRightX);
    int rectangle_bottom_right_Y = max(topLeftY,bottomRightY);
 // No intersection with this node
    if (this->region.x + this->region.width <= rectangle_top_left_X || this->region.x >= rectangle_bottom_right_X  ||
        this->region.y + this->region.height <=  rectangle_top_left_Y || this->region.y >= rectangle_bottom_right_Y )
        return {};
// If this node is a leaf node (no children), check for overlapping partitions
    if (this->children.empty()) {
        for (int pid : this->graphPartitionIDs) {
            const auto& partition = graph[pid];
            bool overlap_x = max(rectangle_top_left_X , partition .x1) < min(rectangle_bottom_right_X, partition .x2);// Horizontal overlap
            bool overlap_y = max(rectangle_top_left_Y, partition .y1) < min( rectangle_bottom_right_Y, partition .y2);// Vertical overlap
           // If both overlaps exist, add the partition's name to the result set
            if (overlap_x && overlap_y) partitionsInRectangle.insert(partition .name);
        }
    } 
     // If not a leaf, recursively check all children and collect their results
    else {
        
        for (auto* ch : this->children) {
         auto sub= ch-> RectQuery(topLeftX, topLeftY, bottomRightX,bottomRightY);
             partitionsInRectangle.insert(sub.begin(), sub.end());
        }
    }

    return vector<string>( partitionsInRectangle.begin(),  partitionsInRectangle.end());
}
               // ================ Line (Net) Intersection ===================
// Returns partitions that intersect the line between two points
vector<string> QuadtreeNode::NetIntersect(int x_start, int y_start, int x_end, int y_end) {
    unordered_set<string>  partitionsOnLine;
 auto& graph = *graphPtr;
      // Bounding box check to skip unnecessary nodes
    if ((max(x_start, x_end) < this->region.x) || (min(x_start, x_end) > this->region.x + this->region.width) ||
        (max(y_start, y_end) < this->region.y) || (min(y_start, y_end) > this->region.y + this->region.height)) {
        return {};
    }

    if (this->children.empty()) {
        for (int pid : this->graphPartitionIDs) {
            const Partition& p = graph[pid];
// Check if either  startvertex or endvertex of the line is inside the partition box
            bool startInside = (x_start >= p.x1 && x_start <= p.x2 && y_start >= p.y1 && y_start <= p.y2);
            bool endInside   = (x_end   >= p.x1 && x_end   <= p.x2 && y_end   >= p.y1 && y_end   <= p.y2);

   // Check if line passes through partition's bounding box
            bool boxOverlap = !(max(x_start, x_end) < p.x1 || min(x_start, x_end) > p.x2 ||
                               max(y_start, y_end) < p.y1 || min(y_start, y_end) > p.y2);

            if (startInside || endInside || boxOverlap) {
                partitionsOnLine.insert(p.name);
            }
        }
    } else {
        for (auto* ch : this->children) {
            vector<string> sub = ch->NetIntersect(x_start, y_start, x_end, y_end);
           partitionsOnLine.insert(sub.begin(), sub.end());
        }
    }
    return vector<string>(partitionsOnLine.begin(), partitionsOnLine.end());}



