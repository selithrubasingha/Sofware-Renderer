#include "tgaimage.h"
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
        if (ierror > bx - ax) {
            y += by > ay ? 1 : -1;
            ierror -= 2 * (bx-ax);
    }

}

int main(int argc, char** argv) {
    constexpr int width  = 64;
    constexpr int height = 64;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    int ax =  7, ay =  3;
    int bx = 12, by = 37;
    int cx = 62, cy = 53;

    line(ax, ay, bx, by, framebuffer, blue);
    line(cx, cy, bx, by, framebuffer, green);
    line(cx, cy, ax, ay, framebuffer, yellow);
    line(ax, ay, cx, cy, framebuffer, red);

    framebuffer.set(ax, ay, white);
    framebuffer.set(bx, by, white);
    framebuffer.set(cx, cy, white);

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}