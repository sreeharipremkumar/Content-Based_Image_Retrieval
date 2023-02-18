int Baseline(char file[256], std::vector<float> &vec_ptr);
int Histogram(char file[256],cv::Mat &hist,int RGB, int bin);
int Hist2Vec(std::vector<float> &vec_ptr, cv::Mat &hist, int RGB);
int MultiHist(char file[256],cv::Mat &hist_c,float center_break,cv::Mat &hist,int RGB, int bin);
int sobelX3x3( cv::Mat &src, cv::Mat &dst );
int sobelY3x3( cv::Mat &src, cv::Mat &dst );
int magnitude( cv::Mat &sx, cv::Mat &sy, cv::Mat &dst );
int TextureColor(char file[256],cv::Mat &hist_c,cv::Mat &hist,int RGB, int bin);
int Filter5x5(cv::Mat &src,cv::Mat &dst, int arr1[5], int arr2[5]);
int grayscale(cv::Mat &src,cv::Mat &dst);
int LawsHist(char file[256],cv::Mat &hist_color,cv::Mat &hist,bool &first_run, bool write, std::vector<float>&vec_laws_ret,std::vector<float>&vec_color_ret);