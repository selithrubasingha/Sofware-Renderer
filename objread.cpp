#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

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

int main()
{
    string line;
    vector<Vertex> vertices;
    
    // We will store faces as a list of triangles.
    // Each triangle is a list of 3 Vertices.
    vector<Points> triangles;


    ifstream myfile("obj/diablo3_pose/diablo3_pose.obj");

    if (!myfile.is_open()) {
        cout << "Error opening file!" << endl;
        return 1;
    }

    while (getline(myfile, line))
    {
        // 1. Parse Vertices
        if (line.substr(0, 2) == "v ")
        {
            istringstream iss(line);
            char trash; 
            Vertex v;
            iss >> trash >> v.x >> v.y >> v.z;
            vertices.push_back(v);
        }
        // 2. Parse Faces
        else if (line.substr(0, 2) == "f ")
        {
            istringstream iss(line);
            char trash;
            iss >> trash;

            Points tri;

            for (int i = 0; i < 3; i++) {
                string segment;
                iss >> segment;

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

    return 0;
}

