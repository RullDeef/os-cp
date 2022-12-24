#pragma once

#include <QMainWindow>
#include "ui_keyboard.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

public slots:
    void highlightKey(unsigned int row, unsigned int col);

private:
    Ui::Keyboard ui;

    std::vector<std::vector<QLabel*>> label_map;
};
