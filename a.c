#include <stdio.h>
#include <stdlib.h>
#include <termio.h>

/*
 * 该函数用于字符串中提取（多个）数字（支持多位数、正负数识别）
 * 数字之间的间隔符不限符号
 * （目前是将回车作为结束，如果想去掉或更换请在第10行换条件）
 * 该函数需要附属函数（tonum_2）
 *
 * 参数：
 * *s ：要提取的字符串
 * *v ：用于储存数字的数组
 * len ：字符串长度（或者有数字部分到开头的长度/）
 *
 * 返回值：
 * 该函数没有返回值。
 * */
void tonum(char *s, int *v, int len)
{
	int tonum_2(int i);
	int j=0, i = 1, numi=0;

	while (1)
	{
		if (len == 0)
			break;
		if ((int)*s!=10 && *s!='\0')
		{
			while (1)
			{
				if ((int)*s>47 && (int)*s<58)
				{
					//判断数字前面是否带负号
					if ((int)*(s-1)==45)
						i=-1; //改变符号
					i*=10, j++;
				} else
				{
					//判断前一个字符是否是数字
					if (i != 1 && i != -1)
					{
						s -= j;  //回退指针
						//多位数组合
						for ( ; i != 1 && i != -1; i /= 10)
						{
							numi += tonum_2((int)*s)*(i/10);
							s++;
						}
						*v = numi, v++;
						numi = 0, j=0, i = 1;
					}
					break;
				}
				s++;
			}
		} else 
			break;
		s++, len--;
	}
}

/*
 * 根据ASCII表数字编码对应的数值返回对应数字
 * */
int tonum_2(int j)
{
	int i=48, num=0;
	if (j > 47 && j < 58)
	{
		while (1)
		{
			if (j == i)
				return num;
			i++, num++;
		}
	} else 
		return 0;
}

/* 将整形数字转为字符串
 * */
void to_str(int value, char *string)
{
	char to_str_2(int i);

	int i, num_t, j;

	for (j=10; (int)value/j; j*=10);

	j /= 10;

	while (j > 0)
	{
		num_t = (int)value/j * j;
		*(string + 0) = to_str_2((int) value/j);
		value -= num_t;
		j /= 10, i++;
	}
}

char to_str_2(int i)
{
	int o = 0, k = 48;

	while (o < 10)
	{
		if (i == o)
		{
			return k;
		}
		o++, k++;
	}
	return 48;
}

/* 用于实现类似Windows下的getch()函数
 * 这个getch() 是最原始的输入模式，
 * 直接接收并返回不会在终端显示用户输入了什么。
 * */
char getch(void)
{
	struct termios tm, tm_old;
	int fd = 0, ch;

	if (tcgetattr(fd, &tm) < 0) //保存现在的终端设置
		return -1;

	tm_old = tm;
	//更改终端设置为原始模式，该模式下所有的输入数据以字节为单位被处理
	cfmakeraw(&tm);
	if (tcsetattr(fd, TCSANOW, &tm) < 0) //设置上更改之后的设置
		return -1;

	ch = getchar();

	if (tcsetattr(fd, TCSANOW, &tm_old) < 0) //更改设置为最初的样子
		return -1;

	return ch;
}

/* 不同于getch()，这个函数在用户输入的时候会显示用户输入
 * */
char getsch(void)
{
	char ch;

	system("stty -icanon");

	ch = getchar();

	system("stty icanon");

	return ch;
}

char getnch(short ifc)
{
	switch (ifc)
	{
		case 0:
			return getch();
		case 1:
			return getsch();
		default :
			return 0;
	}
}
