CC = gcc
CPP = g++
#put your object files here
OBJS = main.o SPImageProc.o SPPoint.o Extraction.o KDArray.o KDTree.o Query.o SPBPriorityQueue.o SPConfig.o SPList.o SPListElement.o SPLogger.o
#The executabel filename
EXEC = SPCBIR
INCLUDEPATH=/usr/local/lib/opencv-3.1.0/include/
LIBPATH=/usr/local/lib/opencv-3.1.0/lib/
LIBS=-lopencv_xfeatures2d -lopencv_features2d \
-lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_core

CPP_COMP_FLAG = -std=c++11 -Wall -Wextra


C_COMP_FLAG = -std=c99 -Wall -Wextra

$(EXEC): $(OBJS)
	$(CPP) $(OBJS) -L$(LIBPATH) $(LIBS) -o $@
main.o: main.cpp SPConfig.h Debug.h Query.h KDTree.h Extraction.h SPLogger.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c $*.cpp

SPImageProc.o: SPImageProc.cpp SPImageProc.h SPConfig.h SPPoint.h SPLogger.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c $*.cpp

#a rule for building a simple c souorce file
#use gcc -MM SPPoint.c to see the dependencies
Extraction.o: Extraction.c Extraction.h SPPoint.h Debug.h
	$(CC) $(C_COMP_FLAG) -c $*.c

KDArray.o: KDArray.c KDArray.h Debug.h
	$(CC) $(C_COMP_FLAG) -c $*.c

KDTree.o: KDTree.c KDTree.h KDArray.h SPPoint.h SPConfig.h SPBPriorityQueue.h Debug.h
	$(CC) $(C_COMP_FLAG) -c $*.c

Query.o: Query.c Query.h Debug.h
	$(CC) $(C_COMP_FLAG) -c $*.c

SPBPriorityQueue.o: SPBPriorityQueue.c SPBPriorityQueue.h SPListElement.h SPList.h
	$(CC) $(C_COMP_FLAG) -c $*.c

SPConfig.o: SPConfig.c SPConfig.h Debug.h
	$(CC) $(C_COMP_FLAG) -c $*.c

SPList.o: SPList.c SPList.h
	$(CC) $(C_COMP_FLAG) -c $*.c

SPListElement.o: SPListElement.c SPListElement.h
	$(CC) $(C_COMP_FLAG) -c $*.c

SPLogger.o: SPLogger.c SPLogger.h Debug.h
	$(CC) $(C_COMP_FLAG) -c $*.c


SPPoint.o: SPPoint.c SPPoint.h
	$(CC) $(C_COMP_FLAG) -c $*.c

clean:
	rm -f $(OBJS) $(EXEC)
