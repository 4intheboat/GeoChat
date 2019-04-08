#ifndef LOGIN_WINDOW_H
#define LOGIN_WINDOW_H

#include <QMainWindow>
#include "../event_worker.hpp"
#include "MainWindow.hpp"

include <QDialog>

namespace Ui {
class login_window;
}

class login_window : public QDialog
{
    Q_OBJECT
    
public:
    explicit login_window(QWidget *parent = 0);
    ~login_window();
    getCommand(Event &&event);

private:
    Ui::login_window *ui;
};

#endif