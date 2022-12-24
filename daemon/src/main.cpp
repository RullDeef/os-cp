#include <fcntl.h>
#include <iostream>
#include <QApplication>
#include "main_window.h"
#include "jskbd_event.h"
#include "itc_data.h"
#include "worker.h"

int main(int argc, char *argv[])
{
    auto data = std::make_shared<itc_data>();
    auto worker = Worker(data);

    data->fd = open("/proc/joystick_kbd", O_RDONLY);
    if (data->fd == -1)
    {
        std::cerr << "kernel module is not loaded or gamepad is not connected\n";
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
