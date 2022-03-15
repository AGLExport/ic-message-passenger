//サンプルライブラリ
#include <stdio.h>

void call_exsample_lib_local1(int a);
void call_exsample_lib_local2(int a);


void call_exsample_lib(int a)
{
	printf("hoge %d\n",a);
	
	call_exsample_lib_local1(1);
	call_exsample_lib_local2(2);
}

void call_exsample_lib_local1(int a)
{
	printf("hoge local 1%d\n",a);

	
}
void call_exsample_lib_local2(int a)
{
	printf("hoge local2 %d\n",a);

	
}


