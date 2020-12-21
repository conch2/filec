# Start

- [Start_02.cpp](https://github.com/conch2/filec/blob/main/myfile/start/Start_02.cpp)

  设计初衷：每次写完源程序都要保存然后到终端编译运行，很麻烦。我就想到了写一个自动化程序来解决这个问题，正好我又想到了Django每次文件发生变化就会自动重启，所以我就想每次文件改变就自动编译并运行。

  设计思路：用一个类来包装，主线程监测文件变化，子线程编译并运行。当主线程监测到文件变化就结束子线程，主线程备份文件，子线程编译并运行。以此循环完成程序。

  

### 类

- Base

  构造器：

  [`Base( std::string file_name )`](https://github.com/conch2/filec/blob/main/myfile/start/Start_02.cpp#L46)

  使用该构造器则将会将时间默认设置为1秒

  [`Base( std::string file_name , unsigned long s_time )`](https://github.com/conch2/filec/blob/main/myfile/start/Start_02.cpp#L54)

  该构造器

  析构器：

  [`~Base()`](https://github.com/conch2/filec/blob/main/myfile/start/Start_02.cpp#L61)

  

