#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <dirent.h>
#include <opencv2/highgui.hpp>
#include "../include/feature_extract.h"
#include "../include/csv_util.h"

int bin = 8;
int RGB = 0;

int main(int argc, char *argv[])
{
	char *Baseline_path = (char *)"../csv/Baseline.csv";
	char *Histogram_path[4] = {(char *)"../csv/Hist_RGB.csv", (char *)"../csv/Hist_RG.csv", (char *)"../csv/Hist_RB.csv", (char *)"../csv/Hist_GB.csv"};
	char *Multi_histo_path = (char *)"../csv/MultiHist.csv";
	char *TextureColor_histo_path = (char *)"../csv/TextureColorHist.csv";
	char dirname[256];
	char buffer[256];
	FILE *fp;
	DIR *dirp;
	struct dirent *dp;
	int i;

	std::vector<float> feature_vec(512);
	std::vector<float> feature_vec2(512);
	bool first_run = true;

	// check for sufficient arguments
	if (argc < 3)
	{
		printf("usage: %s <directory path> <feature_set_number>\n", argv[0]);
		exit(-1);
	}
	int num = atoi(argv[2]);

	// get the directory path
	strcpy(dirname, argv[1]);
	printf("Processing directory %s\n", dirname);

	// open the directory
	dirp = opendir(dirname);
	if (dirp == NULL)
	{
		printf("Cannot open directory %s\n", dirname);
		exit(-1);
	}

	// loop over all the files in the image file listing
	while ((dp = readdir(dirp)) != NULL)
	{

		// check if the file is an image
		if (strstr(dp->d_name, ".jpg") || strstr(dp->d_name, ".png") ||
			strstr(dp->d_name, ".ppm") || strstr(dp->d_name, ".tif"))
		{

			printf("processing image file: %s\n", dp->d_name);

			// build the overall filename
			strcpy(buffer, dirname);
			strcat(buffer, "/");
			strcat(buffer, dp->d_name);

			printf("full path name: %s\n", buffer);

			if (num == 1)
			{
				int h = Baseline(buffer, feature_vec);
				if (h == 0)
				{
					int r = append_image_data_csv(Baseline_path, dp->d_name, feature_vec, first_run);
					if (r == 0)
					{
						first_run = false;
					}
					else
					{
						printf("Error while appending baseline into csv");
					}
				}
				else
				{
					printf("Baseline feature extract Failed");
				}
			}
			else if (num == 2)
			{
				// 0:RGB hist  1:RG hist 2:RB hist  3:Gb hist
				if (first_run)
				{
					std::cout << "Enter 0:RGB hist  1:RG hist 2:RB hist  3:Gb hist \n"
							  << std::endl;
					std::cin >> RGB;
					std::cout << "Bin size : \n"
							  << std::endl;
					std::cin >> bin;
				}
				// Parameters

				cv::Mat hist;
				int h = Histogram(buffer, hist, RGB, bin);
				if (h == 0)
				{
					printf("%f\n", hist.at<float>(0, 0));
					int t = Hist2Vec(feature_vec, hist, RGB);
					int r = append_image_data_csv(Histogram_path[RGB], dp->d_name, feature_vec, first_run);
					if (r == 0)
					{
						first_run = false;
					}
					else
					{
						printf("Error while appending hist into csv");
					}
				}
				else
				{
					printf("Histogram feature extract Failed");
				}
			}
			else if (num == 3)
			{
				// Parameters
				RGB = 0;
				bin = 8;
				float center_square_break = 0.5;

				cv::Mat hist_c, hist;

				int h = MultiHist(buffer, hist_c, center_square_break, hist, RGB, bin);
				if (h == 0)
				{
					int t1 = Hist2Vec(feature_vec, hist, RGB);
					int r1 = append_image_data_csv(Multi_histo_path, dp->d_name, feature_vec, first_run);
					if (r1 == 0)
					{
						first_run = false;
					}

					int t2 = Hist2Vec(feature_vec2, hist_c, RGB);
					int r2 = append_image_data_csv(Multi_histo_path, dp->d_name, feature_vec2, false);
				}
			}
			else if (num == 4)
			{
				// Parameters
				RGB = 0;
				bin = 8;

				cv::Mat hist_c, hist;

				int h = TextureColor(buffer, hist_c, hist, RGB, bin);
				if (h == 0)
				{
					int t1 = Hist2Vec(feature_vec, hist, RGB);
					int r1 = append_image_data_csv(TextureColor_histo_path, dp->d_name, feature_vec, first_run);
					if (r1 == 0)
					{
						first_run = false;
					}

					int t2 = Hist2Vec(feature_vec2, hist_c, RGB);
					int r2 = append_image_data_csv(TextureColor_histo_path, dp->d_name, feature_vec2, false);
				}
			}
			else if (num == 5)
			{
				// Parameters
				RGB = 0;
				bin = 8;

				cv::Mat hist_c, hist;

				int h = LawsHist(buffer, hist_c, hist,first_run,true,feature_vec,feature_vec2);
			}
		}
	}
printf("Terminating\n");

return (0);
}
