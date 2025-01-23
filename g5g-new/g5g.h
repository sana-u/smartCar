//#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <stdio.h>
// #include <python3.7/Python.h>
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>
#include<opencv4/opencv2/core/core.hpp>
#include<opencv4/opencv2/imgproc/types_c.h>
#include <pthread.h>
#include <sys/stat.h>
#include <unistd.h>
#include <portaudio.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <cstring>
#include <chrono>
//#include <numpy/arrayobject.h>
#include <iostream>
#include<stdlib.h>
#include <thread>
#include <condition_variable>
#include <mutex>
#include "pigpio.h"
#include <cmath>
#include <csignal>


using namespace cv;
using namespace std;
using namespace std::chrono;


// add.h
#ifndef ADD_H
#define ADD_H
#define PI 3.1415926

extern cv::Mat frameToProcess;
extern int findab;
extern int blue_card_remove_flag;
extern int find_blue_card;
extern int hasBanma;
extern int Banma_finish;

extern int pass_frame;
extern int red_nums ;
extern double res_y ;

extern int num_yellow;
extern int flag_yellow;

//在这个里面放上声明的函数：
/*
    检测斑马线的函数
 */
int crossroad(Mat frame,int isVisual);

/*
    将图像转化为灰度图，消除噪声和进行边缘检测并返回图像
*/
Mat preprocess_frame(Mat frame);

/*
    截取感兴趣区域(roi),并将截取的区域进行返回
 */
Mat region_of_interest(Mat edges);

/*
    检测跑道
*/
Mat frame_processorByEZ(Mat image);//二值化
Mat frame_processorByHF(Mat image, int cnt);//霍夫

/*
    检测图像中的直线，并且返回检测到的点的集合
 */
vector<Vec4i> detect_lines(Mat cropped_edges);

/*
    在frame 上，将lines上的点用绿色画出来
 */
void draw_lines(Mat frame, vector<Vec4i>& lines);

/*
    语音模块
 */
struct WavHeader {
    char chunkID[4];
    uint32_t chunkSize;
    char format[4];
    char subchunk1ID[4];
    uint32_t subchunk1Size;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    char subchunk2ID[4];
    uint32_t subchunk2Size;
};

/*
    从指定的 WAV 文件中读取采样率
*/
uint32_t GetSampleRate(const char* file);

/*
    播放语言的函数
 */
bool yuyin();

/*
    yolo 目标检测 a,b,左右转
 */
int yoloAB();
int yoloLR();

//蓝色挡板
void blue_card_find(Mat frame);
void blue_card_remove(Mat frame);
Mat blue_card_process(Mat frame);
int detectBlueAreas(const cv::Mat inputImage);



// 红色锥桶
int find_red_cone(Mat img,int isVisual,int isCone);
Rect red(Mat img);

//计算面积
bool Contour_Area(vector<Point> contour1, vector<Point> contour2);

//黄色线：
Mat yellow_hsv(Mat img);
/*
    找黄色中线 返回黄线的坐标
*/
int Tracking_yellow(Mat iamge);

//驱动相关
void Set_gpio();
void Set_dian(int value);
void Set_duo(int angle);
double PID(double error1);

#endif // ADD_H
