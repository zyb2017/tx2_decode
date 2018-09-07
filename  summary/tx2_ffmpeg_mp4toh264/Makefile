VAR_INCLUDE := -I /usr/local/FFmpeg/include
VAR_SHARED_LIB_DIR = -L /usr/lib/x86_64-linux-gnu
VAR_SHARED_LIBS = -l avcodec -l avformat  -l avutil -l swresample -l swscale

main:main.o
	gcc  -g -o $@ $^ $(VAR_INCLUDE)  $(VAR_SHARED_LIB_DIR) $(VAR_SHARED_LIBS) 

%.o:%.cpp
	gcc  -g -c $<  $(VAR_INCLUDE)  $(VAR_SHARED_LIB_DIR) $(VAR_SHARED_LIBS)
clean:
	rm -rf *.o *.bin
