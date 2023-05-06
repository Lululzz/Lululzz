## 视觉

### 预处理大致思路

- 中低级图片的识别
  - 将输入图片利用inrange函数进行二值化处理，使用Canny边缘检测器对二值化后的图像进行检测，接着进行开闭操作处理后findcontours.

- 对高级
  - 由于笔者高级图片的识别办法运用到了直方图等知识，笔者将在此详细对代码进行解读
  - 首先，定义一个名为“drawHist”的函数，统计图像的像素信息，每个颜色级像素的数量是多少，占整幅图像的比例。它接受一个输入图像并返回图像中的主导颜色的值。这个函数首先使用OpenCV的cvtColor函数将输入图像转换为HSV颜色空间。然后，它使用calcHist函数创建一个色调通道的直方图， 统计每个颜色值级别的像素一共有多少，并使用minMaxLoc函数找到具有最高值的bin的索引。这个bin的相应的Hue值被作为图像的主色调返回。
  - 接下来，定义了一个名为ShapeDetector的类，它的构造函数将图像路径作为输入。这个类也有一个名为detectShapes的函数，在输入的图像上执行形状检测算法。
  - 在detectShapes函数中，使用OpenCV的imread函数读取输入图像。然后使用各种OpenCV函数对图像进行预处理。首先，使用cvtColor将其转换为HSV颜色空间。
  - 然后，使用中值模糊函数，应用中值模糊滤波器来减少噪音。使用inRange函数对图像施加二进制阈值，将图像分割成颜色相似的区域。
  - 阈值处理后，对图像进行形态学操作，以去除小的噪声区域，并使用morphologyEx函数填补形状中的空白。
  - 然后将得到的二进制图像分成三个颜色通道（色相、饱和度和值），并使用drawHist函数计算每个通道的直方图。
  - 接下来，三个颜色通道被迭代，与每个通道的主色不匹配的像素被设置为白色，而其余的则使用bitwise_not和bitwise_and函数设置为黑色。
  - 使用morphologyEx函数对得到的二进制图像再次进行形态学操作，以减少噪音并填补形状中的空白。
  - 最后，使用findContours函数在二进制图像中找到轮廓，并使用 approxPolyDP函数对每个轮廓进行近似处理。得到的多边形被存储在一个名为conPoly的向量中，其边界矩形被存储在一个名为boundRect的向量中。 然后，代码通过迭代conPoly中的多边形，并使用多边形顶点上的size函数计算边数，从而计算出具有多个顶点的形状的数量。计数结果用cout函数打印到控制台。
  - 总的来说，这段代码实现了一种形状检测算法，它使用OpenCV的图像处理功能对输入图像进行分割和预处理，提取每个颜色通道的主导颜色，隔离颜色相似的区域，并将得到的形状近似为多边形。


### 对于图形的判断

- 使用findContours函数在二进制图像中找到轮廓，并使用 approxPolyDP函数对每个轮廓进行近似处理。得到的多边形被存储在一个名为conPoly的向量中，其边界矩形被存储在一个名为boundRect的向量中。 然后，代码通过迭代conPoly中的多边形，并使用多边形顶点上的size函数计算边数，从而计算出具有3、4、5个顶点的形状的数量。计数结果用cout函数打印到控制台。


### headline.h

- 包含了运行所需要的头文件

```c++
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;
  using namespace std;
  ```

### get_contours.h
```c++
 #include"headlines.h"

int drawHist(Mat image);//定义绘制直方图函数

class ShapeDetector//定义类和类中函数
{
public:

	ShapeDetector(string imagePath)
	{
		this->imagePath = imagePath;
	}

	void detectShapes();

private:

	string imagePath;
	
};
  ```

#### 类:

```C++
class ShapeDetector
{
public:

	ShapeDetector(string imagePath)
	{
		this->imagePath = imagePath;
	}

	void detectShapes();

private:

	string imagePath;
	
};
```


#### 函数详情:

##### drawHist

```C++
//绘制直方图
//直方图就是用来统计图像的像素信息的，每个颜色级像素的数量是多少，占整幅图像的比例
int drawHist(Mat image)
{
    int numbins = 256;
    float range[] = { 0, 256 }; //定义一个浮点型的一维数组，含两个值
    const float* histRange = { range };//直方图均衡化
    Mat hist;

    calcHist(&image, 1, 0, Mat(), hist, 1, &numbins, &histRange);//计算图像直方图 统计每个颜色值级别的像素一共有多少

    int width = 512;
    int height = 300;

    Mat histImage(height, width, CV_8UC3, Scalar(20, 20, 20));

    float max = 0;
    int nn = 0;
    for (int i = 0; i < numbins; i++)
    {
        if (max < hist.at<float>(i))
        {
            nn = i;
            max = hist.at<float>(i);
        }
    }
    return nn;
}
```

##### detectShapes

```C++
//该类函数为程序主要实现代码，整体逻辑性强，为便于读者阅读，故将其做一个函数书写。且这样书写后，main函数的书写非常简便
void ShapeDetector::detectShapes() {
    //Preprocessing
    Mat img = imread(this->imagePath);
    Mat img0 = img.clone();//拷贝图片 方式为深拷贝
    
    cvtColor(img, imgGray, COLOR_BGR2GRAY);//Blue... to Gray

    medianBlur(img, imgblur, 9);

    medianBlur(img0, imgblur, 9);

    threshold(imgGray, erZhi, 50, 255, THRESH_BINARY_INV);
    for (int row = 0; row < img.rows; row++)
    {
        for (int col = 0; col < img.cols; col++)
        {
            if (erZhi.at<uchar>(row, col) == 255)  //uchar类型
            {
                img0.at<Vec3b>(row, col) = imgblur.at<Vec3b>(row, col); //vector<uchar,3>
            }
        }
    }

    bilateralFilter(img0, imgblur, 15, 25, 15 / 2); //双边滤波
    cvtColor(imgblur, imgHsv, COLOR_BGR2HSV);//Blue... to Gray

    split(imgHsv, mvt);

    //图片识别

    // // 中低级图片
    // inRange(imgGray, imgGray.at<uchar>(0, 0) - 50, imgGray.at<uchar>(0, 0) + 50, dstimage);
    // Canny(dstimage, imgCanny, 15, 45);
    // dilate(imgCanny, imgDil, kernel);
    // erode(imgDil, imgErode, kernel);
    // findContours(imgErode, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);//find contours

    //高级图片
    int hist0 = drawHist(mvt[0]);
    int hist1 = drawHist(mvt[1]);
    int hist2 = drawHist(mvt[2]);
    //初始化输出图像迭代器
    cv::MatIterator_<cv::uint8_t> IterStart = mvt[0].begin<cv::uint8_t>();
    cv::MatIterator_<cv::uint8_t> IterEnd = mvt[0].end<cv::uint8_t>();
    cv::MatIterator_<cv::uint8_t> IterStart1 = mvt[1].begin<cv::uint8_t>();
    cv::MatIterator_<cv::uint8_t> IterStart2 = mvt[2].begin<cv::uint8_t>();
    // 遍历图像反色处理
    while (IterStart != IterEnd)
    {
        if ((*IterStart) > (hist0 + 2) || (*IterStart) < (hist0 - 2) ||
            (*IterStart1) > (hist1 + 50) || (*IterStart1) < (hist1 - 20) ||
            (*IterStart2) > (hist2 + 50) || (*IterStart2) < (hist2 - 20))
        {
            (*IterStart) = 255;
        }
        else
        {
            (*IterStart) = 0;
        }
        // 迭代器递增
        IterStart++;
        IterStart2++;
    }
    erode(mvt[0], imgErode, kernel);
    dilate(imgErode, imgDil, kernel);
    findContours(imgErode, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);//find contours

    vector<vector<Point>> conPoly(contours.size());
    vector<Rect> boundRect(contours.size());

    cout << contours.size() << endl;
    for (int i = 0; i < contours.size(); i++) {
        int area = contourArea(contours[i]);
        string objectType;
        if (area > 100) {
            float peri = arcLength(contours[i], true) - 20;//是否轮廓参数等关闭闭合 若为true，则说明近似曲线是闭合的；反之，若为false，则断开

            cout << peri << endl;
            approxPolyDP(contours[i], conPoly[i], 0.0275 * peri, true);//approxPolyDP()函数是opencv中对指定的点集进行多边形逼近的函数，其逼近的精度可通过参数设置。
            int objCor = (int)conPoly[i].size();
            if (objCor == 3) { objectType = "Trilateral3"; num3++; }
            else if (objCor == 4) { objectType = "Quadrilateral4"; num4++; }
            else if (objCor == 5) { objectType = "Pentagon5"; num5++; }
            else if (objCor == 6) { objectType = "Hexagon6"; num6++; }
            else if (objCor == 7) { objectType = "Heptagon7"; num7++; }
            else if (objCor == 8) { objectType = "Octagon8"; num8++; }
            else { objectType = "Pentagram10"; num10++; }

            boundRect[i] = boundingRect(conPoly[i]);
            rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 1);
            putText(img, objectType, { boundRect[i].x,boundRect[i].y - 5 }, FONT_HERSHEY_PLAIN, 1, Scalar(0, 69, 255), 2);//FONT字体
            cout << objectType << endl;
            cout << conPoly[i].size() << endl;//输出多边形边数
            cout << "多边形 " << i + 1 << " 有 " << conPoly[i].size() << " 条边" << endl;
        }
    }
    cout << "三边形数量  " << num3 << endl; cout << "四边形数量  " << num4 << endl; cout << "五边形数量  " << num5 << endl;
    cout << "六边形数量  " << num6 << endl; cout << "七边形数量  " << num7 << endl; cout << "八边形数量  " << num8 << endl;
    cout << "星形状数量  " << num10 << endl;
    imshow("Detected Shapes", img);
    waitKey(0);
}
````



### main

```C++
	
#include "findcontours.h"
//main函数只需要传入图片路径，调用ShapeDetector类中的detectShapes函数即可
int main() {
    ShapeDetector shapeDetector("school_robot/low/1.jpg");
    //ShapeDetector shapeDetector("school_robot/middle/1.jpg"); //中低级图片识别代码相同，请移至detectShapes函数中修改
    //ShapeDetector shapeDetector("school_robot/high/2.jpg"); //高级图片识别单独一套代码，请移至detectShapes函数中修改
    shapeDetector.detectShapes();
    return 0;
}
```
