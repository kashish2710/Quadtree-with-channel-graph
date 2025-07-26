#include <iostream>
#include <chrono>
#include "Quadtree.h"
#include "ChannelGraph.h"


using namespace std;
//test cases
int CountLeafNodes(const QuadtreeNode* node) {
    if (!node) return 0;
    if (node->GetChildren().empty()) return 1;

    int total = 0;
    for (auto* ch : node->GetChildren()) {
        total += CountLeafNodes(ch);
    }
    return total;
}

void CheckCurrentTreeCorrectness(const QuadtreeNode* root) {
    int actualLeafCount = CountLeafNodes(root);  

    int expectedLeafCount = -1;

    int w = root->getRegion().width;
    int h = root->getRegion().height;

    if (w == 100 && h == 100)
        expectedLeafCount = 16;
    else if (w == 1000 && h == 1000)
        expectedLeafCount = 4096;
    else if (w == 10000 && h == 10000)
        expectedLeafCount = 262144;

    cout << "\n--- Quadtree Verification ---\n";
    if (expectedLeafCount != -1) {
        if (actualLeafCount == expectedLeafCount) {
            cout << "[PASS] Leaf node count is correct: " << actualLeafCount << "\n";
        } else {
            cout << "[FAIL] Expected " << expectedLeafCount
                 << " leaf nodes, got " << actualLeafCount << "\n";
        }
    } else {
        cout << "[SKIPPED] No test defined for region size "
             << w << "x" << h << "\n";
    }
}
int main() {
    QuadtreeNode* tree = nullptr;
    ChannelGraph graph;
    vector<Partition> partitions = {
        {"P1", 0, 0, 10, 100},
        {"P2", 10, 0, 20, 20},
        // {"P3", 10, 20, 20, 60}, 
        {"P4", 20, 0, 50, 100},
        {"P5", 10, 60, 20, 100},
        // {"P6", 50, 0, 75, 50},
        {"P7", 50, 50, 75, 100},
        {"P8", 75, 0, 100, 100}
    };
   vector<pair<Point, Point>> path = {
    {{0,0}, {40,40}},   // horizontal
    {{40,40}, {40,60}},   // vertical
    {{40,60}, {55,60}},   // horizontal
    {{55,60}, {55,70}},   // vertical
    {{55,70}, {100,100}}    // horizontal 
       
    };  
    while (true) {
        cout << "\nCommand: MakeChannelGraph |Subdivide w h minW minH scale | InsertPoint x y | SearchPoint x y | DeletePoint x y | PathToRoot x y | RectQuery x1 y1 x2 y2 | NetIntersect x1 y1 x2 y2 | PrintDot | VerifyTree | PathRectQuery | Exit\n> ";
        string cmd;
        cin >> cmd;

        if (cmd == "Exit") break;

        else if (cmd == "MakeChannelGraph") {
    graph = ChannelGraph();  // create a fresh, empty graph
    makeChannelGraph(graph, partitions);
    cout << "Channel graph created.\n";
}

else if (cmd == "Subdivide") {
    if (tree) {
        delete tree;    // delete old tree
        tree = nullptr;
    }

    int w, h, minW, minH, scale;
    cin >> w >> h >> minW >> minH >> scale;

    if (!boost::num_vertices(graph)) {  // check if graph is empty
        cout << "Please run makechannelgraph first.\n";
        return 0;
    }

    Vertex root(0, 0, w, h, 0);
    tree = new QuadtreeNode(root, &graph);
    tree->GetLeafNodes().clear();

    auto start = chrono::high_resolution_clock::now();
    tree->Subdivide(minW, minH, scale);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Quadtree created in " << elapsed.count() << " seconds.\n";
}

        else if (cmd == "InsertPoint") {
            int point_x, point_y; cin >> point_x >> point_y;
              auto start = chrono::high_resolution_clock::now();
            tree->InsertPoint(point_x,point_y);
              auto end = chrono::high_resolution_clock::now();
              chrono::duration<double> elapsed = end - start;
              cout << "Inserted Points  in " << elapsed.count() << " seconds.\n";
        }
        else if (cmd == "SearchPoint") {
            int point_x, point_y; cin >> point_x >> point_y;
            vector<int>path;
           auto start = chrono::high_resolution_clock::now();
           tree->SearchPoint(point_x,point_y,path);
             auto end = chrono::high_resolution_clock::now();
              chrono::duration<double> elapsed = end - start;
              cout << "Searching done " << elapsed.count() << " seconds.\n";
            
        }
        else if (cmd == "DeletePoint") {
            int point_x, point_y; cin >> point_x >> point_y;
            auto start = chrono::high_resolution_clock::now();
            tree->DeletePoint(point_x, point_y);
             auto end = chrono::high_resolution_clock::now();
              chrono::duration<double> elapsed = end - start;
              cout << "Deleted Points in" << elapsed.count() << " seconds.\n";
            

        }
        else if (cmd == "PathToRoot") {
            int point_x, point_y; cin >> point_x >> point_y;
             vector<int>path;
             
           tree->PathToRoot( point_x, point_y,path);
        }
        else if (cmd == "RectQuery") {
            int x_top, y_top, x_bottom, y_bottom;cin >> x_top >> y_top >> x_bottom >> y_bottom;
             auto start = chrono::high_resolution_clock::now();
            auto res = tree->RectQuery(x_top,y_top,x_bottom,y_bottom);
            for (const auto& r : res) cout << r << " ";
            cout << "\n";
             auto end = chrono::high_resolution_clock::now();
              chrono::duration<double> elapsed = end - start;
              cout << "RectQuery done in " << elapsed.count() << " seconds.\n";
            
        }
        else if (cmd == "NetIntersect") {
            int x1, y1, x2, y2; cin >> x1 >> y1 >> x2 >> y2;

          auto res=tree->NetIntersect(x1, y1, x2, y2);
            for (const auto& r : res) cout << r << " ";
            cout << "\n";
        }
        else if (cmd == "PrintDot") {
    if (!tree) std::cout << "No tree built yet.\n";
    else std::cout << tree->GetDotRepresentation();
}
else if (cmd == "VerifyTree") {
    if (tree)
        CheckCurrentTreeCorrectness(tree);
    else
        cout << "Tree not built yet.\n";
}
else if (cmd == "PathRectQuery") {
    if (path.empty()) {
        cout << "Path vector is empty.\n";
    } else {
        
        Point start = path.front().first;   
        Point end   = path.back().second;   
        
        // Bounding rectangle coordinates
        int minX = min(start.x, end.x);
        int minY = min(start.y, end.y);
        int maxX = max(start.x, end.x);
        int maxY = max(start.y, end.y);

        auto res = tree->RectQuery(minX, minY, maxX, maxY);

        cout << "Partitions in bounding rectangle (" 
             << minX << "," << minY << ") - (" << maxX << "," << maxY << "): ";
        for (const auto& r : res) cout << r << " ";
        cout << "\n";
    }
}
else{
    cout<<"Invalid Command!";
}
    }
       return 0;
}
