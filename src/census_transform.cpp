// g++ census_transform.cpp -o stereo `pkg-config --cflags --libs opencv4`
// env -i HOME=$HOME PATH=/usr/bin:/bin ./stereo

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void census_calc(int left[], int len, int binary_array[])
{
    int count = 0;

    for (int i = 0; i < len; i++)
    {
        if (i == len / 2)
        {
            continue;
        }
        else if (left[i] < left[len / 2])
        {
            binary_array[count] = 0;
            count++;
        }
        else
        {
            binary_array[count] = 1;
            count++;
        }
    }
}

void xor_2_array(int arr1[], int arr2[], int result_array[], int len)
{
    for (int i = 0; i < len - 1; i++)
    {
        result_array[i] = arr1[i] ^ arr2[i];
    }
}

int count_no_of_1(int xor_array[], int xor_array_length)
{
    int no_of_1 = 0;

    for (int i = 0; i < xor_array_length; i++)
    {
        if (xor_array[i])
        {
            no_of_1++;
        }
    }

    return no_of_1;
}

int census_cost(int arr1[], int arr2[], int len)
{
    int binary_array[len - 1];
    int binary_array2[len - 1];

    census_calc(arr1, len, binary_array);
    census_calc(arr2, len, binary_array2);

    int xor_array[len - 1];
    xor_2_array(binary_array, binary_array2, xor_array, len);

    int xor_array_length = sizeof(xor_array) / sizeof(xor_array[0]);

    return count_no_of_1(xor_array, xor_array_length);
}

int main()
{
    Mat left_img = imread("/home/yashop/Desktop/census_transform/images/left.png", IMREAD_GRAYSCALE);
    Mat right_img = imread("/home/yashop/Desktop/census_transform/images/right.png", IMREAD_GRAYSCALE);

    if (left_img.empty() || right_img.empty())
    {
        cout << "Error: Could not load images." << endl;
        return -1;
    }

    if (left_img.rows != right_img.rows || left_img.cols != right_img.cols)
    {
        cout << "Error: image size mismatch" << endl;
        return -1;
    }

    int width = left_img.rows;
    int height = left_img.cols;
    int kernel = 5;
    int half_kernel = kernel / 2;
    int disp = 32;

    int disparity_rows = width - 2 * half_kernel;
    int disparity_cols = height - 2 * half_kernel;

    Mat disparity_map = Mat::zeros(disparity_rows, disparity_cols, CV_8U);

    for (int i = half_kernel; i < width - half_kernel; i++)
    {
        for (int j = half_kernel; j < height - half_kernel; j++)
        {
            int min_cost = 9999;
            int best_d = 0;

            for (int d = 0; d < disp; d++)
            {
                if (j - d - half_kernel < 0)
                {
                    continue;
                }

                int left_window[kernel * kernel];
                int right_window[kernel * kernel];
                int count = 0;

                for (int r = i - half_kernel; r <= i + half_kernel; r++)
                {
                    for (int c = j - half_kernel; c <= j + half_kernel; c++)
                    {
                        left_window[count] = (int)left_img.data[r * height + c];
                        right_window[count] = (int)right_img.data[r * height + (c - d)];
                        count++;
                    }
                }

                int cost = census_cost(left_window, right_window, kernel * kernel);

                if (cost < min_cost)
                {
                    min_cost = cost;
                    best_d = d;
                }
            }

            disparity_map.data[(i - half_kernel) * disparity_cols + (j - half_kernel)] =
                (best_d * 255) / disp;
        }
    }

    imwrite("before_median_filter.png", disparity_map);
    medianBlur(disparity_map, disparity_map, 5);
    imwrite("after_median_filter.png", disparity_map);
    
    imwrite("before_normalize.png", disparity_map);
    normalize(disparity_map, disparity_map, 0, 255, NORM_MINMAX);
    imwrite("after_normalize.png", disparity_map);

    imwrite("disparity.png", disparity_map);
    cout << "Saved disparity image as disparity.png" << endl;

    return 0;
}
