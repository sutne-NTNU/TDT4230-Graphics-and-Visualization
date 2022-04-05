---
toc-title: Contents
eqnPrefix: Equation
figPrefix: Figure
lofTitle: Figures
tblPrefix: Table
lotTitle: Tables
lolPrefix: Listing
lolTitle: Listings
numbersections: false
toc: true
lof: true
lot: true
# 
author: Sivert Utne
date: \today
title: Project Description
subtitle: TDT4230 Graphics and Visualization
header-includes: |
    \fancyhead[l]{TDT4230\\\textbf{Graphics and Visualization}}
    \fancyhead[r]{Project Description\\\textbf{Sivert Utne}}
---


[HDRI image](https://polyhaven.com/a/neurathen_rock_castle)

<!-- https://www.turais.de/how-to-load-hdri-as-a-cubemap-in-opengl/ -->

# Ray Traced Light Refraction and Dispersion

For my project i want to explore realistic light behavior primarily through water and glass. Some things i hope to achieve is:

- Rainbow light after passing white light through a glass prism
- Accurate light refraction through glass/water shapes

The program basics:

Simple scene with a cubemap to create the accurate refractions through glass objects. Able to move the camera around with WASD+Space+LShift. The mouse changes the camera rotation.
The Scene should also contain a "table" with a glass prism, lightsource should be able to be rotated around this glass prism with the mouse scrollwheel, light colour can be changed using RGB, idea is that this will make it possible to see how the light angle and color affects the refraction and dispersion.


**In short, i want to be able to reproduce the following real life effects:**

![White light being dispersed into all visible colors due to their wavelengths.](images/prism.jpg){width=60%}

![Interesting refraction from water in a glass](images/refraction.jpeg){width=60%}




# Ray Traced Reflections, Refraction and Dispersion

## Attempt 1 - CubeMap

Easy, just follow tutorial, decent enough for basic cases, only reflect the cubemap nothing in the scene

## Attempt 2 - Dynamic / Bufferreed Cubemaps

Render the scene multiple times and use the scene to create new cubemaps, placing the camera in reflective objects to find other objects in scene.

This requires rerendering the scene 6 times per object, per "ray bounce" so if i want reflections to go three times the scene is rendered 18 times per reflective object

This is also quite high tech considering it is used in a AAA game realeased just 1 month ago [GTA V](https://www.adriancourreges.com/blog/2015/11/02/gta-v-graphics-study/).



Okay so here is the final idea:

To get "double" refraction, just render the scene in each glass object with GL_CULL_FACE reversed (so the inside will perfor)


## Buttons

- WASD: mode camera physically
- Space/LShift: move camera up and down
- Q: decrease camera sensitivity and speed
- E: increase camera sensitivity and speed
- R: reset camera sensitivity and speed
- T: Pause/Start rotation of objects in the scene
- L: Swap Scene (skybox)


## for reflections

- Render scene 6 times to 6 different (color only) framebuffers
- Create cubemap from these 6 textures
- Use the generated cubemap for the main render


## For double refraction:

- Render scene with frontface culling
- Store normals to color texture (framebuffer)
- Use combination of normals from this texture and from the actual normals to create double refraction
