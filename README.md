## rasterizer

A toy 3D rasterization project.

实现了简易的模型软光栅化。

该程序由cmake进行管理，第三方库：glad+glfw+Eigen+Tinyobjloader。

Tinyobjloader进行模型导入，glad和glfw将光栅信息作为纹理输出到屏幕上，Eigen为数学库。

### 配置教程：

[glad.dav1d.de](https://glad.dav1d.de/)，进入网站下载glad库文件，language: C++，specification: opengl，api gl: version4.5，profile: core，点击 add all 并generate，将header以及lib放在cmake相应的文件夹下。

[An OpenGL library | GLFW](https://www.glfw.org/)， 进入网站下载glfw库，将header以及lib更名后放在cmake相应的文件夹下。

下载Eigen和Tinyobjloader，并搭建相关环境。

用cmake编译项目，可执行文件默认生成在bin中。
