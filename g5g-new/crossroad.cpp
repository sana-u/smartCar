#include "g5g.h"

/*
    检测斑马线   目前这个是最好的参数
*/
int maskcount=0;
/**
 * frame 输入图像
 * isVisual 是否可视化
 */
int crossroad(Mat frame,int isVisual) {

    int rows = frame.rows;
    int cols = frame.cols;

    Mat src,mask1;
    // 定义白色范围--这里依据自己的图像进行修改数值大小
    Scalar lower_white(0, 0, 160);
    Scalar upper_white(180, 50, 255);

    Mat hsv;
    cvtColor(frame, hsv, COLOR_BGR2HSV);
    
    
    inRange(hsv, lower_white, upper_white, mask1);
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    dilate(mask1, mask1, kernel);
    erode(mask1, mask1, kernel);

    // 第一种创建ROI的方式
    // 计算起始行和结束行的索引 
    // int start_row = static_cast<int>(frame.rows * 0.5);
    // int end_row = static_cast<int>(frame.rows * 0.9);

    // // 创建矩形区域
    // Rect roi(0, start_row, frame.cols, end_row - start_row);

    // 第二种创建ROI的方式(更推荐)
    Point bottom_left(0, rows );
    Point top_left(cols * 0.4, rows * 0.55);
    Point top_right(cols * 0.6, rows * 0.55);
    Point bottom_right(cols , rows );
    Point mid_left(0, rows * 0.7);
    Point mid_right(cols , rows*0.7);
    vector<Point> vertices = {bottom_left, mid_left,top_left,  top_right,mid_right,bottom_right};
    vector<vector<Point>> pts = {vertices};
    
    // 创建掩码并填充多边形区域
    Mat polyMask = Mat::zeros(frame.size(), CV_8UC1);
    fillPoly(polyMask, pts, Scalar(255));

    // 提取多边形区域
    mask1.copyTo(src, polyMask);

    // src = mask1(roi);

    int cout1 = 0;//用来记录每一行有多少组白色区域
    int cout2 = 0;//用来记录每一组白色区域的宽度
    int flag = 0;

    int BanMaWidth = 6;//斑马线宽度的门限值
    int BanMaNums = 3;//一行的斑马线组数门限值;
    for (int i = rows*0.55; i < rows; i+=2) {
        if (cout1 < BanMaNums) {
            flag = 0;
        }
        cout1 = 0;
        for (int j = 10; j < cols - 10;) {
            cout2 = 0;
            if(src.at<uchar>(i,j)==0){//这个像素点为黑色
                j++;
                while(j<src.cols - 10 && src.at<uchar>(i,j) == 255){
                    j++;
                    cout2++;
                }
                if(cout2 >= BanMaWidth&&cout2<40){//找到一个斑马块
                    cout1++;//斑马线组数++
                }
            }else{//如果是白色像素点，则后移即可
                j++;
            }
            
            if (cout1 >= BanMaNums) {
                flag++;

                if (flag >= 3) {//连续三行都是
                    hasBanma = 1;//定义的全局变量，用来判断是否检测到斑马线
                    
                }
                break;
            }
        }
    }
    if(isVisual){
        //imshow("roi of banma:",src);
        //imshow("mask",mask1);
        //imwrite("./image/mask/"+to_string(maskcount++)+".jpg",src);//每次保存的文件名都不一样
    }
    return hasBanma;
}