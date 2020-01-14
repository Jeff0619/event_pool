# event_pool

## 基本介绍：

a header-only event-driven library based on c++11.

一个基于c++11标准，仅需要头文件的事件驱动库:)。

## 使用方法：

* 创建event_pool对象并申请一个线程做事件处理，在该线程中调用run函数。
```
    //run the event_pool.
    std::shared_ptr<event_pool> ev(new event_pool());
    std::thread th([=]()->void {
        ev->run();
    });
```
* 创建event_handle和time_handle对象并设置id_，type_,回调函数func_，上下文args_（如果是time_handle则还要设置触发时间）等，push进event_pool对象。
```
    //create time_handle.
    std::shared_ptr<time_handle> h(new time_handle());
    h->id_ = "timer test ";
    h->type_ = time_handle::type::duration;
    h->duration_ = seconds(2);
    h->args_ = nullptr;
    h->func_ = [](std::shared_ptr<time_handle> self)->void {
            std::cout << self->id_ << " wake up !" << std::endl;
    };
    //create event_handle.
    std::shared_ptr<event_handle> eh(new event_handle());
    eh->id_ = "back cout ";
    eh->type_ = event_handle::type::every;
    eh->args_ = nullptr;
    eh->func_ = [](std::shared_ptr<event_handle> self)->void {
        std::cout << self->id_ << " wake up !"<<std::endl;
    };
    //push them into ev.
    ev->push_timer(h);
    ev->push_event(eh);
 ```
* 在需要触发事件的时候调用wake_up函数（time_handle没有wake_up函数，等待时间到达自动触发）。当需要关闭event_pool时，调用stop函数，然后回收线程，没有来得及处理的事件会被丢弃，即使当event_pool
对象完全销毁后，仍然可以调用wake_up函数，此时会直接返回。
 ```
       while (true) {
         char buf[1024];
         gets(buf);
         if (buf[0] == 'q') {
           ev->stop(); // stop the event_pool.
           break;
         }
         eh->wake_up();
       }
       th.join();
 ```
 
## 使用指南：

1. 所有对象均需使用std::shared_ptr创建。

2. 每个time_handle对象和event_handle对象只能push进一个event_pool对象。

3. event_handle对象可设置两种类型：every和once，every类型允许不限次数的wake_up,event_pool会处理每次wake_up，而once类型只能被唤醒一次，但允许多次调用wake_up函数（线程安全），这意味着可以在多个线程并发的触发事件。

4. time_handle对象可设置两种类型：duration和time_point，其中duration类型通过设置duration_成员来指定从此刻开始，每间隔多少时间就触发一次。time_point类型通过设置time_point_成员来指定在哪个时刻仅触发一次。

5. 回调函数的输入参数就是该事件对象本身，你可以通过其访问设置的id_，type_，args_等等。

6. event_pool的run函数可以在多个线程并发执行（maybe？），这一点暂且不保证。

## 特点：

1.轻量级，200行源代码，语言层面的跨平台，基于c++11标准。

2.仅需要头文件，即拿即用。

## todo:
1. 定义更便于使用，减少出错概率的接口。
2. 补充测试。
