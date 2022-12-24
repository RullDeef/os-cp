#include <limits>
#include "worker.h"
#include "jskbd_event.h"

Worker::Worker(std::shared_ptr<itc_data> data)
    : data(data)
{
}

Worker::~Worker()
{
    if (thread != nullptr)
    {
        if (thread->joinable())
            thread->join();
    }
}

void Worker::start()
{
    if (thread == nullptr)
    {
        thread = std::make_unique<std::thread>([this]()
                                               { this->handler(); });
        thread->detach();
    }
}

void *Worker::handler()
{
    unsigned int last_row = std::numeric_limits<unsigned int>::max();
    unsigned int last_col = std::numeric_limits<unsigned int>::max();

    ssize_t res;
    joystick_kbd_event event;
    while ((res = read(data->fd, &event, sizeof(struct joystick_kbd_event))) != 0)
    {
        if (res == -1)
        {
            perror("read");
            break;
        }

        if (event_is_exit(event))
        {
            printf("exit event\n");
            break;
        }

        // show/hide on Y press
        if (event.y_released)
        {
            if (data->window_shown)
                emit hideWindow();
            else
                emit showWindow();

            data->window_shown = !data->window_shown;
        }

        // move virtual cursor
        if (last_row != event.keyboard_cursor_row || last_col != event.keyboard_cursor_col)
        {
            emit highlightKey(event.keyboard_cursor_row, event.keyboard_cursor_col);
            last_row = event.keyboard_cursor_row;
            last_col = event.keyboard_cursor_col;
        }
    }

    emit closeWindow();
    return nullptr;
}
