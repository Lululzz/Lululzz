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
  - 代码读取输入的图像并对其进行几个预处理步骤。这些步骤包括双边过滤器来去除噪音，将降噪后的图片转为HSV空间，再将图像分割成三个颜色通道（色相、饱和度和值），并计算每个通道的直方图。直方图被用来寻找图像中的主导颜色，然后用来进一步对图像进行阈值处理。
  - 预处理后，该算法使用 "findContours() "函数在二进制图像中找到轮廓。每个轮廓代表图像中的一个形状。然后，该算法对每个轮廓进行迭代，并分别使用`contourArea()`和`arcLength()`函数计算其面积和周长。
  - 然后使用 "approxPolyDP() "函数用一个多边形来近似每个轮廓。该函数需要两个参数：原始轮廓和原始轮廓与其近似值之间的最大距离。然后，近似值根据它的边数被归类为一个特定的形状。
  - 最后，该算法使用`boundingRect()`函数在每个检测到的形状周围画出矩形，并使用`putText()`函数为每个形状标注相应的名称。
  - 总的来说，这段代码实现了一种形状检测算法，它使用OpenCV的图像处理功能对输入图像进行分割和预处理，提取每个颜色通道的主导颜色，隔离颜色相似的区域，并将得到的形状近似为多边形。
  
- 补充（对函数的补充，可略读）：

  -直方图：在图像处理上，直方图是图像信息统计的有力工具。其实也就是统计一幅图某个亮度像素数量。直方图的规范化主要完成的任务是提高图像的对比度，实现过程主要如下，将灰度图像的的直方图计算出来，然后通过将不均匀的直方图进行拉伸（即对原图像中的灰度图进行调整）使得整个直方图灰度分布较为平均，这样得到的图像的对比度较高。是预处理阶段较为常见的而有效的图像处理手段。
  
  -void calcHist( const Mat* images, int nimages,
                          const int* channels, InputArray mask,
                          OutputArray hist, int dims, const int* histSize,
                          const float** ranges, bool uniform=true, bool accumulate=false );
参数解释：images：输入的图像的指针； nimages：输入图像个数；channels：需要统计直方图的第几通道； mask：掩模，mask必须是一个8位（CV_8U）的数组并且和images的数组大小相同； hist：直方图计算的输出值； dims：输出直方图的维度（由channels指定）； histSize：直方图中每个dims维度需要分成多少个区间（如果把直方图看作一个一个竖条的话，就是竖条的个数）； ranges：统计像素值的区间；uniform=true：是否对得到的直方图数组进行归一化处理； accumulate=false：在多个图像时，是否累积计算像素值的个数；

   -双边滤波：中值滤波，高斯滤波等单纯地考虑空间信息，造成了边界模糊和部分信息的丢失。双边滤波（Bilateral filter）是一种非线性的滤波方法，本质是基于高斯滤波，目的是解决高斯滤波造成的边缘模糊。结合图像的空间邻近度和像素值相似度的一种折处理，同时考虑空域信息和灰度相似性，达到保边去噪的目的。具有简单、非迭代、局部的特点。双边滤波器的好处是可以做边缘保存（edge preserving），一般用高斯滤波去降噪，会较明显地模糊边缘，对于高频细节的保护效果并不明显。双边滤波器顾名思义比高斯滤波多了一个高斯核。它是基于像素颜色分布的高斯滤波函数，所以在边缘附近，当两个像素距离很近时，只有同时当颜色很接近时影响才会较大，反之，虽然距离很近，但颜色差距较大，那么平滑权重也会很小。这样就保证了边缘附近像素值的保持，起到了保边的效果。在OpenCV中，实现双边滤波的函数是cv2.bilateralFilter（），该函数的语法是：dst=cv2.bilateralFilter（src,d,sigmaColor,sigmaSpace,borderType）。式中：
        ● dst是返回值，表示进行双边滤波后得到的处理结果。
        ● src 是需要处理的图像，即原始图像。它能够有任意数量的通道，并能对各通道独立处理。图像深度应该是CV_8U、CV_16U、CV_16S、CV_32F或者CV_64F中的一 种。   
        ● d是在滤波时选取的空间距离参数，这里表示以当前像素点为中心点的直径。如果该值为非正数，则会自动从参数 sigmaSpace 计算得到。如果滤波空间较大（d>5），则速度较慢。因此，在实时应用中，推荐d=5。对于较大噪声的离线滤波，可以选择d=9。
        ● sigmaColor是滤波处理时选取的颜色差值范围，该值决定了周围哪些像素点能够参与到滤波中来。与当前像素点的像素值差值小于 sigmaColor 的像素点，能够参与到当前的滤波中。该值越大，就说明周围有越多的像素点可以参与到运算中。该值为0时，滤波失去意义；该值为255时，指定直径内的所有点都能够参与运算。
        ● sigmaSpace是坐标空间中的sigma值。它的值越大，说明有越多的点能够参与到滤波计算中来。当d>0时，无论sigmaSpace的值如何，d都指定邻域大小；否则，d与 sigmaSpace的值成比例。
        ● borderType是边界样式，该值决定了以何种方式处理边界。一般情况下，不需要考虑该值，直接采用默认值即可。
        为了简单起见，可以将两个sigma（sigmaColor和sigmaSpace）值设置为相同的。如果它们的值比较小（例如小于10），滤波的效果将不太明显；如果它们的值较大（例如大于150），则滤波效果会比较明显，会产卡通效果。
        在函数cv2.bilateralFilter（）中，参数borderType是可选参数，其余参数全部为必选参数。
  
   -迭代器：迭代器是一种极为方便的可以遍历数组或集合的方法，使用opencv的自带的Mat中的迭代器，可以方便的进行遍历数组操作。迭代器是一种检查容器内元素并遍历元素的数据类型。迭代器提供对一个容器中的对象的访问方法，并且定义了容器中对象的范围。迭代器（Iterator）是指针（pointer）的泛化，它允许程序员用相同的方式处理不同的数据结构（容器）。在迭代法中，你所需要做的仅仅是获得图像矩阵的begin和end，然后增加迭代直至从begin到end。将*操作符添加在迭代指针前，即可访问当前指向的内容。对于一个cv::Mat的实例，你可以通过image.begin<cv::Vec3b>()来得到图像左上角位置的迭代器。如果想从第二行开始，可以使用image.begin<cv::Vec3b>()+image.rows来初始化迭代器。根据迭代器实现的不同功能，C++迭代器分为：输入迭代器，输出迭代器，正向迭代器，双向迭代器，随机迭代器。在这里只介绍代码中运用到的输出迭代器：输出迭代器。向容器中写入元素。输出迭代器只能一次一个元素向前移动。输出迭代器只支持一遍算法，同一输出迭代器不能两次遍历一个序列。
   
   
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
    
    cvtColor(img, imgGray, COLOR_BGR2GRAY);//Blue... to Gray

    bilateralFilter(img, imgblur, 30, 28, 15 / 2); //双边滤波
    
    cvtColor(imgblur, imgHsv, COLOR_BGR2HSV);//代码将图像分割成三个颜色通道（色相、饱和度和值），并计算每个通道的直方图.直方图被用来寻找图像中的主导颜色，然后用来进一步对图像进行阈值处理。

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
            (*IterStart1) > (hist1 + 200) || (*IterStart1) < (hist1 - 200) ||
            (*IterStart2) > (hist2 + 150) || (*IterStart2) < (hist2 - 20))
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
    //ShapeDetector shapeDetector("school_robot/low/1.jpg");
    //ShapeDetector shapeDetector("school_robot/middle/1.jpg"); //中低级图片识别代码相同，请移至detectShapes函数中修改
    ShapeDetector shapeDetector("school_robot/high/1.jpg"); //高级图片识别单独一套代码，请移至detectShapes函数中修改
    shapeDetector.detectShapes();
    return 0;
}
```
