#vpath %.h ../inc
#vpath %.c ../common
#vpath %.c ../client
VPATH=../inc:../common

obj=client.o common.o passwd.o

client:$(obj)
	cc -o client $(obj) -lpthread -lmysqlclient -g
client.o:client.c client.h define.h common.h
	gcc -c client.c -g
common.o:common.c define.h passwd.h common.h
	gcc -c ../common/common.c -g
passwd.o:passwd.c passwd.h
	gcc -c ../common/passwd.c -g

.PHONY:clean
clean:
	rm $(obj) client
