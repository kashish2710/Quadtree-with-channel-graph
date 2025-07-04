#include <iostream>
#include <chrono>
#include "Quadtree.h"
#include "ChannelGraph.h"


using namespace std;

int main() {
    QuadtreeNode* tree = nullptr;
    ChannelGraph graph;
// vector<Partition> partitions = {
//     {"P1", 0, 0, 1000, 1000},
//     {"P2", 500, 500, 1500, 1500},
//     {"P3", 1000, 1000, 2000, 2000},
//     {"P4", 1500, 1500, 2500, 2500},
//     {"P5", 2000, 2000, 3000, 3000},
//     {"P6", 2500, 2500, 3500, 3500},
//     {"P7", 3000, 3000, 4000, 4000},
//     {"P8", 0, 3000, 4000, 4000}
// };

    // vector<Partition> partitions = {
    //     {"P1", 0, 0, 100, 1000},
    //     {"P2", 100, 0, 200, 200},
    //     {"P3", 100, 200, 200, 600},
    //     {"P4", 200, 0, 500, 1000},
    //     {"P5", 100, 600, 200, 1000},
    //     {"P6", 500, 0, 750, 500},
    //     {"P7", 500, 500, 750, 1000},
    //     {"P8", 750, 0, 1000, 1000}
    // };
    //100000 X 100000
vector<Partition> partitions = {
    {"P1", 12345, 6789, 13210, 7489},
    {"P2", 45678, 9876, 46234, 10432},
    {"P3", 78901, 23456, 79500, 24200},
    {"P4", 32145, 65432, 33000, 66310},
    {"P5", 87654, 9876, 88500, 10500},
    {"P6", 45612, 12345, 46234, 13010},
    {"P7", 98123, 45678, 99000, 46678},
    {"P8", 22334, 77889, 23000, 78550},
    {"P9", 66778, 99887, 67300, 100000},
    {"P10", 10101, 11111, 10700, 11890},
    {"P11", 30303, 40404, 31100, 41250},
    {"P12", 50505, 60606, 51320, 61310},
    {"P13", 70707, 80808, 71500, 81670},
    {"P14", 90909, 10000, 91700, 10750},
    {"P15", 80000, 90000, 80800, 90800},
    {"P16", 1000, 2000, 1500, 2700},
    {"P17", 23456, 34567, 24000, 35400},
    {"P18", 34567, 45678, 35300, 46450},
    {"P19", 1234, 5678, 1800, 6400},
    {"P20", 60000, 10000, 60650, 10720},
    {"P21", 33000, 44000, 33540, 44800},
    {"P22", 44444, 55555, 45100, 56300},
    {"P23", 9999, 8888, 10600, 9500},
    {"P24", 88888, 22222, 89540, 22880},
    {"P25", 11111, 33333, 11760, 34000},
    {"P26", 22222, 44444, 22990, 45290},
    {"P27", 33333, 55555, 33999, 56333},
    {"P28", 44444, 66666, 45111, 67400},
    {"P29", 55555, 77777, 56200, 78440},
    {"P30", 66666, 88888, 67330, 89700},
    {"P31", 77777, 11111, 78540, 12000},
    {"P32", 88888, 22222, 89670, 23000},
    {"P33", 99999, 33333, 100000, 34200},
    {"P34", 1111, 2222, 1600, 2950},
    {"P35", 2222, 3333, 2780, 3900},
    {"P36", 3333, 4444, 3890, 5100},
    {"P37", 4444, 5555, 4960, 6200},
    {"P38", 5555, 6666, 6120, 7100},
    {"P39", 6666, 7777, 7250, 8230},
    {"P40", 7777, 8888, 8440, 9350},
    {"P41", 8888, 9999, 9600, 10420},
    {"P42", 9999, 11111, 10700, 11750},
    {"P43", 11111, 12345, 11900, 13080},
    {"P44", 12345, 13579, 13150, 14290},
    {"P45", 13579, 14789, 14340, 15400},
    {"P46", 14789, 15999, 15560, 16660},
    {"P47", 15999, 17111, 16740, 17750},
    {"P48", 17111, 18333, 17890, 18990},
    {"P49", 18333, 19555, 19090, 20230},
    {"P50", 19555, 20777, 20280, 21450}
};

    while (true) {
        cout << "\nCommand: MakeChannelGraph |Subdivide w h minW minH scale | InsertPoint x y | SearchPoint x y | DeletePoint x y | PathToRoot x y | RectQuery x1 y1 x2 y2 | NetIntersect x1 y1 x2 y2 | Exit\n> ";
        string cmd;
        cin >> cmd;

        if (cmd == "Exit") break;
        else if (cmd == "MakeChannelGraph") {
            makeChannelGraph(graph, partitions);
            cout << "Channel graph created.\n";
        }
        else if (cmd == "Subdivide") {
            int w, h, minW, minH, scale;
            cin >> w >> h >> minW >> minH >> scale;
            if (!graph.m_vertices.empty()) {
                Vertex root(0, 0, w, h, 0);
              tree = new QuadtreeNode(root);
              QuadtreeNode::getLeafNodes().clear();
          auto start = chrono::high_resolution_clock::now();
              tree->Subdivide(minW, minH, scale, graph);//tree object should not be passed in function
                auto end = chrono::high_resolution_clock::now();
                chrono::duration<double> elapsed = end - start;
                cout << "Quadtree created in " << elapsed.count() << " seconds.\n";
            } else {
                cout << "Please run makechannelgraph first.\n";
            }
        }
        else if (cmd == "InsertPoint") {
            int x, y; cin >> x >> y;
            tree->InsertPoint(x,y);
        }
        else if (cmd == "SearchPoint") {
            int x, y; cin >> x >> y;
          tree->SearchPoint(x,y);
            
        }
        else if (cmd == "DeletePoint") {
            int x, y; cin >> x >> y;
            if (tree) QuadtreeNode::DeletePoint(tree, x, y);
        }
        else if (cmd == "PathToRoot") {
            int x, y; cin >> x >> y;
           tree->PathToRoot( x, y);
        }
        else if (cmd == "RectQuery") {
            int x1, y1, x2, y2; cin >> x1 >> y1 >> x2 >> y2;
            auto res = QuadtreeNode::RectQuery(tree, x1, y1, x2, y2, graph);
            for (const auto& r : res) cout << r << " ";
            cout << "\n";
        }
        else if (cmd == "NetIntersect") {
            int x1, y1, x2, y2; cin >> x1 >> y1 >> x2 >> y2;
            auto res = QuadtreeNode::NetIntersect(tree, x1, y1, x2, y2, graph);
            for (const auto& r : res) cout << r << " ";
            cout << "\n";
        }
    }

    return 0;
}
