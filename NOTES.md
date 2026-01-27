# Explaining shadow mapping 


Imagine you are drawing a picture of your friend standing in a sunny park. To make it look real, you need to know which parts of the ground are in sunlight and which parts are in your friend's shadow.

Computers do this using a trick called **Shadow Mapping**. It works in two big steps, like taking two different photos.

---

### Step 1: The "Light's Eye View" (The Shadow Map)

First, imagine the **Sun is a camera**. If the Sun took a picture of your friend, it would only see the parts of them that are lit up. It *cannot* see the shadowy parts because the friend's body blocks the view.

In this step, the computer doesn't care about colors. It only cares about **distance**. It records "How far is the closest object to the light?" for every single pixel.

* **The Result:** A special black-and-white image called a **Depth Map** (or Shadow Map).
* **Dark pixels** = Object is close to the light.
* **Light pixels** = Object is far away.



**In your code:**
This happens inside the block `{ // shadow rendering pass ... }`.

1. **`lookat(light, center, up);`**: You move the "camera" to the light source.
2. **`rasterize(...)`**: You draw the model, but you ignore colors. You only save the **Z-buffer** (depth).
3. **`zbuffer2.tga`**: This file (saved by `drop_zbuffer`) is your Shadow Map!

---

### Step 2: The "Real Camera View" (The Normal Render)

Now, you go back to your real camera (the `eye` position). You take a normal picture of your friend. You see everything: their face, their clothes, and the ground behind them.

But wait! You don't know where the shadows are yet. You just have a colorful picture.

**In your code:**
This happens at the start of `main()`:

1. **`lookat(eye, center, up);`**: Move the camera to where you (the player) are.
2. **`rasterize(...)`**: Draw the Diablo model with colors and textures.
3. **`framebuffer.tga`**: This is the clean image *without* shadows (the one you uploaded!).

---

### Step 3: The Magic Question (Is it in Shadow?)

Now comes the math trick. You have a pixel on the ground in your normal picture. You want to know: **"Is this pixel in shadow?"**

To answer this, you do a teleportation trick:

1. You take that point on the ground (from the Camera's view).
2. You mathematically move it to the **Light's view**.
3. You ask the Shadow Map: **"Hey Light, how far was the closest thing you saw at this spot?"**

**The Comparison:**

* **Case A (Sunlight):**
* The distance you calculated matches the distance in the Shadow Map.
* *Conclusion:* The Light can see this spot directly. It is **LIT**.


* **Case B (Shadow):**
* The Shadow Map says, "The closest thing I saw was 5 meters away."
* But your point is **10 meters** away from the light.
* *Conclusion:* Something else (like the Diablo model) is closer to the light and is blocking the view. This point is in **SHADOW**.



**In your code:**
This is the complex loop at the end of `main()`:

1. **`vec4 fragment = M * ...`**: "Teleport" the pixel from Camera View to Light View.
2. **`p.z > zbuffer[...]`**: Compare depths.
* `p.z`: The distance of the current pixel to the light.
* `zbuffer[...]`: The distance recorded in the Shadow Map (closest object).


3. If `p.z` is bigger (further away), it means something is blocking the light. `lit` becomes `false`.

---

### Summary of the Files

* **`framebuffer.tga`**: Step 2 only. Just the model, no shadows. (This is what you saw).
* **`shadow.tga`**: The Final Result. It takes `framebuffer.tga` and darkens the pixels that failed the "Magic Question" test in Step 3.

**Open `shadow.tga` and you should see the shadows!**

Here is the abstract breakdown of your "production line." You are essentially running a movie studio where you have to film the same scene twice to get the lighting right.

### The 4 Files (The Outputs)

1. **`framebuffer.tga` (The Beauty Shot):**
* **What it is:** The normal photo of your monster taken from the main camera.
* **Abstract Meaning:** "This is what the world looks like *before* we figure out where the shadows go."


2. **`zbuffer1.tga` (The Camera's Depth Chart):**
* **What it is:** A black-and-white image matching the beauty shot.
* **Abstract Meaning:** It proves the camera knows what is close (white) and what is far away (black). It’s purely for you to check if the 3D math is working.


3. **`shadowmap.tga` (The Light's "Useless" Photo):**
* **What it is:** A photo of the monster taken by the **Light Source**.
* **Abstract Meaning:** The code confusingly calls the variable `trash` because we don't actually need the *colors* of this image. We only rendered it to trick the engine into calculating the depth for the light.


4. **`zbuffer2.tga` (The REAL Shadow Map):**
* **What it is:** The black-and-white depth chart taken from the **Light's perspective**.
* **Abstract Meaning:** **This is the most important file for shadows.** It is the "Cheat Sheet." It tells the computer exactly how far the light can see before it hits an object. Any point further than this distance is in shadow.



---

### The Code Blocks (The Actions)

**1. The First Loop (The Main Pass)**

* **Goal:** Draw the `framebuffer.tga`.
* **Action:** It looks at the model from your camera's point of view and paints the pixels. It creates the "Beauty Shot."

**2. The `drop_zbuffer` Function**

* **Goal:** Debugging.
* **Action:** It takes the invisible math numbers (depth) that the computer uses and converts them into a visible picture (`zbuffer1.tga` or `zbuffer2.tga`) so you can look at it and say, "Ah, yes, the nose is closer than the tail."

**3. The Second Loop (The Shadow Pass)**

* **Goal:** Create the "Cheat Sheet" (`zbuffer2.tga`).
* **Action:** It moves the camera to the Sun's position and takes a picture. It throws away the color photo (`trash` / `shadowmap.tga`) but keeps the depth data (`zbuffer`). This creates the map needed to check for shadows later.

**4. The Third Loop (The Comparison Loop)**

* **Goal:** The Magic Logic.
* **Action:** It walks through every pixel of your "Beauty Shot" and asks the "Cheat Sheet" a question: *"Hey, can the Sun see this specific spot on the monster?"*
* If **Yes**: It marks that pixel as `lit` (Light).
* If **No**: It marks that pixel as `mask` (Shadow).



**5. The Fourth Loop (The Final Touch)**

* **Goal:** Apply the makeup.
* **Action:** It reads the `mask` created in the previous loop. If a pixel is marked as "Shadow," it ignores it (or darkens it). If it is "Lit," it saves the color. This produces the final **`shadow.tga`**.