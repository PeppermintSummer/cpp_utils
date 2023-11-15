# cpp_utils

整理一下自己平时使用的cpp utils 😃

- **easy_ffmpeg_rtmp_pusher** 简单的推流器，不包含线程模型
- **Makefile&.vscode** 包含vscode的配置，包含平时用于调试的debug配置
- **log&json** 包含了常用的封装的log写入和 [nlohmann/json](https://github.com/nlohmann/json) 头文件
- **string_view** 包含了非c++17实现的string_view，方便嵌入到自己的项目中
- **Dockerfile** Docker镜像
- **HttpClient** http request封装，包含get和post请求
- **AutoTimer** 函数自动记录cpu时间(c++和python实现)。(如果想记录gpu执行时间，使用cudaEvent)
- **cpp_py_tensor** 用于c++和python中的tensor的相互转化
