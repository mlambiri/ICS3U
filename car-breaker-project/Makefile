CCFLAGS =	-g -I/usr/local/include -DPONGDEBUG
CC = g++

OBJS =		car-breaker.o

LIBS = -g -L/usr/local/lib -lallegro -lallegro_main -lallegro_image -lallegro_primitives -lallegro_font -lallegro_ttf -lallegro_audio -lallegro_acodec


TARGET =	car-breaker

$(OBJS) : car-breaker-main.cpp car-breaker-graphics.cpp config-manager.cpp car-breaker-graphics.h car-breaker.cpp write-results.cpp
	$(CC) -c $(CCFLAGS) -o car-breaker.o car-breaker.cpp

$(TARGET):	$(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
