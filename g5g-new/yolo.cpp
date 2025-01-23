#include "g5g.h"
#include <cstdlib>
int yoloAB(){
    
    //setenv("PATH", "/usr/local/bin:/usr/bin:/bin", 1);  // 设置正确的路径
    system("/usr/local/bin/python3 /home/pi/g5g-new/yolo.py > res.txt");
    // 打开temp.txt文件以读取输出
    std::ifstream file("res.txt");
    if (file.is_open()) {
        std::string line;
        while (getline(file, line)) {
            // 打印每一行输出
            return std::stoi(line);
        }
        file.close();
    } else {
        std::cerr << "无法打开文件" << std::endl;
    }
    return -1;
}
int yoloLR(){
    
    //setenv("PATH", "/usr/local/bin:/usr/bin:/bin", 1);  // 设置正确的路径
    system("/usr/local/bin/python3 /home/pi/g5g-new/yoloLR.py > lr.txt");
    // 打开temp.txt文件以读取输出
    std::ifstream file("lr.txt");
    if (file.is_open()) {
        std::string line;
        while (getline(file, line)) {
            // 打印每一行输出
            return std::stoi(line);
        }
        file.close();
    } else {
        std::cerr << "无法打开文件" << std::endl;
    }
    return -1;
}
///home/pi/.local/lib/python3.7/site-packages/onnxruntime/__init__.py