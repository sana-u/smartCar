#include"g5g.h"
int num_yellow = 0;
int flag_yellow = 0;

std::vector<cv::Point> mid_yellow;
int yellow_image_count = 0;
int yellowImageCount=0;
Mat region_selection1( Mat image) {
    Mat mask = Mat::zeros(image.size(), image.type());
    int channel_count = image.channels();
    Scalar ignore_mask_color = (channel_count == 3) ? Scalar(255, 255, 255) : Scalar(255);
    int rows = image.rows;
    int cols = image.cols;
    Point bottom_left(0, rows * 0.9);
    Point top_left(0, rows * 0.7);
    Point top_right(cols, rows * 0.7);
    Point bottom_right(cols , rows * 0.9);
    vector<Point> vertices = {bottom_left, top_left, top_right, bottom_right};
    vector<vector<Point>> pts = {vertices};
    fillPoly(mask, pts, ignore_mask_color);
    Mat masked_image;
    bitwise_and(image, mask, masked_image);
    return masked_image;
}
/*
    检测图像中黄色区域
*/
Mat yellow_hsv(Mat img) {
    Mat cropped_image, image;
    Scalar lowerb = Scalar(20, 20, 20);//室内
    //Scalar lowerb = Scalar(0, 0, 0);//操场
    Scalar upperb = Scalar(34, 255, 255);
    // Scalar lowerb = Scalar(0, 0, 200);
    // Scalar upperb = Scalar(180, 50, 255);
    int yellow_num = 0;
    int rows = img.rows;
    image = img.clone();
    cropped_image = image;
    cvtColor(cropped_image, cropped_image, COLOR_BGR2HSV);
    morphologyEx(cropped_image, cropped_image, MORPH_OPEN, getStructuringElement(MORPH_RECT, Size(5, 5)));
    inRange(cropped_image, lowerb, upperb, cropped_image);
    Mat res = region_selection1(cropped_image);
    //imshow("yuan", img);
    //imshow("title", res);
    string filename = "./image/yellow/" + to_string(yellow_image_count++) + "yellow_" + ".jpg";
    imwrite(filename, res);
    int sum = 0;
    for (int i = rows * 0.9; i>= rows * 0.7; i--) {
        for (int j = 0; j < 320; j++) {
            if (res.at<uchar>(i, j) == 255) {
                sum++;
            }
        }
    }
    cout << sum << endl;
    if (sum > 1000) {
        if (num_yellow) {
            num_yellow++;
        } else {
            flag_yellow++;
        }
    } else {
        if (flag_yellow) {
            num_yellow++;
            flag_yellow = 0;
            Set_dian(10000);
        }
    }
    return res;

}
/*
    找黄色中线 返回黄线的坐标
*/
int Tracking_yellow(Mat image) {
    mid_yellow.clear();
    int begin = 160;
    int rows = image.rows;
    int cols = image.cols;
    for(int i = rows*0.7;i<=rows*0.9;i++){
        for(int j = 10;j<=310;j++){
            if(image.at<uchar>(i, j) == 255&&image.at<uchar>(i, j+1) == 255&&image.at<uchar>(i, j+3) == 255){
                //找到黄线了 列在前 行在后
                mid_yellow.push_back(cv::Point(j, i));
                break;
            }
        }
    }
    int size = mid_yellow.size()/2;
    if(size+5>mid_yellow.size()){
        size = mid_yellow.size();
    }
    int mid_line = 0;
    for(int i = mid_yellow.size()/2;i<size;i++){
        mid_line += mid_yellow[i].x;
    }
    mid_line/= (size - mid_yellow.size() / 2);

    return mid_line;

}