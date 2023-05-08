## 车车难造队—视觉-卢峥

### 大致思路

- 中低级图片的识别
  - 将输入图片利用inrange函数进行二值化处理，使用Canny边缘检测器对二值化后的图像进行检测，接着进行开闭操作处理后findcontours.

- 高级图片的识别
  - 由于笔者高级图片的识别办法运用到了直方图等知识，笔者将在此详细对代码进行解读
  - 首先，它定义了一个名为drawHist的函数，以一个名为image的Mat对象作为其参数。这个函数计算并返回输入图像中最频繁的像素值。
  - 该函数的第一行声明了一个整数变量numbins并将其初始化为256。这个值代表了将用于创建直方图的bin或divisions的数量。
  - 第二行定义了一个名为range的浮动数组，有两个元素，0和256。这个数组将被用作直方图的取值范围。
  - 第三行创建一个名为histRange的浮点数组的指针，并将其分配给范围数组的地址。这个指针将在后面用来指定直方图的取值范围。
  - 第四行创建一个名为hist的Mat对象，它将被用来存储直方图。
  - 第五行调用calcHist函数来计算输入图像的直方图。传递给该函数的参数是：&image：对输入图像的一个；1：图像的数量（本例中只有一个）；0：用于直方图的通道的索引（0表示灰阶）；Mat(): 应用于图像的可选遮罩（在本例中不使用）；hist：将存储直方图的Mat对象；1：要计算的直方图的数量（本例中只有一个）；&numbins：用于直方图的bin的数量；&histRange：直方图的取值范围。
  - 直方图计算完毕后，该函数通过迭代直方图中的所有bin来寻找最频繁的像素值。它声明了一个浮动变量max并将其初始化为0，以及一个整数变量nn并将其初始化为0。
  - 在每次迭代中，它将直方图中当前bin的值与当前的最大值（max）进行比较，如果当前bin的值更高，则更新nn和max。最后，该函数返回nn的值，它代表输入图像中最频繁的像素值。注意，输入图像中的像素值必须在[0, 255]范围内，这个函数才能正常工作，因为这是范围数组中指定的范围。如果输入图像有不同的数值范围，范围数组应作相应调整。
  - 接下来，定义了一个名为ShapeDetector的类，它的构造函数将图像路径作为输入。这个类也有一个名为detectShapes的函数，在输入的图像上执行形状检测算法。
  - 代码读取输入的图像并对其进行几个预处理步骤。这些步骤包括将图像转换为灰度，应用中值和双边过滤器来去除噪音，并对图像进行阈值处理以创建二进制图像。
  - 然后，二进制图像被用来从原始图像中去除背景，用模糊图像中的相应像素替换背景中的像素。
  - 接下来，代码将图像分割成三个颜色通道（色相、饱和度和值），并计算每个通道的直方图。直方图被用来寻找图像中的主导颜色，然后用来进一步对图像进行阈值处理。
  - 预处理后，该算法使用 "findContours() "函数在二进制图像中找到轮廓。每个轮廓代表图像中的一个形状。然后，该算法对每个轮廓进行迭代，并分别使用`contourArea()`和`arcLength()`函数计算其面积和周长。
  - 然后使用 "approxPolyDP() "函数用一个多边形来近似每个轮廓。该函数需要两个参数：原始轮廓和原始轮廓与其近似值之间的最大距离。然后，近似值根据它的边数被归类为一个特定的形状。
  - 最后，该算法使用`boundingRect()`函数在每个检测到的形状周围画出矩形，并使用`putText()`函数为每个形状标注相应的名称。
  - 总的来说，这段代码实现了一种形状检测算法，它使用OpenCV的图像处理功能对输入图像进行分割和预处理，提取每个颜色通道的主导颜色，隔离颜色相似的区域，并将得到的形状近似为多边形。
- 补充

  -迭代器：迭代器是一种极为方便的可以遍历数组或集合的方法，使用opencv的自带的Mat中的迭代器，可以方便的进行遍历数组操作。在迭代法中，你所需要做的仅仅是获得图像矩阵的begin和end，然后增加迭代直至从begin到end。将*操作符添加在迭代指针前，即可访问当前指向的内容。对于一个cv::Mat的实例，你可以通过image.begin<cv::Vec3b>()来得到图像左上角位置的迭代器。如果想从第二行开始，可以使用image.begin<cv::Vec3b>()+image.rows来初始化迭代器。
注意：如果你的操作对象是const cv::Mat,或者你想强调当前当前循环不会对CV：：Mat的实例进行修改，那么你就因该创建常量迭代器。常量迭代器的声明如下：
cv::Mat ConstIterator_<cv::Vec3b>it;
或者：
cv::Mat_<cv::Vec3b> ::Const_Iterator it;

  -直方图：直方图的规范化主要完成的任务是提高图像的对比度，实现过程主要如下，将灰度图像的的直方图计算出来，然后通过将不均匀的直方图进行拉伸（即对原图像中的灰度图进行调整）使得整个直方图灰度分布较为平均，这样得到的图像的对比度较高。是预处理阶段较为常见的而有效的图像处理手段。最简单的直方图是线性变换通过对密集区域的拉伸实现灰度的平均分布。但有时难以达到理想的效果，所以一种改进的的方法可以对原图像的灰度直方图进行分段，分为拉伸端和压缩段已达到更好的效果。另一种改进的方法是先设计一种灰度拉伸的结果然建立原图像的灰度直方图和拉升后的灰度直方图的映射，可以不同于原图像分布的可以订制的直方图分布。
### 对于图形的判断

- 使用findContours函数在二进制图像中找到轮廓，并使用 approxPolyDP函数对每个轮廓进行近似处理。得到的多边形被存储在一个名为conPoly的向量中，其边界矩形被存储在一个名为boundRect的向量中。 然后，代码通过迭代conPoly中的多边形，并使用多边形顶点上的size函数计算边数，从而计算出具有多个顶点的形状的数量。计数结果用cout函数打印到控制台。


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
int drawHist(Mat image)//这个函数计算并返回输入图像中最频繁的像素值。
{
    int numbins = 256; //这个值代表了将用于创建直方图的bin或divisions的数量。
    float range[] = { 0, 256 }; //定义了一个名为range的浮动数组，有两个元素，0和256。这个数组将被用作直方图的取值范围。
    const float* histRange = { range };//直方图均衡化 浮点数组的指针，并将其分配给范围数组的地址。这个指针将在后面用来指定直方图的取值范围。
    Mat hist; //它将被用来存储直方图

    calcHist(&image, 1, 0, Mat(), hist, 1, &numbins, &histRange);//计算图像直方图 统计每个颜色值级别的像素一共有多少
    //调用calcHist函数来计算输入图像的直方图。传递给该函数的参数是：&image：对输入图像的一个；1：图像的数量（本例中只有一个）；0：用于直方图的通道的索引（0表示灰阶）；Mat(): 应用于图像的可选遮罩（在本例中不使用）；hist：将存储直方图的Mat对象；1：要计算的直方图的数量（本例中只有一个）；&numbins：用于直方图的bin的数量；&histRange：直方图的取值范围。

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
}//在每次迭代中，它将直方图中当前bin的值与当前的最大值（max）进行比较，如果当前bin的值更高，则更新nn和max。最后，该函数返回nn的值，它代表输入图像中最频繁的像素值。注意，输入图像中的像素值必须在[0, 255]范围内，这个函数才能正常工作，因为这是范围数组中指定的范围。如果输入图像有不同的数值范围，范围数组应作相应调整。
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

    threshold(imgGray, erZhi, 50, 255, THRESH_BINARY_INV);//代码将图像分割成三个颜色通道（色相、饱和度和值），并计算每个通道的直方图.直方图被用来寻找图像中的主导颜色，然后用来进一步对图像进行阈值处理。
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
	    //该函数需要两个参数：原始轮廓和原始轮廓与其近似值之间的最大距离。然后，近似值根据它的边数被归类为一个特定的形状。
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
    //操作时仅修改图片路径即可
    ShapeDetector shapeDetector("school_robot/low/1.jpg");
    //ShapeDetector shapeDetector("school_robot/middle/1.jpg"); //中低级图片识别代码相同，请移至detectShapes函数中修改
    //ShapeDetector shapeDetector("school_robot/high/2.jpg"); //高级图片识别单独一套代码，请移至detectShapes函数中修改
    shapeDetector.detectShapes();
    return 0;
}
```
