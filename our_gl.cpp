#include <algorithm>
#include "our_gl.h"

mat<4,4> ModelView, Viewport, Perspective; // "OpenGL" state matrices
std::vector<double> zbuffer;               // depth buffer

void lookat(const vec3 eye, const vec3 center, const vec3 up) {
/*
OK so there is the world frame and the camera frame . the world frame center is O and has i,j,k unit vectors . 
the camera frame center is C and has l,m,n unit vectros . The thing we need to convert the world frame
to the camera frame coordintes . This can be done by multiplying the following matrices and it gives the x,y,z, coords
according to the camera frame . There are some hard mathematics regarding this you can read the documentation for further info . 

 website - https://haqr.eu/tinyrenderer/camera/
*/
    vec3 n = normalized(eye-center);
    vec3 l = normalized(cross(up,n));
    vec3 m = normalized(cross(n, l));
    ModelView = mat<4,4>{{{l.x,l.y,l.z,0}, {m.x,m.y,m.z,0}, {n.x,n.y,n.z,0}, {0,0,0,1}}} *
                mat<4,4>{{{1,0,0,-center.x}, {0,1,0,-center.y}, {0,0,1,-center.z}, {0,0,0,1}}};
}

void init_perspective(const double f) {
    /*
the alternative to the persp function . when the perspective matrix multiplies a vec4 [vx,vy,vz,1]
the result is [vx, vy, vz, -vz/f + 1] . when we do the homogeneous division by w , 
we get exactly the same result as the persp function!

*/
    Perspective = {{{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0, -1/f,1}}};
}

void init_viewport(const int x, const int y, const int w, const int h) {
    /*
Instead of using the project funtion . No wwe do something similar with a matrix multiplication.
multiplying the viewport with a [vx,vy,vz,1] vector gives us the same result as the project function
but here we do x+w/2 ... the x+ is for generalizing the point .(x,y) is the top left point . 
*/
    Viewport = {{{w/2., 0, 0, x+w/2.}, {0, h/2., 0, y+h/2.}, {0,0,1,0}, {0,0,0,1}}};
}

void init_zbuffer(const int width, const int height) {
    zbuffer = std::vector(width*height, -1000.);
}

void rasterize(const Triangle &clip, const IShader &shader, TGAImage &framebuffer) {
/*
Summary of the Flow
clip: Where is the triangle in 3D space? (4D inputs)
.ndc: Squish it into a standard $1\times1$ box (Divide by W)
.screen: Scale it to the image size (Pixels).
ABC/bc: For every single pixel in the box, ask: "Am I inside the triangle?"
zbuffer: If inside, am I closer to the camera than what was drawn before?
*/
    vec4 ndc[3]    = { clip[0]/clip[0].w, clip[1]/clip[1].w, clip[2]/clip[2].w };                // normalized device coordinates
    vec2 screen[3] = { (Viewport*ndc[0]).xy(), (Viewport*ndc[1]).xy(), (Viewport*ndc[2]).xy() }; // screen coordinates

    mat<3,3> ABC = {{ {screen[0].x, screen[0].y, 1.}, {screen[1].x, screen[1].y, 1.}, {screen[2].x, screen[2].y, 1.} }};
    if (ABC.det()<1) return; // backface culling + discarding triangles that cover less than a pixel

    auto [bbminx,bbmaxx] = std::minmax({screen[0].x, screen[1].x, screen[2].x}); // bounding box for the triangle
    auto [bbminy,bbmaxy] = std::minmax({screen[0].y, screen[1].y, screen[2].y}); // defined by its top left and bottom right corners
#pragma omp parallel for
    for (int x=std::max<int>(bbminx, 0); x<=std::min<int>(bbmaxx, framebuffer.width()-1); x++) {         // clip the bounding box by the screen
        for (int y=std::max<int>(bbminy, 0); y<=std::min<int>(bbmaxy, framebuffer.height()-1); y++) {
            vec3 bc_screen = ABC.invert_transpose() * vec3{static_cast<double>(x), static_cast<double>(y), 1.}; // barycentric coordinates of {x,y} w.r.t the triangle
            vec3 bc_clip   = { bc_screen.x/clip[0].w, bc_screen.y/clip[1].w, bc_screen.z/clip[2].w };     // check https://github.com/ssloy/tinyrenderer/wiki/Technical-difficulties-linear-interpolation-with-perspective-deformations
            bc_clip = bc_clip / (bc_clip.x + bc_clip.y + bc_clip.z);
            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue;                                                    // negative barycentric coordinate => the pixel is outside the triangle
            double z = bc_screen * vec3{ ndc[0].z, ndc[1].z, ndc[2].z };  // linear interpolation of the depth
            if (z <= zbuffer[x+y*framebuffer.width()]) continue;   // discard fragments that are too deep w.r.t the z-buffer
            auto [discard, color] = shader.fragment(bc_clip);
            if (discard) continue;                                 // fragment shader can discard current fragment
            zbuffer[x+y*framebuffer.width()] = z;                  // update the z-buffer
            framebuffer.set(x, y, color);                          // update the framebuffer
        }
    }
}

