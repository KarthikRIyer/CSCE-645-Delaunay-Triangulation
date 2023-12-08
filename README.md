# CSCE-645 Project: Delaunay Triangulation and Voronoi Diagram

## How to build and run

### Pre-requisites:
- CMake
- make
- A C++17 compiler
- GLM
- GLFW  
- GLEW
- OpenGL

Set the following environment variables:

```console
GLM_INCLUDE_DIR=/Users/karthik/lib/glm-0.9.9.8;GLFW_DIR=/Users/karthik/lib/glfw-3.3.8;GLEW_DIR=/Users/karthik/lib/glew-2.1.0;
```

### Build instructions

From the root directory of the project run:
```console
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4
./DelaunayTri ../resources
```

### How to use

- Click within the rectangular domain to insert a point
- Press the `G` key to generate a random set of point
- Press the `T` key to generate the Delaunay Triangulation and corresponding Voronoi diagram

# Acknowledgements

The boilerplate code for OpenGL rendering has been borrowed from the boilerplate provided for CSCE-689 Computer Animation by Prof. Shinjiro Sueda.
This boilerplate can be found on the [course website](https://people.engr.tamu.edu/sueda/courses/CSCE450/2023F/index.html).

