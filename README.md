### Hi there 👋
# 车车难造队视觉组技术文档 #

## 一、使用方法

### 1.1 环境配置

ps:博主本人使用的是ubantu系统

由于博主使用的是cmake来构建与维护代码，所以需要在安装cmake条件下进行编译，cmakelists.txt文件已经写好并放在目录下，您可以根据需要对其进行修改。

linux下需要的依赖：Opencv4.2.0、build-essential（编译工具链 gcc - 9.4.0）、cmake

### 1.2 运行程序

如果您打算重新编译，可以删除build文件后，再重新生成，并在工程（XBOTCON）目录下使用以下指令编译：

您可以在工程目录下执行以下操作：

```
rm - rf build    //删除旧的build文件
mkdir build      //生成一个新的build文件
cd build/		 //cd到build目录下
cmake ..		 //使用cmake指令生成cmakefile，由于cmakelists.txt在上级目录，所以注意加..
make -j4		 //你可以根据你的喜好选择使用几个线程编译
./XBOTCON		 //此时生成了绿色的可执行文件，让我们运行它
```

如果您不打算重新编译，可以尝试直接运行目录下的XBOTCON文件直接运行我们的程序，不过大概率会识别：

```
./XBOTCON
```

### 1.3 更改颜色

如果你要运行我们的示例程序，可以通过以下方法查看：

-   视频测试

视频是橙色灯条，你可以在AmourPlate类中的预处理函数Treatment中注释掉：inRange(hsv, Scalar(78, 43, 46), Scalar(124, 255, 255), binary);

```c++
inRange(hsv, Scalar(LowH, LowS, LowV), Scalar(HighH, HighS, HighV), binary);
//inRange(hsv, Scalar(78, 43, 46), Scalar(124, 255, 255), binary);
```

并且注释掉main函数中的：frame = imread("../1.png"); 

```c++
//frame = imread("../1.png"); 
capture >> frame;
```

-   图片测试

    图片同理：主函数中：

    ```c++
    frame = imread("../1.png"); 
    //capture >> frame;
    ```

    在AmourPlate类中的预处理函数Treatment中注释掉：

    ```c++
    //inRange(hsv, Scalar(LowH, LowS, LowV), Scalar(HighH, HighS, HighV), binary);
    inRange(hsv, Scalar(78, 43, 46), Scalar(124, 255, 255), binary);
    ```

    

## 二、我们的特性

### 2.1装甲板类（基类与派生类）

为了方便流程化操作，降低二次开发以及开发其他类似项目的难度：
我们构建一个装甲板的虚基类，并且在类中留下了接口函数。如果你要实现其他的项目，基于这个基类，你可以依照你的意愿去重写或修改我们的代码，以实现不同的目的。
在本次视觉工程中，我们在基类的基础上重写了函数，得到适用于本次工程的派生类AmourPlate，使得它能适应比赛中复杂的环境，并准确地识别出装甲板。
AmourPlate类中的主要功能函数介绍:

-   预处理函数Treatment
    在比赛中我们的摄像头拍摄的原图往往需要经过一系列处理才能才能转化为计算机方便处理的数据。其中最常用的就是灰度图和二值图。本次我们封装了三个预处理函数。可以依照不同场景下的不同选择来挑选适用的函数。
    预处理函数1，就是为了解决BGR图到HSV图，再从HSV图中过滤出轮廓的标准流程，在不同的项目工程中可以随时调用这个函数处理BGR图并返回二值图。
    预处理函数2，在环境光干扰比较大的场景，使用通道分离，然后将通道之间相减，并且经过不同的数学运算可以滤出蓝色光。
    预处理函数3，在环境光干扰比较大的场景，使用通道分离，然后将通道之间相减，并且经过不同的数学运算可以滤出橙色光。

-   过滤函数Screen

    处理之后的二值图势必要筛选出我们的灯条，根据灯条的发光、长宽比、凸度等信息，我们建立了一个筛选函数。

    这个函数较好的将传进来的二值图binary筛选出符合灯条特征的轮廓，将符合条件的轮廓以点集形式返回。

-   匹配函数matchingForArmo

    仅仅得到灯条还不够，比赛中我们要瞄准装甲板，所以对这些灯条一对一对地进行匹配，并返回符合筛选条件的成对灯条（以Rect矩形的形式）

-   基于卡尔曼的预测函数

    面对快速移动的装甲板，我们封装了基于kalman滤波的预测函数，这是新增了加速度来修正模型的卡尔曼滤波器，所以在面对加速运动的物体更加稳定。

### 2.2 测距类

为了使测距更加准确，我们构建了一个测距类专门负责视觉的测距部分，并在类中留出了几个接口，可以实现对已识别物体距离、俯仰角和偏航角的获取，以实现不同的需求。
AttitudeSolution类中主要功能函数及变量介绍：

-   姿态解算函数一Similarity
    使用了相似的数学原理，将我们已知的参数如装甲板投影在相机底板的长度等，进行数学计算，从而得到我们需要的距离、俯仰角和偏航角。
-   姿态解算函数二Solvepnp
    使用了OpenCV自带的solvePnP函数，将我们识别的物体的四个角点、相机参数和根据物体长宽形成的三维数据输入进去后，就可以得到物体的x、y、z世界坐标，从而求出它的距离、俯仰角和偏航角。
-   根据Solvepnp函数得到的距离distance_pnp
-   根据Solvepnp函数得到的角度pitch_angle_pnp_pnp
-   根据Solvepnp函数得到的距离yaw_angle_pnp
-   根据Similarity函数得到的距离distance_similarity
-   根据Similarity函数得到的距离pitch_angle_similarity
-   根据Similarity函数得到的距离yaw_angle_similarity
    需要说明的是，我们使用两个的两个姿态解算函数是并列的关系，是为了减少测量中可能会有的误差，在使用过程中，调用两者之一就可以了。

### 2.3 串口通信类--SerialPort

博主封装了串口通信的类，并实现了流程化，你可以使用这个类实现接受、发送单（多）字节的数据，用来和下位机通信。

**在我的设计中，实现linux设备与stm32通信只需要三步，**

-   使用USB转TTL模块连接stm32上对应的gpio口。
-   初始化串口通信类。函数SerialPort_init()
-   将你想发送的信息传入发送函数Send()或者使用我们重载的<<运算符来直接输出数据，这一点我们会在代码中体现，你也可以根据注释来学习如何使用。

## 三、视觉原理 ##

### （1）识别原理 ###

#### 1、图像的处理上 ####

我们过滤颜色采用了两种方法，一将颜色的BGR三通道分离，然后将通道之间相减，这样可以将环境光的大部分影响消除，并且经过不同的数学运算可以分别滤出蓝色光和橙色光。我们将这个过滤代码组成了一个函数，传进去的参数是摄像头拍到的原图片，输出的结果是处理后的图片。

```c++
Mat whiteLightblur_blue(const Mat frame)//过滤出蓝色光
{
	vector<Mat> temp;
	split(frame, temp);
	Mat frame_have;
	frame_have = 2*temp[0];
	frame_have -= (temp[2] * 0.5 + temp[1] * 0.5);
	threshold(frame_have, frame_have, 140, 255, THRESH_BINARY);
	return frame_have;
}
Mat whiteLightblur_red(Mat frame)//过滤出橙色光
{
	vector<Mat> temp;
	split(frame, temp);
	Mat frame_have;
	frame_have = (temp[2] * 0.5 + temp[1] * 0.5) * 1.3;
	frame_have -= temp[0];
	threshold(frame_have, frame_have, 160, 255, THRESH_BINARY);
	return frame_have;
}
```

二是将图片从BGR转变为HSV格式，然后使用了inRange函数，通过调节参数，来滤出橙色光或蓝色光。

~~~c++
inRange(hsv, Scalar(78, 43, 46), Scalar(124, 255, 255), binary);//过滤出蓝色光，其中binary为转变为HSV格式后的图片
inRange(hsv, Scalar(11, 43, 46), Scalar(34, 255, 255), binary);//过滤出橙色光
~~~

在我们获得处理好的图片之后，便把图片继续用传统的方法去处理，如：高斯滤波（GaussianBlur）、腐蚀操作（erode）、膨胀操作（dilate）。

#### 2、筛选 ####

我们首先用findContours函数从二值图中找到所有矩形轮廓，在通过轮廓的长宽比、轮廓和它的最小矩形的面积比、轮廓的凸度、轮廓的角度及轮廓之间的角度差（装甲板灯条是平行的）（使用了旋转椭圆函数）、轮廓之间的高度差（装甲板测试视频有时候会旋转装甲板，所以这个筛选条件设置的阈值较大）共五个筛选条件，通过调整修改设置的参数阈值，这些筛选条件筛除了大部分轮廓，有效地降低轮廓数量，提高了识别成功率。

#### 3、匹配 ####

即使是过滤后，也不可能将所有的非灯条的轮廓去除，况且有时会出现许多个灯条暴露在镜头面前的情况（RM），因此，我们有针对性地在灯条和灯条之间进行匹配，使用包括相对面积（两个灯条的面积是否相差不太远）、两灯条构成的矩形是否近似符合装甲板的长宽比例、位置的差值是否过于不符合常理、灯条之间的夹角是否过于离谱等条件筛选。

#### 4. 预测

卡尔曼滤波（karlman fliter）

一般情况写预测算法就是用当前帧的图像坐标对上一帧的图像坐标做差，求得速度，然后以此估计下一帧的坐标，高级点可以引入加速度。然而这样的方法存在不确定性，预测基于的信息太少，应对不了复杂情况，虽说效果在一般情况下也够用了。

卡尔曼滤波简单说就是在上一帧给到当前帧的预测量与当前帧的测量量（传感器的返回值）之间，根据二者的权重修正模型计算出可信度更高的“预测值”，用于下一帧。如此往复，不断修正，不断预测测量，从而达到一个平滑的预测效果。实际操作主要通过改变系统噪声方差矩阵Q、测量噪声方差矩阵R来改变卡尔曼增益，以实现不同的预测效果，乃至跟踪效果。

而且我们在卡尔曼中建立了加速度模型，相信对非匀速运动物体有很好的效果。

### （2）测距原理 ###

在测距方面，我们使用了两种方法，一是solvePNP位姿解算，二是依据数学原理计算距离。

#### 1、solvePNP ####

参考博客：

 https://blog.csdn.net/qq_33728095/article/details/123310400?utm_source=app&app_version=5.5.0&code=app_1562916241&uLinkId=usr1mkqgl919blen

#### 2、数学原理 ####

我们使用了相似的原理对距离进行计算。根据视频中装甲板距离视屏中心的像素距离、相机的像素单元长度、装甲板的实际大小和装甲板在视频中的大小可以计算出移动距离和高度。
设装甲板长度为d，投影在相机底板的长度为d'（根据像素单元长度计算得出），焦距为f，装甲板投影在水平面上与相机的垂直距离为L'，装甲板相对相机中央移动距离为L，装甲板在相机底面距离中心的水平长度为d''，竖直长度为L''，装甲板实际高度为H，装甲板（正六边形）的边长为s。可得数学公式d'/d=f/L'=（d''+0.5*d'）/（L+0.5*d）;L''/f=H/L'。解得L'=d*f/d';L=d*(d''+0.5*d')/d'-0.5*d;H=L''*L'/f

-   **图片展示**

    ![QQ图片20220614143218](https://s2.loli.net/2022/06/14/iSQ9BbwMrFckv6u.png)

    ![QQ图片20220614143245](https://s2.loli.net/2022/06/14/QzZp6khi5cSANmY.png)

![QQ图片20220614143259](https://s2.loli.net/2022/06/14/jXrEilP7vmcegby.png)

![QQ图片20220614143507](https://s2.loli.net/2022/06/14/QyuSrhkzivObcUa.png)

![QQ图片20220614143543](https://s2.loli.net/2022/06/14/IjoReZFPbcY3xq5.png)

### （3）预测原理 ###

以下：

[2021_kalman滤波_SCUT_Auto_Boeing_哔哩哔哩_bilibili](https://www.bilibili.com/video/BV1xR4y1W7BJ?spm_id_from=333.999.0.0)

[卡尔曼滤波- CSDN搜索](https://so.csdn.net/so/search?spm=1000.2115.3001.7498&q=卡尔曼滤波&t=&u=&utm_term=卡尔曼滤波&utm_medium=distribute.pc_toolbar_associateword.none-task-associate_word-opensearch_query-1-卡尔曼<%2Fem>滤波-null-null.nonecase&depth_1-utm_source=distribute.pc_toolbar_associateword.none-task-associate_word-opensearch_query-1-卡尔曼<%2Fem>滤波-null-null.nonecase&request_id=165508826716782388039649&opensearch_request_id=165508826716782388039649)

### （4）测角度原理 ###

根据测距得到的各个数据，依据三角函数，可得角度。

#### 1、偏航角 ####

偏航角θ=arctan((L+0.5*d)/L')

#### 2、俯仰角 ####

俯仰角α=arctan((H+0.5*s)/L')

### （5）通信原理 ###

[(17条消息) Linux下串口通信详解（上）打开串口和串口初始化详解_HAN_UESTC的博客-CSDN博客_linux下串口通信](https://blog.csdn.net/specialshoot/article/details/50707965?ops_request_misc=%7B%22request%5Fid%22%3A%22165521744316782248518525%22%2C%22scm%22%3A%2220140713.130102334..%22%7D&request_id=165521744316782248518525&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~sobaiduend~default-1-50707965-null-null.142^v14^control,157^v14^new_3&utm_term=Linux+串口&spm=1018.2226.3001.4187)

[(17条消息) Linux下串口通信详解（下）读写串口及关闭串口_HAN_UESTC的博客-CSDN博客](https://blog.csdn.net/specialshoot/article/details/50709257?spm=1001.2014.3001.5506)

## 四、指标和参数 ##

第一，我们基本实现了对视频中移动的灯带的检测和预测和测距，视频包含在工程目录下，你可以根据你的需要进行使用。

第二，我们实现了对图片中三个装甲板的识别，图片包含在工程目录下，你可以根据你的需要进行使用。

![image-20220614221818063](https://s2.loli.net/2022/06/14/I6mlGMCbX9JEa57.png)

第三，我们发现简单环境下识别的成功率在95以上（偶尔能保持100%），面对复杂多变的环境也有90%以上的成功率。

第四，帧率 ：getTickCount() 和getTickFrequency()函数，得出一次循环用时，得出帧率。测试视频这种低负荷场景帧率一般能超过30帧。

具体的测试视频可以在工程目录下查看。

## 五、总结 ##

### （1）遇到的问题及解决 ###

朱：1.为了能最大程度的除去干扰项，因此在过滤和灯条匹配环节，设置了很多约束条件，使得调参的过程非常的痛苦。

​		2.同时，尽管约束条件已经放得很宽了，但是复杂环境下偶尔会出现应该识别成功但是却没有的问题，说明这份代码的鲁棒性还不够		好，还需要更多的时间调参。

​		3.由于这个比赛持续了4个月……，所以部分代码在我C++都没学明白的时候写的，没用上指针这个C++的灵魂。

温：1.一开始在检测矩形轮廓后检测函数只使用了长宽比筛选函数，导致筛选后仍有很多轮廓，后来想出了如凸度检测等多种筛选方法，		使得成功率大大提高

​		2.一开始使用使用inrange函数筛选橙色光时，若背景有白光，则会影响检测，后来我们改进了方法，加入了通道相减的方法，使得		背景光对我们检测影响大大降低，识别更稳定了

### （2）目前还存在的问题 ###

1.使用通道相加减来过滤光时，如果背景光光强度变化，则其中的参数也需要调整，不是很方便。

2.测距方面还可以更准。

3.卡尔曼的预测的两个噪声矩阵会很大程度的影响预测效果，需要进一步调参。
<!--
**Lululzz/Lululzz** is a ✨ _special_ ✨ repository because its `README.md` (this file) appears on your GitHub profile.

Here are some ideas to get you started:

- 🔭 I’m currently working on ...
- 🌱 I’m currently learning ...
- 👯 I’m looking to collaborate on ...
- 🤔 I’m looking for help with ...
- 💬 Ask me about ...
- 📫 How to reach me: ...
- 😄 Pronouns: ...
- ⚡ Fun fact: ...
-->
