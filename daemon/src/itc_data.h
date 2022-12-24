#pragma once

#include <memory>
#include "main_window.h"

// inter-thread communication data
struct itc_data
{
    int fd = -1; // event file descriptor
    std::unique_ptr<MainWindow> window = nullptr;
    bool window_shown = false;
};
