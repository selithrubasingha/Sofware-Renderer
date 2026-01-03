#include <cmath>
#include <tuple>
#include "geometry.h"
#include "model.h"
#include "tgaimage.h"

using namespace std;

constexpr int width  = 128;
constexpr int height = 128;

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

void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer, TGAColor color) {
    line(ax, ay, bx, by, framebuffer, color);
    line(bx, by, cx, cy, framebuffer, color);
    line(cx, cy, ax, ay, framebuffer, color);
}

int main(int argc, char** argv) {
    TGAImage framebuffer(width, height, TGAImage::RGB);

    triangle(  7, 45, 35, 100, 45,  60, framebuffer, red);
    triangle(120, 35, 90,   5, 45, 110, framebuffer, white);
    triangle(115, 83, 80,  90, 85, 120, framebuffer, green);
    
    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}