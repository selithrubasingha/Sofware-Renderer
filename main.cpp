#include <cmath>
#include <tuple>
#include "geometry.h"
#include "model.h"
#include "tgaimage.h"

using namespace std;

constexpr int width  = 800;
constexpr int height = 800;

constexpr TGAColor white   = {255, 255, 255, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};


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

// We accept vec4, but we only use v.x and v.y. 
std::tuple<int,int> project(vec4 v) { 
    return { (v.x + 1.) * width/2, 
             (v.y + 1.) * height/2 }; 
}

int main(int argc, char** argv) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " obj/model.obj" << endl;
        return 1;
    }

    Model model(argv[1]);
    TGAImage framebuffer(width, height, TGAImage::RGB);

    for (int i=0; i<model.nfaces(); i++) { 
        auto [ax, ay] = project(model.vert(i, 0));
        auto [bx, by] = project(model.vert(i, 1));
        auto [cx, cy] = project(model.vert(i, 2));
        line(ax, ay, bx, by, framebuffer, red);
        line(bx, by, cx, cy, framebuffer, red);
        line(cx, cy, ax, ay, framebuffer, red);
    }

    for (int i=0; i<model.nverts(); i++) { 
        vec4 v = model.vert(i);            
        auto [x, y] = project(v);          
        framebuffer.set(x, y, white);
    }

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}