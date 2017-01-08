#include "../inc/passwd.h"
int getpasswd(char *passwd,int num)
{
	//char passwd[20];
	int i;
	for(i=0;i<num;i++)
	{
		passwd[i] = getch();
		if(passwd[i] == 10)
			break;
		printf("*");
	}
	passwd[i] = '\0';

	//printf("\nwhat you input are %s\n",passwd);
	return i;
}

int getch()
{
	int c=0;
	struct termios org_opts,new_opts;
	int res = 0;

	res = tcgetattr(STDIN_FILENO,&org_opts);
	assert(res == 0);

	memcpy(&new_opts,&org_opts,sizeof(new_opts));
	new_opts.c_lflag &= ~(ICANON|ECHO|ECHOE|ECHOK|ECHONL|ECHOPRT|ECHOKE|ICRNL);
	tcsetattr(STDIN_FILENO,TCSANOW,&new_opts);
	c = getchar();
	res = tcsetattr(STDIN_FILENO,TCSANOW,&org_opts);
	assert(res == 0);
	return c;
}
