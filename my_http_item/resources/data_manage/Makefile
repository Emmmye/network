ROOT=$(shell pwd)
INCLUDE=-I$(ROOT)/../../lib/include
LIB=-L$(ROOT)/../../lib/lib
LIB_NAME=-lmysqlclient


.PHONY:all
all: regist select selectAll adjust
regist:regist.c
	gcc -o $@ -g $^ $(INCLUDE) $(LIB) $(LIB_NAME)
select:select.c
	gcc -o $@ -g $^ $(INCLUDE) $(LIB) $(LIB_NAME)
selectAll:selectAll.c
	gcc -o $@ -g $^ $(INCLUDE) $(LIB) $(LIB_NAME)
adjust:adjust.c
	gcc -o $@ -g $^ $(INCLUDE) $(LIB) $(LIB_NAME)
.PHONY:clean
clean:
	rm -f regist  select adjust selectAll
