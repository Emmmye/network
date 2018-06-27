ROOT=$(shell pwd)
INCLUDE=-I$(ROOT)/../../lib/include
LIB=-L$(ROOT)/../../lib/lib
LIB_NAME=-lmysqlclient


.PHONY:all
all:cgi_cal insertData selectData
cgi_cal:cgi_cal.c
	gcc -o $@ -g $^
insertData:insertData.c 
	gcc -o $@ -g $^ $(INCLUDE) $(LIB) $(LIB_NAME)
selectData:selectData.c
	gcc -o $@ -g $^ $(INCLUDE) $(LIB) $(LIB_NAME)
.PHONY:clean
clean:
	rm -f cgi_cal insertData selectData
