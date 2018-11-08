OpenCL-GameOfLife

Project by Matt Goerwell and Jason Cheung

This program implements Conaway's Game of Life in one of four modes.

1) Serially
2) OpenCL using CPU
3) OpenCL using GPU
4) OpenCL using GPU and CPU

The modes may be switched between in main via commenting and uncommenting the relevant simulators and update calls for each, and then recompiling.

All Game of Life patterns implemented are designed to go on indefinitely. 
OpenCL currently requires that you have an Intel-based CPU and NVIDIA GPU to work properly.