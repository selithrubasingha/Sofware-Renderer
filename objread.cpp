#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

// Keep your structs here
struct Vertex { 
    float x;
    float y;
    float z;
};

struct Points {
    int a;
    int b;
    int c;
};

// CHANGE: Rename 'main' to 'load_obj' and pass vectors by reference
void load_obj(vector<Vertex> &vertices, vector<Points> &triangles)
{
    string line;
    ifstream myfile("obj/diablo3_pose/diablo3_pose.obj");

    if (!myfile.is_open()) {
        cout << "Error opening file!" << endl;
        return;
    }

    while (getline(myfile, line))
    {
        if (line.substr(0, 2) == "v ")
        {
            istringstream iss(line);
            char trash; 
            Vertex v;
            iss >> trash >> v.x >> v.y >> v.z;
            vertices.push_back(v);
        }
        else if (line.substr(0, 2) == "f ")
        {
            istringstream iss(line);
            char trash;
            iss >> trash;

            Points tri;
            for (int i = 0; i < 3; i++) {
                string segment;
                iss >> segment;
                // Note: using -1 because OBJ is 1-based, C++ is 0-based
                int index = stoi(segment.substr(0, segment.find('/'))) - 1;

                if (i == 0) tri.a = index;
                if (i == 1) tri.b = index;
                if (i == 2) tri.c = index;
            }
            triangles.push_back(tri);
        }
    }
    
    cout << "Loaded " << vertices.size() << " vertices." << endl;
    cout << "Loaded " << triangles.size() << " faces." << endl;
}