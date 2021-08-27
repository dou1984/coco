HELLO_WORLD =
TARGET = coco
SUBDIR = 
CXXFLAGS= -ggdb -O2 -std=c++2a -Wall -fPIC -frtti -fexceptions -W -Wall -Wno-deprecated -Wno-unused-parameter -Wno-unused-variable -fstack-protector-all -fcoroutines

INCLUDE= -I../libco
LIBSDIR= -L../libco
LIBEXT= -lcoco -lev
LIBS+= -lpthread -lstdc++ -lm
LIBS+= $(LIBEXT) $(LIBSDIR)

all: $(SUBDIR) exe

################################################
CC = gcc
CXX = g++
AR = ar

TARGET_LIB = lib$(TARGET).a

OBJECTS += $(patsubst %.cpp, %.o, $(wildcard *.cpp))
OBJECTS += $(patsubst %.cc, %.o, $(wildcard *.cc))
OBJECTS += $(patsubst %.c, %.o, $(wildcard *.c))

exe: $(OBJECTS) 
	$(CXX) -o $(TARGET) $(OBJECTS) $(LIBS) 

lib: $(OBJECTS)	
	$(AR) -r $(TARGET_LIB) $(OBJECTS)


$(SUBDIR): FLUSH
	make -C $@

FLUSH:
	echo "begin compiling"
	
clean: 
	$(RM) -rf $(TARGET) $(TARGET_LIB) 
	$(RM) $(OBJECTS)
	
.SUFFIXES: .cpp .cc .c
.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) -o $@ $<
	
.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) -o $@ $<
	
.c.o:
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) -o $@ $<
