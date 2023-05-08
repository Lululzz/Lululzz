#include "findcontours.h"

int main() {
    ShapeDetector shapeDetector("school_robot/low/1.jpg");
    //ShapeDetector shapeDetector("school_robot/middle/1.jpg"); //中低级图片识别代码相同，请移至detectShapes函数中修改
    //ShapeDetector shapeDetector("school_robot/high/2.jpg"); //高级图片识别单独一套代码，请移至detectShapes函数中修改
    shapeDetector.detectShapes();
    return 0;
}               
