
# 🎨 Tiny Renderer: Pure C++ & Mathematics

> **"It's all about triangles and algebra. ;) "**

This is my implementation of a software rasterizer built from scratch in C++ (based on the legendary [ssloy/tinyrenderer](https://github.com/ssloy/tinyrenderer) documentation).

Over the last 1.5 months, I stepped away WebDev and AI  to understand **how graphics and GPU's actually work under the hood**. No libraries, no black boxes—just raw C++, linear algebra, and a whole lot of pixels.

![demo-image](/images/toonyMon.png)

---

## 🚀 What I Built

This is a fully functional 3D renderer that takes `.obj` files and renders them into `.tga` images. It features:

* **Wireframe Rendering:** The basics of drawing lines.
* **Triangle Rasterization:** Converting 3D coordinates into 2D filled shapes.
* **Z-Buffering:** Solving the visibility problem (handling depth).
* **Shaders**
* **Flat & Gouraud Shading:** Basic lighting models.
* **Phong Shading:** Per-pixel lighting for smooth surfaces.
* **Toon Shading:** Non-photorealistic rendering (cell shading + outlines).
* **Shadow Mapping:** Two-pass rendering to create realistic shadows.


* **Camera System:** Full 3D camera movement with LookAt transformations.

---

## 🧠 What I Learned (The "Aha!" Moments)

This project was a deep dive into the intersection of Computer Science and Mathematics. Here are my biggest takeaways:

### 1. Math is Real 📐

I finally saw the real-world application of **Linear Algebra**. Matrix multiplication isn't just abstract numbers on a whiteboard—it's how you move worlds.

* **Model-View-Projection (MVP):** I built the matrices manually to transform vertices from 3D space to screen coordinates.
* **Barycentric Coordinates:** The magic math used to fill triangles and interpolate textures.

### 2. High-Performance C++ 🔥

* **Low-Level Memory:** I got my hands dirty with raw pointers, memory management, and understanding how data is laid out in RAM.
* **Parallel Processing:** The coolest optimization I added was using OpenMP (`#pragma omp parallel for`). I learned how to distribute the rendering workload across all CPU cores, massively speeding up the frame generation.

### 3. The Graphics Pipeline 🖼️

I now understand the journey of a pixel:

1. **Vertex Processing:** Moving points in 3D space.
2. **Rasterization:** Turning shapes into pixels.
3. **Fragment Processing:** Deciding the color of each pixel (lighting, textures, shadows).

### 4. File Formats

* **OBJ:** Parsing raw text files to extract vertex and face data.
* **TGA:** Writing a binary image format from scratch, byte by byte.


---

## 📸 Gallery
*(playing around with rasterization and colored triangles)*
![demo-image](/images/triangle.png)
---
*(drawing a 3D iimage with only using a bunch of red trianlges!)*
![demo-image](/images/line-monster.png)
---
*(final image after drawing , coloring , shading , adding shadows)*
![demo-image](/images/completeMon.png)

---

## Acknowledgments

* Huge thanks to **Dmitry V. Sokolov (ssloy)** for the incredible [Tiny Renderer Wiki](https://github.com/ssloy/tinyrenderer/wiki).


---

*Built with ❤️, C++, and a lot of debugging by Selith.*