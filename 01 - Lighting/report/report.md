---
template: assignment.tex
subjectcode: TDT4230
subjectname: Graphics and Visualization
sec-num-depth: 0
assignment: 1
title: Lighting
date: February 27, 2022
---

\clearpage
# Task 1

## (f) Ambient

![Ambient Light Only. Small ball is location of single lightsource.](images/ambient.png)

The lights location has no effect on the ambient light. For this task the normal of the surfaces are used for their color to tell them apart, for all subsequent tasks the color is white for all objects in the scene. I implemented it such that the ambient light is a combination of all the light colors in the scene. In other words, if there is only single a red lightsource, the ambient light will also be red.

## (g) Diffuse

![Ambient + Diffuse. Small ball is location of single lightsource.](images/ambient+diffuse.png)

## (i) Specular Highlights{-}

![Ambient + Diffuse + Specular. Small ball is location of single lightsource.](images/ambient+diffuse+specular.png)



# Task 2

## (a) Attenuation

![After Adding Light Attenuation (Light gets dimmer over distance). Small ball is location of single lightsource.](images/attenuation.png)

## (b) Dithering

![Applying Dithering. Small ball is location of single lightsource.](images/dithering.png)

## (c) Shadows

![Adding Shadows. Small ball is location of single lightsource.](images/shadows.png)



# Task 3

## (b) Coloured Lights

![Coloured Lights. 1 Red, 1 Green and 1 Blue light placed in center of box.](images/color-blending.png)



# Task 4

![Smoothed Shadows. Coloured lights were moved closer to camera, hence to different shadow positions. There is also a white light placed high above the ball to create a shadow on the ground that is easy to follow when playing the game, this also illuminates the scene even more.](images/soft-shadows.png)
