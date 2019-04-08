#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../event_worker.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void getCommand(Event &&event);
private slots:
// here will be buttons command;
private:
    EventsWorker &Worker;
    Ui::MainWindow *ui;
}
#endif