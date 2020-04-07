TARGET_DIR := $(shell pwd)/bin
TARGET := Server

CC := g++

 
src :=TcpServer.cpp TcpServer.hpp TcpServer.h 

######################################################
#arguments of compile
#
######################################################

CC_FLAG= -g

######################################################

BASE_DIR=$(shell pwd)
OBJ_DIR=$(shell pwd)/build

OBJS = TcpServer.o

#CPP_FILES := $(wildcard *.cpp)
#OBJ_FILES := $(addprefix $(OBJ_DIR)/,$(notdir $(CPP_FILES:.cpp=.o)))

$(TARGET_DIR)/TARGET:$(OBJ_DIR)/$(OBJS)
	@ if [ ! -d $(TARGET_DIR) ]; then mkdir -p $(TARGET_DIR); fi;\
	$(CC) $(OBJ_DIR)/$(OBJS) -o $(TARGET_DIR)/$(TARGET)


#$(OBJ_FILES):$(CPP_FILES)
$(OBJ_DIR)/%.o:$(src)
	@ if [ ! -d $(OBJ_DIR) ]; then mkdir -p $(OBJ_DIR); fi;\
	$(CC) -c -o $@ $<   


.PHONY:clean
clean:
	@rm -rf $(OBJ_DIR)/* $(TARGET_DIR)/* 
