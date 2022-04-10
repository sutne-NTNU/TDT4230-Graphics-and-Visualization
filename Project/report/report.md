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
lot: false
# 
author: Sivert Utne
date: \today
title: Reflection and Refraction with Dynamic Cubemaps and Framebuffers
subtitle: TDT4230 Graphics and Visualization
header-includes: |
    \fancyhead[l]{TDT4230\\\textbf{Graphics and Visualization}}
    \fancyhead[r]{Project Description\\\textbf{Sivert Utne}}
---



<!-- Briefly describe your project topic. -->
# Project Description

## Initial Idea

My initial idea for tis project was to create as realistic light behavior as possible. This would include light reflection, refraction, dispersion...

## Final Project
The final iteration of the project achieves most of what i sent out to achieve, however the major difference here is that it only applies these effects on light going directly into the camera. In other words the project was simplified from a global lighting, where for instace the refraction of a glass sphere would impact the light behind it, to only handle light going into the camera. The main parts of the project are shown in the subsections below:

# Results

## Reflection

![Reflection of the skybox.](images/reflection/skybox-reflection.png){#fig:skybox-reflection width=100%}

## Refraction

![Refraction of the skybox](images/refraction/1-refraction.png)

## Fresnel

![Visualization of how the fresnel effect applies reflection to the surface.](images/refraction/2-visualized-fresnel.png)

![Fresnel effect applied.](images/refraction/3-fresnel.png)


## Chromatic Abberition

![Chromatic abberition, notice the subtle red colour along the edges of the refracted mountains.](images/refraction/4-chromatic-abberition.png)

## End result

![Reflection, refraction and chromatic abberition.](images/refraction/demo.png)


<!-- How does your implementation achieve its goal? -->
# Implementation

My implementation relies heavily on the use of FrameBuffers. A Framebuffer is a buffer containing both depth, color and stencil, but for my project i only used the color buffer.

To create accurate reflections and refraction that include the objects in the scene, each Object has its own framebuffer. When the scene is rendered the first frame this framebuffer is empty. On each render the scene is rendered 6 times per reflective/refractive object to create a cubemap of the scene for that particular object.

:::{#fig:cubemap}
![right ](../res/cubemaps/forest/right.png){width=16%} \  
![left  ](../res/cubemaps/forest/left.png){width=16%} \  
![top   ](../res/cubemaps/forest/top.png){width=16%} \  
![bottom](../res/cubemaps/forest/bottom.png){width=16%} \  
![front ](../res/cubemaps/forest/front.png){width=16%} \  
![back  ](../res/cubemaps/forest/back.png){width=16%}

![right ](images/cubemap/px.png){width=16%}\  
![left  ](images/cubemap/nx.png){width=16%}\  
![top   ](images/cubemap/py.png){width=16%}\  
![bottom](images/cubemap/ny.png){width=16%}\  
![front ](images/cubemap/pz.png){width=16%}\  
![back  ](images/cubemap/nz.png){width=16%}

The original skybox (top) vs the dynamically created cubemap for an object (bottom).
:::



The object will then in the final render pass use this cubemap instead of the normal skybox to fetch its reflection/refractions. The great thing about this approach is that when the dynamic cubemap for this object is created the next frame, all the other objects in the scene will be using the dynamic cubemap they created the previous frame. On the scene this has the effect of light bouncing back and forth between reflective objects. This is great news as this means we essentially get infinite "ray bounces", with no additional rendering cost. Ofcourse creating a cubemap for each object isnt free, but this will be discussed in the [Advantages/Disadvantages] section.

![''Infinite'' reflections / refractions. Note that most of this image is a reflection itself (on a sphere), as you can notice on the far right side.](images/reflection/infinite-reflections.png)

<!-- What are some notable problems you encountered on the way? How did you solve them? -->
# Problems

## Implementation Problems

![Artifacting when reflecting itself](images/reflection/artifact-when-reflecting-itself.png)

![Attempt at double refraction using a cubemap, result was confusing to look at so i decied single refraction was better](images/refraction/back-side-attempts/attempt-at-double-refraction.png)

:::{#fig:back-normals}
![Shapes Front Normals](images/refraction/back-side-attempts/shapes-front.png){width=50%}\  
![Texture With Backside Normals](images/refraction/back-side-attempts/shapes-back.png){width=50%}

![Bust Front Normals](images/refraction/back-side-attempts/bust-front.png){width=50%}\  
![Texture With Bust Backside Normals](images/refraction/back-side-attempts/bust-back.png){width=50%}

Visualization of how the double refraction could have been implemented, had i not ran out of time.
:::

## Techincal Problems

One of the most infuriating problems for me this project was not actually realted to the project itself, but the fact that my computer (running bootcamp on MacOS), did not have proper functional drivers that did not cause a perticualr issue. This issue is that if i tried to sample a texture in a shader, the gometry would either turn black or 100% transparent. Now working with cubemaps that aessentially are textures this caused a fair deal of headaches.

<!-- What did you find out about the method in terms of its advantages, its limitations, and how
to use it effectively? -->
# Advantages/Disadvantages


![Low Qualit](images/reflection/low-quality.png)

<!-- Briefly mention what resources did you used to learn about the technique. No need to include
every link to everything you read, but I should get a general idea of how you figured it out,
even if the answer ends up being pure experimentation! -->
# Sources and Progress


All skyboxes and the marble bust model were downloaded from [Poly Haven]
[HDRI image](https://polyhaven.com/a/neurathen_rock_castle)