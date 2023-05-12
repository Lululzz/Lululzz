#include"findcontours.h"

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

//该类函数为程序主要实现代码，整体逻辑性强，为便于读者阅读，故将其做一个函数书写。且这样书写后，main函数的书写非常简便
void ShapeDetector::detectShapes() {
    //Preprocessing
    Mat img = imread(this->imagePath);
  
    bilateralFilter(img0, imgblur, 30, 28, 15 / 2); //双边滤波

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
