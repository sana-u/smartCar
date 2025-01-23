#include "g5g.h"
Rect red(Mat img)//识别到红色 
{
    Mat frame_a = img.clone();//深拷贝一帧
	Mat kernel_3 = Mat::ones(Size(3, 3), CV_8U);
	Mat HSV;
	// 计算起始行和结束行的索引
    int start_row = static_cast<int>(frame_a.rows * 0.55);
    int end_row = static_cast<int>(frame_a.rows * 0.9);

    // // 创建矩形区域
    Rect region(0, start_row, frame_a.cols, end_row - start_row);

	Mat roi = frame_a(region);
	cvtColor(roi, HSV, COLOR_BGR2HSV);

	// 定义红色的 HSV 范围
    Scalar lower_red1 = Scalar(0, 43, 46);
    Scalar upper_red1 = Scalar(10, 255, 255);
    Scalar lower_red2 = Scalar(153, 43, 46);
    Scalar upper_red2 = Scalar(180, 255, 255);

    // 根据给定的颜色范围生成二值掩膜
    Mat mask1, mask2, mask;
    inRange(HSV, lower_red1, upper_red1, mask1);
    inRange(HSV, lower_red2, upper_red2, mask2);
    mask = mask1 | mask2; // 合并两个掩膜

	Mat erosion;
	erode(mask, erosion, kernel_3, Point(-1, -1), 1, BORDER_CONSTANT, morphologyDefaultBorderValue());
	Mat dilation;
	dilate(erosion, dilation, kernel_3, Point(-1, -1), 1, BORDER_CONSTANT, morphologyDefaultBorderValue());
	Mat target;
	bitwise_and(roi, roi, target, dilation);
	Mat binary;
	threshold(dilation, binary, 127, 255, THRESH_BINARY);
	vector<vector<Point>> contours;
	findContours(binary, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	if (contours.size() == 0) {
		return Rect();
	}
	else {
        // 绘制轮廓
		return boundingRect(contours[0]);
	}
}
int find_red_cone(Mat img,int isVisual,int isCone)//识别到锥桶
{
	Rect obstacle_object = red(img);//识别到的红色物体
	if (obstacle_object.height > 5 && obstacle_object.height < 100)//对识别到的红色物体限幅,
	{
		if(isVisual){
			Mat image = img.clone();
			rectangle(image, Point(obstacle_object.x, obstacle_object.y ), Point(obstacle_object.x + obstacle_object.width, obstacle_object.y + obstacle_object.height ), Scalar(255, 0, 0), 5);
			imshow("detect red",image);
		}
		
        if (isCone&&(obstacle_object.y < res_y - 40)) {
            red_nums++;
            cout<<"检测到第"<<red_nums<<"个锥桶\n";
        }
        res_y = obstacle_object.y;
		if(red_nums%2==0){
            return obstacle_object.x+ obstacle_object.width;//返回红色锥桶的右坐标
        }else{
            return obstacle_object.x;//返回红色锥桶的左坐标
        }
	}
	return -1;//未识别到红色物体
}