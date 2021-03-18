HELLO_WORLD =
TARGET = co
SUBDIR = 
CXXFLAGS= -ggdb -O0 -std=c++2a -Wall -fPIC -frtti -fexceptions -fcoroutines -W -Wall -Wno-deprecated -Wno-unused-parameter -Wno-unused-variable -fstack-protector-all

INCLUDE= -I./
LIBSDIR= 
LIBEXT= -lev
LIBS+= -lpthread -lstdc++ -lm
LIBS+= $(LIBEXT) $(LIBSDIR)

all: $(SUBDIR) lib

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
