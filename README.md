# filec

### [stdSystem.c](https://github.com/conch2/filec/blob/main/stdSystem.c)
- 学生管理系统   
目前使用单链表进行存储数据，使用内存池的垃圾回收机制（内存池容量1024）
  - 结构体：  
  这里我用了`typedef`来起了个别名，实际使用时也是用的别名  
  定义成员的时候还利用了结构体的我也不记得是什么名称的位操作 (:) 来节省空间  
  
  用于存放学生成绩
  ```
  typedef struct Report
  {
    	int english : 9;
    	int mathematics : 9;
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

### [web_c/](https://github.com/conch2/filec/tree/main/web_c)
- 网络编程案例

### [findStr.c](https://github.com/conch2/filec/blob/main/findStr.c)
- 目前只在termux上编译运行，Linux、Windows等上运行都会出现问题。
- 功能：查找字符串并高亮显示
