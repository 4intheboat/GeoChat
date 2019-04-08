#ifndef REGISTER_WINDOW_H
#define REGISTER_WINDOW_H

#include <QMainWindow>
#include "../event_worker.hpp"
#include "MainWindow.hpp"

include <QDialog>

namespace Ui {
class register_window;
}

class register_window : public QDialog
{
    Q_OBJECT
    
public:
    explicit register_window(QWidget *parent = 0);
    ~register_window();
    getCommand(Event &&event);

private:
    Ui::register_window *ui;
};

#endif