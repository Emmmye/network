ROOT=$(shell pwd)
INCLUDE=-I$(ROOT)/../../lib/include
LIB=-L$(ROOT)/../../lib/lib
LIB_NAME=-lmysqlclient


.PHONY:all
all: regist login adjust upload
regist:regist.c
	gcc -o $@ -g $^ $(INCLUDE) $(LIB) $(LIB_NAME)
login:login.c
	gcc -o $@ -g $^ $(INCLUDE) $(LIB) $(LIB_NAME)
adjust:adjust.c
	gcc -o $@ -g $^ $(INCLUDE) $(LIB) $(LIB_NAME)
upload:upload.c
	gcc -o $@ -g $^ $(INCLUDE) $(LIB) $(LIB_NAME)
.PHONY:clean
clean:
	rm -f regist adjust login upload
