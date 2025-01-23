#include "g5g.h"

// 打开摄像头（参数2表示第二个摄像头，可以改为视频文件路径）
VideoCapture cap(2);//-1 0 1 2
// 初始化帧计数器
int frame_count = 0;

int find_blue_card=0;//蓝色挡板标志->0：未找到蓝色挡板
int blue_card_remove_flag=0;//蓝色挡板移开标志->0：未移开蓝色挡板
int hasBanma=1;//有无斑马线标志
int Banma_finish=1;//斑马线结束标志


double error1 = 0;
double min_angle = 0;
double max_angle =200;
//红色锥桶
int red_nums =0;
double res_y = 240.0;
int pass_nums =0;

int LandR = -1;
int changeDao_finish =0;//左右变道结束标志->测试结果：不太稳定
int red_cone_finish=0; //红色锥桶结束标志
int AB_finish=0;//ab 结束标志
int AandB =-1;
int AB_changeDao_finsih=0;//ab后入库要有一个延迟

int LandRpass=0; 

int yellow_finish=0;

//左右中线
std::vector<cv::Point> left_line;
std::vector<cv::Point> right_line;
std::vector<cv::Point> mid;
double mid_final = 160;//中线

// 全局变量，用于控制循环
bool running = true;
// 信号处理函数
void signalHandler(int signum) {
    cout << "Interrupt signal (" << signum << ") received.\n";
    
    running = false; // 设置运行标志为false，以退出循环
}

string str = "sudo cp /home/pi/.Xauthority /root/";
int flag = system(str.c_str());

int imageCount=0;
bool check_fps = 0;
int Tracking(Mat dilated_image, Mat frame) {
    int begin = 160;
    left_line.clear();
    right_line.clear();
    mid.clear();
    int rows = dilated_image.rows;
    int cols = dilated_image.cols;
    for (int i = 239; i >= rows*0.5; i--) {
        int find_l = 0;
        int find_r = 0;
        int to_left = begin;
        int to_right = begin;
        while (to_left != 1) {
            if (dilated_image.at<uchar>(i, to_left) == 255 && dilated_image.at<uchar>(i, to_left + 1) == 255) {
                find_l = 1;
                left_line.push_back(cv::Point(to_left, i));
                break;
            } else {
                to_left--;
            }
        }
        if (to_left == 1) {
            left_line.push_back(cv::Point(1, i));
        }
        while (to_right != 318) {
            if (dilated_image.at<uchar>(i, to_right) == 255 && dilated_image.at<uchar>(i, to_right - 2) == 255) {
                find_r = 1;
                right_line.push_back(cv::Point(to_right, i));
                break;
            } else {
                to_right++;
            }
        }
        if (to_right == 318) {
            right_line.push_back(cv::Point(318, i));
        }
        cv::Point midx1 = left_line.back();
        cv::Point midx2 = right_line.back();
        mid.push_back(cv::Point(int((midx1.x + midx2.x) / 2), i));
        begin = (to_right + to_left) / 2;
        if (to_left == 1 && to_right == 318) mid.pop_back();
    }

    if (mid.size() == 0) {
        return 160;//默认
    }
    int size = mid.size() / 2;
    if (size + 5 > mid.size()) size = mid.size();
    else size = size + 5;
    mid_final= 0;
    for (int i = mid.size() / 2; i < size; i++) {
        mid_final += mid[i].x;
        cout << "mid: " << mid[i] << "    ";
        cv::circle(frame, mid[i], 2, cv::Scalar(0, 0, 255), -1);
    }
    for (int i = 0; i < left_line.size(); i++) {
        cv::circle(frame, left_line[i], 2, cv::Scalar(0, 255, 0), -1);
    }
    for (int i = 0; i < right_line.size(); i++) {
        cv::circle(frame, right_line[i], 2, cv::Scalar(0, 255, 0), -1);
    }
    imwrite("./image/mid/"+to_string(imageCount++)+".jpg",frame);
    mid_final /= (size - mid.size() / 2);
    return mid_final;


}

void solve() {
    //第一步：初始化python环境
    
    int red_position=-1;  //红色锥桶坐标 

    double angle;

    double fps = 0.0;
    int frameCount = 0;
    int startTime = static_cast<int>(getTickCount());

    high_resolution_clock::time_point time1,time2;

    Mat frame,frame_with_line ;
    while (running&&yellow_finish==0) {
        
        cap >> frame;
        if (frame.empty()) {
            cout<<"帧读取错误\n";
            break;
        }
        //imshow("frame", frame);
        if(check_fps==1&&find_blue_card==0){//一开始先找蓝色挡板
            cout<<"找蓝色挡板开启\n";
            //cap.set(CAP_PROP_FPS, 10);

            //blue_card_find(frame);//找挡板
            if(detectBlueAreas(frame)>60000)find_blue_card=1;
        }
        if(blue_card_remove_flag==0&&find_blue_card){//找到了蓝色挡板，但是还未移除
            cout<<"找到了蓝色挡板，但是还未移除\n";
           // cap.set(CAP_PROP_FPS, 10);
            if(detectBlueAreas(frame)<10000)blue_card_remove_flag=1;
            //blue_card_remove(frame);//检查是否移除
            if(blue_card_remove_flag){//减缓帧堆积
                check_fps = 0;
                frameCount++;
                std::this_thread::sleep_for(std::chrono::seconds(3));
                continue;
            }
        }
        if(blue_card_remove_flag&&hasBanma == 0){//蓝色挡板移除，可以开始运动
            if (check_fps) Set_dian(11000);
            cout<<"蓝色挡板移除\n";
            cout<<"正在找斑马线\n";

            hasBanma = crossroad(frame, true);//过曝原因可能是处理不够快,帧堆积了
        }
        
        if(hasBanma&&Banma_finish==0){//找到斑马线，控制小车停下，播语音
                Banma_finish = 1;//标记为结束
                cout<<"找到斑马线\n";
                Set_dian(10000);
                // 让主程序休眠2秒
                std::this_thread::sleep_for(std::chrono::seconds(2));
                //确认停下后
                cout<<"播报语音\n";
                //yuyin();
                //停止五秒
                // 让主程序休眠3秒
                std::this_thread::sleep_for(std::chrono::seconds(2));
                cout<<"斑马线部分结束\n";
                Set_dian(10800);
               // break;
        }
            
        if(Banma_finish&&LandR==-1&&changeDao_finish==0){
            cout<<"斑马线结束，正在准备变道\n";
            Set_dian(10800);
            if(find_red_cone(frame,false,false)!=-1){
                cout<<"检测到红色 yolo 开始 检测左右变道\n";
                usleep(500000);
                Set_dian(10000);
                LandR = yoloLR();
                cout<<"左右结果为："<<LandR<<"\n";
                LandR = 3;//默认左变道
                if(LandR!=-1){
                    //changeDao_finish =1;//标记为变道完成
                    if(LandR==3){
                        Set_dian(10800);
                        Set_duo(180);
                        usleep(2300000);
                        //Set_duo(170);
                        cout<<"变道结束\n";
                    }else{
                        Set_dian(10800);
                        Set_duo(170);
                        usleep(2300000);
                        cout<<"变道结束\n";
                    }
                }
            }
            
            
        }
        if(changeDao_finish==0&&LandR!=-1){
            LandRpass++;
            if(LandRpass>=40){
                changeDao_finish=1;//防止把左右变道识别为红锥桶
            }
        }
        if(changeDao_finish&&red_cone_finish==0){
            cout<<"开始红色锥桶识别\n";
            Set_dian(11000);
            red_position = find_red_cone(frame,false,true);
            if(red_nums>=3){
                pass_nums++;
                if(pass_nums>80){
                    red_cone_finish =1;
                    cout<<"红色锥桶识别结束\n";
                }
            }
            if(red_position!=-1){                       
                cout<<"找到红锥桶" << red_nums << "\n";
                if(red_nums%2==1){
                    error1 = 160 - (red_position-55);
                    cv::circle(frame, Point(red_position-53,120), 2, cv::Scalar(0, 0, 255), -1);
                }else{
                    error1 = 160 - (red_position+55);
                    cv::circle(frame, Point(red_position+53,120), 2, cv::Scalar(0, 0, 255), -1);
                }
                    
                angle = PID(error1);
                angle = 156.5 + angle;
                cout << "after angle: " << angle << endl;
                if (angle > max_angle) angle = max_angle;
                if (angle < min_angle) angle = min_angle;
                Set_duo(angle);
                frameCount++;
                continue;
            }

        }
        if(red_cone_finish&&AB_finish==0){//避障结束而AB 识别未结束
            cout<<"避障结束，开始ab识别\n";
            Set_dian(10800);
            if(find_red_cone(frame,false,false)!=-1){
                Set_dian(10000);
                usleep(500000);
                cout<<"有A B 红色出现，开启yolo\n";
                AandB = yoloAB();
                cout<<"AB识别结果为："<<AandB<<"\n";
                AandB = 2;//默认B
                if(AandB!=-1){
                    AB_finish = 1;
                    cout<<"AB 识别结束\n";
                }
            }
            
            
        }
        if(AB_finish){
            cout<<"AB 结束，开始找黄线\n";
            Set_dian(10800);
           
        
            Mat res = yellow_hsv(frame);
            //int mid = Tracking_yellow(res);
            if (num_yellow == 2) {
                yellow_finish = 1;
                cout << "停车!" << endl;
                gpioPWM(13, 10000);
                break;
            }
            
            //continue;
        }
 

        Mat edges = frame_processorByHF(frame, frameCount);
        mid_final = Tracking(edges, frame);

        cout<<"中线为：:"<<mid_final<<"\n";
        if(AB_finish){//大概2秒？
            //error1 = 240 - mid_final;
            cout<<"正在转向入库\n";//->不会
            error1 = 160 - (mid_final + 40);
            //AB_changeDao_finsih ++;
        }else{
            error1 = 160 - mid_final;
        }
        
        angle = PID(error1);
        angle = 156.5 + angle;
        cout << "after angle: " << angle << endl;
        if (angle > max_angle) angle = max_angle;
        if (angle < min_angle) angle = min_angle;
        Set_duo(angle);
            
    
        cout<<"fps:"<<fps<<"\n";

        //计算并打印帧率
        frameCount++;
        int currentTime = static_cast<int>(getTickCount());
        double timeDiff = (currentTime - startTime) / getTickFrequency();
    
        if (timeDiff >= 1.0) {
                fps = frameCount / timeDiff;
                frameCount = 0;
                startTime = currentTime;
                if (fps >= 10) check_fps = true;
        }
        
        putText(frame, "FPS: " + std::to_string(fps), Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);


        
        if(waitKey(1)>=0){
            break;
        }
    }

    // 释放资源
    
    cap.release();
    destroyAllWindows();

   // return NULL;
}

int main() {
    //setenv("PATH", "/usr/local/bin:/usr/bin:/bin", 1);  // 设置正确的路径
    //初始化
    Set_gpio();
  
  // 检查摄像头是否打开成功
  if (!cap.isOpened()) {
      printf("Error: Could not open video source.");
      exit(1);
  }
    cap.set(CAP_PROP_FRAME_WIDTH, 320);
    cap.set(CAP_PROP_FRAME_HEIGHT, 240);
   // cap.set(CAP_PROP_FPS, 30);

     // 注册信号 SIGINT 和信号处理程序
    signal(SIGINT, signalHandler);

    // pthread_t tid;
    // pthread_create(&tid, NULL, solve, &frame_count);
    // pthread_join(tid, NULL);
    solve();

   
    
    return 0;
}

