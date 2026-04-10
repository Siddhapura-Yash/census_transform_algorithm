# Stereo Disparity Map using Census Transform

This project generates a disparity map from stereo images using Census Transform and block matching in C++ with OpenCV.

## How it works

The program first loads the left and right grayscale stereo images. For each valid pixel, it takes a 5×5 window around that pixel from both images. It then applies Census Transform, where each neighboring pixel in the window is compared with the center pixel. If the neighbor is smaller than the center, it stores 0; otherwise, it stores 1. This creates a binary pattern that represents the local structure of the image.

The same process is done for the matching window in the right image. The two binary patterns are compared using XOR, and the number of mismatched bits is counted (Hamming distance). This cost tells how similar the two windows are.

For every pixel, the program checks multiple disparity values (0 to 31 pixels) by shifting the right image window horizontally. The disparity with the lowest matching cost is selected as the best match. This value is stored in the output disparity map, where brighter pixels represent closer objects and darker pixels represent farther objects.

Finally, a median filter is applied to reduce noise, and normalization is used to improve contrast so the depth map becomes clearer.

## Output
- `before_median_filter.png`
- `after_median_filter.png`
- `before_normalize.png`
- `after_normalize.png`
- `disparity.png`

## Uses
- Depth estimation
- Stereo vision
- Robotics
- Obstacle detection
- FPGA / embedded vision


## How to compile and run
```cpp
g++ census_transform.cpp -o stereo `pkg-config --cflags --libs opencv4`
env -i HOME=$HOME PATH=/usr/bin:/bin ./stereo
```

## Important
Set correct image path in code before running:

```cpp
Mat left_img = imread("/home/yashop/Desktop/census_transform/images/left.png", IMREAD_GRAYSCALE);
Mat right_img = imread("/home/yashop/Desktop/census_transform/images/right.png", IMREAD_GRAYSCALE);
```
