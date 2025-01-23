#include "g5g.h"
/*
Mat preprocess_frame(Mat image) {
    Mat grayscale;
    cvtColor(image, grayscale, COLOR_BGR2GRAY);
    int kernel_size = 5; // 核越大噪点越少
    GaussianBlur(grayscale, grayscale, Size(kernel_size, kernel_size), 0);
    int low_t = 60; // 下限越大，噪点越少，但也容易不稳定
    int high_t = 150;
    Mat edges;
    Canny(grayscale, edges, low_t, high_t);
    //imshow("edges", edges);
    Mat region = region_of_interest(edges);
    vector<Vec4i> hough = detect_lines(region);
    draw_lines(image,hough);
    
    return image;
}

Mat region_of_interest(Mat image) {

    Mat mask = Mat::zeros(image.size(), image.type());
    int channel_count = image.channels();
    Scalar ignore_mask_color = (channel_count == 3) ? Scalar(255, 255, 255) : Scalar(255);

    int rows = image.rows;
    int cols = image.cols;
    Point bottom_left(0, rows * 0.5);
    Point top_left(0, rows);
    Point bottom_right(cols, rows);
    Point top_right(cols, rows * 0.5);

    vector<Point> vertices = {bottom_left, top_left,  bottom_right,top_right};
    vector<vector<Point>> pts = {vertices};
    fillPoly(mask, pts, ignore_mask_color);

    Mat masked_image;
    bitwise_and(image, mask, masked_image);
    return masked_image;
    
    int height = edges.rows;
    int width = edges.cols;
    
    Mat mask = Mat::zeros(edges.size(), edges.type());
    vector<Point> polygon;
    polygon.push_back(Point(0, height));
    polygon.push_back(Point(width, height));
    polygon.push_back(Point(width, height * 0.6));
    polygon.push_back(Point(0, height * 0.6));
    fillPoly(mask, vector<vector<Point>>{polygon}, Scalar(255));
    Mat cropped_edges;
    bitwise_and(edges,mask,cropped_edges);
    return cropped_edges;
}

vector<Vec4i> detect_lines(Mat image) {
    int rho = 1;
    double theta = CV_PI / 180;
    int threshold = 20;
    int minLineLength = 20;
    int maxLineGap = 500;
    vector<Vec4i> temp;
    HoughLinesP(image, temp, rho, theta, threshold, minLineLength, maxLineGap);

    vector<Vec4i>lines;
    //将一些点过滤掉
    for (const auto& line : temp) {
        int x1 = line[0];
        int y1 = line[1];
        int x2 = line[2];
        int y2 = line[3];
        double angle = atan2(line[3] - line[1], line[2] - line[0]) * 180.0 / CV_PI;
        
        if(angle<70&&angle>15||angle<-15&&angle>-70){//20的数值要改
            lines.push_back(line);
        }
        
        
    }

    return lines;
}

void draw_lines(Mat frame, vector<Vec4i>& lines) {
    for (const auto& line : lines) {
        int x1 = line[0];
        int y1 = line[1];
        int x2 = line[2];
        int y2 = line[3];
        cv::Scalar color(0, 255, 0); // 绿色
        int thickness = 5;
        cv::line(frame, cv::Point(x1, y1), cv::Point(x2, y2), color, thickness);
    }
}*/

Mat region_selection( Mat image) {
    Mat mask = Mat::zeros(image.size(), image.type());
    int channel_count = image.channels();
    Scalar ignore_mask_color = (channel_count == 3) ? Scalar(255, 255, 255) : Scalar(255);
    int rows = image.rows;
    int cols = image.cols;
    Point bottom_left(0, rows );
    Point top_left(cols * 0.45, rows * 0.5);
    Point top_right(cols * 0.55, rows * 0.5);
    Point bottom_right(cols , rows );
    Point mid_left(0, rows * 0.7);
    Point mid_right(cols , rows*0.7);
    vector<Point> vertices = {bottom_left, mid_left,top_left,  top_right,mid_right,bottom_right};
    vector<vector<Point>> pts = {vertices};
    fillPoly(mask, pts, ignore_mask_color);
    Mat masked_image;
    bitwise_and(image, mask, masked_image);
    return masked_image;
}

vector<Vec4i> detect_lines(Mat image) {
    int rho = 2;
    double theta = CV_PI / 180;
    int threshold = 20;
    int minLineLength = 70;
    int maxLineGap = 20;
    vector<Vec4i> temp;
    HoughLinesP(image, temp, rho, theta, threshold, minLineLength, maxLineGap);
    vector<Vec4i>lines;
    for (const auto& line : temp) {
        int x1 = line[0];
        int y1 = line[1];
        int x2 = line[2];
        int y2 = line[3];
        double angle = atan2(line[3] - line[1], line[2] - line[0]) * 180.0 / CV_PI;
         if(angle<70&&angle>20||angle<-20&&angle>-70){//angle<70&&angle>20||angle<-20&&angle>-70
             lines.push_back(line);
         }
    }
    return lines;
}
void draw_lines(Mat frame, vector<Vec4i>& lines) {
    for (const auto& line : lines) {
        int x1 = line[0];
        int y1 = line[1];
        int x2 = line[2];
        int y2 = line[3];
        Scalar color(255); 
        int thickness = 2;
        cv::line(frame, cv::Point(x1, y1), cv::Point(x2, y2), color, thickness);
    }
}
void draw_lines_on_binary_image(const vector<Vec4i>& lines, Mat& binary_image) {
    for (const Vec4i& detected_line : lines) {  // Change 'line' to 'detected_line' to avoid conflict
        Point pt1(detected_line[0], detected_line[1]);
        Point pt2(detected_line[2], detected_line[3]);
        line(binary_image, pt1, pt2, Scalar(255), 2, LINE_AA); // Draw the line
    }
}
Mat frame_processorByHF(Mat image, int cnt) {
    Mat grayscale;
    cvtColor(image, grayscale, COLOR_BGR2GRAY);
    //操场试试不用高斯滤波
    int kernel_size = 5; // 核越大噪点越少
    GaussianBlur(grayscale, grayscale, Size(kernel_size, kernel_size), 0);
    int low_t = 70; // 下限越大，噪点越少，但也容易不稳定
    int high_t = 150;
    /*操场可以改*/
    // int low_t = 30; // 下限越大，噪点越少，但也容易不稳定
    // int high_t = 100;
    Mat edges;
    Canny(grayscale, edges, low_t, high_t);
    //imshow("edges", edges);
    Mat region = region_selection(edges);
    vector<Vec4i> hough = detect_lines(region);
    draw_lines(region,hough);
    Mat binary_image = Mat::zeros(image.size(), CV_8UC1);
    draw_lines_on_binary_image(hough, binary_image);
    imwrite("./image/check/" + to_string(cnt) + ".jpg" ,binary_image);
    cout<<"保存成功\n";
    return binary_image;
}
//二值化方法进行直线检测
Mat frame_processorByEZ(Mat image) {
    

     // 定义白色范围
    Scalar lower_white(0, 0, 180);
    Scalar upper_white(180, 50, 255);
    //黄色
    // Scalar lower_white(10, 35, 35);
    // Scalar upper_white(34, 255, 255);
    Mat hsv;
    cvtColor(image, hsv, COLOR_BGR2HSV);
    
    Mat mask1;
    inRange(hsv, lower_white, upper_white, mask1);
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    dilate(mask1, mask1, kernel);
    erode(mask1, mask1, kernel);
    
    //mask1 = region_selection(mask1);
    return mask1;
}
