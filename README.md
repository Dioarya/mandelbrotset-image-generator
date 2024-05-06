MIG (Mandelbrotset Image Generator) is a program that - as said in the title - generates mandelbrotset images.

### Features/Goals:
- Tile by tile image generation, allowing the program to be run at any time and produce progress towards the final image.
- Threading, speeds up the image generation by the number of threads you use.
- SIMD, speeds up the image generation by size of your SIMD registers divided by the size of a double. (normally this results in 8x performance increases)
- Optional GUI for displaying extra subsidiary information, showing the progress of threads's progress through their tile in an animated way, bigger progress bar, time estimates and more.
- Stylish progress bar, the progress bar doesn't lie. It shows your progress through the current tile being generated.
- PNG compression, decreases file size dramatically for most images. Uses png's serial encoding to use the least amount of memory when saving the image.