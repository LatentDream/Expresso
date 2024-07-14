<!-- Display ./example.png -->
<p align="center">
	<img width="800" src="./demo.png">
<p align="center">

# Expresso

Old school 3D engine from scratch in C.

## Dev Env

```
# Install C tooling and SDL2
sudo apt install build-essential
sudo apt install libsdl2-dev

# Install the Rust toolchain
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh

# Build the cargo bin & C project
just build

# Expose the rust dll to the C project
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/project/path/external/

# Run the project
./bin/linux/release/expresso
```

---

# Dev Log & Ressources
- My notes on rendering pipeline from Pikuma's course: [notes](./notes/rendering_pipeline.md)
- My notes on parallel programming from Carnegie Mellon University: [notes](./notes/parallel_computer_architecture_and_programming.md)
- For the next learning topic, see branch: [dev](https://github.com/LatentDream/Expresso/tree/dev)

#### Logs:
In a non-chronological order, here are the logs of the development. This is what I want to explore and learn. Keep in mind that the project is still in progress and a learning exercise.
- [x] Drawing to the screen
- [x] Basic 3D asset
- [x] Rendering Loop
    - [x] CPU rasterization
    - [x] Backface culling
    - [x] Frustum clipping 
    - [ ] Subpixel rasterization
    - [ ] GPU rasterization 
	- (Not sure if I look into implementing it to learn some stuff or swtich to OpenGL, Vulkan, DirectX, (or WebGL || WebGPU) ?)
- [ ] Animation
    - [ ] TODO: Research
- [ ] Collision Detection
    - [ ] TODO: Research
- [ ] Physics
    - [ ] TODO: Research
- [ ] Game Loop
    - [ ] TODO: Research
- [ ] Audio
- [ ] Game Menu / Static UI
- [ ] Lighting
    - [x] Basic Lightnight
    - [ ] Advanced shadding
	- Ditchering to achieve shading (could be a cool background with retro look)
	- Gouraud shading
	- [Phong Model](https://graphics.stanford.edu/courses/cs348b-07/lectures/reflection_ii/reflection_ii.pdf)
- [x] Texture
- [ ] _current focus_ Learning parallel & low-level optimization. Currently following / reading:
    - On branch _cargenie-parallel-course_: [Note from Parallel Programming - Carnegie Mellon University](https://github.com/LatentDream/Expresso/tree/carnegie-parallel-course)
    - Game Engine Architecture Book
- [ ] Camera
    - [x] Basic FPS camera
    - [x] Basic Look-at camera
    - [ ] Follow camera
    - [ ] Well done and "real time" input
    - [ ] Mouse input
- [ ] Math sutffs:
    - [ ] Integrate fix math [libfixmatrix](https://github.com/PetteriAimonen/libfixmatrix)
    - [ ] Quaternion for rotation
    - [ ] Random number generator
- [ ] Debugging
    - Logging
    - Tracing
- [ ] Networking (0 planned, but interested to learn how it fit in the engine)

#### Ressources used:
- [Pikuma's course on 3D Computer Graphics](https://pikuma.com/courses).
- [Game Engine Architecture](https://www.gameenginebook.com/)
- [Perspective-Correct Interpolation](https://s3.amazonaws.com/thinkific/file_uploads/167815/attachments/c06/b72/f58/KokLimLow-PerspectiveCorrectInterpolation.pdf)
- [Parallel Rasterization](https://www.cs.drexel.edu/~deb39/Classes/Papers/comp175-06-pineda.pdf)
- [fgiesen blog](https://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice/)

#### Libraries:
- [upng](https://github.com/elanthis/upng)
- [SDL2](https://www.libsdl.org/)

#### Ressources to check next:
**To Check**:
- [Foundations of Game Engine Development](https://foundationsofgameenginedev.com/) 
- [Graphics Programming Black Book](https://www.drdobbs.com/parallel/graphics-programming-black-book/184404919)
- [Texture Mapping Technical Articles](https://www.chrishecker.com/Miscellaneous_Technical_Articles)
- [Rasterization Rules & the Edge Function](https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/rasterization-stage)
- [Rasterization Rules](https://docs.microsoft.com/en-us/windows/win32/direct3d9/rasterization-rules#triangle-rasterization-rules)
- [CUDA C Programming](https://docs.nvidia.com/cuda/archive/9.1/pdf/CUDA_C_Programming_Guide.pdf)
- [Parallel Computer Architecture and programming](http://15418.courses.cs.cmu.edu/spring2016/lectures)
- [Computer Graphics at Carnegie Mellon University](https://github.com/CMU-Graphics/Scotty3D)
- [TheNumbat's list](https://github.com/TheNumbat/Lists)

**C ressources**:
- [How I program C](https://www.youtube.com/watch?v=443UNeGrFoM)
- [Build your own Lisp](https://buildyourownlisp.com/chapter1_introduction#how_to_learn_c)
- [Fedor Pikus's talks](https://www.youtube.com/watch?v=g-WPhYREFjk)
- [Cache Effect](https://igoro.com/archive/gallery-of-processor-cache-effects/)
- Perf Profiler, check orbit
- Valgring

