#include <iostream>
#include "event_pool.h"

int main()
{
    //run the event_pool.
    std::shared_ptr<event_pool> ev(new event_pool());
    std::thread th([=]()->void {
        ev->run();
    });

    using namespace std::chrono;
    //create time_handle.
    for(int i = 1;i<10;++i) {
        std::shared_ptr<time_handle> h(new time_handle());
        h->id_ = "timer test " + std::to_string(i);
        h->type_ = time_handle::type::timepoint;
        h->time_point_ = time_point_cast<milliseconds>(system_clock::now()) + seconds(1);
        std::shared_ptr<std::promise<void>> ptr(new std::promise<void>());
        h->args_ = ptr;
        auto it = ptr->get_future();
        h->func_ = [](std::shared_ptr<time_handle> self)->void {
                std::cout << self->id_ << " wake up !" << std::endl;
                static_cast<std::promise<void>*>(self->args_.get())->set_value();
        };
        ev->push_timer(h);
        it.get();
    }
    //create event_handle.
    std::shared_ptr<event_handle> eh(new event_handle());
    eh->id_ = "back cout ";
    eh->type_ = event_handle::type::every;
    eh->args_ = nullptr;
    eh->func_ = [](std::shared_ptr<event_handle> self)->void {
        std::cout << self->id_ << " wake up !"<<std::endl;
    };

    //push them into ev.
    ev->push_event(eh);

    //try to wake up the event_handle.
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
    std::cout <<"over\n";
    return 0;
}
