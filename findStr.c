#include "myheader.h"

unsigned int mystrlen(char *arr)
{
	unsigned int i;
	for (i=0; arr[i] != '\0'; i++);
	return i;
}

void showColor(char *str1, char *str2)
{
	char neat[1024];
//	int arr = 0;
	int i=0, j=0, now=0;
	int len[2] = {mystrlen(str1), mystrlen(str2)};

	if (len[1])
	{
		for (i=0; i < len[0]; i++)
		{
			if (!strncmp(&str1[i], str2, len[1]) && len[1])
			{
				if (!j)
				{
					if (i)
						strncpy(neat, &str1[now], i);
					neat[i] = '\0';
					printf("%s", neat);
				}
				strncpy(neat, &str1[i], len[1]);
				neat[len[1]] = '\0';
				printf("\033[43m%s\033[0m", neat);
				i += len[1] - 1;
				now = i;
				j = 1;
			} else if (j)
				putchar(str1[i]);
		}
		if (j)
			putchar('\n');
	} else 
		printf("%s\n", str1);
	/*
	// 一个一个字符的判断打印，遇到中文会乱码
	if (!strncmp(&str1[i], str2, len[1]))
	{
		for (j=0; j < len[0]; j++)
		{
			if (j == now && now < i+len[1])
			{
				printf("\033[43m");
				now++;
				putchar(str1[j]);
				printf("\033[0m");
			} 
			else if (now == i+len[1] && now)
			{
				arr = showColor(&str1[j], str2, 1);
				j = len[0] - arr;
			} else
				putchar(str1[j]);
		}
		if (!arr && !ifd)
			putchar('\n');
	} 
	else if (ifd)
			printf("%s",str1);

	return arr;
	*/
}

void find(char *(strs[]))
{
	int shift = 0; // 输出偏移度
	char ch, *chs, *ty;
	unsigned int len = 0;  // 字符串的长度
	unsigned int seat = 0; // 光标在字符串的位置

	chs = (char *)malloc(1025);
	// 清屏，并将光标放到第一行第一列
	printf("\033[2J\033[1;1H\033[s");
	// 按下ESC结束
	while((ch=getch()) != 27)
	{
		// 清屏，并将光标放到第一行第一列
		printf("\033[2J\033[1;1H");
		if (ch == 127 && len && seat)
		{
			int dsize = 1;
			for (int i=seat-1; chs[i] != '\0'; i++)
			{
				if (i-2 >= 0 && chs[i-2] >= 200)
				{
					chs[i-2] = chs[i+1];
					len -= 2, seat -= 2;
					dsize += 2, i -= 2, shift += 1;
				} else
					chs[i] = chs[i+dsize];
			}
			len--, seat--;
		} else if (ch != 127)
		{
			chs[seat] = ch;
			seat++, len++;
			if (ch >= 200)
			{
				shift -= 1;
			}
		}

		fprintf(stdout, "%s\n\n", chs);

		for (int i=0; strs[i] != 0; i++)
		{
			if (mystrlen(strs[i]) < mystrlen(chs))
			{
				continue;
			}
			showColor(strs[i], chs);
		}

		printf("\033[s\033[1;%dH", mystrlen(chs)+1+shift);
	}

	printf("\033[u\n");

	free(chs);
}

int main(int argc, char *argv[])
{
	char *strs[1024] = {"sbd_ybf", "xguo0ib", "fcgh_cfyh", "abcdefg", "ggshhfg", "固定的工作。"};

	find(strs);

	return 0;
}
