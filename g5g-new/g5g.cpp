#include "g5g.h"
/*
bool yoloProcessed = false;
cv::Mat yoloCv;
std::condition_variable cv_condition;
std::mutex mtx;
bool newFrameAvailable = false;*/
cv::Mat frameToProcess;
int findab;
int find_blue_card;//找蓝色挡板的标志
int blue_card_remove_flag;//蓝色挡板移除标志
int hasBanma;//有无斑马线标志
int Banma_finish;//斑马线结束标志

int red_nums ;
double res_y ;

int num_yellow;
int flag_yellow ;