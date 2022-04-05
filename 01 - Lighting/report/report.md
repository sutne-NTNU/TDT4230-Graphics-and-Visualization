---
lang: en-US 
papersize: a4 
geometry: margin=3cm 
numbersections: true
colorlinks: true
links-as-notes: true
toc: true
toc-depth: 3
toc-title: Contents
figPrefix: Figure
tblPrefix: Table
eqnPrefix: Equation
title: Assignment 1 - Report
author: Sivert Utne
date: \today
header-includes: |
    \AtBeginDocument{\floatplacement{figure}{H}}
    \AtBeginDocument{\floatplacement{codelisting}{H}}
    \usepackage{lastpage}
    \usepackage{cancel}
    \usepackage{listings}
    \usepackage{bm}
    \usepackage{fancyhdr}
    \pagestyle{fancy}
    \renewcommand{\headrulewidth}{0.1px}
    \renewcommand{\footrulewidth}{0.1px}
    \fancyfoot[c]{Page \thepage\ of \pageref{LastPage}}
    \fancyhead[l]{TDT4230\\\textbf{Graphics and Visualization}}
    \fancyhead[c]{}
    \fancyhead[r]{Assignment 1\\\textbf{Sivert Utne}}
---

\clearpage
# Task 1 {-}

## (f) Ambient {-}

![Ambient Light Only. Small ball is location of single lightsource.](images/ambient.png)

The lights location has no effect on the ambient light. For this task the normal of the surfaces are used for their color to tell them apart, for all subsequent tasks the color is white for all objects in the scene. I implemented it such that the ambient light is a combination of all the light colors in the scene. In other words, if there is only single a red lightsource, the ambient light will also be red.

## (g) Diffuse {-}

![Ambient + Diffuse. Small ball is location of single lightsource.](images/ambient+diffuse.png)

## (i) Specular Highlights{-}

![Ambient + Diffuse + Specular. Small ball is location of single lightsource.](images/ambient+diffuse+specular.png)



# Task 2 {-}

## (a) Attenuation {-}

![After Adding Light Attenuation (Light gets dimmer over distance). Small ball is location of single lightsource.](images/attenuation.png)

## (b) Dithering {-}

![Applying Dithering. Small ball is location of single lightsource.](images/dithering.png)

## (c) Shadows {-}

![Adding Shadows. Small ball is location of single lightsource.](images/shadows.png)



# Task 3 {-}

## (b) Coloured Lights {-}

![Coloured Lights. 1 Red, 1 Green and 1 Blue light placed in center of box.](images/color-blending.png)



# Task 4 {-}

![Smoothed Shadows. Coloured lights were moved closer to camera, hence to different shadow positions. There is also a white light placed high above the ball to create a shadow on the ground that is easy to follow when playing the game, this also illuminates the scene even more.](images/soft-shadows.png)
