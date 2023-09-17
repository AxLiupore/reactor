# Reactor

本项目是一个基于 Reactor 模式的 LinuxC++ 网络服务器框架，支持多线程 TCP 服务器，单线程 TCP 服务器，可以让开发者专注于业务，快速开发出一个高效的服务器应用

## 概览

![overview](https://github.com/AxLiupore/reactor/blob/master/images/overview.jpg)

## 反应堆模型

可以通过这个反应堆检测事件，检测完这个事件之后，可以把相应的事件进行一系列的处理，这就是一个反应堆模型

### Listener

监听器，这里有用于监听的端口和用于监听的文件描述符

### Channel

![channel](https://github.com/AxLiupore/reactor/blob/master/images/channel.jpg)

通道，将用于监听和通信的文件描述符进行封装，对应的就是一个通道，一个服务器需要接受客户端的连接需要一个文件描述符，所有的客户端发送的连接都需要通过这个文件描述符进行连接，每个客户端都对应一个文件描述符

封装这个 Channel 需要一个文件描述符 fd，可以是通信的也可以是监听的，不管是通信的还是监听的描述符，最终都要放到 I/O 多路复用模型里面进行检测，还需要封装这个文件描述符需要检测的事件：r、w、rw

这个模块主要有以下函数：

1. 初始化一个 Channel
2. 修改 fd 的事件（检测 or 不检测）
    - 这里用到了 C 语言的通用属性做法，通过标志位来判断是否检测
    - 当要增加这个检测就对`WRITE_EVENT`进行按位或，不检测就是先对`WRITE_EVENT`取反再按位与
3. 判断是否需要检测文件描述符的写事件

### ChannelMap

![channelmap](https://github.com/AxLiupore/reactor/blob/master/images/channelmap.jpg)

里面存储的就是一个对应关系，每个文件描述符都对应一个 Channel，基于一个文件描述符就可以找到对应的 Channel 的实例

这里主要是用空间来换时间，通过数组的下标来快速找到要处理的 Channel

1. 创建一个结构体用于映射，有两个成员变量：`size`和`list` ，前者是存储指针指向的数组的元素总个数，后者是存储`channel`的数组
1. 初始化一个 channel_map，要初始化数组元素的大小和这个数组元素的地址
1. 清空 channel_map，因为指针指向的是堆内存，所以需要清空对应数组里面的资源，在这里需要释放两部分资源：`list`所指向的数组元素里面的的地址要释放、`list`所直线的地址要释放
1. 给 channel_map 重新分配空间，只有当前 channel_map 的大小小于指定的大小，才需要重新分配；这里指定一个分配规则：每次扩容前面的 2 倍，通过 realloc 函数将数组大小扩容，因为这里可能会重新分配内存导致`list`指向的地址发生改变，所以需要改变`list`的指向，在初始化完成之后，需要将新开辟的数组里面的元素内容初始化为 0，然后更新数组的大小

### Dispatcher

![dispatcher](https://github.com/AxLiupore/reactor/blob/master/images/dispatcher.jpg)

I/O 多路复用的模型，这里有三种模型可以选择：epoll、poll、select，这三个是三选一，不是同时使用，通过 Dispatcher 检测是一些系列的事件，将对应的事件注册到了反应堆，当有时间发生之后，就会调用相关的处理动作：回调函数

这相当于中央处理器，用于控制事件的处理

在这个模块里有 6 个主要的函数：

1. `init` ：用于初始化 IO 多路复用的模型，3 种：selcet、epoll、poll，不管是哪个模型都需要用到多种数据块，在这里面就是用来初始化这些数据块
2. `add`：将待检测的文件描述符添加到 select、epoll、poll 节点上面
3. `remove`：将文件描述符从节点上删除
4. `modify`：修改文件描述符在节点上
5. `dispatch`：对发生的事件进行检测，看是哪个 IO 复用模型上的事件需要检测，就对那个节点进行处理
6. `clear`：将`dispatcher`从`EventLoop`上删除

### EventLoop

![eventloop](https://github.com/AxLiupore/reactor/blob/master/images/eventloop.jpg)

事件循环，当服务器启动之后，会有不停的事件触发，事件包括：客户端的新连接、已经建立连接的客户端和服务器之间的通信

可以通过这个向 Dispatcher 进行添加事件，就是事件对应的文件描述符原来不在 Dispatcher 上，把文件描述符添加到了这个上面，待检测的节点就多了一个；还有就是 Dispatcher 上的节点已经和客户端断开了连接，就不需要再次对他进行检测了，因此就需要将这个节点从 Dispatcher 上删除

## 多线程

这里用到了 ThreadPool，先使用单个 Thread 的模型，再基于单个 Thread 的模型去编写一个 ThreadPool

### WorkerThread

### ThreadPool

## I/O模型

本模块涉及数据的读写操作，读写数据都是需要一块内存，例如：read 数据需要一块内存，将对端发送过来的数据放到内存里面，再将内存中的数据读取出来进行一些列的后续处理，这一块数据其实就是 Http 请求消息；当要发送数据首先需要一块内存，组织一个数据块就是 Http 响应消息，将数据写到这块内存里面，然后通过 write 发送到对端，将读数据和写数据封装成了一个 Buffer

在本项目中，在 Tcp 通信中，有用于监听的文件描述符还有用于通信的文件描述符，在 TcpConnection 里面封装了用于通信的文件描述符，基于这个文件描述符就可以接受数据和发送数据了

### Buffer

### TcpConnection

## 服务器

### TcpServer

### HttpServer

## Http

这个模块是用来解析客户端发送来的数据和发送服务端的数据，将请求行和请求头解析出来的数据保存起来，然后基于这些数据去组织 Http 响应，就是组织回复的数据

### HttpRequest

### HttpResponse

