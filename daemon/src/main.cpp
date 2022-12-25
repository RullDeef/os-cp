#include <fcntl.h>
#include <iostream>
#include <thread>
#include <QApplication>
#include "main_window.h"
#include "jskbd_event.h"
#include "itc_data.h"
#include "worker.h"

constexpr auto joystick_path = "/proc/joystick_kbd";

int try_open()
{
    while (true)
    {
        int fd = open(joystick_path, O_RDONLY);
        if (fd != -1)
            return fd;

        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}

int main(int argc, char *argv[])
{
    auto data = std::make_shared<itc_data>();
    auto worker = Worker(data);

    data->fd = try_open();
    if (data->fd == -1)
    {
        std::cerr << "failed to open /proc/joystick_kbd\n";
        return -1;
    }

    QApplication app(argc, argv);
    data->window = std::make_unique<MainWindow>();
    data->window->show();
    data->window->hide();

    worker.connect(&worker, &Worker::showWindow, data->window.get(), &MainWindow::show);
    worker.connect(&worker, &Worker::hideWindow, data->window.get(), &MainWindow::hide);
    worker.connect(&worker, &Worker::closeWindow, data->window.get(), &MainWindow::close);
    worker.connect(&worker, &Worker::highlightKey, data->window.get(), &MainWindow::highlightKey);

    worker.start();
    exit(app.exec());
}
