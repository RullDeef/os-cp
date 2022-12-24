#include <limits>
#include <iostream>
#include "main_window.h"

MainWindow::MainWindow()
{
    ui.setupUi(this);

    // setup label map
    label_map.reserve(6);

    { // row #1
        std::vector<QLabel*> row;
        row.reserve(14);

        row.push_back(ui.label_esc);
        row.push_back(ui.label_f1);
        row.push_back(ui.label_f2);
        row.push_back(ui.label_f3);
        row.push_back(ui.label_f4);
        row.push_back(ui.label_f5);
        row.push_back(ui.label_f6);
        row.push_back(ui.label_f7);
        row.push_back(ui.label_f8);
        row.push_back(ui.label_f9);
        row.push_back(ui.label_f10);
        row.push_back(ui.label_f11);
        row.push_back(ui.label_f12);
        row.push_back(ui.label_delete);

        label_map.push_back(std::move(row));
    }

    { // row #2
        std::vector<QLabel*> row;
        row.reserve(14);

        row.push_back(ui.label_14);
        row.push_back(ui.label_1);
        row.push_back(ui.label_2);
        row.push_back(ui.label_3);
        row.push_back(ui.label_4);
        row.push_back(ui.label_5);
        row.push_back(ui.label_6);
        row.push_back(ui.label_7);
        row.push_back(ui.label_8);
        row.push_back(ui.label_9);
        row.push_back(ui.label_0);
        row.push_back(ui.label_minus);
        row.push_back(ui.label_equal);
        row.push_back(ui.label_backspace);

        label_map.push_back(std::move(row));
    }
    
    { // row #3
        std::vector<QLabel*> row;
        row.reserve(14);

        row.push_back(ui.label_tab);
        row.push_back(ui.label_q);
        row.push_back(ui.label_w);
        row.push_back(ui.label_e);
        row.push_back(ui.label_r);
        row.push_back(ui.label_t);
        row.push_back(ui.label_y);
        row.push_back(ui.label_u);
        row.push_back(ui.label_i);
        row.push_back(ui.label_o);
        row.push_back(ui.label_p);
        row.push_back(ui.label_leftbrace);
        row.push_back(ui.label_rightbrace);
        row.push_back(ui.label_backslash);

        label_map.push_back(std::move(row));
    }

    { // row #4
        std::vector<QLabel*> row;
        row.reserve(13);

        row.push_back(ui.label_capslock);
        row.push_back(ui.label_a);
        row.push_back(ui.label_s);
        row.push_back(ui.label_d);
        row.push_back(ui.label_f);
        row.push_back(ui.label_g);
        row.push_back(ui.label_h);
        row.push_back(ui.label_j);
        row.push_back(ui.label_k);
        row.push_back(ui.label_l);
        row.push_back(ui.label_semicolon);
        row.push_back(ui.label_apostrophe);
        row.push_back(ui.label_enter);

        label_map.push_back(std::move(row));
    }

    { // row #5
        std::vector<QLabel*> row;
        row.reserve(12);

        row.push_back(ui.label_leftshift);
        row.push_back(ui.label_z);
        row.push_back(ui.label_x);
        row.push_back(ui.label_c);
        row.push_back(ui.label_v);
        row.push_back(ui.label_b);
        row.push_back(ui.label_n);
        row.push_back(ui.label_m);
        row.push_back(ui.label_comma);
        row.push_back(ui.label_dot);
        row.push_back(ui.label_slash);
        row.push_back(ui.label_rightshift);

        label_map.push_back(std::move(row));
    }

    { // row #6
        std::vector<QLabel*> row;
        row.reserve(11);

        row.push_back(ui.label_leftctrl);
        row.push_back(ui.label_55);
        row.push_back(ui.label_macro);
        row.push_back(ui.label_leftalt);
        row.push_back(ui.label_space);
        row.push_back(ui.label_rightalt);
        row.push_back(ui.label_rightctrl);
        row.push_back(ui.label_left);
        row.push_back(ui.label_up);
        row.push_back(ui.label_down);
        row.push_back(ui.label_right);

        label_map.push_back(std::move(row));
    }

    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint);
}

void MainWindow::highlightKey(unsigned int row, unsigned int col)
{
    static unsigned int prev_row = std::numeric_limits<unsigned int>::max();
    static unsigned int prev_col = std::numeric_limits<unsigned int>::max();

    // shadow previous key
    if (prev_row != std::numeric_limits<unsigned int>::max())
        label_map[prev_row][prev_col]->setStyleSheet("");

    // highlight new one
    label_map[row][col]->setStyleSheet("QLabel { background-color : rgba(255, 255, 255, 0.2); color : rgb(200, 255, 200); }");

    prev_row = row;
    prev_col = col;
}
