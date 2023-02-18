OS : Ubuntu 20.04 LTS
with VSCode and g++ compiler

To extract data from image set the code:
1) Open Terminal
2) Go to src folder within Project-2
3) Run "make" command

4) Run "./extract <directory_path> <feature_set_number>"
	Where: 
	<directory_path> is the path to the folder with the images.
	<feature_set_number> is the Task number
		the input would be:
		1 - 9x9 Baseline Matching 
		2 - Histogram Matching (Colo Hist)
		3 - Multi-Histogram (Color whole image + Center Box of image)
		4 - Texture and Color (Color + Sobel Magnitude)
		5 - Custom Design (Color Hist + Laws Filter) 
		
	Example : "./extract ../Landscape/ 5" or "./extract ../olympus/ 3"
	
Their corresponding CSV files would be created and stored in the csv folder

Additional Inputs in Histogram matching while running, asks for: 
bin size
type of Histogram 0 for RGB, 1 for RG hist, 2 for RB hist,  3 for Gb hist.


To Run the CBIR after extraction:
5) Run "./matching <image path> <feature_set_number> <feature_set_file_csv> "
	Where:
	<image path> is the path to the target image, for which you need to find similar images for
	<feature_set_number> is to select which Task you need to run, is the task number
	<feature_set_file_csv> is the path to the csv file
	
	Example: "./matching ../olympus/pic.1016.jpg 1 ../csv/Baseline.csv" or "./matching ../Testing\ Data/00000010_\(2\).jpg 5 ../csv/Landscape.csv"
	
	
Extension:
Extension is the implementation of Laws Filter for task 5


TIME TRAVEL:
                "I WONT BE USING MY TIME TRAVEL DAYS"
