#include"g5g.h"

//驱动相关参数
double kp = 0.25;
double ki = 0.00;
double kd = 0.125;
double last_error = 0;
double sum_error = 0;


void Set_gpio() {
	if (gpioInitialise() < 0) exit(1);
	gpioSetMode(13, PI_OUTPUT);
	gpioSetPWMrange(13, 40000);
	gpioSetPWMfrequency(13, 200);

    if (gpioInitialise() < 0) exit(1);
    gpioSetMode(12, PI_OUTPUT);
    gpioSetPWMrange(12,30000);
    gpioSetPWMfrequency(12,50);
    //�Ȼ���
    gpioPWM(13, 10000);
    Set_duo(156.5);
}

int last_dian = 10000;
void Set_dian(int value) {
    if (value > 10800) {
        for (int i = max(10800, last_dian); i <= value; i += 50) {
            gpioPWM(13, min(i, value));
            usleep(25000);
        }
    } else gpioPWM(13, value);
    last_dian = value;
}

double PID(double error1) {
     
    double angle = kp*error1  + kd*(error1-last_error);
    last_error = error1;
    return angle;
}
void Set_duo(int angle) {
	double value = (0.5 + (2 / 270.0) * angle) / 20 * 30000;//�ǶԵ�
	cout << "value: " << value << "\n";
	gpioPWM(12, value);
}
