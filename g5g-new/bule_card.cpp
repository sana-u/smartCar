#include"g5g.h"
bool Contour_Area(vector<Point> contour1, vector<Point> contour2)
{
    return contourArea(contour1) > contourArea(contour2);
}

Mat blue_card_process(Mat frame){
    Mat frame_a = frame;//深拷贝一帧
    cvtColor(frame_a, frame_a, COLOR_BGR2HSV);//表示要从 BGR 转换到 HSV 颜色空间,frame_a 是转换后的图像
    //HSV 颜色空间 这里对应蓝色
    Mat mask;
    Scalar scalarl = Scalar(100, 43, 46);//scalarl 定义了一个 HSV 颜色范围的下限，分别代表 H、S、V 的三个通道值
    Scalar scalarH = Scalar(124, 255, 255);//scalarl 定义了一个 HSV 颜色范围的上限，分别代表 H、S、V 的三个通道值
    
    inRange(frame_a, scalarl, scalarH, mask);//用于根据给定的颜色范围生成二值掩膜  mask 是输出的二值图像，只有在指定的颜色范围内的像素会被设置为白色（255），其他像素为黑色（0）
    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));//生成一个结构元素，用于形态学操作。 ORPH_RECT 指定了结构元素的形状为矩形 Size(5, 5) 指定了结构元素的大小为 5x5 像素。
    morphologyEx(mask, mask, MORPH_OPEN, kernel);//去除小的噪声 输入和输出都是 mask
    morphologyEx(mask, mask, MORPH_CLOSE, kernel);//用于填补小的空洞，进一步改善图像的连通性。

    for (int y = 0; y < mask.rows; y++)
    {
        for (int x = 0; x < mask.cols; x++)
        {
            if (y < 70 || y > 120)//只保留行索引在 70 到 120 之间的像素，其余的像素都将被设置为 0
            {
                mask.at<uchar>(y, x) = 0;//将当前像素的值设置为 0（黑色），表示该区域在掩膜中被去除。
            }
        }
    }
   // imshow("bule mask:",mask);
    return mask;
}
void blue_card_find(Mat frame)
{
   
    Mat mask = blue_card_process(frame);
    vector<vector<Point>> contours;
    vector<Vec4i> hierarcy;
    findContours(mask, contours, hierarcy, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    if (contours.size() > 0)
    {
        sort(contours.begin(), contours.end(), Contour_Area);
        vector<vector<Point>> newContours;
        for (const vector<Point> &contour : contours)
        {
            Point2f center;
            float radius;
            minEnclosingCircle(contour, center, radius);
            if (center.y > 70 && center.y < 120)
            {
                newContours.push_back(contour);
            }
        }

        contours = newContours;

        if (contours.size() > 0)
        {
            if (contourArea(contours[0]) > 10000)
            {
                cout << "find biggest blue," << "contourArea(contours[0])="<<contourArea(contours[0])<<endl;
                Point2f center;
                float radius;
                minEnclosingCircle(contours[0], center, radius);
                //circle(frame, center, static_cast<int>(radius), Scalar(0, 255, 0), 2);
                find_blue_card = 1;//找到了蓝色挡板
            }
            else
            {
                cout << "not found blue" << endl;
            }
        }
    }
    else
    {
        cout << "not found blue" << endl;
    }
}

void blue_card_remove(Mat frame)
{
    Mat mask = blue_card_process(frame);
    //cout << "entry move blue process" << endl;
    vector<vector<Point>> contours;
    vector<Vec4i> hierarcy;
    findContours(mask, contours, hierarcy, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    if (contours.size() > 0)
    {
        sort(contours.begin(), contours.end(), Contour_Area);
        vector<vector<Point>> newContours;
        if (contourArea(contours[0]) < 10000)
        {
           blue_card_remove_flag = 1;
            cout<<"蓝色挡板移开\n";
            cout << "move" << endl;
        }
        
        
    }
    else
    {
        blue_card_remove_flag = 1;
        cout << "蓝色挡板移开" << endl;
        
    }
}

// 检测蓝色区域的函数
int detectBlueAreas(const cv::Mat inputImage) {
    // 转换为HSV颜色空间
    cv::Mat hsvImage;
    cv::cvtColor(inputImage, hsvImage, cv::COLOR_BGR2HSV);

    // 定义蓝色的HSV范围
    cv::Scalar lowerBlue = cv::Scalar(100, 150, 0);
    cv::Scalar upperBlue = cv::Scalar(140, 255, 255);

    // 创建掩码
    cv::Mat mask;
    cv::inRange(hsvImage, lowerBlue, upperBlue, mask);

    // 查找轮廓
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

   // 计算蓝色区域的总面积
    double totalBlueArea = 0.0;
    for (size_t i = 0; i < contours.size(); ++i) {
        totalBlueArea += cv::contourArea(contours[i]);
    }
    
    cout<<"蓝色总面积:"<<totalBlueArea<<"\n";
    return totalBlueArea;
}