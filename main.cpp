#include <cmath>
#include <tuple>
#include "geometry.h"
#include "model.h"
#include "tgaimage.h"

using namespace std;

/*
IN THIS BRANCH ALL THE RASTERIZATION STUFF IN THE TUTORIAL IS COMPLETE
*/
constexpr int width  = 800;
constexpr int height = 800;

constexpr TGAColor white   = {255, 255, 255, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor green   = {  0, 255,   0, 255};

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

double signed_triangle_area(int ax, int ay, int bx, int by, int cx, int cy) {
    return .5*((by-ay)*(bx+ax) + (cy-by)*(cx+bx) + (ay-cy)*(ax+cx));
}

// We accept vec4, but we only use v.x and v.y. 
std::tuple<int,int> project(vec4 v) { 
    return { (v.x + 1.) * width/2, 
             (v.y + 1.) * height/2 }; }

void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer, TGAColor color) {

    int bbminx = std::min(std::min(ax, bx), cx); // bounding box for the triangle
    int bbminy = std::min(std::min(ay, by), cy); // defined by its top left and bottom right corners
    int bbmaxx = std::max(std::max(ax, bx), cx);
    int bbmaxy = std::max(std::max(ay, by), cy);
    double total_area = signed_triangle_area(ax, ay, bx, by, cx, cy);

    if (total_area<1) return;  // backface culling + discarding triangles that cover less than a pixel


    /*
preproccer directives (just like include)
tells the computer to do stuff before it starts compiling
pragma tells the comp : "if you understand the instruction, do it . If not , ignore it "
omp - open multi processing (tells the comp. to use all the cores for the for loop)

*/
#pragma omp parallel for 
        for (int x=bbminx; x<=bbmaxx; x++) {
            for (int y=bbminy; y<=bbmaxy; y++) {
/*
Barycentric coordinates 
 are proportional to sub-triangle areas area(PCB), area(PCA)and area(PAB)
. If all three coordinates are positive, 
 lies within the triangle. If any barycentric coordinate is negative, 
 lies outside the triangle.
*/
                double alpha = signed_triangle_area(x, y, bx, by, cx, cy) / total_area;
                double beta  = signed_triangle_area(x, y, cx, cy, ax, ay) / total_area;
                double gamma = signed_triangle_area(x, y, ax, ay, bx, by) / total_area;
                if (alpha<0 || beta<0 || gamma<0) continue; // negative barycentric coordinate => the pixel is outside the triangle
             
                framebuffer.set(x, y, color);
            }
    }
    
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " obj/model.obj" << std::endl;
        return 1;
    }

    Model model(argv[1]);
    TGAImage framebuffer(width, height, TGAImage::RGB);

    for (int i=0; i<model.nfaces(); i++) { // iterate through all triangles
        auto [ax, ay] = project(model.vert(i, 0));
        auto [bx, by] = project(model.vert(i, 1));
        auto [cx, cy] = project(model.vert(i, 2));
        TGAColor rnd;
        for (int c=0; c<3; c++) rnd[c] = std::rand()%255;
        triangle(ax, ay, bx, by, cx, cy, framebuffer, rnd);
    }

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}