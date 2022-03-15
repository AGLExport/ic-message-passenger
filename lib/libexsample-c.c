//サンプルライブラリ
#include <stdio.h>

void call_exsample_c_lib_lc(int a);

void call_exsample_c_lib(int a)
{
	printf("hoge %d\n",a);
	call_exsample_c_lib_lc(100);
}


void call_exsample_c_lib_lc(int a)
{
	printf("hoge %d\n",a);
}

