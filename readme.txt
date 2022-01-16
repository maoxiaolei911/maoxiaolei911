  The Xiuwen package is a simplified TeX/LaTeX system with a TeX compiler 
based on the aleph based implementation in Qt C++, and the entire package 
consists of:
  1. DVI/PDF reader supports dvipdfmx, dvips driver, DVI/PDF to text/picture, 
     DVI to PDF, with landscape viewing and on-screen markup.
  2. TeX/LaTeX reader converts text files directly to DVI view.
  3. LaTeX text editor reduces the difficulty of use for beginners, 
     supporting left-right horizontal and left-right vertical rows. 
     the possibility of editing text files with any text.
  4. TeX/LaTeX text editor for skilled users, with syntax highlighting and 
     keyword completion.
  5. Format file editor for format developers, syntax highlighting, keyword 
     completion.
  6. Several CJK classes can be arranged horizontally and vertically in CJK 
     characters.
  7. Formula Editor for users unfamiliar with TeX/LaTeX formula syntax.
  8. Tikz graphical editor for users unfamiliar with the Tikz graphical language.
  9. font tool to FreeType2 implementation, and added pk font support, you can 
     view a variety of font information. Automatic generation of tfm, ofm.

1. Catalogue description
  build   Compile in this directory.
  include Header files of dependent external libraries
  pool    strings.
  pro     qmake project file.
  res     resources.
  src     C++ files.
      It is recommended to put the project in your Xiuwen runtime directory, so 
  that the compiled result  will be ready to use directly.

2. Dependent external libraries
  Qt4        The whole project is built on Qt, so please install Qt first.
  freetype2  Font rendering, ofm generation all use this library.
  jpeg       dvi to PDF when using
  png        dvi to PDF when using
  zlib       dvi to PDF when using

3. Compilation
      Compilation should be done in the build directory. It is recommended to 
  create a directory associated with the platform or compiler first. directory, 
  e.g. windows, into which you can also create subdirectories. If you intend to 
  compile submodules.  Run the qmake command in the directory with the argument 
  pro  directory, which will generate the relevant Makefile file, and then run 
  the corresponding make.
  
  秀文软件包是一个简化的TeX/LaTeX系统，它的TeX编译器是在aleph的
基础上以Qt C++实现，整个软件包包括：
  1、DVI/PDF阅读器支持dvipdfmx、dvips驱动，DVI/PDF转文本、图片，
     DVI转PDF，可以横向浏览，可以在屏幕上标记；
  2、TeX/LaTeX阅读器把文本文件直接转换为DVI浏览；
  3、LaTeX文本编辑器降低初学者的使用难度，支持左右横排和左右竖排，
     可以编辑任意文字的文本文件；
  4、TeX/LaTeX文本编辑器针对熟练用户，语法高亮度，关键字补全；
  5、格式文件编辑器针对格式开发人员，语法高亮度，关键字补全；
  6、几个CJK类可以横排、竖排中日韩文字；
  7、公式编辑器针对不熟悉TeX/LaTeX公式语法的用户；
  8、Tikz图形编辑器针对不熟悉Tikz图形语言的用户；
  9、字体工具以FreeType2实现，又增加了pk字体支持，可以查看各种字体信息，
     自动生成tfm、ofm。

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

  Xiuwen パッケージは、aleph をベースにした TeX コンパイラを備えた簡略化された 
TeX/LaTeX システムです。をベースにしたQt C++の実装で、パッケージ全体は以下のよ
うに構成されています:
  1、DVI/PDFリーダーは、dvipdfmx、dvipsドライバ、テキスト、画像へのDVI/PDFをサ
     ポートしています。DVIからPDFへ、横向き表示と画面上でのマークアップが可能です。
  2、TeX/LaTeX リーダーは、テキストファイルを直接 DVI に変換して表示します。
  3、LaTeXテキストエディターは、左右の横列、左右の縦列に対応し、初心者でも使いや
     すいように配慮しています。テキストファイルを任意のテキストで編集できる可能性が
     あること。
  4、シンタックスハイライトやキーワード補完機能を備えた熟練者向けのTeX/LaTeXテキス
     トエディタです。
  5、フォーマット開発者のためのフォーマットファイルエディタ、シンタックスハイライト、
     キーワード補完。
  6、いくつかのCJKクラスでは、水平および垂直のCJKテキストを使用することができます。
  7、TeX/LaTeXの数式構文に不慣れなユーザー向けの数式エディタ。
  8、Tikzグラフィック言語に不慣れなユーザー向けのTikzグラフィックエディター。
  9、フォントツールはFreeType2で実装されており、様々なフォントの情報を見ることができ
     るpkフォントのサポートが追加されています。tfm、ofmの自動生成。

1 カタログ記載内容
  build   このディレクトリでコンパイルしてください。
  include 依存する外部ライブラリのヘッダーファイル。
  pool    文字列ファイルです。
  pro     qmake用プロジェクトファイル
  res     リソースと説明ファイル。
  src     C++ファイルです。
      Xiuwenの実行ディレクトリにプロジェクトを配置し、コンパイルした結果は、
  すぐに使用することができます。

2 依存する外部ライブラリ
  Qt4       このプロジェクトはすべてQtで構築されていますので、まずQtをインストールしてください。
  freetype2 このライブラリは、フォントのレンダリングとofmの生成に使用されます。
  jpeg      dviからPDFに変換する際に使用します。
  png       dviからPDFに変換する際に使用します。
  zlib      dviからPDFに変換する際に使用します。

3 コンピレーション
      コンパイルは、ビルドディレクトリで行う必要があります。 最初にプラットフォームや
  コンパイラに関連したディレクトリを作成することをお勧めします。のようなディレクトリを
  作成し、その中にサブディレクトリを作成することもできます。サブモジュールをコンパイル
  する場合。 引数proを指定して、そのディレクトリでqmakeコマンドを実行します。ディレクト
  リに移動し、関連する Makefile ファイルを生成し、その後を実行し、対応するmakeを実行します。
