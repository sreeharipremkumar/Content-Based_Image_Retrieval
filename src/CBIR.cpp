#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <dirent.h>
#include <iostream>
#include <limits>
#include <opencv2/highgui.hpp>
#include "../include/feature_extract.h"
#include "../include/csv_util.h"

// bool output_full = false;
struct Output
{
    char *name;
    float sum;
};

float SSE(std::vector<float> &Ft, std::vector<float> &Fi)
{
    float sum = 0;

    for (int i = 0; i < Fi.size(); i++)
    {
        sum = sum + (Ft[i] - Fi[i]) * (Ft[i] - Fi[i]);
    }
    return sum;
}

float Intersection(std::vector<float> &Ft, std::vector<float> &Fi)
{
    float sum = 0;

    for (int i = 0; i < Fi.size(); i++)
    {
        float minimum = (Ft[i] <= Fi[i]) ? Ft[i] : Fi[i];
        sum = sum + minimum;
    }
    return sum;
}

int feature_iter(std::vector<float> &Ft,std::vector<float> &Ft2, std::vector<std::vector<float>> csv_data,
                 std::vector<char *> img_names, std::vector<Output> &sorted, int distance_type, bool multisum,
                 float multisum_ratio, std::vector<Output> &rev_sorted)
{
    float sum, sum2;
    int num_out = sorted.size();
    for (int i = 0; i < csv_data.size(); i++)
    {
        if (distance_type == 1)
        {
            sum = SSE(Ft, csv_data[i]);

            for (int j = 0; j < num_out; j++)
            {
                int flag1 = 0,flag2 =0;
                if (sorted[j].sum >= sum)
                {
                    for (int k = num_out - 1; k > j; k--)
                    {
                        sorted[k] = sorted[k - 1];
                    }
                    sorted[j].name = img_names[i];
                    sorted[j].sum = sum;
                    flag1 = 1;
                }
                if(rev_sorted[j].sum<= sum)
                {
                    for (int k = num_out-1;k>j;k--)
                    {
                        rev_sorted[k] = rev_sorted[k-1];
                    }
                    rev_sorted[j].name = img_names[i];
                    rev_sorted[j].sum = sum;
                    flag2 = 1;
                }
                if(flag1 == 1 || flag2 ==1)
                {
                    break;
                }
            }
        }
        else
        {
            if (distance_type == 2)
            {
                sum = Intersection(Ft, csv_data[i]);
                if (multisum == true)
                {
                    sum2 = Intersection(Ft2, csv_data[i + 1]);
                    i++;
                    sum = multisum_ratio * sum + (1 - multisum_ratio) * sum2;
                }
            }
            else if(distance_type == 3)
            {
                sum = Intersection(Ft, csv_data[i]);
                if (multisum == true)
                {
                    sum2 = SSE(Ft2, csv_data[i + 1]);
                    i++;
                    sum = multisum_ratio * sum - ((1 - multisum_ratio) * sum2);
                }
            }
        
            for (int j = 0; j < num_out; j++)
            {
                int flag1 = 0, flag2 =0;
                if (sorted[j].sum <= sum)
                {
                    for (int k = num_out - 1; k > j; k--)
                    {
                        sorted[k] = sorted[k - 1];
                    }
                    sorted[j].name = img_names[i];
                    sorted[j].sum = sum;
                    flag1 = 1;
                }

                if(rev_sorted[j].sum>= sum)
                {
                    for (int k = num_out-1;k>j;k--)
                    {
                        rev_sorted[k] = rev_sorted[k-1];
                    }
                    rev_sorted[j].name = img_names[i];
                    rev_sorted[j].sum = sum;
                    flag2 = 1;
                }
                if(flag1 == 1 || flag2 ==1)
                {
                    break;
                }
            }
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("usage: %s <image path> <feature_set_number> <feature_set_file_csv> \n", argv[0]);
        exit(-1);
    }

    char *Baseline_path = (char *)"../csv/Baseline.csv";
    char *Histogram_path[4] = {(char *)"../csv/Hist_RGB.csv", (char *)"../csv/Hist_RG.csv", (char *)"../csv/Hist_RB.csv", (char *)"../csv/Hist_GB.csv"};
    char *Multi_histo_path = (char *)"../csv/MultiHist.csv";
    char *TextureColor_histo_path = (char *)"../csv/TextureColorHist.csv";


    std::vector<float> feature_vec(512);
    std::vector<float> feature_vec2(512);

    // reading args
    char img_path[256];
    strcpy(img_path, argv[1]);
    int num = atoi(argv[2]);
    char *feature_set_path = (char *)argv[3];

    std::vector<char *> img_names;
    std::vector<std::vector<float>> csv_data;
    int number_of_output = 5;

    std::vector<Output> sorted(number_of_output);
    std::vector<Output> rev_sorted(number_of_output);

    if (num == 1)
    {

        for (int i = 0; i < number_of_output; i++)
        {
            sorted[i].sum = std::numeric_limits<float>::infinity();
            rev_sorted[i].sum = -std::numeric_limits<float>::infinity();
        }
        int h = Baseline(img_path, feature_vec);
        if (h == 0)
        {
            int r = read_image_data_csv(Baseline_path, img_names, csv_data, 0);
            if (r != 0)
            {
                printf("Error when reading baseline csv");
            }
            else
            {
                int t = feature_iter(feature_vec,feature_vec2, csv_data, img_names, sorted, 1, false, 0,rev_sorted);
            }
        }
    }
    if (num == 2)
    {
        int RGB = 0; // 0:RGB hist  1:RG hist 2:RB hist  3:Gb hist
        int bin = 8;

        if (strcmp(argv[3], "../csv/Hist_RGB.csv") == 0)
        {
            RGB = 0;
        }
        else if (strcmp(argv[3], "../csv/Hist_RG.csv") == 0)
        {
            RGB = 1;
        }
        else if (strcmp(argv[3], "../csv/Hist_RB.csv") == 0)
        {
            RGB = 2;
        }
        else if (strcmp(argv[3], "../csv/Hist_GB.csv") == 0)
        {
            RGB = 3;
        }

        std::cout << "\nBin size : " << std::endl;
        std::cin >> bin;

        for (int i = 0; i < number_of_output; i++)
        {
            sorted[i].sum = -std::numeric_limits<float>::infinity();
            rev_sorted[i].sum = std::numeric_limits<float>::infinity();
        }
        cv::Mat hist;
        int h = Histogram(img_path, hist, RGB, bin);
        if (h == 0)
        {
            int t = Hist2Vec(feature_vec, hist, RGB);
            int r = read_image_data_csv(Histogram_path[RGB], img_names, csv_data, 0);
            if (r != 0)
            {
                printf("Error when reading histogram csv");
            }
            else
            {
                int t = feature_iter(feature_vec,feature_vec2, csv_data, img_names, sorted, 2, false, 0,rev_sorted);
            }
        }
        else
        {
            printf("Histogram feature extract Failed");
        }
    }
    if (num == 3)
    {
        // Parameters
        float center_square_break = 0.5;

        int RGB = 0;
        int bin = 8;
        float weight_distance = 0.5;

        cv::Mat hist_c, hist;
        for (int i = 0; i < number_of_output; i++)
        {
            sorted[i].sum = -std::numeric_limits<float>::infinity();
            rev_sorted[i].sum = std::numeric_limits<float>::infinity();
        }
        int h = MultiHist(img_path, hist_c, center_square_break, hist, RGB, bin);

        if (h == 0)
        {
            int t1 = Hist2Vec(feature_vec, hist, RGB);
            int t2 = Hist2Vec(feature_vec2, hist_c, RGB);

            int r = read_image_data_csv(Multi_histo_path, img_names, csv_data, 0);
            if (r != 0)
            {
                printf("Error when reading histogram csv");
            }
            else
            {
                int t = feature_iter(feature_vec,feature_vec2, csv_data, img_names, sorted, 2, true, weight_distance,rev_sorted);
            }
        }
    }
    if (num == 4)
    {
        // Parameters

        int RGB = 0;
        int bin = 8;
        float weight_distance = 0.35;

        cv::Mat hist_c, hist;
        for (int i = 0; i < number_of_output; i++)
        {
            sorted[i].sum = -std::numeric_limits<float>::infinity();
            rev_sorted[i].sum = std::numeric_limits<float>::infinity();
        }
        int h = TextureColor(img_path, hist_c, hist, RGB, bin);
        if (h == 0)
        {
            int t1 = Hist2Vec(feature_vec, hist, RGB);
            int t2 = Hist2Vec(feature_vec2, hist_c, RGB);
            int r = read_image_data_csv(TextureColor_histo_path, img_names, csv_data, 0);
            if (r != 0)
            {
                printf("Error when reading histogram csv");
            }
            else
            {
                int t = feature_iter(feature_vec,feature_vec2, csv_data, img_names, sorted, 3, true, weight_distance,rev_sorted);
            }
        }
    }
    if(num == 5)
    {

        float weight_distance = 0.5;

        cv::Mat hist_c, hist;
        for (int i = 0; i < number_of_output; i++)
        {
            sorted[i].sum = -std::numeric_limits<float>::infinity();
            rev_sorted[i].sum = std::numeric_limits<float>::infinity();
        }
        bool test = false;
        int h = LawsHist(img_path,hist_c,hist,test,false,feature_vec2,feature_vec);

        if (h == 0)
        {
            int r = read_image_data_csv((char*)"../csv/Landscape.csv", img_names, csv_data,0);
            if (r != 0)
            {
                printf("Error when reading csv");
            }
            else
            {
                int t = feature_iter(feature_vec,feature_vec2, csv_data, img_names, sorted, 2, true, weight_distance,rev_sorted);
            }
        }
    }
    cv::Mat image;
    for (int i = 0; i < number_of_output; i++)
    {
        printf("Best Match (%d) = %s  (dist_metric_val = %f)\n",i+1, sorted[i].name, sorted[i].sum);
    }
    printf("\n");
    for (int i = 0; i < number_of_output; i++)
    {
        printf("Worst Match (%d) = %s  (dist_metric_val = %f) \n",i+1, rev_sorted[i].name, rev_sorted[i].sum);
    }
}