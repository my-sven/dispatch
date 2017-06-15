#################################################################
#文 件 名：
#        Makefile
#编译方法：
#     A、release版本：make
#        或           make clean;make
#        或           make clean;make -f Makefile
#     B、debug版本：  make mode=debug
#        或           make clean;make mode=debug
#        或           make clean;make mode=debug -f Makefile
#注意事项：
#        行首使用TAB键,避免使用空格
#################################################################

#基本路径【以下部分请勿修改】
BASE=$(PWD)

#基本路径【以下部分请勿修改】
BASE_PATH=$(BASE)/../..

#obj文件生成路径【以下部分固化后请勿修改】
OBJ_PATH=$(BASE)/obj

#LIB路径【以下部分固化后请勿修改】
LIB_PATH=$(BASE)/lib

#=====================Begin:需要修改的部分==================
#源文件路径
SRC_MODULE=$(BASE)/src

#头文件路径
BASE_INCLUDE=$(BASE_PATH)/include
INC_MODULE=include

#需要引入的所有源文件路径
VPATH=$(SRC_MODULE):$(OBJ_PATH):$(LIB_PATH)

#需要引入的所有头文件路径
INCLUDES=-I$(INC_MODULE)

#需要引入的库文件及路径
LIB_ALL=-L$(LIB_PATH) \
	-ltinyxml \
	-lpthread \
	-llog4cplus

#需要生成的程序名
BIN_FILE=se-mrf_FTP

#=====================End:需要修改的部分====================

#需要编译的源文件【以下部分请勿修改】
C_SRC_FILE=$(notdir $(wildcard $(SRC_MODULE)/*.c))
CPP_SRC_FILE=$(notdir $(wildcard $(SRC_MODULE)/*.cpp))

#需要生成的obj文件【以下部分请勿修改】
OBJ=$(CPP_SRC_FILE:%.cpp=%.o)
OBJ+=$(C_SRC_FILE:%.c=%.o)


#【以下部分请勿修改】
ifeq ($(mode),debug)
	CXX=g++ -ggdb3 
	CC=gcc -ggdb3 
else
	CXX=g++
	CC=gcc 
endif

#【以下部分请勿修改】
FLAG=-fexceptions 

#【以下部分请勿修改】
build:$(BIN_FILE)

#【以下部分请勿修改】
$(BIN_FILE):$(OBJ)
	@cd $(OBJ_PATH); $(CXX) $(LIB_ALL) $(OBJ) -o $(BASE)/$(BIN_FILE)


#【以下部分请勿修改】
%.o:%.cpp
	@if [ ! -d $(OBJ_PATH) ]; then mkdir -p $(OBJ_PATH); fi;
	@echo Compile file $<,`more $<|wc -l` lines ...
	@$(CXX) -c $(INCLUDES) $(FLAG) $< -o $(OBJ_PATH)/$@
	
%.o:%.c
	@if [ ! -d $(OBJ_PATH) ]; then mkdir -p $(OBJ_PATH); fi;
	@echo Compile file $<,`more $<|wc -l` lines ...
	@$(CC) -c $(INCLUDES) $(FLAG) $< -o $(OBJ_PATH)/$@

#【以下部分请勿修改】
.PHONY:clean

clean:
	@echo Delete $(OBJ) $(BIN_FILE)
	@rm -f $(BIN_FILE); cd $(OBJ_PATH); rm -f $(OBJ)
