#include"headlines.h"

int drawHist(Mat image);

class ShapeDetector
{
public:

	ShapeDetector(string imagePath)
	{
		this->imagePath = imagePath;
	}

	void detectShapes();


private:
	int num3 = 0; int num4 = 0; int num5 = 0; int num6 = 0; int num7 = 0; int num8 = 0; int num9 = 0; int num10 = 0; int Num = 0;
	Mat imgGray, imgblur, erZhi, imgHsv, imgCanny, imgErode, imgDil, mvt[3], dstimage;//定义 Mat是指针类型，矩阵数据类型
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));//结构元素
	vector<vector<Point>> contours;//vector--contour--point.  //每张图片都包含好几个contours，而每个contour又包含好几个points
	vector<Vec4i> hierarchy;//层次结构  //里面有四个整数
	string imagePath;


};
