#pragma once

#include <thread>
#include <QObject>
#include "itc_data.h"

class Worker : public QObject
{
    Q_OBJECT

public:
    Worker(std::shared_ptr<itc_data> data);
    ~Worker();

    void start();

signals:
    void showWindow();
    void hideWindow();
    void closeWindow();

    void highlightKey(unsigned int row, unsigned int col);

private:
    void *handler();

    std::shared_ptr<itc_data> data;
    std::unique_ptr<std::thread> thread;
};
