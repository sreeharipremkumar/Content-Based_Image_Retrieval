#include <cstdio>
#include <iostream>
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include "../include/csv_util.h"

// using namespace std;
// template <typename S>
// ostream& operator<<(ostream& os,
//                     const vector<S>& vector)
// {
//     // Printing all the elements
//     // using <<
//     for (auto element : vector) {
//         os << element << " ";
//     }
//     return os;
// }

int Baseline(char file[256], std::vector<float> &vec_ptr)
{

    cv::Mat image;
    image = cv::imread(file, cv::IMREAD_COLOR);
    if (image.empty())
    {
        std::cout << "Could not open file" << std::endl; // throw error if file not present
        return 1;
    }

    int x_start = image.cols / 2 - 4;
    int y_start = image.rows / 2 - 4;
    int h = 0;

    for (int i = y_start; i <= y_start + 8; i++)
    {
        cv::Vec3b *ptr = image.ptr<cv::Vec3b>(i);

        for (int j = x_start; j <= x_start + 8; j++)
        {
            vec_ptr[h++] = ptr[j][0];
            vec_ptr[h++] = ptr[j][1];
            vec_ptr[h++] = ptr[j][2];
        }
    }
    return 0;
}

int Hist2Vec(std::vector<float> &vec_ptr, cv::Mat &hist, int RGB)
{
    int h = 0;
    for (int i = 0; i < hist.size[0]; i++)
    {
        for (int j = 0; j < hist.size[1]; j++)
        {
            if (RGB == 0)
            {
                for (int k = 0; k < hist.size[2]; k++)
                {
                    vec_ptr[h++] = hist.at<float>(i, j, k);
                }
            }
            else
            {
                vec_ptr[h++] = hist.at<float>(i, j);
            }
        }
    }
    return 0;
}

int Histogram(char file[256], cv::Mat &hist, int RGB, int bin)
{

    cv::Mat image;
    image = cv::imread(file, cv::IMREAD_COLOR);
    if (image.empty())
    {
        std::cout << "Could not open file" << std::endl; // throw error if file not present
        return 1;
    }

    int divisor = 256 / bin;

    if (RGB == 0)
    {
        int dim[3] = {bin, bin, bin};
        hist = cv::Mat::zeros(3, dim, CV_32F);
    }
    else if (RGB == 4)
    {
        int dim[1] = {bin};
        hist = cv::Mat::zeros(1, dim, CV_32F);
    }
    else
    {
        int dim[2] = {bin, bin};
        hist = cv::Mat::zeros(2, dim, CV_32F);
    }

    for (int i = 0; i < image.rows; i++)
    {
        cv::Vec3b *ptr = image.ptr<cv::Vec3b>(i);
        float hist_r, hist_g, hist_b;

        for (int j = 0; j < image.cols; j++)
        {
            if (RGB == 0)
            {
                hist_r = ptr[j][2] / divisor;
                hist_g = ptr[j][1] / divisor;
                hist_b = ptr[j][0] / divisor;
                hist.at<float>(hist_r, hist_g, hist_b)++;
            }
            else if (RGB == 1)
            {
                hist_r = ptr[j][2] / divisor;
                hist_g = ptr[j][1] / divisor;
                hist.at<float>(hist_r, hist_g)++;
            }
            else if (RGB == 2)
            {
                hist_r = ptr[j][2] / divisor;
                hist_b = ptr[j][0] / divisor;
                hist.at<float>(hist_r, hist_b)++;
            }
            else if (RGB == 3)
            {
                hist_g = ptr[j][1] / divisor;
                hist_b = ptr[j][0] / divisor;
                hist.at<float>(hist_g, hist_b)++;
            }
            else if (RGB == 4)
            {
                hist_r = ptr[j][0] / divisor;
                hist.at<float>(hist_g)++;
            }
        }
    }
    hist = hist / (image.rows * image.cols);
    return 0;
}

int MultiHist(char file[256], cv::Mat &hist_c, float center_break, cv::Mat &hist, int RGB, int bin)
{
    cv::Mat image;
    image = cv::imread(file, cv::IMREAD_COLOR);
    if (image.empty())
    {
        std::cout << "Could not open file" << std::endl; // throw error if file not present
        return 1;
    }

    int center_x = image.cols / 2;
    int center_y = image.rows / 2;
    int img_size_x = center_break * image.cols;
    int img_size_y = center_break * image.rows;

    int start_x = center_x - img_size_x / 2;
    int start_y = center_y - img_size_y / 2;

    int divisor = 256 / bin;

    int dim[3] = {bin, bin, bin};
    hist = cv::Mat::zeros(3, dim, CV_32F);
    hist_c = cv::Mat::zeros(3, dim, CV_32F);

    for (int i = 0; i < image.rows; i++)
    {
        cv::Vec3b *ptr = image.ptr<cv::Vec3b>(i);
        for (int j = 0; j < image.cols; j++)
        {
            float hist_r, hist_g, hist_b;

            hist_r = ptr[j][2] / divisor;
            hist_g = ptr[j][1] / divisor;
            hist_b = ptr[j][0] / divisor;
            hist.at<float>(hist_r, hist_g, hist_b)++;

            if (i >= start_y && i < start_y + img_size_y)
            {
                if (j >= start_x && j < start_x + img_size_x)
                {
                    hist_c.at<float>(hist_r, hist_g, hist_b)++;
                }
            }
        }
    }
    hist = hist / (image.rows * image.cols);
    hist_c = hist_c / (img_size_y * img_size_x);
    return 0;
}

int sobelX3x3(cv::Mat &src, cv::Mat &dst)
{

    cv::Mat temp;
    temp = cv::Mat::zeros(src.size(), CV_16SC3);
    // separable filter for 3x3 sobelX
    // int ar1[3] = {1,2,1};
    // int ar2[3] = {-1,0,1};

    for (int i = 0; i < src.rows; i++)
    { // convolution row-wise
        cv::Vec3b *rptr = src.ptr<cv::Vec3b>(i);
        cv::Vec3s *dptr = temp.ptr<cv::Vec3s>(i);

        for (int j = 0; j < src.cols; j++)
        {

            for (int k = 0; k < 3; k++)
            {

                if (j == 0)
                {
                    dptr[j][k] = rptr[1][k];
                }
                else if (j == src.cols - 1)
                {
                    dptr[j][k] = -rptr[j - 1][k];
                }
                else
                {
                    dptr[j][k] = rptr[j + 1][k] - rptr[j - 1][k];
                }
            }
        }
    }

    dst = cv::Mat::zeros(src.size(), CV_16SC3);

    for (int i = 0; i < src.rows; i++)
    { // convolution columbn-wise

        cv::Vec3s *rptrm1;
        cv::Vec3s *rptr = temp.ptr<cv::Vec3s>(i);
        cv::Vec3s *rptrp1;
        if (i >= 1)
        {
            rptrm1 = temp.ptr<cv::Vec3s>(i - 1);
        }
        if (i <= src.rows - 1)
        {
            rptrp1 = temp.ptr<cv::Vec3s>(i + 1);
        }

        cv::Vec3s *dptr = dst.ptr<cv::Vec3s>(i);

        for (int j = 0; j < src.cols; j++)
        {

            for (int k = 0; k < 3; k++)
            {

                if (i == 0)
                {
                    dptr[j][k] = (rptr[j][k] * 2 + rptrp1[j][k]) / 4;
                }
                else if (i == src.rows - 1)
                {
                    dptr[j][k] = (rptr[j][k] * 2 + rptrm1[j][k]) / 4;
                }
                else
                {
                    dptr[j][k] = (rptrp1[j][k] + rptr[j][k] * 2 + rptrm1[j][k]) / 4;
                }
            }
        }
    }
    return 0;
}

int sobelY3x3(cv::Mat &src, cv::Mat &dst)
{

    cv::Mat temp;
    temp = cv::Mat::zeros(src.size(), CV_16SC3);
    // separable filter for sobelY 3x3
    // int ar1[3] = {1,0,-1};
    // int ar2[3] = {1,2,1};

    for (int i = 0; i < src.rows; i++)
    { // convolution row-swise
        cv::Vec3b *rptr = src.ptr<cv::Vec3b>(i);
        cv::Vec3s *dptr = temp.ptr<cv::Vec3s>(i);

        for (int j = 0; j < src.cols; j++)
        {

            for (int k = 0; k < 3; k++)
            {

                if (j == 0)
                {
                    dptr[j][k] = (rptr[0][k] * 2 + rptr[1][k]) / 4;
                }
                else if (j == src.cols - 1)
                {
                    dptr[j][k] = (rptr[j][k] * 2 + rptr[j - 1][k]) / 4;
                }
                else
                {
                    dptr[j][k] = (rptr[j + 1][k] + rptr[j][k] * 2 + rptr[j - 1][k]) / 4;
                }
            }
        }
    }

    dst = cv::Mat::zeros(src.size(), CV_16SC3);

    for (int i = 0; i < src.rows; i++)
    { // convolution column wise

        cv::Vec3s *rptrm1;
        cv::Vec3s *rptr = temp.ptr<cv::Vec3s>(i);
        cv::Vec3s *rptrp1;
        if (i >= 1)
        {
            rptrm1 = temp.ptr<cv::Vec3s>(i - 1);
        }
        if (i <= src.rows - 1)
        {
            rptrp1 = temp.ptr<cv::Vec3s>(i + 1);
        }

        cv::Vec3s *dptr = dst.ptr<cv::Vec3s>(i);

        for (int j = 0; j < src.cols; j++)
        {

            for (int k = 0; k < 3; k++)
            {

                if (i == 0)
                {
                    dptr[j][k] = -rptrp1[j][k];
                }
                else if (i == src.rows - 1)
                {
                    dptr[j][k] = rptrm1[j][k];
                }
                else
                {
                    dptr[j][k] = -rptrp1[j][k] + rptrm1[j][k];
                }
            }
        }
    }
    return 0;
}

int magnitude(cv::Mat &sx, cv::Mat &sy, cv::Mat &dst)
{

    dst = cv::Mat::zeros(sx.size(), CV_8UC3);
    for (int i = 0; i < sx.rows; i++)
    {
        cv::Vec3s *xptr = sx.ptr<cv::Vec3s>(i);
        cv::Vec3s *yptr = sy.ptr<cv::Vec3s>(i);
        cv::Vec3b *dptr = dst.ptr<cv::Vec3b>(i);

        for (int j = 0; j < sx.cols; j++)
        { // calculating magnitude
            dptr[j][0] = sqrt(pow(xptr[j][0], 2) + pow(yptr[j][0], 2));
            dptr[j][1] = sqrt(pow(xptr[j][1], 2) + pow(yptr[j][1], 2));
            dptr[j][2] = sqrt(pow(xptr[j][2], 2) + pow(yptr[j][2], 2));
        }
    }
    return 0;
}

int TextureColor(char file[256], cv::Mat &hist_c, cv::Mat &hist, int RGB, int bin)
{
    cv::Mat image, filtered, filtered2, final_img;
    image = cv::imread(file, cv::IMREAD_COLOR);
    if (image.empty())
    {
        std::cout << "Could not open file" << std::endl; // throw error if file not present
        return 1;
    }

    int hx = sobelX3x3(image, filtered); // getting sobelX first
    if (hx == 0)
    {
        int hy = sobelY3x3(image, filtered2); // getting sobelY next
        if (hy == 0)
        {
            int h = magnitude(filtered, filtered2, final_img); // using the result from sobelX and sobelY
                                                               // to find magnitude
        }
        else
        {
            printf("sobelY failed");
        }
    }
    else
    {
        printf("sobelX failed");
    }

    int divisor = 256 / bin;
    int dim[3] = {bin, bin, bin};

    hist = cv::Mat::zeros(3, dim, CV_32F);
    hist_c = cv::Mat::zeros(3, dim, CV_32F);

    for (int i = 0; i < image.rows; i++)
    {
        cv::Vec3b *ptr = image.ptr<cv::Vec3b>(i);
        cv::Vec3b *c_ptr = final_img.ptr<cv::Vec3b>(i);

        for (int j = 0; j < image.cols; j++)
        {
            hist.at<float>(ptr[j][2] / divisor, ptr[j][1] / divisor, ptr[j][0] / divisor)++;
            hist_c.at<float>(c_ptr[j][2] / divisor, c_ptr[j][1] / divisor, c_ptr[j][0] / divisor)++;
        }
    }

    hist = hist / (image.rows * image.cols);
    hist_c = hist_c / (image.rows * image.cols);
    return 0;
}

int Filter5x5(cv::Mat &src, cv::Mat &dst, int arr1[5], int arr2[5])
{ // gaussian blur

    cv::Mat temp;
    temp = cv::Mat::zeros(src.size(), src.type());
    int sum1 = 0;
    for (int i = 0; i < 5; i++)
    {
        if (arr1[i] >= 0)
        {
            sum1 = sum1 + arr1[i];
        }
    }

    int sum2 = 0;
    for (int i = 0; i < 5; i++)
    {
        if (arr2[i] >= 0)
        {
            sum2 = sum2 + arr2[i];
        }
    }

    // using separable First convolving the image row-wise
    for (int i = 0; i < src.rows; i++)
    {
        uchar *rptr = src.ptr<uchar>(i);
        uchar *dptr = temp.ptr<uchar>(i);

        for (int j = 0; j < src.cols; j++)
        {

            if (j == 0)
            {
                dptr[j] = (rptr[0] * arr1[2] + rptr[1] * arr1[3] + rptr[2] * arr1[4]) / sum1; // edge case first/
            }
            else if (j == 1)
            {
                dptr[j] = (rptr[0] * arr1[1] + rptr[1] * arr1[2] + rptr[2] * arr1[3] + rptr[3] * arr1[4]) / sum1; // edge case second
            }
            else if (j == src.cols - 1)
            {
                dptr[j] = (rptr[j] * arr1[2] + rptr[j - 1] * arr1[1] + rptr[j - 2] * arr1[0]) / sum1; // edge case last row
            }
            else if (j == src.cols - 2)
            {
                dptr[j] = (rptr[j + 1] * arr1[3] + rptr[j] * arr1[2] + rptr[j - 1] * arr1[1] + rptr[j - 2] * arr1[0]) / sum1; // edge case secondlast row
            }
            else
            {
                dptr[j] = (rptr[j + 2] * arr1[4] + rptr[j + 1] * arr1[3] + rptr[j] * arr1[2] + rptr[j - 1] * arr1[1] + rptr[j - 2] * arr1[0]) / sum1; // remaining
            }
        }
    }

    dst = cv::Mat::zeros(src.size(), src.type());

    for (int i = 0; i < src.rows; i++)
    { // Now convolving column-wise

        uchar *rptrm2;
        uchar *rptrm1;
        uchar *rptr = temp.ptr<uchar>(i);
        uchar *rptrp1;
        uchar *rptrp2;
        if (i >= 2)
        {
            rptrm2 = temp.ptr<uchar>(i - 2);
        }
        if (i >= 1)
        {
            rptrm1 = temp.ptr<uchar>(i - 1);
        }
        if (i <= src.rows - 1)
        {
            rptrp1 = temp.ptr<uchar>(i + 1);
        }
        if (i <= src.rows - 2)
        {
            rptrp2 = temp.ptr<uchar>(i + 2);
        }

        uchar *dptr = dst.ptr<uchar>(i);

        for (int j = 0; j < src.cols; j++)
        {

            if (i == 0)
            {
                dptr[j] = (rptr[j] * arr2[2] + rptrp1[j] * arr2[3] + rptrp2[j] * arr2[4]) / sum2;
            }
            else if (i == 1)
            {
                dptr[j] = (rptrm1[j] * arr2[1] + rptr[j] * arr2[2] + rptrp1[j] * arr2[3] + rptrp2[j] * arr2[4]) / sum2;
            }
            else if (i == src.rows - 1)
            {
                dptr[j] = (rptr[j] * arr2[2] + rptrm1[j] * arr2[1] + rptrm2[j] * arr2[0]) / sum2;
            }
            else if (i == src.rows - 2)
            {
                dptr[j] = (rptrp1[j] * arr2[3] + rptr[j] * arr2[2] + rptrm1[j] * arr2[1] + rptrm2[j] * arr2[0]) / sum2;
            }
            else
            {
                dptr[j] = (rptrp2[j] * arr2[4] + rptrp1[j] * arr2[3] + rptr[j] * arr2[2] + rptrm1[j] * arr2[1] + rptrm2[j] * arr2[0]) / sum2;
            }
        }
    }

    return 0;
}

int LawsHist(char file[256], cv::Mat &hist_color, cv::Mat &hist, bool &first_run, bool write, std::vector<float> &vec_laws_ret, std::vector<float> &vec_color_ret)
{

    cv::Mat image, gray, filtered, gray_filtered;
    image = cv::imread(file, cv::IMREAD_COLOR);
    gray = cv::imread(file, cv::IMREAD_GRAYSCALE);

    if (image.empty())
    {
        std::cout << "Could not open file" << std::endl; // throw error if file not present
        return 1;
    }

    int dim1 = {8};
    int dim[3] = {8, 8, 8};
    hist = cv::Mat::zeros(1, dim1, CV_32F);
    hist_color = cv::Mat::zeros(3, dim, CV_32F);

    std::vector<float> vec_color(512);

    int h = Histogram(file, hist_color, 0, 8);
    if (h == 0)
    {
        int t = Hist2Vec(vec_color, hist_color, 0);
    }
    if (write == true)
    {
        append_image_data_csv((char *)"../csv/Landscape.csv", file, vec_color, first_run);
    }
    else
    {
        vec_color_ret.assign(vec_color.begin(), vec_color.end());
        append_image_data_csv((char *)"../csv/test2.csv", "testing", vec_color_ret, true);
    }
    if (first_run == true)
    {
        first_run = false;
    }

    int Laws[5][5] = {{1, 4, 6, 4, 1}, {1, 2, 0, -2, -1}, {-1, 0, 2, 0, -1}, {1, -2, 0, 2, -1}, {1, -4, 6, -4, 1}};
    std::vector<float> vec(8);
    std::vector<float> vec_laws;

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            Filter5x5(gray, gray_filtered, Laws[i], Laws[j]);
            cv::convertScaleAbs(gray_filtered, filtered);

            int divisor = 256 / 8;

            hist = cv::Mat::zeros(1, 8, CV_32F);

           

            for (int i = 0; i < filtered.rows; i++)
            {
                uchar *ptr = filtered.ptr<uchar>(i);
                int hist_g;

                for (int j = 0; j < filtered.cols; j++)
                {
                    hist_g = ptr[j] / divisor;
                    hist.at<float>(hist_g)++;
                }
            }
            hist = hist / (filtered.rows * filtered.cols);
            int num = 0;
            for (int i = 0; i < hist.size[1]; i++)
            {
                vec[num] = hist.at<float>(i)/25;
                num++;
            }
            
            //std::cout<<vec<<std::endl;
            // cv::imshow("test",filtered);
            // cv::waitKey(0);
            vec_laws.end() = vec_laws.begin();
            vec_laws.insert(vec_laws.end(), vec.begin(), vec.end());
        }
    }
    if (write == true)
    {
        append_image_data_csv((char *)"../csv/Landscape.csv", file, vec_laws, first_run);
    }
    else
    {
        vec_laws_ret.assign(vec_laws.begin(), vec_laws.end());
        append_image_data_csv((char *)"../csv/test2.csv", "testing", vec_laws_ret, false);
    }
    return 0;
}