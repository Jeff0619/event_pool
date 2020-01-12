#include <iostream>
#include "event_pool.h"

int main()
{
    //run the event_pool.
    std::shared_ptr<event_pool> ev(new event_pool());
    std::thread th([=]()->void {
        ev->run();
    });

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
