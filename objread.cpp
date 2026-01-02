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

int main()
{
    string line;
    vector<Vertex> vertices;
    
    // We will store faces as a list of triangles.
    // Each triangle is a list of 3 Vertices.
    vector<vector<Vertex>> triangles; 

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
            iss >> trash; // Eat the 'f'

            vector<Vertex> face; // To hold the 3 vertices of this triangle
            string segment;
            
            // Loop 3 times to get the 3 vertices of the triangle
            // Each 'segment' will look like "1193/1240/1193"
            for (int i = 0; i < 3; i++) {
                iss >> segment; 

                // We need to stop at the first '/' to get the vertex index
                string indexStr = segment.substr(0, segment.find('/'));
                
                // Convert string to int
                int index = stoi(indexStr);

                // OBJ indices are 1-based, C++ vectors are 0-based. Subtract 1.
                index--; 

                // Retrieve the vertex from our list and add to the face
                // Safety check to ensure index is valid
                if(index >= 0 && index < vertices.size()){
                    face.push_back(vertices[index]);
                }
            }
            triangles.push_back(face);
        }
    }

    cout << "Loaded " << vertices.size() << " vertices." << endl;
    cout << "Loaded " << triangles.size() << " faces." << endl;

    return 0;
}