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
lof: false
lot: false
# 
author: Sivert Utne
date: \today
title: Dynamic Reflections and Refractions
subtitle: TDT4230 Graphics and Visualization
header-includes: |
    \fancyhead[l]{TDT4230\\\textbf{Graphics and Visualization}}
    \fancyhead[r]{Dynamic Reflections and Refractions\\\textbf{Sivert Utne}}
---


<!-- Briefly describe your project topic. -->
# Project Description


My initial idea for this project was to create as realistic light behavior as possible. This would include light reflection, refraction and dispersion where i would have multiple light sources and the light would would be refracted and reflected for all objects in the scene. So in essence, i wanted to perform Ray-Tracing. However i didn't actually want to implement it by tracing each ray, but look into alternative ways to achieve comparable results. The final iteration of the project achieves semi-realistic reflections, refractions and dispersion using dynamic cubemaps and framebuffers, a more classic approach when ray-tracing is too expensive.

This approach is for instance used for [reflections in the game GTA 5](https://www.adriancourreges.com/blog/2015/11/02/gta-v-graphics-study/).




<!-- How does your implementation achieve its goal? -->
# Implementation

## The Skybox

Implementing a skybox is relatively simple. To do this you create a CubeMap Texture with the different faces of the skybox, making sure to either draw it first with depth testing disabled (my approach), or draw it last, making sure that the dimensions of the cube are further away than any geometry in the scene.

![Cubemap Texture Example](images/theory/cubemap.png){#fig:theory-cubemap width=50%}

## Reflection

Basic reflection is the easiest of the light behaviors to achieve. To reflect only the skybox all you need is to calculate the direction of the refracted ray at a given fragments position, then the direction can be used directly to sample from the skybox's cubemap texture. Reflection of the skybox is shown in [@fig:skybox-reflection].

![Finding Reflection Vector](images/theory/reflection.gif){#fig:theory-reflection width=50%}

## Refraction

Refraction is done in almost exactly the same way as reflection, only using a different formula for getting the ray direction. Here the materials refraction index is used to determine how much the light will bend as it enters the object. Refraction of the skybox is shown in [@fig:skybox-refraction].

![Finding Refraction Vector](images/theory/refraction.jpeg){#fig:theory-refraction width=50%}

### Fresnel

The fresnel effect is an effect that occurs for all objects light can pass through, and is the effect that light is both reflected and refracted on the same object. An easy example for this is when you are standing on the shore of a quiet lake, looking straight down you see through the water, but as you look more and more towards the horizon the water turns more and more reflective until it turns into a perfect mirror. Accurate fresnel effect is quite complicated and time consuming to compute, as the behavior is quite complex. I instead simplified this to simply use a ratio based on the normal vector of the surface and the ray going from the fragment into the camera. To visualize the effect i am in [@fig:fresnel-viz] demonstrating how the reflection of the skybox will be applied on top of the refracted light. [@fig:fresnel] Shows the end result, notice how the top of the shoulders now reflect the sky, instead of refracting the water below.

![Demonstration of the Fresnel effect on calm body of water](images/theory/fresnel.png){#fig:theory-fresnel width=50%}


### Chromatic Aberration

Chromatic aberration is a form of light dispersion. Light dispersion is caused by the different wavelength (colors) of light having slightly different refraction indexes. This is what causes white light to turn into a rainbow when reflected of rain, or refracted through glass. To implement this i slightly offset each color channel's (RGB) refraction ratio. Applying this again to the same model in [@fig:dispersion] you can notice how the edges of the refracted mountains turn ''rainbow like''. The effect is very subtle, but adds a lot to the realism when moving around in the scene.

![Light Dispersion/Chromatic Aberration as light is refracted](images/theory/chromatic%20aberration.png){#fig:theory-chrome width=50%}


## Frame Buffers

My implementation relies heavily on the use of FrameBuffers. A Framebuffer is in essence a collection of buffers used to render the scene, among these are the color and depth buffers. by manually creating extra framebuffers you can tell OpenGL to render to these instead of the one used for the screen. These rendering can then be saved as textures that you can sample afterwards.

To create accurate reflections and refraction that include the objects in the scene, each object in my scene has its own framebuffer. When rendering the scene, the scene is rendered 6 times per reflective/refractive object to create a cubemap of the scene for that particular object. In [@fig:cubemap] you can see the difference between the skybox faces, and the dynamically created cubemap faces for one such reflective object.

The object will then in the final render pass use this cubemap instead of the normal skybox to fetch its reflection/refractions. The great thing about my approach is that when the dynamic cubemap for this object is created the next frame, all the other objects in the scene will be using the dynamic cubemap they created the previous frame. On the scene this has the effect of light bouncing back and forth between reflective objects. This is great news as this means we essentially get infinite "ray bounces", with no additional rendering cost. Of course creating a cubemap for each object isn't free, but this will be discussed in the "Limitations and Advantages" Section.

The final result can be seen in [@fig:result].





<!-- What are some notable problems you encountered on the way? How did you solve them? -->
# Problems

## Implementation Problems

My implentation workflow usually followed the path of:

- Look at a tutorial for an effect.
- Thinking that it will be quick and/or easy to implement.
- Implement it over 1 day and get weird results.
- Spend 2 days debugging why the results are weird.
- Find tiny bug in code, results look great.

One of the most obvious cases of this was after i implemented the dynamic cubemaps, and for some reason the right and left faces of the cubemap didn't align properly with the rest of the cubemap, ruining the appearance of reflections/refractions. After two days of debugging and refactoring, i realized i never changed the aspect ratio of my perspective matrix correctly, only the FOV.

All these bugs and annoyances cost me a fair bit of time, which is a shame as that kept me from finishing my implementation of double refraction. When performing ray tracing doubel refraction is simple, you enter the object, light bends and continues, ray hits other side of object, light bends and continues. But since i don't simulate the rays i had to find a way to figure out not only the refraction of the light as it hits the object, but also when it leaves it. My first though was that if i simply perform refraction when creating the dynamic cubemap, by switching from back-face to front-face culling to see the object from the inside, and then. However the result of this looked more like the refractions were broken because it assumes all refracted light passes through the center of the object. The result of attempting this approach is seen in [@fig:back-refraction-attempt]. Thats when i discovered another approach where you render the entire scene from the cameras perspective, again using front face culling and rendering the normals of the backside of refractive objects to a texture. When performing the final render pass this would mean you have access to both the front facing normal, and the back facing normal at each fragment position, along with the depth value of the front and back of the object. This approach is outlined in the paper listed under "Sources". However because i had spent so much time trying to fix bugs, i didn't have the time to fully implement this, although i did manage to create the texture with the back-side normals (see [@fig:back-normals]). This is really a shame because i feel like that is the only part of my scene that does not look realistic.

I smaller problem i had was that when i attempted to let an object reflect itself, it would not render anything as there wasn't anything to sample from, this is seen in [@fig:self]. Instead of finding a fix for this, i just skip rendering the object itself when creating its cubemap.

## Technical Problems

One of the most infuriating problems for me this project was not actually related to the project itself, but the fact that my computer (running bootcamp on MacOS), did not have proper functional drivers that did not cause a particular issue. This issue is that if i tried to sample a texture in a shader, the geometry would either turn black or 100% transparent. Now working with cubemaps that essentially are textures this caused a fair deal of headaches. This problem cost me even more days of trying to debug, only to find out all my code was working properly, but only on other computers. The problem is visualized in [@fig:driver-issue], where you see how calling the function (the problem is that inside this function i sample the skybox an extra time) without using the result for anything changes all geometry in the scene to turn black. This increased the complexity of working on this project considerably as i had limited access/time to run my code on computers that worked properly.

<!-- What did you find out about the method in terms of its advantages, its limitations, and how
to use it effectively? -->
# Limitations and Advantages {#sec:pro-con}

The limitations of the way i calculate the reflections and refractions instead of raytracing, is that the result loses a bit of realism. This is perhaps most noticeable in reflections of refractions, where the refraction will appear identical from all angles because the camera that creates hte cubemap never. The advantage of this approach is of course performance, in many cases the approximations appear ''good enough'' that without really analyzing it no one would be able to tell the difference.

One of the greatest limitations of the dynamic cubemap approach to reflections and refractions is the fact that each scene must be re-rendered several times to create each dynamic cubemap. If the reflection resolution is equal to the screen size, then that means the entire scene is rendered at full scale $6$ times per object $+ 1$ final render to the screen. For my scene which contains a maximum of 7 reflective/refractive objects, this means in worst case my scene would have to be rendered $7*6+1=43$ times per frame. Lets say rendering the frame without reflections/refractions i can achieve $1000FPS$, i would now only get approximately $1000/43=23FPS$.

This leads me to one of the greatest advantages of this approach, and that is that the approach is highly customizable. Customization means there is a lot of room for optimization. The most obvious one would be to reduce the resolution of the reflections, demonstrated in [@fig:low-resolution]. This is what was done GTA V, where the reflections had a resolution of $128\times128$ pixels. For perfect geometric shapes like the ones in my scene this is quite noticeable, but in a complex scene with complex models, the decrease in resolution is in most cases negligible. Other possible optimization would be decreasing the ''refresh-rate'' of the reflections, for instance only updating the reflection every other frame, or updating only one side of the cubemap each frame. In GTA V an optimization they did was to only create and use the top side of the cubemap, as reflections below the cars are rarely ever seen. It also isn't much point in updating reflections/refractions for objects far away from the camera, as the result isn't noticeable anyway. For my scene this could have been done for the top/bottom faces, as all my geometry is placed horizontally. These cubemaps can also, in the case where the objects don't move much, be pre rendered/created when the scene first loads. For objects placed in approximately the same location, a cubemap can also be shared. Of course rendering the entire scene might not be necessary either, and the scene can rendered without fancy textures, reduced geometry, closer fra-plane-clipping etc. Applying any or all of these optimization in a smart way will in theory achieve very realstic results, without impacting the performance in a drastic way.

If i were to optimize the reflections and refractions for my scene, i would first not recalculate the top/bottom face of the cubemap as nothing changes in these faces. I would then apply a dynamic resolution to the cubemaps such that objects very close to the camera have a high resolution, while those far away use a low resolution cubemap, or not create one at all. Since my scene is mostly stationary as well, there isn't really much point in updating the cubemaps unless something in the scene changes, like the bust rotating, or changing the materials of the objects.


<!-- Briefly mention what resources did you used to learn about the technique. No need to include
every link to everything you read, but I should get a general idea of how you figured it out,
even if the answer ends up being pure experimentation! -->
# Sources

Most of my results came from experimentation, and going back and forth, but my implementations are based on these tutorials:

- Cubemaps: [https://learnopengl.com/Advanced-OpenGL/Cubemaps](\url)
- Framebuffers: [https://learnopengl.com/Advanced-OpenGL/Framebuffers](\url)

I also took inspiration from the article about GTV linked earlier in the report to get a general idea about how i could go about creating and using cubemaps dynamically.

but when looking into ways to achieve the double refraction i also took some inspiration from this paper: [http://cwyman.org/papers/graphite05_InteractiveNearbyRefraction.pdf](\url), although as mentioned, i dint actually get around to implementing it completely.

All skyboxes and the marble bust model were downloaded from [https://polyhaven.com](\url)


\clearpage
# Appendix {-}

## Reflection and Refraction Screenshots{-}

![Reflection of the skybox.](images/reflection/skybox-reflection.png){#fig:skybox-reflection}

![Refraction of the skybox](images/refraction/1-refraction.png){#fig:skybox-refraction}

![Visualization of how the fresnel effect applies reflection to a transparent surface.](images/refraction/2-visualized-fresnel.png){#fig:fresnel-viz}

![Fresnel effect applied. Notice the reflection of the sky on the shoulders.](images/refraction/3-fresnel.png){#fig:fresnel}

![Chromatic aberration, notice the subtle rainbow effect along edges of the refracted mountains.](images/refraction/4-chromatic-aberration.png){#fig:dispersion}


## Dynamic Cubemap Screenshots {-}

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

Comparison of static (top) and dynamic (bottom) cubemaps.
:::

![Reflections of reflections. Note that most of this image is a reflection itself (on a sphere), as you notice on the far right side.](images/reflection/infinite-reflections.png){#fig:infinite-reflections}

![Identical to [@fig:infinite-reflections], but with a highly reduced reflection resolution.](images/reflection/low-quality.png){#fig:low-resolution}

![Reflection, refraction and chromatic aberration using dynamic cubemaps. Notice how the cube is both transparent and reflective because of the fresnel effect, and the chromatic aberration of refracted light on the side of the cube.](images/refraction/demo.png){#fig:result}


## Double refraction Attempts

![Attempt at double refraction using a cubemap, result was confusing to look at so i decied single refraction was better](images/refraction/back-side-attempts/attempt-at-double-refraction.png){#fig:back-refraction-attempt}

:::{#fig:back-normals}
![Shapes Front Normals](images/refraction/back-side-attempts/shapes-front.png){width=50%}\  
![Texture With Backside Normals](images/refraction/back-side-attempts/shapes-back.png){width=50%}

![Bust Front Normals](images/refraction/back-side-attempts/bust-front.png){width=50%}\  
![Texture With Bust Backside Normals](images/refraction/back-side-attempts/bust-back.png){width=50%}

Visualization of how the double refraction could have been implemented using created normal textures, had i not ran out of time.
:::


## Screenshots of problems {-}

![Artifacting (pink) when object attempted to reflect itself](images/reflection/artifact-when-reflecting-itself.png){#fig:self-reflection}

:::{#fig:driver-issue}
![Code where geometry is rendered correctly](images/annoying-driver-issue/code-blue-sphere.png){width=70%}\  
![Correctly rendered sphere](images/annoying-driver-issue/blue-sphere.png){width=20%}

![Code where geometry misbehaves, note that the value is never used, nor does it change any values in the function.](images/annoying-driver-issue/code-black-sphere.png){width=70%}\  
![Sphere turns black, this happened for ALL geometry in the scene.](images/annoying-driver-issue/black-sphere.png){width=20%}

Demonstration of how infuriating the graphics driver issue was to work with.
:::


