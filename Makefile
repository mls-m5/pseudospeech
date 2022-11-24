COMPILER=g++

OBJS=src/main.o src/soundengine.o src/buffer.o src/speech.o src/wavfile.o

LIBS= -lpthread -lportaudio -lsndfile
CFLAGS=-g -std=c++11 -O0
TARGET=speech

all: $(TARGET) headercreator

#Calculating dependincies
.depend: $(wildcard src/*.cpp src/*.h) Makefile
	$(CXX) $(CFLAGS) -MM src/*.cpp > ./.dependtmp
	cat ./.dependtmp | sed 's/h$$/h \n\t \$(CXX) -c $$(CFLAGS) $$< -o $$@/' > ./.depend
	rm ./.dependtmp

${TARGET}: .depend $(OBJS) #cleancpp
	${COMPILER} ${FLAGS} -o ${TARGET} ${PROGOBJECS} $(OBJS) ${LIBS}

headercreator: src/headercreator.cpp
	g++ src/headercreator.cpp -o headercreator -lsndfile -g -std=c++11
	
include .depend

clean:
	rm -f $(LIB)
	rm src/*.o
	rm .depend

rebuild: clean ${TARGET}
