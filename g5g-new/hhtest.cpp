#include <cstdlib>
#include <iostream>
int main() {
    const char* path = std::getenv("PATH");
    if (path != nullptr) {
        std::cout << "PATH: " << path << std::endl;
    } else {
        std::cout << "PATH environment variable not found." << std::endl;
    }

    // 获取并打印 HOME 环境变量
    const char* home = std::getenv("HOME");
    if (home != nullptr) {
        std::cout << "HOME: " << home << std::endl;
    } else {
        std::cout << "HOME environment variable not found." << std::endl;
    }

    // 获取并打印 PYTHONPATH 环境变量
    const char* pythonPath = std::getenv("PYTHONPATH");
    if (pythonPath != nullptr) {
        std::cout << "PYTHONPATH: " << pythonPath << std::endl;
    } else {
        std::cout << "PYTHONPATH environment variable not found." << std::endl;
    }
    system("/usr/bin/python /home/pi/g5g-new/yolo.py > res.txt");
    return 0;
}
