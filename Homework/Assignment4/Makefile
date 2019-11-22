CCFLAGS =	-std=c++0x -g -I/usr/local/include -DPONGDEBUG
CC = g++

OBJS =		monarch.o

LIBS = -std=c++0x -g -L/usr/local/lib -lallegro -lallegro_main -lallegro_image -lallegro_primitives -lallegro_font -lallegro_ttf -lallegro_audio -lallegro_acodec -lallegro_dialog


TARGET =	monarch

$(OBJS) : monarch.cpp
	$(CC) -c $(CCFLAGS) -o monarch.o monarch.cpp

$(TARGET):	$(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
