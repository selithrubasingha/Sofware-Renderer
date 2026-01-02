#include "tgaimage.h"
#include "objread.cpp"
#include <cmath>
using namespace std;
constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

void line(int ax, int ay, int bx, int by, TGAImage &framebuffer, TGAColor color){

    bool steep = abs(ax-bx)<abs(ay-by);

    if (steep){ // if the line is steep we tranpose the image??
        swap(ax,ay);
        swap(bx,by);
    }

    if (ax>bx){//making the ax smaller x and bx the alarger x all the time.
        swap(ax,bx);
        swap(ay,by);
    }

    
    //The error variable is just tracking "How far away is the real line from the pixel I am currently drawing?"
    int y = ay;
    int ierror = 0;
    for (int x=ax; x<=bx; x++) {
        if (steep)
            framebuffer.set(y,x,color);
        else
            framebuffer.set(x,y,color);
        
        /*
        this ierror calculation is genius mathematics .
        error is the (dy)/(dx)..if error is er > 0.5 ... then it's time to move to the next pixel 
        but deviding in a computer is not that efficient ! 
        so instead of dy/dx>0.5 ----> we do :: 2*dy >dx
        */
        ierror += 2 * abs(by-ay);
        //if statement is removed and ternary is used ... because CPU is more efficient when ternary is used. 
        y += (by > ay ? 1 : -1) * (ierror > bx - ax);
        ierror -= 2 * (bx-ax)   * (ierror > bx - ax);

}

}

void triangle(Points P, vector<Vertex> &verts, TGAImage &framebuffer , TGAColor color){
    Vertex v_a = verts[P.a]; 
    Vertex v_b = verts[P.b];
    Vertex v_c = verts[P.c];

    // 1. Define screen dimensions (Must match your TGAImage size)
    int width = 500;
    int height = 500;

    // 2. Convert World Coordinates (float) -> Screen Coordinates (int)
    // We add 1 and divide by 2 to move the range from [-1, 1] to [0, 1]
    int x0 = (v_a.x + 1.) * width / 2.;
    int y0 = (v_a.y + 1.) * height / 2.;

    int x1 = (v_b.x + 1.) * width / 2.;
    int y1 = (v_b.y + 1.) * height / 2.;

    int x2 = (v_c.x + 1.) * width / 2.;
    int y2 = (v_c.y + 1.) * height / 2.;

    // 3. Draw the 3 lines connecting the points
    line(x0, y0, x1, y1, framebuffer, color); // Line A -> B
    line(x1, y1, x2, y2, framebuffer, color); // Line B -> C
    line(x2, y2, x0, y0, framebuffer, color); // Line C -> A


}

int main(int argc, char** argv) {
    constexpr int width  = 500;
    constexpr int height = 500;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    vector<Vertex> vertices;
    vector<Points> faces;

    // 2. Load the data using the function from objread.cpp
    load_obj(vertices, faces);

    for (int i = 0; i < faces.size(); i++) {
        // Pass the specific face AND the full list of vertices
        triangle(faces[i], vertices, framebuffer , red);
    }

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}