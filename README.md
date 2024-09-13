# SGD240-Planets
For this assessment, I have decided to undertake procedural planet generation in Unreal Engine 5.4.2 using a marching cubes algorithm.
This serves as a core game system to be iterated upon (i.e a survival game that takes place on planets).
The goal is to implement the algorithm with it being performant and modular, using professional coding conventions.

# Sprint 1 - Week 7

In its current form, planets are able to be generated procedurally at runtime. The approach is very 'primitive', all the work is currently being done
on the CPU. Future iterations of this project will try to offload some of the work to the GPU so mesh can be generated in parallel. Mesh chunking, hyperthreading
and a dynamic LODs are features I am also considering. Currently the procedural planet (which is not planet sized) takes approx. 10-30 seconds to generate at runtime
on 32GB RAM, R7 7800X3D, RTX 4090, so it is incredibly likely that more average systems may run out of memory at runtime.

Hurdles overcome so far:
- Voxel Grid (and debug visualization*)
- Marching cubes algorithm with lookup table (stored in a seperate class)
- Ability to assign materials
- Ability to adjust Radius (Voxel Density and Bounds are still hardcoded)
- Perlin3D noise to create more dynamic mesh
- Normals are recalculated after mesh generation and deformation

Hurdles to overcome:
- Set up classes for compute shader in unreal (incl. readback to CPU)
- Make meshing algorithm in compute shader
  

# Engine of Choice:
Unreal Engine 5.3.2

# IDE
Jetbrains Rider

# 3rd Party Plugins/API?
ShadeUp

# References/Resources

-  Polygonising a Scalar Field, Paul Bourke
   https://paulbourke.net/geometry/polygonise/

-  Sebastian Lague, Coding Adventures - Terraforming
   https://www.youtube.com/watch?v=vTMEdHcKgM4&t=169s&pp=ygUbc2ViYXN0YW4gbGFndWUgdGVycmFmb3JtaW5n
