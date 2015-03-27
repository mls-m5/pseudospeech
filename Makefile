COMPILER=g++

OBJS=main.o soundengine.o buffer.o

LIBS= -lpthread -lportaudio -lsndfile
CFLAGS=-g -std=c++11 -O0
TARGET=speech

all: $(TARGET) headercreator

#Calculating dependincies
.depend: $(wildcard ./*.cpp ./*.h) Makefile
	$(CXX) $(CFLAGS) -MM *.cpp > ./.dependtmp
	cat ./.dependtmp | sed 's/h$$/h \n\t \$(CXX) -c $$(CFLAGS) $$< -o $$@/' > ./.depend
	rm ./.dependtmp

${TARGET}: .depend $(OBJS) #cleancpp
	${COMPILER} ${FLAGS} -o ${TARGET} ${PROGOBJECS} $(OBJS) ${LIBS}

headercreator: headercreator.cpp
	g++ headercreator.cpp -o headercreator -lsndfile -g -std=c++11
	
include .depend

clean:
	rm -f $(LIB)
	rm *.o
	rm .depend

rebuild: clean ${TARGET}
