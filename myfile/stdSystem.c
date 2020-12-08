/* 学生管理系统
 * 
 * 目前功能：
 *  增加学生
 *  查看全部学生
 *  删除学生
 *  添加学生成绩
 *
 * 当前只在termux上进行编译和测试
 * Ubuntu18.4的环境下也能完美运行，其它环境下并不能确保是否能运行
 * 目前是所以一个单链表来存放学生信息，学生数量越多占用的内存越多，而且不能永久存放学生信息，程序结束学生信息就会被释放。
 *
 * 作者：
 * QQ：2946859498
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SJ_NUM 5            // 学科数量
#define SDP_MAX 1024        // 学生内存池的大小
#define SJP_MAX 1024        // 成绩内存池的大小
#define NAME_SIZE 48        // 学生姓名的长度
#define RESEX(num) (num? "男" : "女")

/* 学科也是用个单链表来存储成绩，例如：语文
 * */
typedef struct Subject
{
	short achievement;
	struct Subject *next;
	char name[NAME_SIZE];   // 学科名称
} SUBJECT;

typedef struct Student
{
	unsigned int sex : 1;   // 只能存放0和1
	unsigned int age : 16;
	SUBJECT *subject;
	struct Student *next;
	unsigned int id;
	char name[NAME_SIZE];
} STUDENT;

char *SubjectName[SJ_NUM] = 
{
	"语文",
	"数学",
	"英语",
	"物理",
	"化学"
};

STUDENT *last;              // 学生链表的最后一个学生位置
SUBJECT *SjPool;            // 成绩内存池
STUDENT *StdPool;           // 学生内存池
unsigned int MaxId=0;       // 当前学生的最大ID
unsigned short conent=0;    // 用于记录内存池当前的大小
unsigned short SjConent=0;  // 用于记录成绩内存池当前的大小
unsigned int num_of_std=0;  // 当前学生数量

int tonum_2(int);
void freeR(STUDENT **);
void readStd(STUDENT **);
void writeStd(STUDENT **);
void addGredes(STUDENT **);
void reset_sdt(STUDENT **);
void addStudent(STUDENT **);
void delStudent(STUDENT **);
void findStudent(STUDENT *);
void showStudents(STUDENT *);
void tonum(char *, int *, int);
void readSubj(FILE **, STUDENT **);
short whetherOrNotPrint(STUDENT *, unsigned int *);

void tonum(char *s, int *v, int len)
{
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

void reset_sdt(STUDENT **student)  // 重置一个学生
{
	SUBJECT *n = (*student)->subject;

	(*student)->id = 0;
	(*student)->sex = 0;
	(*student)->age = 0;
	(*student)->next = NULL;
	if(n != NULL)
	{
		SUBJECT *o = NULL;
		for(; n->next != NULL; n = n->next)
		{
			if(o != NULL && SjConent < SJP_MAX)
			{
				o->next = SjPool;
				SjPool = o;
				SjConent++;
			} else if(o != NULL)
				free(o);
			o = n;
		}
		if(o != NULL && SjConent < SJP_MAX)
		{
			o->next = SjPool;
			SjPool = o;
			SjConent++;
		} else if(o != NULL)
			free(o);
		if(n != NULL && SjConent < SJP_MAX)
		{
			n->next = SjPool;
			SjPool = n;
			SjConent++;
		} else if(n != NULL)
			free(n);
	}
	memset((*student)->name, 0, sizeof((*student)->name));
}

void addStudent(STUDENT **students)   // 增加一个学生
{
	char *str;
	STUDENT *now;
	unsigned int i;

	// 如果内存池中还有空间就在内存池中生成
	if (conent)
	{
		conent--;
		now = StdPool;
		reset_sdt(&now);
		StdPool = StdPool->next;
	} else  // 内存池不为空，申请一个新的内存空间
	{
		now = (STUDENT *)malloc(sizeof(STUDENT));
		if (now == NULL)
		{
			printf("内存分配失败！\n");
			exit(1);
		}
	}

	// 录入信息
	printf("请输入姓名：");
	scanf("%s", now->name);
	printf("请输入性别：");
	// 限定性别只能是男或女
	// 男对应的值是1，女对应的值是0
	scanf("%s", str);
	while (1)
	{
		if (!strcmp(str, "男"))
		{
			now->sex = 1;
			break;
		}
		else if (!strcmp(str, "女"))
		{
			now->sex = 0;
			break;
		} else 
		{
			fprintf(stdout, "请输入一个正确的性别（男、女）");
			scanf("%s", str);
		}
	}
	printf("请输入年龄：");
	scanf("%u", &i), getchar();
	now->age = i;
	now->subject = NULL;
	now->id = (MaxId += 1);
	fprintf(stdout, "录入学生成功，开始写入内存...\n");

	// 先判断students是否为空，防止踩空
	if (*students != NULL)
	{
		last->next = now;
		now->next = NULL;
		last = now;
	} else
	{
		*students = now;
		now->next = NULL;
		last = now;
	}

	num_of_std++;  // 当前学生数量自加1
	fprintf(stdout, "成功添加一名学生！\n\n");
}

/* 打印全部学生信息
 * 在打印前会询问用户是否要打印学生成绩。
 *
 * Input
 *   *students: 学生链表
 * return
 *   void
 * */
void showStudents(STUDENT *students)
{
	char s;
	int num = 0, i;
	STUDENT *coent;
	SUBJECT *net;
	coent = students;

	if (students == NULL)
	{
		fprintf(stdout, "当前系统没有学生，快去添加吧！\n");
		return ;
	}

	printf("是否要显示学生成绩？(Y/N)");
	scanf("%c", &s);
	switch (s)
	{
		// 
		case 10:
		case 'y':
		case 'Y':
			i = 1;
			break;
		default :
			i = 0;
			break;
	}

	/* 遍历students链表并打印
	 * */
	while (coent != NULL)
	{
		num++;
		fprintf(stdout, "\n学生 %d\n", coent->id);
		fprintf(stdout, "姓名：%s\n", coent->name);
		fprintf(stdout, "性别：%s\n", RESEX(coent->sex));
		fprintf(stdout, "年龄：%u\n", coent->age);
		// 如果用户需要打印成绩就打印
		if (i)
		{
			if(coent->subject == NULL)
				printf("该学生未录入成绩！\n");
			else
			{
				net = coent->subject;
				for(; net != NULL;net = net->next)
				{
					printf("%s: ", net->name);
					if(net->achievement < 0)
						printf("无成绩！\n");
					else 
						printf("%d\n", net->achievement);
				}
			}
		}

		if(coent->next == NULL)
		{
			last = coent;
		}
		coent = coent->next;
	}

	num_of_std = num;  // 刷新一次学生数量
}

/* 找到要找的学生
 *
 * Input:
 *  *students: 学生链表
 *  *num: 学生所在的链表位置
 * Return:
 *  是否有学生，有返回1，没找有到返回0
 * */
short whetherOrNotPrint(STUDENT *students, unsigned int *num)
{
	char ca;
	int lnum;

	if (students == NULL)
	{
		printf("当前没有学生信息，快去添加吧！\n");
		return 0;
	}

	fprintf(stdout, "是否需要打印一次学生信息？(Y/N) ");
	scanf("%c", &ca);
	switch (ca)
	{
		case 10:
		case 'y':
		case 'Y':
		{
			showStudents(students);
			break;
		}
		default :
			break;
	}

	do
	{
		printf("请输入学生号码：");
		scanf("%d", &lnum);
	} while(lnum == 0 || (lnum > MaxId && lnum*(-1) > MaxId));

	// 支持倒序删除
	if (lnum < 0)
	{
		*num = MaxId + lnum + 1;
	}
	else 
		*num = lnum;

	return 1;
}

/* 删除学生
 *
 * Input:
 *   **students：指向STUDENT指针的指针，要删除成员的单链表
 * Output: void
 * */
void delStudent(STUDENT **students)
{
	unsigned int num;  // 学生的链表位置
	STUDENT *cen;
	STUDENT *old;
	old = (cen = *students);

	/* 找到学生的链表位置
	 * 即确定 num 的值
	 * */ 
	if (!whetherOrNotPrint(*students, &num))
	{
		return ;
	}

	// 定位要删除的学生在链表的位置
	while (num != cen->id && cen != NULL)
	{
		old = cen;
		cen = cen->next;
	}

	if(cen == NULL)
	{
		printf("未找到该学生请确认输入值是否正确。\n");
		return ;
	}

	// 防止students的头指针被删除
	if (cen != *students)
	{
		old->next = cen->next;
	} else 
	{
		*students = (*students)->next;
	}
	// 内存池没满就丢到内存池中，满了就删除
	if (conent < SDP_MAX)
	{
		cen->next = StdPool;
		StdPool = cen;
		conent++;
	} else 
	{
		free(cen);
	}

	num_of_std--; // 更新学生数量
	printf("成功删除一个学生信息！\n");
}

/* 添加（一个）学生成绩
 * */
void addGredes(STUDENT **students)
{
	short i, fi = 0;
	unsigned int num;  // 学生在链表的位置
	STUDENT *coent;
	SUBJECT *ach;
	coent = *students;

	if (!whetherOrNotPrint(*students, &num))
		return ;

	while (coent != NULL && num != coent->id)
	{
		coent = coent->next;
	}
	if(coent == NULL)
	{
		printf("未找到该学生请确认输入值是否正确。\n");
		return ;
	}

	printf("目前开设的课程有:\n");
	for(i=0; i < SJ_NUM;i++)
	{
		printf("%d: %s\n", i+1, SubjectName[i]);
	}

	printf("输入序号：");
	scanf("%hd", &fi);
	fi -= 1;
	while(fi >= 0)
	{
		if(fi < SJ_NUM)
		{
			ach = coent->subject;
			i = 0;
			while(ach != NULL)
			{
				if(!strcmp(SubjectName[fi], ach->name))
				{
					printf("该学生的%s成绩是：", SubjectName[fi]);
					scanf("%hd", &fi);
					ach->achievement = fi;
					printf("成功录入成绩！\n");
					fi = -1;
				}
				if(ach->next == NULL)
					break;
				ach = ach->next;
				i++;
			}
			if(coent->subject == NULL)
			{
				if(SjConent)
				{
					ach = SjPool;
					SjPool = SjPool->next;
					SjConent--;
				}
				else
				{
					ach = (SUBJECT *)malloc(sizeof(SUBJECT));
					if(ach == NULL)
					{
						printf("内存分配失败！\n");
						exit(1);
					}
				}
				strcpy(ach->name, SubjectName[fi]);
				printf("成绩：");
				scanf("%hd", &fi);
				ach->achievement = fi;
				ach->next = NULL;
				coent->subject = ach;
				printf("成功录入成绩！\n");
			}
			else if(ach->next == NULL && fi != -1)
			{
				SUBJECT *now;
				now = (SUBJECT *)malloc(sizeof(SUBJECT));
				strcpy(now->name, SubjectName[fi]);
				printf("成绩：");
				scanf("%hd", &fi);
				now->achievement = fi;
				now->next = NULL;
				ach->next = now;
				printf("成功录入成绩！\n");
			}
		}
		printf("请输入序号，输入小于等于0的数退出");
		scanf("%hd", &fi);
		fi -= 1;
	}
}

/* 按照条件查找学生
 * */
void findStudent(STUDENT *student)
{
	char str[NAME_SIZE];
	STUDENT *coent;
	coent = student;
	int numOfFind = 0; // 找到学生的数量

	printf("请输入要查找的字符串：");
	scanf("%s", str);

	// 开始查找
	while(coent != NULL)
	{
		if (!strcmp(str, coent->name))
		{
			numOfFind++;
			// 找到后输出
			fprintf(stdout, "已找到学生：%d\n", coent->id);
			fprintf(stdout, "姓名：%s\n", coent->name);
			fprintf(stdout, "性别：%s\n", RESEX(coent->sex));
			fprintf(stdout, "年龄：%d\n", coent->age);
		}
		coent = coent->next;
	}

	if (!numOfFind)
		printf("找不到该学生！\n");
}

// 释放内存
void freeR(STUDENT **p)
{
	STUDENT *ne = NULL;

	while(*p != NULL)
	{
		ne = *p;
		*p = (*p)->next;
		free(ne);
	}
}

// 写入文件
void writeStd(STUDENT **students)
{
	int i;
	SUBJECT *subj;

	if(*students==NULL)
		return ;

	FILE *fp;
	fp = fopen(".students", "w");
	if(fp==NULL)
	{
		printf("fail to open the file!\n文件操作失败！无法打开文件！\n");
		return ;
	}

	STUDENT *nowstd=*students;

	while(nowstd != NULL)
	{
		subj = nowstd->subject;
		fprintf(fp, "`@ID`%d``@NAME`%s`", nowstd->id, nowstd->name);
		fprintf(fp, "`@SEX`%u``@AGE`%u`", nowstd->sex, nowstd->age);
		fprintf(fp, "`@SUBJECT`");
		for(subj=nowstd->subject; subj != NULL; subj = subj->next)
		{
			fprintf(fp, "$%s$%hd$", subj->name, subj->achievement);
		}
		fprintf(fp, "`\n");
		nowstd = nowstd->next;
	}
	fclose(fp);
}

void readSubj(FILE **fp, STUDENT **std)
{
	int i = 0;
	char c = 0;
	char str[NAME_SIZE];

	while(c != '\n')
	{
		if((c=fgetc(*fp)) == '`' && (c=fgetc(*fp)) == '@')
		{
			for(i=0; i < NAME_SIZE; i++)
			{
				str[i] = (c=fgetc(*fp));
				if(c == '`')
				{
					str[i] = '\0';
					break;
				}
			}
			if(!strcmp(str, "SUBJECT"))
			{
				if((c=fgetc(*fp)) == '$')
				{
					for(i=0; i < NAME_SIZE; i++)
					{
						str[i] = (c=fgetc(*fp));
						if(c == '$')
						{
							str[i] = '\0';
							break;
						}
					}
					for(int j=0; j<SJ_NUM; j++)
					{
						if(!strcmp(str, SubjectName[j]))
						{
						}
					}
				}
			}
			else if(i < NAME_SIZE)
				fseek(*fp, i*(-1), SEEK_CUR);
		}
		else if(c == '`')
			fseek(*fp, -1, SEEK_CUR);
	}
}

/* 从文件中读取学生数据写入链表
 * */
void readStd(STUDENT **students)
{
	int i, creat_bool=0;
	char c, str[NAME_SIZE];
	char stra[NAME_SIZE];
	FILE *fp;
	STUDENT *nowd;

	if((fp = fopen(".students", "r")) == NULL)
	{
		fp = fopen(".students", "a+");
		fclose(fp);
		return ;
	}

	while(!feof(fp))
	{
		if((c=fgetc(fp)) == '`' && (c=fgetc(fp)) == '@')
		{
			if(conent && !creat_bool)
			{
				conent--;
				nowd = StdPool;
				reset_sdt(&nowd);
				StdPool = StdPool->next;
			}
			else if(!creat_bool)
			{
				nowd = (STUDENT *)malloc(sizeof(STUDENT));
				if (nowd == NULL)
				{
					printf("内存分配失败！\n");
					return ;
				}
			}

			for(i=0; '`' != (c=fgetc(fp)); i++)
			{
				str[i] = c;
			}
			str[i] = '\0';
			if(!strcmp(str, "ID"))
			{
				for(i=0; (c=fgetc(fp)) != '`' && i < 12; i++)
				{
					stra[i] = c;
				}
				stra[i] = '\0';

				tonum(stra, &i, 12);
				nowd->id = i;
			}
			else if(!strcmp(str, "NAME"))
			{
				for(i=0; (c=fgetc(fp)) != '`' && i+1 <= sizeof(str); i++)
				{
					(nowd->name)[i] = c;
				}
			}
			else if(!strcmp(str, "SEX"))
			{
				c = fgetc(fp);
				if(c == 49)
					nowd->sex = 1;
				else 
					nowd->sex = 0;
			}
			else if(!strcmp(str, "AGE"))
			{
				for(i=0; (c=fgetc(fp)) != '`' && i < 5; i++)
				{
					stra[i] = c;
				}
				stra[i] = '\0';

				tonum(stra, &i, 5);
				nowd->age = i;
			}

			if(strlen(nowd->name) && nowd->age)
			{
				if(*students == NULL)
				{
					*students = nowd;
					nowd->next = NULL;
				}
				else
				{
					last->next = nowd;
					nowd->next = NULL;
				}
				last = nowd;
				creat_bool = 0;
				num_of_std++;
				readSubj(&fp, &nowd);
				if(MaxId < nowd->id)
					MaxId = nowd->id;
			}
			else 
				creat_bool = 1;
		}
		else if(c == '`')
			fseek(fp, -1, SEEK_CUR);
	}

	fclose(fp);
}

int main(void)
{
	short inp;
	STUDENT *students = NULL;
	last = students;

	fprintf(stdout, "~~~~~~学生管理系统~~~~~~\n");
	fprintf(stdout, "\t1、添加一个学生\n");
	fprintf(stdout, "\t2、查看全部学生\n");
	fprintf(stdout, "\t3、删除一个学生\n");
	fprintf(stdout, "\t4、添加一个学生成绩\n");
	fprintf(stdout, "\t5、查找学生\n");
	fprintf(stdout, "\t-1、退出系统\n");

	readStd(&students);

	while (inp > 0)
	{
		printf("请输入序号：");
		scanf("%hd", &inp), getchar();

		switch (inp)
		{
			case 1:
				addStudent(&students);
				break;
			case 2:
				showStudents(students);
				break;
			case 3:
				delStudent(&students);
				break;
			case 4:
				addGredes(&students);
				break;
			case 5:
				findStudent(students);
				break;
			default:
				break;
		}
	}

	writeStd(&students);
	freeR(&students);
	freeR(&StdPool);

	return 0;
}

