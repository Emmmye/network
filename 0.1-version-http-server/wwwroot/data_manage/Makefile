ROOT=$(shell pwd)
INCLUDE=-I$(ROOT)/../../lib/include
LIB=-L$(ROOT)/../../lib/lib
LIB_NAME=-lmysqlclient


.PHONY:all
all:  login upload select
login:login.c
	gcc -o $@ -g $^ $(INCLUDE) $(LIB) $(LIB_NAME)
upload:upload.c
	gcc -o $@ -g $^ $(INCLUDE) $(LIB) $(LIB_NAME)
select:select.c
	gcc -o $@ -g $^ $(INCLUDE) $(LIB) $(LIB_NAME)
.PHONY:clean
clean:
	rm -f  login upload select
