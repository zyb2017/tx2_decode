CC = g++
CFLAGS  := -Wall -O3 -std=c++0x 

OPENCV_INC_ROOT = /usr/local/include/opencv 
OPENCV_LIB_ROOT = /usr/local/lib
OPENCV_INC= -I $(OPENCV_INC_ROOT)
MY_INC =-I/usr/include -I/usr/include/gstreamer-1.0  -I/usr/lib/aarch64-linux-gnu/gstreamer-1.0 -I/usr/lib/aarch64-linux-gnu/gstreamer-1.0/include -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/lib/aarch64-linux-gnu/glib-2.0 -I/usr/lib/aarch64-linux-gnu/glib-2.0/include

EXT_INC = $(OPENCV_INC) $(MY_INC)

OPENCV_LIB_PATH = -L $(OPENCV_LIB_ROOT)
MY_LIB_PATH =-L/usr/lib -L/usr/lib/glib-2.0/include -L/usr/lib/x86_64-linux-gnu -lgio-2.0 -lglib-2.0 -lgobject-2.0 -lgmodule-2.0 -lgstreamer-1.0 -lgstbase-1.0 -lgstapp-1.0 -lgstnet-1.0

EXT_LIB = $(OPENCV_LIB_PATH) $(MY_LIB_PATH) 

OPENCV_LIB_NAME = -lopencv_highgui -lopencv_imgproc -lopencv_core 
#MY_LIB_NAME = -lcmoonergy
#$(MY_LIB_NAME)
all:test

test:main.cpp
	$(CC) -g $(CFLAGS) main.cpp $(EXT_INC) $(EXT_LIB) $(OPENCV_LIB_NAME) -o test
clean:
	rm -rf test
