# Tairitsu bot

Tairitsu Bot是一个QQ群聊机器人，基于酷Q以及[richardchien的第三方酷Q C++ SDK](https://github.com/richardchien/coolq-cpp-sdk)。以后可能会包含更多的酷Q Pro才能使用的功能。  
（好像是）采用了音乐游戏Arcaea中対立（Tairitsu）的人物设定，但是会出现各种OOC台词。  
目前包含UNO，黑白棋AI等功能，以后可能会添加更多。  
只是个人消遣空闲时间的产物，所以代码可能会很难看，不过也无所谓了。

## 生成
若想要生成本应用，你需要修改项目设置中的几个路径：

    对平台 Win32 的 所有配置，在 配置属性->调试 中，修改 命令 一栏为你的酷Q可执行文件的位置。
    对平台 Win32 的 所有配置，在 配置属性->生成事件->生成后事件 中，按照你安装酷Q的实际位置 命令行 中的酷Q App文件夹。

并且，程序中使用到很多第三方C++库，若需要编译该项目你需要配置好这些库（Windows上推荐使用vcpkg管理，非常方便）：

    boost-algorithm: 原SDK中使用了这个库，也许在Tairitsu Bot今后的版本中会排除对此库的依赖；
    boost-regex: 因为MSVC中C++标准库的正则表达式效率极差，故使用boost的正则表达式库作为代替；
    libiconv: 原SDK对此库有依赖，用于在GB18030和UTF-8编码间转换字符串；
    fmt: 非常优秀的字符串格式化库，比ostream的使用更简洁且高效；
    CImg: 图像处理库，在几个需要图片输出的功能中使用到，以后可能会用FreeImage替换；
    nlohmann-json: C++的json序列化/反序列化库，目前用于读取某些分开存储的字符串信息，以后可能会有更多的对此库的依赖。
