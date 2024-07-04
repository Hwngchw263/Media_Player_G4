#Name project
NAME := Myapp

#Output path
PATH_OUTPUT := ./Output
#Compiler
CC = g++
#Flag
CCFLAGS = -c -std=c++17 -I /usr/include/taglib
LDFLAGS = -ltag -lSDL2 -lSDL2_mixer -lpthread -lavformat -lavcodec -lavutil -pthread
#Library
#Include files
INC_DIRS += App/Model\
			App/Controller\
			App/View					
#Source files
SRC_DIRS += App/CLI\
			App/Model\
			App/Controller\
			App/View

#All include file and source file in project
SRC_FILES += $(foreach SRC_DIRS,$(SRC_DIRS),$(wildcard $(SRC_DIRS)/*.cpp))

#Object files
OBJ_FILES := $(notdir $(SRC_FILES))
OBJ_FILES := $(subst .cpp,.o,$(OBJ_FILES))
PATH_OBJ := $(foreach OBJ_FILES,$(OBJ_FILES),$(PATH_OUTPUT)/$(OBJ_FILES))

#CC options for INC_DIRS
INC_DIRS_OPT += $(foreach INC_DIRS,$(INC_DIRS),-I $(INC_DIRS))

vpath %.cpp $(SRC_DIRS)
build:$(NAME).exe

#Make object file from source file
%.o:%.cpp
	@$(CC) $(INC_DIRS_OPT) $(CCFLAGS) $^ -o $(PATH_OUTPUT)/$@
	
#Linking object files
$(NAME).exe:$(OBJ_FILES)
	@$(CC) $(PATH_OBJ) -o $(PATH_OUTPUT)/$@ $(LDFLAGS)
	
run:
	@$(PATH_OUTPUT)/$(NAME).exe

clean:
	rm -rf ./Output/*
	
print-%:
	@echo $($(subst print-,,$@))
	