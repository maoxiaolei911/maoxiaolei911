1 目录说明
  build   在这个目录中编译。
  include 依赖的外部库的头文件。
  pool    字符串文件。
  pro     qmake的工程文件（project file）。
  res     资源和描述文件。
  src     C++文件。
      建议把项目放在你的秀文运行目录下，这样一来，编译结果
  就可以直接使用了。

2 依赖的外部库
  Qt4       整个项目是建立在Qt基础上的，所以请先安装Qt。
  freetype2 字体渲染，ofm生成都用这个库。
  jpeg      dvi转PDF时使用。
  png       dvi转PDF时使用。
  zlib      dvi转PDF时使用。

3 编译
      编译应该在build目录中进行。建议先建立一个平台或编译
  器关联的目录，  比如windows，进入目录后还可以建立子目录，
  如果你打算编译子模块。  在目录中运行qmake命令，参数是pro
  目录中的某个pro文件，这样可以生成相关的Makefile文件，再
  运行相应的make即可。
