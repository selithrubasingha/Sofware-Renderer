#include <iostream>
#include <fstream>
#include <string>
#include <sstream> // Required for stringstream
#include <vector>  // Required for std::vector

using namespace std;

// 1. Create a structure to hold one 3D point
struct Vertex {
    float x;
    float y;
    float z;
};

int main()
{
    string line;
    
    // 2. Use a vector instead of a fixed array. 
    // This allows the list to grow as big as the file needs.
    vector<Vertex> vertices;
    vector<vector<Vertex>> triangles;

    ifstream myfile("obj/diablo3_pose/diablo3_pose.obj");

    if (!myfile.is_open()) {
        cout << "Error opening file!" << endl;
        return 1;
    }

    while (getline(myfile, line))
    {
        // Check if the line starts with "v" and a space (to avoid "vt" or "vn")
        if (line.substr(0, 2) == "v ")
        {
            // 3. THE FIX: Create a stringstream from the CURRENT line
            istringstream iss(line);
            
            char trash; // To hold the letter 'v'
            Vertex v;   // A temporary variable to hold the numbers

            // Read from the stringstream, NOT the file
            iss >> trash >> v.x >> v.y >> v.z;

            // Add the vertex to our list
            vertices.push_back(v);
        }
    }

    // Verification: Print out what we stored
    cout << "Successfully loaded " << vertices.size() << " vertices." << endl;
    
    // Example: Accessing the first vertex (Index 0)
    if (!vertices.empty()) {
        cout << "First Vertex: " << vertices[0].x << ", " << vertices[0].y << ", " << vertices[0].z << endl;
    }

    return 0;
}