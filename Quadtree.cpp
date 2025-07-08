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
//in boundary function=>px-pointX and py-pointY
bool QuadtreeNode::InBoundary(QuadtreeNode* n, int x, int y) {
    return (x >= n->region.x && x < n->region.x + n->region.width &&
            y >= n->region.y && y < n->region.y + n->region.height);
}
//subdivide-API
void QuadtreeNode::Subdivide(int minW, int minH, int scale, const ChannelGraph& graph) {
    int w = region.width, h = region.height;
    if (w <= minW && h <= minH) {
        for (auto vertexID : boost::make_iterator_range(vertices(graph))) {
            const Partition& partition = graph[vertexID];
            bool overlaps = !(partition.x2 <= region.x || partition.x1 >= region.x + w ||
                              partition.y2 <= region.y || partition.y1 >= region.y + h);
            if (overlaps) {
                graphPartitionIDs.push_back(vertexID);
            }
        }
        leafNodes.push_back(this);
        return;
    }

   int subWidth = (region.width + scale - 1) / scale;
int subHeight = (region.height + scale - 1) / scale;

int parentX = region.x;
int parentY = region.y;
int nextLevel = region.level + 1;

for (int i = 0; i < scale; ++i) {
    for (int j = 0; j < scale; ++j) {
        int childX = parentX + i * subWidth;
        int childY = parentY + j * subHeight;

        int actualWidth = std::min(subWidth, parentX + region.width - childX);
        int actualHeight = std::min(subHeight, parentY + region.height - childY);

        if (actualWidth > 0 && actualHeight > 0) {
            auto* childNode = new QuadtreeNode(Vertex(childX, childY, actualWidth, actualHeight, nextLevel));
            childNode->parent = this;
            children.push_back(childNode);
        }
    }
}

for (auto* child : children) {
    child->Subdivide(minW, minH, scale, graph);  // Recursive call on each child
}

}
//insertPoint-API
void QuadtreeNode::InsertPoint(int x, int y) {
    if (!InBoundary(this, x, y)) {
        cout << "Point (" << x << "," << y << ") is outside the root boundary. Cannot insert.\n";
        return;
    }

    if (children.empty()) {
        if (InBoundary(this, x, y)) {
            points.emplace_back(x, y);
            cout << "Inserted at node " << id << "\n";
        } else {
            cout << "Point (" <<x << "," << y << ") is outside all current leaf nodes. Cannot insert.\n";
        }
        return;
    }

    bool inserted = false;
    for (auto* ch : children) {
        if (ch && InBoundary(ch, x, y)) {
            ch->InsertPoint(x, y);
            inserted = true;
            break;
        }
    }

    if (!inserted) {
        cout << "Point (" << x << "," << y << ") is outside all current leaf nodes. Cannot insert.\n";
    }
}
//searchPoint-API
void QuadtreeNode::SearchPoint(int x, int y, std::vector<int>& path, const ChannelGraph& graph) {
    if (!InBoundary(this, x, y)) {
        cout << "Point (" << x << "," << y << ") is outside this boundary.\n";
        return;
    }

    path.push_back(this->id); 

    if (this->children.empty()) {
      
        cout << "Node ID " << this->id << " region (" << this->region.x << "," << this->region.y
                  << ") " << this->region.width << "x" << this->region.height << "\nPartitions: ";
        for (int pid : this->graphPartitionIDs)
           cout << pid +1 << " ";

     
        std::cout << "\nPath: ";
        for (size_t i = 0; i < path.size(); ++i) {
            cout << path[i];
            if (i != path.size() - 1) cout << "-> ";
        }
        cout << "\n";
        return;
    }

    bool found = false;
    for (auto* ch : this->children) {
        if (ch && InBoundary(ch, x, y)) {
            ch->SearchPoint(x, y, path,graph);  // Recurse
            found = true;
            break;
        }
    }

    if (!found) {
        cout << "Point (" << x << "," << y << ") is outside all current leaf nodes.\n";
    }
}

//deletepoint-API
void QuadtreeNode::DeletePoint(int x, int y) {
    if (!InBoundary(this, x, y)) {
        cout << "Point (" << x << "," << y << ") does not lie inside the tree.\n";
        return;
    }

    if (this->children.empty()) {
   
        auto it = find(leafNodes.begin(), leafNodes.end(), this);
        if (it != leafNodes.end()) leafNodes.erase(it);

        // Remove from parent's children list
        if (this->parent) {
            auto& siblings = this->parent->children;
            siblings.erase(remove(siblings.begin(), siblings.end(), this), siblings.end());
        }

       cout << "Deleted leaf node containing (" << x << "," << y << ")\n";
        delete this;  
        return;
    }

    bool found = false;
    for (auto*& ch : this->children) {
        if (ch && InBoundary(ch, x, y)) {
            ch->DeletePoint(x, y);  // Recurse
            found = true;
            return;
        }
    }

    if (!found) {
        cout << "No leaf node contains point (" << x << "," << y << "). Cannot delete.\n";
    }
}

//pathtoroot-API
void QuadtreeNode::PathToRoot( int x, int  y,vector<int>& path, const ChannelGraph& graph ) {
    SearchPoint( x, y,path,graph);
}

vector<QuadtreeNode*>& QuadtreeNode::GetLeafNodes() {
    return leafNodes;
}

//rectQuery-API
vector<string> QuadtreeNode::RectQuery( int topLeftX, int topLeftY, int bottomRightX, int bottomRightY, const ChannelGraph& graph) {
    unordered_set<string> partitionsInRectangle;
    if (!this) return {};

    int rectangle_top_left_X = min(topLeftX,bottomRightX);
    int rectangle_top_left_Y = min(topLeftY,bottomRightY);
    int rectangle_bottom_right_X = max(topLeftX,bottomRightX);
    int rectangle_bottom_right_Y = max(topLeftY,bottomRightY);

    if (this->region.x + this->region.width <= rectangle_top_left_X || this->region.x >= rectangle_bottom_right_X  ||
        this->region.y + this->region.height <=  rectangle_top_left_Y || this->region.y >= rectangle_bottom_right_Y )
        return {};

    if (this->children.empty()) {
        for (int pid : this->graphPartitionIDs) {
            const auto& partition = graph[pid];
            bool overlap_x = max(rectangle_top_left_X , partition .x1) < min(rectangle_bottom_right_X, partition .x2);
            bool overlap_y = max(rectangle_top_left_Y, partition .y1) < min( rectangle_bottom_right_Y, partition .y2);
            if (overlap_x && overlap_y) partitionsInRectangle.insert(partition .name);
        }
    } else {
        for (auto* ch : this->children) {
         auto sub= ch-> RectQuery(topLeftX, topLeftY, bottomRightX,bottomRightY, graph);
             partitionsInRectangle.insert(sub.begin(), sub.end());
        }
    }

    return vector<string>( partitionsInRectangle.begin(),  partitionsInRectangle.end());
}
                //netIntersect-API
vector<string> QuadtreeNode::NetIntersect(int x_start, int y_start, int x_end, int y_end, const ChannelGraph& graph) {
    unordered_set<string>  partitionsOnLine;

  
    if ((max(x_start, x_end) < this->region.x) || (min(x_start, x_end) > this->region.x + this->region.width) ||
        (max(y_start, y_end) < this->region.y) || (min(y_start, y_end) > this->region.y + this->region.height)) {
        return {};
    }

    if (this->children.empty()) {
        for (int pid : this->graphPartitionIDs) {
            const Partition& p = graph[pid];

            bool startInside = (x_start >= p.x1 && x_start <= p.x2 && y_start >= p.y1 && y_start <= p.y2);
            bool endInside   = (x_end   >= p.x1 && x_end   <= p.x2 && y_end   >= p.y1 && y_end   <= p.y2);

            bool boxOverlap = !(max(x_start, x_end) < p.x1 || min(x_start, x_end) > p.x2 ||
                               max(y_start, y_end) < p.y1 || min(y_start, y_end) > p.y2);

            if (startInside || endInside || boxOverlap) {
                partitionsOnLine.insert(p.name);
            }
        }
    } else {
        for (auto* ch : this->children) {
            vector<string> sub = ch->NetIntersect(x_start, y_start, x_end, y_end, graph);
           partitionsOnLine.insert(sub.begin(), sub.end());
        }
    }

    return vector<string>(partitionsOnLine.begin(), partitionsOnLine.end());
}


