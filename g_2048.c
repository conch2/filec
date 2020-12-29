/*
 * 这是一个终端版 2048 （完美在termux上运行）
 * 目前只在手机termux下进行测试和调试，所以其他系统可能不能完美运行。
 *
 * 在无聊的时候逛TapTap发现了2048这个游戏，以前我也玩过这游戏。这游戏还挺有意思的，
 * 就下来玩，后来玩着玩着就想到能不能自己做个玩玩，
 * 后来就抱着试试的态度做出来了（没写注释的原因AQA，当然还有部分是我懒哈哈），
 * 这个游戏还是有一些可玩性的，无聊的时候可以玩玩。
 * 代码有点长但我觉得至少还可以优化150行代码，来日方长，不知道还会不会来优化
 *
 * 开工时间：2020年10月22日
 * 竣工时间：2020年10月26日
 * 作者：
 * QQ：2946859498
 *
 * 维护和优化：
 * 2020年10月27日优化了游戏界面，使界面更简洁，并且添加了数字颜色显示使界面更优美
 *
 * BUG：
 * 在游戏界面时如果直接回车刷新了界面就会出现后续输入一个字符时程序无反应
 * 暂时解决方法：输入多个（两个）字符即可
 *
 * 缺点：
 * 没写注释！没写注释！没写注释！
 * 代码臃肿、阅读困难
 * */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int num[4][4]={{0}};
int void_or_true[16]={0}; //用于保存num中哪里有值哪里没有
char s;

void print_k();
void print_kt(int);
int create_num(int);
void restart_vot();
void rand_cn();
int judge_win();
int win();
int up();
void up_2(int, int *);
int down();
void down_2(int, int *);
int left();
void left_2(int, int *);
int right();
void right_2(int, int *);

void print_k()
{
    printf("\n\t\t\t开始游戏----2048\n\n\
\t/-----------------------------------------------\\\n");
	print_kt(0);
	printf("\t|-----------------------------------------------|\n");
	print_kt(1);
	printf("\t|-----------------------------------------------|\n");
	print_kt(2);
	printf("\t|-----------------------------------------------|\n");
	print_kt(3);
	printf("\t\\-----------------------------------------------/\n");

	printf("\n\t   W 向上  A 向左 S 向右 D 向下 T 退出\t");
}

void print_kt(int value)
{
	printf("\
\t|           |           |           |           |\n\
\t|           |           |           |           |\n\t");

	for (int i=0; i<4; i++)
	{
		if (!num[value][i])
		{
			printf("|           ");
		} else
			switch (num[value][i])
			{
				case 2:
					printf("|     \033[1;32m2\e[0m     ");
					break;

				case 4:
					printf("|     \033[1;34m4\e[0m     ");
					break;

				case 8:
					printf("|     \033[1;35m8\e[0m     ");
					break;

				case 16:
					printf("|    \033[1;33m16\e[0m     ");
					break;

				case 32:
					printf("|    \033[1;36m32\033[0m     ");
					break;

				case 64:
					printf("|    \033[1;31m64\033[0m     ");
					break;

				case 256:
					printf("|    \033[1;32m256\033[0m    ");
					break;

				case 512:
					printf("|    \033[1;35m512\033[0m    ");
					break;

				case 1024:
					printf("|   \033[1;31m1024\033[0m    ");
					break;

				case 2048:
					printf("|   \033[1;32m2\033[1;34m0\e[1;31m4\e[1;36m8\033[0m    ");
					break;

				default:
					printf("|    \e[1m%-4d\033[0m   ", num[value][i]);
					break;
			}
	}

	printf("|\n\
\t|           |           |           |           |\n\
\t|           |           |           |           |\n");
}

/*
 * 生成随机数(0-100)
 * 当t 为2时返回的是随机2或4
 * 当t 为16时返回的是1-16随机数
 * */
int create_num(int t)
{
	int v_num;
	v_num = (rand() % 100);

	if (t==2)
	{
		if (v_num % 2)
			return 2;
		else 
			return 4;
	} else if (t==16)
	{
		for (int i=1; i<7; i++)
		{
			if (v_num > (i-1)*16 && v_num <= i*16)
			{
				v_num -= (i-1)*16;
				break;
			} else 
			{
				i=1;
				do 
				{
					v_num = (rand() % 100);
				} while (v_num > 96);

			}
		}
		return v_num;
	} else
		return v_num;
}

/*
 * 重置void_or_true
 * */
void restart_vot()
{
	int i, j;
	for (i=0; i<4; i++)
	{
		for (j=0; j<4; j++)
		{
			if (num[i][j] == 0)
			{
				void_or_true[i*4+j] = 0;
			} else 
			{
				void_or_true[i*4+j] = 1;
			}
		}
	}
}

/*
 * 随机在空的地方生成数字
 * */
void rand_cn()
{
	int n = create_num(16);
	int i, j, y, r=1;
	while (r)
	{
		for (y=0; y<16; y++)
		{
			if (!void_or_true[y])
			{
				r = 1;
				break;
			} else
				r = 0;
		}

		if (void_or_true[n-1])
		{
			do 
			{
				n = create_num(16);
			} while(void_or_true[n-1] && r);
		} else
		{
			i = (int)(n / 4);
			j = (int)(n % 4);
			if (!j && i)
				i -=1, j = 3;
			else if (j!=0)
				j -= 1;
			num[i][j] = create_num(2);
			restart_vot();
			break;
		}
	}
}

int up()
{
	int i, j, t = 0;

	for (i=0; i<4; i++)
	{
		up_2(i, &t);

		for (j=0; j<3; j++)
		{
			if (num[j][i] && num[j][i] == num[j+1][i])
			{
				num[j][i] += num[j+1][i];
				num[j+1][i] = 0;
				restart_vot();
				t = 1;
			}
		}
		up_2(i, &t);
	}

	return t;
}

void up_2(int i, int *t)
{
	int j, v;

	for (j=0; j<13; j+=4)
	{
		v = j;

		while (!void_or_true[i+j] && (v + 4) < 13)
		{
			v += 4;
			if (void_or_true[i+v])
			{
				num[j/4][i] = num[v/4][i];
				num[v/4][i] = 0;
				restart_vot();
				*t = 1;
				break;
			}
		}
	}
}

int down()
{
	int i, j, t = 0;

	for (i=0; i<4; i++)
	{
		down_2(i, &t);

		for (j=3; j>0; j--)
		{
			if (num[j][i] && num[j][i] == num[j-1][i])
			{
				num[j][i] += num[j-1][i];
				num[j-1][i] = 0;
				restart_vot();
				t = 1;
			}
		}
		down_2(i, &t);
	}

	return t;
}

void down_2(int i, int *t)
{
	int j, v;

	for (j=12; j>0; j-=4)
	{
		v = j;

		while (!void_or_true[i+j] && (v-4)>=0)
		{
			v -= 4;
			if (void_or_true[i+v])
			{
				num[j/4][i] = num[v/4][i];
				num[v/4][i] = 0;
				restart_vot();
				*t = 1;
				break;
			}
		}
	}
}

int left()
{
	int i = 0, j, t = 0;

	do
	{
		left_2(i, &t);

		for (j = 0; j<3; j++)
		{
			if (num[i][j] && num[i][j] == num[i][j+1])
			{
				num[i][j] += num[i][j+1];
				num[i][j+1] = 0;
				restart_vot();
				t = 1;
			}
		}

		left_2(i, &t);

		i++;
	} while(i < 4);

	return t;
}

void left_2(int i, int *t)
{
	int j, v;

	for (j=i*4; j<i*4+3; j++)
	{
		v = j;

		while (!void_or_true[j] && v<i*4+3)
		{
			v++;
			if (void_or_true[v])
			{
				num[i][j-i*4] = num[i][v-i*4];
				num[i][v-i*4] = 0;
				restart_vot();
				*t = 1;
				break;
			}
		}
	}
}

int right()
{
	int i = 0, j, t = 0;

	do
	{
		right_2(i, &t);

		for (j=3; j>0; j--)
		{
			if (num[i][j] && num[i][j] == num[i][j-1])
			{
				num[i][j] += num[i][j-1];
				num[i][j-1] = 0;
				restart_vot();
				t = 1;
			}
		}
		right_2(i, &t);
		i++;
	} while(i < 4);

	return t;
}

void right_2(int i, int *t)
{
	int j, v;

	for (j=i*4+3; j>i*4; j--)
	{
		v = j;

		while (!void_or_true[j] && v>i*4)
		{
			v--;
			if (void_or_true[v])
			{
				num[i][j-i*4] = num[i][v-i*4];
				num[i][v-i*4] = 0;
				restart_vot();
				*t = 1;
				break;
			}
		}
	}
}

int judge_win()
{
	int i, j;

	for (i = 0; i<16; i++)
	{
		if (!void_or_true[i])
		{
			return 0;
		}
	}

	for (i = 0; i<4; i++)
	{
		for (j = 0; j<3; j++)
		{
			if (num[i][j] == num[i][j+1] || num[j][i] == num[j+1][i])
			{
				return 0;
			}
		}
	}

	return 1;
}

int win()
{
	int i, j;

	for (i=0; i<4; i++)
	{
		for (j=0; j<4; j++)
		{
			if (num[i][j]==2048)
			{
//				system("clear");
//				printf("\e[2J");

				printf("\e[1;32m\n\n\
   1010          1010          1010\n\
    0101        010101        0101\n\
     1010      01010101      0101\n\
      0110    1010  1010    1010\n\
       0010  0101    0101  0101\n\
        10100101      10101010\n\
         010010        010101\n\
          1010          1010\e[0m\n\n");

				printf("\e[1;35m\n\
              011011010\n\
            0110100100100\n\
          10110100101010101\n\
        0101010       0101010\n\
       010101           101101\n\
      101010             010100\n\
      101011             101001\n\
      011001             010011\n\
      110101             101101\n\
      110001             100101\n\
       011001           010101\n\
        1001010       0101010\n\
          01101010010101011\n\
            0011010010101\n\
              011011010\e[0m\n\n");

				printf("\033[1;31m\n\
   1010          1010          1010\n\
    0101        010101        0101\n\
     1010      01010101      0101\n\
      0110    1010  1010    1010\n\
       0010  0101    0101  0101\n\
        10100101      10101010\n\
         010010        010101\n\
          1010          1010\e[0m\n");

				return 1;
			}
		}
	}

	return 0;
}

int main(int argc, char *argv[])
{
	srand(time(NULL));
	rand_cn(), rand_cn();

	while ((int)s!=116 && (int)s!=84)
	{
		srand(time(NULL));
		restart_vot();

		system("clear"); //清屏
//		printf("\033[2J");
		print_k();

		if (win())
			break;

		if (judge_win())
			break;

		scanf("%c", &s);
		getchar();

		switch (s)
		{
			case 'W':
			case 'w':
				if (up())
					rand_cn();
				break;
			case 'A':
			case 'a':
				if (left())
					rand_cn();
				break;
			case 'S':
			case 's':
				if (down())
					rand_cn();
				break;
			case 'D':
			case 'd':
				if (right())
					rand_cn();
				break;
			default:
				break;
		}
	}

	printf("\n\n\n\n\t\t\t游 戏 结 束\n\n");

	return 0;
}

