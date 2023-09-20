# Reactor

本项目是一个基于 Reactor 模式的 LinuxC++ 网络服务器框架，支持多线程 TCP 服务器，单线程 TCP 服务器，可以让开发者专注于业务，快速开发出一个高效的服务器应用

## 概览

这是项目各个模块所需要的技术和内容

![overview](https://github.com/AxLiupore/reactor/blob/master/images/overview.jpg)

这个是这个项目的运行流程

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

这相当于中央处理器，用于控制事件的处理，事件分发模型，检测对应的文件描述符的事件

在这个模块里有 6 个主要的函数：

1. `init` ：用于初始化 IO 多路复用的模型，3 种：selcet、epoll、poll，不管是哪个模型都需要用到多种数据块，在这里面就是用来初始化这些数据块
2. `add`：将待检测的文件描述符添加到 select、epoll、poll 节点上面
3. `remove`：将文件描述符从节点上删除
4. `modify`：修改文件描述符在节点上
5. `dispatch`：对发生的事件进行检测，看是哪个 IO 复用模型上的事件需要检测，就对那个节点进行处理
6. `clear`：将`dispatcher`从`EventLoop`上删除



### EventLoop

![eventloop](https://github.com/AxLiupore/reactor/blob/master/images/eventloop.jpg)

事件循环，当服务器启动之后，会有不停的事件触发，事件包括：客户端的新连接、已经建立连接的客户端和服务器之间的通信，一个 EventLoop 就对应一个反应堆模型

#### Dispatcher

可以通过这个向 Dispatcher 进行添加事件，就是事件对应的文件描述符原来不在 Dispatcher 上，把文件描述符添加到了这个上面，待检测的节点就多了一个；还有就是 Dispatcher 上的节点已经和客户端断开了连接，就不需要再次对他进行检测了，因此就需要将这个节点从 Dispatcher 上删除

#### TaskQueue

![eventloop](https://github.com/AxLiupore/reactor/blob/master/images/taskqueue.jpg)

- 里面有一个 TaskQueue，用于处理要处理的任务，里面存储的是 Channel*，如果是添加事件的节点就加到 Dispatcher 对应的检测集合中，如果是删除的就从 Dispatcher 上删除

- 这里用到了生产者、消费者模型，消费者：Dispatcher，生产者：其他的线程（主线程和客户端建立了连接，就需要通信）
- 用到了链表，链表的节点是 ChannelElement 类型，这是一个结构体，有三个成员：`type`、`Channel`、`next`，根据`type`对`Channel`进行操作
- 节点处理的细节：
    - 当前的线程是子线程：对于链表节点的添加：可能是当前线程也可能是其他线程（主线程），修改文件描述符的事件，因为是由它自己发起的，所以处理还是它自己；添加新的文件描述符，添加任务节点的操作是由主线程发起的
    - 当前的线程是主线程：不会让主线程进行节点的处理，主线程只负责和客户端进行连接，建立连接之后，剩下的操作都是需要由子线程进行操作的，不能让主线程处理任务队列，需要由当前的自线程进行处理；子线程在工作、子线程被阻塞：select、poll、epoll，可以通过向这三个上面添加文件描述符来解除阻塞，对应的线程就解除阻塞


#### ChannelMap

有一个 ChannelMap，基于数组实现的，通过这个就可以通过文件描述符找到对应的 Channel

#### Others

还有一些其他数据：`ThreadID`、`ThreadName`、`ThreadMutex`、`ThreadCondition`
- `ThraedID`：因为在当前的服务器里面有多个 EventLoop，每个 EventLoop 都属于一个线程，这个 ThreadID 就是记录那个子线程的线程 ID
- `ThreadName`：子线程的名称
- `ThreadMutex`：互斥锁，保护的是任务的队列，因为这个任务队列会被多个线程操作
- `ThreadCondition`：条件变量

## 多线程

![threadpool](https://github.com/AxLiupore/reactor/blob/master/images/threadpool.jpg)

这里用到了 ThreadPool，先使用单个 Thread 的模型，再基于单个 Thread 的模型去编写一个 ThreadPool

### WorkerThread

这是工作的线程，这里面有一个 EventLoop 反应堆实例，然后通过线程去执行反应堆，子线程需要执行的任务都在任务队列里面，不需要进行额外的操作

在启动线程的时候，有可能这个函数执行完了，线程的回调函数还没有执行完，为了保证子线程被成功初始化，通过条件变量和互斥锁对主线程进行阻塞，知道子线程的 EventLoop 被初始完之后，才放行

### ThreadPool

线程池，管理了一个 WorkThreads 数组，里面有若干个元素，每一个元素都有一个 WorkerThread 对象，初始化好了之后，每个 WorkerThread 里面都有一个 EventLoop 反应堆

- `state`：线程池的状态，判断是否开启
- `ThreadNum`：线程的数量
- `index`：用于访问子线程
- `struct EventLoop`：
- `WorkerThread`：主线程和客户端建立了一个连接，连接建立之后，就需要和客户端进行通信，这个通信的流程需要交给子线程去处理，每个子线程里面都有一个 EventLoop 反应堆模型，需要把通信的文件描述符交给这个反应堆去管理

## I/O模型

本模块涉及数据的读写操作，读写数据都是需要一块内存，例如：read 数据需要一块内存，将对端发送过来的数据放到内存里面，再将内存中的数据读取出来进行一些列的后续处理，这一块数据其实就是 Http 请求消息；当要发送数据首先需要一块内存，组织一个数据块就是 Http 响应消息，将数据写到这块内存里面，然后通过 write 发送到对端，将读数据和写数据封装成了一个 Buffer

在本项目中，在 Tcp 通信中，有用于监听的文件描述符还有用于通信的文件描述符，在 TcpConnection 里面封装了用于通信的文件描述符，基于这个文件描述符就可以接受数据和发送数据了

### Buffer

![threadpool](https://github.com/AxLiupore/reactor/blob/master/images/buffer.jpg)

服务端和客户端都是通过这个 Buffer 进行数据的操作，当 Buffer 大小不足时，需要扩容，有以下三种情况：

1. 内存够用 -- 不需要扩容
2. 内存需要合并才够用 -- 不需要扩容
3. 内存不够用 -- 需要扩容

### TcpConnection

封装的是建立连接之后，等到的用于通信的文件描述符，基于这个文件描述符服务端可以发送数据，发送数据的时候可以写到一块内存中去，然后再从这个内存中的数据发送给客户端；接受数据，先将接受到的数据放到一块内存中

## 服务器

### TcpServer

### HttpServer

## Http

这个模块是用来解析客户端发送来的数据和发送服务端的数据，将请求行和请求头解析出来的数据保存起来，然后基于这些数据去组织 Http 响应，就是组织回复的数据

### HttpRequest

### HttpResponse

