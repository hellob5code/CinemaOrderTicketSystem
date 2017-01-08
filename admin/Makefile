#vpath %.h ../inc
#vpath %.c ../common
#vpath %.c ../admin
VPATH=../inc:../common

obj=admin.o common.o passwd.o

admin:$(obj)
	cc -o admin $(obj) -lpthread -lmysqlclient
admin.o:admin.c admin.h define.h common.h
	gcc -c admin.c
common.o:common.c define.h passwd.h common.h
	gcc -c ../common/common.c 
passwd.o:passwd.c passwd.h
	gcc -c ../common/passwd.c 

.PHONY:clean
clean:
	rm $(obj) admin
