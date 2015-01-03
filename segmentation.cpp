#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>

using namespace cv;
using namespace std;

static void help(char** argv)
{
    cout << "\nDemonstrate mean-shift based color segmentation in spatial pyramid.\n"
    << "Call:\n   " << argv[0] << " image\n"
    << "This program allows you to set the spatial and color radius\n"
    << "of the mean shift window as well as the number of pyramid reduction levels explored\n"
    << endl;
}

double* segment_avg(Mat segment)
{
	long int sum_b = 0, sum_g = 0, sum_r = 0;
	for(int i = 0; i < 20; i++)
		for(int j = 0; j < 20; j++)
		{
			sum_b += segment.ptr<cv::Vec3b>(j)[i][0];
			sum_g += segment.ptr<cv::Vec3b>(j)[i][1];
			sum_r += segment.ptr<cv::Vec3b>(j)[i][2];
		}
	double ret[3];
	ret[0] = sum_b / 400;
	ret[1] = sum_g / 400;
	ret[2] = sum_r / 400;

	return ret;
}

//rectangular segmentation
static void rectangularSegmentation(Mat input)
{
	int height = input.rows/20+1, width = input.cols/20+1;
	int startx = 0, starty = 0;
	int sumx = 0, sumy = 0;
	Mat roi = input( Rect(0,20,50,100));
	imshow("roi_test",roi);

	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j++)
		{
			Mat roi = input( Rect(i*20, j*20, 20, 20));
			double* seg_avg = segment_avg(roi);
			Mat A = Mat(1, 3, CV_32FC1, &seg_avg, 2);
		}
	}
}

//This colors the segmentations
static void floodFillPostprocess( Mat& img, const Scalar& colorDiff=Scalar::all(1) )
{
    CV_Assert( !img.empty() );
    RNG rng = theRNG();
    Mat mask( img.rows+2, img.cols+2, CV_8UC1, Scalar::all(0) );
    for( int y = 0; y < img.rows; y++ )
    {
        for( int x = 0; x < img.cols; x++ )
        {
            if( mask.at<uchar>(y+1, x+1) == 0 )
            {
                Scalar newVal( rng(256), rng(256), rng(256) );
                floodFill( img, mask, Point(x,y), newVal, 0, colorDiff, colorDiff );
            }
        }
    }
}

string winName = "meanshift";
int spatialRad, colorRad, maxPyrLevel;
Mat img, res;
//Mat cov, mu;

const double PI  =3.141592653589793238463;
const float  PI_F=3.14159265358979f;

static void meanShiftSegmentation( int, void* )
{
    cout << "spatialRad=" << spatialRad << "; "
         << "colorRad=" << colorRad << "; "
         << "maxPyrLevel=" << maxPyrLevel << endl;
    pyrMeanShiftFiltering( img, res, spatialRad, colorRad, maxPyrLevel );
    floodFillPostprocess( res, Scalar::all(2) );
    imshow( winName, res );
}

double probability(double x, double avg, double sd)
{
	return (1/(sd*sqrt(2*PI)))*exp(pow(x-avg, 2)/-2*pow(sd,2));
}


int main(int argc, char** argv)
{
	cout<<"Hello World!"<<endl;

  //  if( argc !=2 )
   // {
     //   help(argv);
       // return -1;
    //}

	//long double *avg = avg_calc(128, 0, 0,"3_3_s","/media/amir/New Volume2/Univ/iust/SPR93/project/dataset/MSRC_ObjCategImageDatabase_v1/",".bmp");
	//long double *sd = sd_calc(128, 0, 0,"3_3_s","/media/amir/New Volume1/Univ/iust/SPR93/project/dataset/MSRC_ObjCategImageDatabase_v1/",".bmp");
	//cout<<sd[0]<<"\t"<<sd[1]<<"\t"<<sd[2]<<endl;




	char* address = "/media/amir/New Volume1/Univ/iust/SPR93/project/dataset/MSRC_ObjCategImageDatabase_v1/5_7_s.bmp";
    img = imread(address,1);
    imshow("main",img);

    	//cvWaitKey(0);

    if( img.empty() )
        return -1;

    spatialRad = 35;
    colorRad = 35;
    maxPyrLevel = 5;

    namedWindow( winName, WINDOW_AUTOSIZE );

    createTrackbar( "spatialRad", winName, &spatialRad, 80, meanShiftSegmentation );
    createTrackbar( "colorRad", winName, &colorRad, 60, meanShiftSegmentation );
    createTrackbar( "maxPyrLevel", winName, &maxPyrLevel, 5, meanShiftSegmentation );

    meanShiftSegmentation(0, 0);
    waitKey();
    return 0;
}

