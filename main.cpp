#include "model.h"
#include <cstdlib>
#include "our_gl.h"
#include <algorithm> 
using namespace std;

/// saving the colored image work
extern mat<4,4> ModelView, Perspective; // "OpenGL" state matrices and
extern std::vector<double> zbuffer;     // the depth buffer

struct RandomShader : IShader {
    const Model &model;
    vec4 l;
    vec2 varying_uv[3];  // triangle uv coordinates, written by the vertex shader, read by the fragment shader
    vec4 varying_nrm[3]; // triangle normal coordinates, written by the vertex shader, read by the fragment shader
    vec4 tri[3]; //triangle in view coordinates . 
    TGAColor color = {};
    


    RandomShader(const vec3 light, const Model &m) : model(m) {
        l = normalized((ModelView*vec4{light.x, light.y, light.z, 0.})); // transform the light vector to view coordinates
        }

    virtual vec4 vertex(const int face, const int vert) {
        varying_uv[vert] = model.uv(face, vert);
        varying_nrm[vert] = ModelView.invert_transpose() * model.normal(face, vert);

        /*
        GL position is ::"Where this point is relative to the camera."
        You are using it as a temporary variable to hold the vertex after the camera moved it, but before the perspective squashed it
        */
        vec4 gl_Position = ModelView * model.vert(face, vert);
        tri[vert] = gl_Position;                                   // triangle in view coordinates
        return Perspective * gl_Position;                         // in clip coordinates                       // in clip coordinates
    }

    virtual std::pair<bool,TGAColor> fragment(const vec3 bar) const {
        TGAColor gl_FragColor = {255, 255, 255, 255};             // output color of the fragment

        vec2 uv = (varying_uv[0] * bar[0] +
                  varying_uv[1] * bar[1] +
                  varying_uv[2] * bar[2]);

        /*
        Calculation of the TBN matrix!
        */

        vec4 e1 = tri[1] - tri[0];
        vec4 e2 = tri[2] - tri[0];

        mat<4,2> E ;

        E[0] = vec2{e1.x, e2.x};
        E[1] = vec2{e1.y, e2.y};
        E[2] = vec2{e1.z, e2.z};
        E[3] = vec2{e1.w, e2.w};

        vec2 uv_diff1 = varying_uv[1] - varying_uv[0];
        vec2 uv_diff2 = varying_uv[2] - varying_uv[0];

        mat<2,2> U;
        U[0] = vec2{uv_diff1.x, uv_diff2.x};
        U[1] = vec2{uv_diff1.y, uv_diff2.y};

        // Invert U and multiply to get Tangent (T) and Bitangent (B)
        mat<4,2> tb = E * U.invert();

        mat<4,3> TBN;

        vec4 n = varying_nrm[0]*bar[0] + varying_nrm[1]*bar[1] + varying_nrm[2]*bar[2];
        n = normalized(n);

                // Normalize T and B (Crucial for lighting!)
        vec4 T = normalized(vec4{tb[0][0], tb[1][0], tb[2][0], 0});
        vec4 B = normalized(vec4{tb[0][1], tb[1][1], tb[2][1], 0});

        // Fill the TBN Matrix (4 Rows, 3 Columns)
        // Row 0 (X components)
        TBN[0] = vec3{T.x, B.x, n.x};

        // Row 1 (Y components)
        TBN[1] = vec3{T.y, B.y, n.y};

        // Row 2 (Z components)
        TBN[2] = vec3{T.z, B.z, n.z};

        // Row 3 (W components - usually 0 for direction vectors)
        TBN[3] = vec3{0, 0, 0};



        
        

        

        
        const TGAImage& diffusemap = model.diffuse();
        int u_diff = uv.x * diffusemap.width();
        int v_diff = uv.y * diffusemap.height();
        TGAColor base_color = diffusemap.get(u_diff, v_diff);

        const TGAImage& specmap = model.specular(); 
        int u_spec = uv.x * specmap.width();
        int v_spec = uv.y * specmap.height();
        double specular_intensity = specmap.get(u_spec, v_spec)[0];



        vec4 n = normalized(ModelView.invert_transpose() * model.normal(uv));
        vec4 r = normalized(n * (n * l)*2 - l);                   // reflected light direction
        
        
        double ambient = .3;                                      // ambient light intensity
        double diff = std::max(0., n * l);                        // diffuse light intensity
        double spec = std::pow(std::max(r.z, 0.), 35);            // specular intensity, note that the camera lies on the z-axis (in eye coordinates), therefore simple r.z, since (0,0,1)*(r.x, r.y, r.z) = r.z
        for (int channel : {0,1,2}){
            double color_part = base_color[channel] * (ambient + diff);

            double shine_part = 0.6 * spec * specular_intensity;

            double result = color_part + shine_part;

            gl_FragColor[channel] = std::min(255., result);}
        
    return {false, gl_FragColor};
    }

};


int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " obj/model.obj" << std::endl;
        return 1;
    }
    constexpr int width  = 800;      // output image size
    constexpr int height = 800;

    constexpr vec3    eye{0, 1, 2}; // camera position
    constexpr vec3 center{ 0, 0, 0}; // camera direction
    // constexpr vec3 center{ 0, 0, 0}; // camera direction
    constexpr vec3     up{ 0, 1, 0}; // camera up vector
    constexpr vec3 light{1,1,1};
    
    

    lookat(eye, center, up);                                   // build the ModelView   matrix
    init_perspective(norm(eye-center));                        // build the Perspective matrix
    init_viewport(width/16, height/16, width*7/8, height*7/8); // build the Viewport    matrix
    init_zbuffer(width, height);
    TGAImage framebuffer(width, height, TGAImage::RGB, {177, 195, 209, 255});

    for (int m=1; m<argc; m++) {                    // iterate through all input objects
        Model model(argv[m]);                       // load the data
        RandomShader shader(light,model);
        for (int f=0; f<model.nfaces(); f++) { // iterate through all facets

            
            

            double intensity = 0.2;

            

            Triangle clip = { shader.vertex(f, 0),  // assemble the primitive
                              shader.vertex(f, 1),
                              shader.vertex(f, 2) };
            
            shader.color = { 
                (unsigned char)(intensity * 255), 
                (unsigned char)(intensity * 255), 
                (unsigned char)(intensity * 255), 
                255 
            };

            
            rasterize(clip, shader, framebuffer);   // rasterize the primitive

        }
    }

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}