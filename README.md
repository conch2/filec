# filec #

### [stdSystem.c](https://github.com/conch2/filec/blob/main/stdSystem.c) ###
- 学生管理系统   
目前使用单链表进行存储数据，使用内存池的垃圾回收机制（内存池容量1024）  
  
  **结构体：**  
  这里我用了`typedef`来起了个别名，实际使用时也是用的别名  
  定义成员的时候还利用了结构体的我也不记得是什么名称的位操作 (:) 来节省空间  
  
  用于存放学生成绩
  ```
  typedef struct Report
  {
    	int english : 9;  // 只能存放9位二进制位的值
    	int mathematics : 9;  // 同上
  } REPORT;
  ```
  学生基本信息
  ```
  typedef struct Student
  {
  	unsigned int sex : 1;
    	unsigned int age : 9;
    	REPORT report;
    	struct Student *next;
    	char name[48];
  } STUDENT;
  ```
  **函数：**  
    
  [reSex()](https://github.com/conch2/filec/blob/main/stdSystem.c#L130) 原型：
  `char *reSex(int num)`
  
  通过传递的数字返回对应的性别；这个函数非常简单，函数内只有一行代码直接使用了三目运算：`return num? "男" : "女";`实现了功能。定义这一个函数就是为了防止出现多个重复代码。  
  当传入的int为 0 时返回 "女"，其他值返回 "男"。
  
  ----------
  [freeR()](https://github.com/conch2/filec/blob/main/stdSystem.c#L372) 原型：
  `void freeR(STUDENT *p)`
  
  释放内存，通过传进来的单链表进行释放。释放整个单链表。执行时是边遍历边释放，指针往下走然后删除前一个信息。  
  ```
  STUDENT *ne = NULL;
  while(p != NULL)
  {
  	ne = p;
  	p = p->next;
  	free(ne);
  }
  ```
  该程序一般用于程序结束时释放堆内存里的空间。
  
  ----------
  [addGredes()](https://github.com/conch2/filec/blob/main/stdSystem.c#L315) 原型：
  `void addGredes(STUDENT **students)`
  
  待更新...
  
  ----------
  [reset_sdt()](https://github.com/conch2/filec/blob/main/stdSystem.c#L52) 原型：
  `void reset_sdt(STUDENT **student)`

  待更新...
  
  ----------
  [addStudent()](https://github.com/conch2/filec/blob/main/stdSystem.c#L61) 原型：
  `void addStudent(STUDENT **students)`
  
  待更新...
  
  ----------
  [delStudent()](https://github.com/conch2/filec/blob/main/stdSystem.c#L267) 原型：
  `void delStudent(STUDENT **students)`
  
  待更新...
  
  ----------
  [findStudent()](https://github.com/conch2/filec/blob/main/stdSystem.c#L340) 原型：
  `void findStudent(STUDENT *student)`
  
  待更新...
  
  ----------
  [showStudents()](https://github.com/conch2/filec/blob/main/stdSystem.c#L150) 原型：
  `void showStudents(STUDENT *students)`
  
  待更新...
  
  ----------
  [whetherOrNotPrint()](https://github.com/conch2/filec/blob/main/stdSystem.c#L221) 原型：
  `short whetherOrNotPrint(STUDENT *students, short *num)`
  
  待更新...
  

### [web_c/](https://github.com/conch2/filec/tree/main/web_c) ###
- 网络编程案例

### [findStr.c](https://github.com/conch2/filec/blob/main/findStr.c) ###
- 目前只在termux上编译运行，Linux、Windows等上运行都会出现问题。
- 功能：查找字符串并高亮显示
