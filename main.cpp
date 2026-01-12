#include "model.h"
#include <cstdlib>
#include "our_gl.h"
using namespace std;


extern mat<4,4> ModelView, Perspective; // "OpenGL" state matrices and
extern std::vector<double> zbuffer;     // the depth buffer

struct RandomShader : IShader {
    const Model &model;
    TGAColor color = {};
    vec3 tri[3];  // triangle in eye coordinates

    RandomShader(const Model &m) : model(m) {
        }

    virtual vec4 vertex(const int face, const int vert) {
        vec3 v = model.vert(face, vert);                          // current vertex in object coordinates
        vec4 gl_Position = ModelView * vec4{v.x, v.y, v.z, 1.};
        tri[vert] = gl_Position.xyz();                            // in eye coordinates
        return Perspective * gl_Position;                         // in clip coordinates
    }

    virtual std::pair<bool,TGAColor> fragment(const vec3 bar) const {
        return {false, color};                     
    }

};


int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " obj/model.obj" << std::endl;
        return 1;
    }
    constexpr int width  = 800;      // output image size
    constexpr int height = 800;

    constexpr vec3    eye{-1, 0, 2}; // camera position
    constexpr vec3 center{ 0, 0, 0}; // camera direction
    constexpr vec3     up{ 0, 1, 0}; // camera up vector
    constexpr vec3 light{-1,-1,-1};

    lookat(eye, center, up);                                   // build the ModelView   matrix
    init_perspective(norm(eye-center));                        // build the Perspective matrix
    init_viewport(width/16, height/16, width*7/8, height*7/8); // build the Viewport    matrix
    init_zbuffer(width, height);
    TGAImage framebuffer(width, height, TGAImage::RGB, {177, 195, 209, 255});

    for (int m=1; m<argc; m++) {                    // iterate through all input objects
        Model model(argv[m]);                       // load the data
        RandomShader shader(model);
        for (int f=0; f<model.nfaces(); f++) { // iterate through all facets

            vec3 v0 = model.vert(f, 0);
            vec3 v1 = model.vert(f, 1);
            vec3 v2 = model.vert(f, 2);

            vec3 edge1 = v1-v0;
            vec3 edge2 = v2-v0;
            vec3 n =normalized(cross(edge1,edge2));
            vec3 l = normalized(light);
            double intensity = std::max(0.,(n*l));
            shader.color = { 
                (unsigned char)(intensity * 255), 
                (unsigned char)(intensity * 255), 
                (unsigned char)(intensity * 255), 
                255 
            };
            Triangle clip = { shader.vertex(f, 0),  // assemble the primitive
                              shader.vertex(f, 1),
                              shader.vertex(f, 2) };
            rasterize(clip, shader, framebuffer);   // rasterize the primitive

        }
    }

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}