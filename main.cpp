#include "model.h"
#include <cstdlib>
#include "our_gl.h"
#include <algorithm> 
using namespace std;


extern mat<4,4> ModelView, Perspective; // "OpenGL" state matrices and
extern std::vector<double> zbuffer;     // the depth buffer

struct RandomShader : IShader {
    const Model &model;
    vec4 l;
    vec2 varying_uv[3];  // triangle uv coordinates, written by the vertex shader, read by the fragment shader
    vec4 varying_nrm[3];

    TGAColor color = {};
    


    RandomShader(const vec3 light, const Model &m) : model(m) {
        l = normalized((ModelView*vec4{light.x, light.y, light.z, 0.})); // transform the light vector to view coordinates
        }

    virtual vec4 vertex(const int face, const int vert) {
        varying_uv[vert] = model.uv(face, vert);

        varying_nrm[vert] = ModelView.invert_transpose() * model.normal(face, vert);
        vec4 gl_Position = ModelView * model.vert(face, vert);
        return Perspective * gl_Position;                         // in clip coordinates                       // in clip coordinates
    }

    // virtual std::pair<bool,TGAColor> fragment(const vec3 bar) const {
    //     TGAColor gl_FragColor = {255, 255, 255, 255};             // output color of the fragment

    //     vec2 uv = (varying_uv[0] * bar[0] +
    //               varying_uv[1] * bar[1] +
    //               varying_uv[2] * bar[2]);
        
    //     const TGAImage& diffusemap = model.diffuse();
    //     int u_diff = uv.x * diffusemap.width();
    //     int v_diff = uv.y * diffusemap.height();
    //     TGAColor base_color = diffusemap.get(u_diff, v_diff);

    //     const TGAImage& specmap = model.specular(); 
    //     int u_spec = uv.x * specmap.width();
    //     int v_spec = uv.y * specmap.height();
    //     double specular_intensity = specmap.get(u_spec, v_spec)[0];



    //     vec4 n = normalized(ModelView.invert_transpose() * model.normal(uv));
    //     vec4 r = normalized(n * (n * l)*2 - l);                   // reflected light direction
        
        
    //     double ambient = .3;                                      // ambient light intensity
    //     double diff = std::max(0., n * l);                        // diffuse light intensity
    //     double spec = std::pow(std::max(r.z, 0.), 35);            // specular intensity, note that the camera lies on the z-axis (in eye coordinates), therefore simple r.z, since (0,0,1)*(r.x, r.y, r.z) = r.z
        
    //     for (int channel : {0,1,2}){
    //         double color_part = base_color[channel] * (ambient + diff);

    //         double shine_part = 0.6 * spec * specular_intensity;

    //         double result = color_part + shine_part;

    //         gl_FragColor[channel] = std::min(255., result);}
        
    // return {false, gl_FragColor};
    // }


    virtual std::pair<bool, TGAColor> fragment(const vec3 bar) const {
    // ... (diffuse calculation remains the same) ...
    TGAColor gl_FragColor = {255, 255, 255, 255};             // output color of the fragment

    vec2 uv = (varying_uv[0] * bar[0] +
                varying_uv[1] * bar[1] +
                varying_uv[2] * bar[2]);
    
    vec4 bn = (varying_nrm[0]*bar[0] + varying_nrm[1]*bar[1] + varying_nrm[2]*bar[2]);
    bn = normalized(bn); // Normalize the result
    
    const TGAImage& diffusemap = model.diffuse();
    int u_diff = uv.x * diffusemap.width();
    int v_diff = uv.y * diffusemap.height();
    TGAColor base_color = diffusemap.get(u_diff, v_diff);
    // 1. Specular Fallback
    double specular_intensity = 0;
    if (model.has_specular()) {
        // If map exists, sample it
        int u_spec = uv.x * model.specular().width();
        int v_spec = uv.y * model.specular().height();
        specular_intensity = model.specular().get(u_spec, v_spec)[0];
    } else {
        // FALLBACK: If no map, assume a constant shine (e.g., skin/cloth usually has some specularity)
        specular_intensity = 10.0; 
    }

    // 2. Normal Mapping Fallback
    vec4 n;

    if (model.has_normal()) {
        // If map exists, use normal mapping
        n = normalized(ModelView.invert_transpose() * model.normal(uv));
    } else {
        // FALLBACK: Use the interpolated geometry normal
        // You need to pass the normal from the vertex shader to the fragment shader
        // via a new varying variable (e.g., varying_n) just like you did with varying_uv.
        // For now, assuming you add that:
        n = bn;
    }

    vec4 r = normalized(n * (n * l)*2.0 - l);  

    double ambient = .3;                                      // ambient light intensity
    double diff = std::max(0., n * l);                        // diffuse light intensity
    double spec = std::pow(std::max(r.z, 0.), 35);            // specular intensity, note that the camera lies on the z-axis (in eye coordinates), therefore simple r.z, since (0,0,1)*(r.x, r.y, r.z) = r.z
    for (int channel : {0,1,2}){
        double color_part = base_color[channel] * (ambient + diff);

        double shine_part = 0.6 * spec * specular_intensity;

        double result = color_part + shine_part;

        gl_FragColor[channel] = std::min(255., result);}
        
    return {false, gl_FragColor};
    
    // ... (lighting calculation remains the same) ...
}
};


int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " obj/model.obj" << std::endl;
        return 1;
    }
    constexpr int width  = 800;      // output image size
    constexpr int height = 800;

    constexpr vec3    eye{0, 0.8, 1}; // camera position
    constexpr vec3 center{ 0, 0.8, 0}; // camera direction
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

        double min_x = 1e10, max_x = -1e10;
        double min_y = 1e10, max_y = -1e10;
        double min_z = 1e10, max_z = -1e10;

        for(int i=0; i<model.nverts(); i++) {
            vec4 v = model.vert(i);
            min_x = std::min(min_x, v[0]); max_x = std::max(max_x, v[0]);
            min_y = std::min(min_y, v[1]); max_y = std::max(max_y, v[1]);
            min_z = std::min(min_z, v[2]); max_z = std::max(max_z, v[2]);
        }
        
        std::cerr << "=== MODEL STATS ===" << std::endl;
        std::cerr << "Height (Y): " << min_y << " to " << max_y << std::endl;
        std::cerr << "Width  (X): " << min_x << " to " << max_x << std::endl;
        std::cerr << "Depth  (Z): " << min_z << " to " << max_z << std::endl;
        std::cerr << "Center Y:   " << (min_y + max_y) / 2.0 << std::endl;
        std::cerr << "===================" << std::endl;


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