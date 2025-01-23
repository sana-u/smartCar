CXX = g++
PYTHON_CFLAGS = $(shell python3.7-config --cflags)
PYTHON_LDFLAGS = $(shell python3.7-config --ldflags)
CXXFLAGS = `pkg-config --cflags opencv4` -Wall  $(PYTHON_CFLAGS)  
LDFLAGS = `pkg-config --libs opencv4` -lpigpio -lpthread -lopenal $(PYTHON_LDFLAGS) `python3.7-config --ldflags` -L/home/pi/.local/lib/python3.7/site-packages/numpy -lpython3.7
TARGET = line
SRCS = crossroad.cpp yuyin.cpp paodao.cpp line.cpp bule_card.cpp red_cone.cpp yellow_edges.cpp qudong.cpp yolo.cpp
OBJS = $(SRCS:.cpp=.o)
HEADERS = g5g.h


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

