OpenCL-GameOfLife

Project by Matt Goerwell and Jason Cheung

This program implements Conaway's Game of Life in one of four modes.

1) Serially
2) OpenCL using CPU
3) OpenCL using GPU
4) OpenCL using GPU and CPU

The modes may be switched by pressing numbers 1-4 (not numpad).

All Game of Life patterns implemented are designed to go on indefinitely. 
This project currently requires that you have an Intel-based CPU and NVIDIA GPU to work properly (this is for the GPU mode which uses opencl+opengl interoperability).

Performance:
For Intel i5-6600 CPU and Nvidia GeForce GTX 970
1) ~110,000,000 nano-seconds per frame. 
2) ~10,000,000 nano-seconds per frame. 
3) ~1,000,000 nano-seconds per frame (note that this is using a specialized kernel).
4) ~22,000,000 nano-seconds per frame.