#pragma once

#include <QMainWindow>

// Forward declaration of the class that will be generated from mainwindow.ui
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui; // Pointer to the UI class generated from mainwindow.ui
};