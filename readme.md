# learn-nanite

## init
记录了我学习vulkan与nanite的仓库，现在为了赶春招十天写出来的，后面写完整后会写个博客

test是vulkan画三角形

test1是想用类似rendergraph的想法用json文件记录管线状态，方便使用，但怎么和代码交互还没想太明白，遂放弃，后面可能会去了解一下AMD开源的RenderPipelineShader

test2是一个简易的vulkan封装，主要是为了让调用写起来更好看点，（但封的过程还是得写vk原来丑丑的api）

test3是qem网格简化与vulkan窗口操作等，qem测试的模型不是水密的，没实现临近顶点的合并，会有破碎的情况

## upd 2023-4-9
简单组织了一下文件结构

mesh文件夹：mesh头文件、qem网格简化、三角形分簇、簇的分组等

program文件夹：简单的viewer程序，用于debug显示，

vk、vk_win文件夹：vulkan与window的简单封装

用边哈希重新实现qem网格简化，可以处理重边与重点

实现三角形的分簇 (clustering triangles) 在cluster.h内

实现三角形簇的分组 (grouping clusters) 在cluster.h内


实现一个简易的viewer程序,在program/viewer内，

用w,a,s,d进行移动

j,k切换显示模式(triangle id、cluster id、group id)

u,i切换mip_level

长按b显示鼠标、移动鼠标转换镜头