## 视觉

### 预处理大致思路

- 对中低级
  - 灰度后直接Canny即可进行findContours

- 对高级
  - 由于椒盐噪声多,所以先进行中值滤波,然后将图像转为hsv后获取背景h值,利用h值进行二值化后 进行开闭运算处理噪声,之后再canny加findContours

> 对高级用了滑动条进行调参
>
> median_blur , h_high , h_low, size , size_1 , dilate_size;
>
> 中值滤波参数 , inRange处理的h上下偏差 , 闭运算 , 开运算 , 扩张系数

### 对于图形的判断

- findContours后对获取的点进行approxPolyDP来获得角点,并利用角点判断图形

### headline.h

- 包含了运行所需要的头文件

  ```c++
  #pragma once
  #include<iostream>
  #include<opencv2/opencv.hpp>
  
  using namespace cv;
  using namespace std;
  ```

### get_contours

#### 类:

```C++
class myimg
{
public:

	void load_img(Mat image);//加载处理图像

	void load_img_draw(Mat image);//加载显示图像

	Mat get_img();//获取处理图像
	
	Mat get_img_draw();//获取显示图像

	double load_h();//获得背景h值

	double get_h();//调用h值

	void test_pre(int median_blur ,int h_high ,int h_low ,int size,int size_1,int dilate_size);//对高级图像的预处理

	void low_pre();//对低级图像的预处理

	void get_contours();//获取轮廓

	void test_high_treat(string path, int median_blur, int h_high, int h_low,int size,int size_1, int dilate_size);//对高级图像的处理(包含test_pre与get_contours)

	void test_low_treat(string path);//对低级和中级图像的处理(包含low_pre与get_contours)
	
	void show_result(string name);//显示识别后图像

private:
	Mat img;//处理图像
	Mat img_draw;//显示图像
	double h;//将背景转为hsv后的h值
	vector<vector<Point>> contours;//存放角点
	vector<Vec4i> hierarchy;
};
```



> define Run用于显示处理过程(若需显示则注释掉这句话)

#### 函数详情:

##### load_h

```C++
double myimg::load_h()
{
	if (img.empty())//判空
	{
		cout << "could not find image..." << endl;
		return -1;
	}

	Mat hsvImg;
	cvtColor(img, hsvImg, COLOR_BGR2HSV);
	waitKey(1);
	h = hsvImg.at<Vec3b>(1, 1)[0];//获取h值
    return h；
}
```

##### test_pre

```C++
void myimg::test_pre(int median_blur, int h_high, int h_low,int size,int size_1, int dilate_size)
{
	Mat kernel = getStructuringElement(MORPH_RECT, Size(size,size));//闭运算核
	Mat kernel1 = getStructuringElement(MORPH_RECT, Size(size_1,size_1));//开运算核

	medianBlur(img, img, median_blur);//中值滤波
	
	load_h();//获取背景h值
	
	cvtColor(img, img, COLOR_BGR2HSV);//转hsv
	
	int hmin = h - h_low, smin = 0, vmin = 0;
	int hmax = h + h_high, smax = 255, vmax = 255;
	Scalar lower(hmin, smin, vmin);
	Scalar upper(hmax, smax, vmax);
	inRange(img, lower, upper, img);//二值化
	
	morphologyEx(img,img,3,kernel);//闭运算
	
	morphologyEx(img,img,2,kernel1);//开运算

	Canny(img, img, 50 ,50);//Canny
	
	Mat d_kernel = getStructuringElement(MORPH_RECT, Size(dilate_size, dilate_size));//扩张核
	
	dilate(img, img, d_kernel);//扩张
	
}
```

##### low_pre

```C++
void myimg::low_pre()
{
	//预处理:灰度,边际检测,扩张
	cvtColor(img, img, COLOR_BGR2GRAY);

	Canny(img , img ,25 , 60);
	
    Mat d_kernel_2 = getStructuringElement(MORPH_RECT, Size(2, 2));
	dilate(img, img, d_kernel_2);
}
```

##### get_contours

````C++
void myimg::get_contours()
{
	findContours(img, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);//获取轮廓并将点存进contours中
	
    drawContours(img_draw, contours, -1, Scalar(255, 0, 255), 2);//在显示图像上画轮廓

	vector<vector<Point>> conPoly(contours.size());//存放角点
	vector<Rect> boundRect(contours.size());//存放外接矩形点

	for (int i = 0; i < contours.size(); i++)
	{
		int area = contourArea(contours[i]);//获取面积

		string objectType;//记录图形类型

		if (500 < area)//进行筛选,去除面积过小的噪声
		{
			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);//获得角点
			drawContours(img_draw, conPoly, i, Scalar(255, 0, 255), 2);//连线

			//cout << conPoly[i].size() << endl;
			boundRect[i] = boundingRect(conPoly[i]);
			rectangle(img_draw, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 5);//用矩形框住图形


			int objCor = (int)conPoly[i].size();//获得角点个数
			//根据角点个数判断图形并显示
			if (objCor == 3) { objectType = "3Tri"; }
			if (objCor == 4) {
				float aspRatio = (float)boundRect[i].width / (float)boundRect[i].height;
				if (aspRatio < 0.95 && aspRatio < 1.05) { objectType = "4Square"; }
				else { objectType = "4Rect"; }
			}
			if (objCor == 5) { objectType = "5pentagon"; }
			if (objCor == 6) { objectType = "6hexagon"; }
			if (objCor == 7) { objectType = "7heptagon"; }
			if (objCor == 8) { objectType = "8octagon"; }
			if (objCor == 10 || objCor == 9) { objectType = "star"; }
			if (objCor > 10) { objectType = "Circle"; }
			
			putText(img_draw, objectType, { boundRect[i].x,boundRect[i].y - 5 }, FONT_HERSHEY_DUPLEX, 0.75, Scalar(0, 69, 255), 2);
		}
	}

}
````

##### treat

````C++
void myimg::test_high_treat(string path, int median_blur, int h_high, int h_low,int size,int size_1, int dilate_size)
{
	Mat temp_img1 = imread(path);
	Mat temp_img2 = imread(path);
	load_img(temp_img1);     		//载入处理图像
	load_img_draw(temp_img2);		//载入显示图像

	test_pre(median_blur, h_high, h_low,size,size_1, dilate_size);//预处理
	get_contours();												  //获得轮廓
}

void myimg::test_low_treat(string path)
{
	Mat temp_img1 = imread(path);
	Mat temp_img2 = imread(path);
	load_img(temp_img1);
	load_img_draw(temp_img2);

	low_pre();
	get_contours();
}
````

##### show_result

````C++
void myimg::show_result(string name)
{
	//resize(img_draw, img_draw, Size(), 0.4, 0.4);  //根据需要调整最终显示的大小
	imshow(name, img_draw);
}
````

### main

```C++
#include"include/get_contours.h"
#include"include/headline.h"
#include <conio.h>

#define low			//区分中低级难度与高级难度

int main()
{
    //对高级图像处理
	#ifndef low
	
	
	//median_blur , h_high , h_low, size , size_1 , dilate_size;
	int Median_blur = 3, H_high = 1, H_low = 1,size = 2,size_1=2, Dilate_size = 2;
	
	
	//创建滑动条
	namedWindow("Trackbars", (640, 200));
	createTrackbar("median_blur", "Trackbars", &Median_blur, 30);
	createTrackbar("h_high", "Trackbars", &H_high, 10);
	createTrackbar("h_low", "Trackbars", &H_low, 10);
	createTrackbar("size", "Trackbars", &size, 20);
	createTrackbar("size_1", "Trackbars", &size_1, 20);
	createTrackbar("dilate_size", "Trackbars", &Dilate_size, 15);
	
	while (1)
	{	
		int true_median_blur=Median_blur*2-1;//获取真实中值滤波参数(该参数只能为奇数)
		
		myimg photo1, photo2, photo3, photo4;
        
        //获取图像路径
		string path1 = "/home/qiuzhenhua/robotgame/photo/high/1.jpg";
		// string path2 = "/home/qiuzhenhua/robotgame/photo/high/2.jpg";
		// string path3 = "/home/qiuzhenhua/robotgame/photo/high/3.jpg";
		// string path4 = "/home/qiuzhenhua/robotgame/photo/high/4.jpg";
        
        //处理图像
		photo1.test_high_treat(path1 , true_median_blur , H_high , H_low,size ,size_1 , Dilate_size);
		// photo2.test_treat(path2 , true_median_blur , H_high , H_low,size ,size_1 , Dilate_size);
		// photo3.test_treat(path3 , true_median_blur , H_high , H_low,size ,size_1 , Dilate_size);
		// photo4.test_treat(path4 , true_median_blur , H_high , H_low,size ,size_1 , Dilate_size);
        
        //显示图像
		photo1.show_result("img1");
		// photo2.show_result("img2");
		// photo3.show_result("img3");
		// photo4.show_result("img4");
		// }

	}
	#endif

    //对低级处理
	#ifdef low
		myimg photo1, photo2, photo3, photo4;
		string path1 = "/home/qiuzhenhua/robotgame/photo/low/1.jpg";
		photo1.test_low_treat(path1);
		photo1.show_result("img1");
		waitKey(0);
	#endif
    
	return 0;
}
```
