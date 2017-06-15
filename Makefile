#################################################################
#�� �� ����
#        Makefile
#���뷽����
#     A��release�汾��make
#        ��           make clean;make
#        ��           make clean;make -f Makefile
#     B��debug�汾��  make mode=debug
#        ��           make clean;make mode=debug
#        ��           make clean;make mode=debug -f Makefile
#ע�����
#        ����ʹ��TAB��,����ʹ�ÿո�
#################################################################

#����·�������²��������޸ġ�
BASE=$(PWD)

#����·�������²��������޸ġ�
BASE_PATH=$(BASE)/../..

#obj�ļ�����·�������²��ֹ̻��������޸ġ�
OBJ_PATH=$(BASE)/obj

#LIB·�������²��ֹ̻��������޸ġ�
LIB_PATH=$(BASE)/lib

#=====================Begin:��Ҫ�޸ĵĲ���==================
#Դ�ļ�·��
SRC_MODULE=$(BASE)/src

#ͷ�ļ�·��
BASE_INCLUDE=$(BASE_PATH)/include
INC_MODULE=include

#��Ҫ���������Դ�ļ�·��
VPATH=$(SRC_MODULE):$(OBJ_PATH):$(LIB_PATH)

#��Ҫ���������ͷ�ļ�·��
INCLUDES=-I$(INC_MODULE)

#��Ҫ����Ŀ��ļ���·��
LIB_ALL=-L$(LIB_PATH) \
	-ltinyxml \
	-lpthread \
	-llog4cplus

#��Ҫ���ɵĳ�����
BIN_FILE=se-mrf_FTP

#=====================End:��Ҫ�޸ĵĲ���====================

#��Ҫ�����Դ�ļ������²��������޸ġ�
C_SRC_FILE=$(notdir $(wildcard $(SRC_MODULE)/*.c))
CPP_SRC_FILE=$(notdir $(wildcard $(SRC_MODULE)/*.cpp))

#��Ҫ���ɵ�obj�ļ������²��������޸ġ�
OBJ=$(CPP_SRC_FILE:%.cpp=%.o)
OBJ+=$(C_SRC_FILE:%.c=%.o)


#�����²��������޸ġ�
ifeq ($(mode),debug)
	CXX=g++ -ggdb3 
	CC=gcc -ggdb3 
else
	CXX=g++
	CC=gcc 
endif

#�����²��������޸ġ�
FLAG=-fexceptions 

#�����²��������޸ġ�
build:$(BIN_FILE)

#�����²��������޸ġ�
$(BIN_FILE):$(OBJ)
	@cd $(OBJ_PATH); $(CXX) $(LIB_ALL) $(OBJ) -o $(BASE)/$(BIN_FILE)


#�����²��������޸ġ�
%.o:%.cpp
	@if [ ! -d $(OBJ_PATH) ]; then mkdir -p $(OBJ_PATH); fi;
	@echo Compile file $<,`more $<|wc -l` lines ...
	@$(CXX) -c $(INCLUDES) $(FLAG) $< -o $(OBJ_PATH)/$@
	
%.o:%.c
	@if [ ! -d $(OBJ_PATH) ]; then mkdir -p $(OBJ_PATH); fi;
	@echo Compile file $<,`more $<|wc -l` lines ...
	@$(CC) -c $(INCLUDES) $(FLAG) $< -o $(OBJ_PATH)/$@

#�����²��������޸ġ�
.PHONY:clean

clean:
	@echo Delete $(OBJ) $(BIN_FILE)
	@rm -f $(BIN_FILE); cd $(OBJ_PATH); rm -f $(OBJ)
