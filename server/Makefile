#vpath %.h ../inc
#vpath %.c ../server
VPATH=../inc:../common

obj=server.o admin_operation.o threadpool.o common.o passwd.o client_operation.o

server:$(obj)
	cc -o server $(obj) -lmysqlclient -lpthread -g
server.o:server.c define.h threadpool.h admin_operation.h server.h
	cc -c server.c -lmysqlclient -g
threadpool.o:threadpool.c define.h threadpool.h
	cc -c threadpool.c -lpthread -g
admin_operation.o:admin_operation.c define.h admin_operation.h common.h
	cc -c admin_operation.c -lmysqlclient -g
common.o:common.c define.h passwd.h common.h
	cc -c ../common/common.c -g
passwd.o:passwd.c passwd.h
	cc -c ../common/passwd.c -g
client_operation.o:client_operation.c define.h common.h client_operation.h
	cc -c client_operation.c -g

.PHONY:clean
clean:
	rm $(obj) server
