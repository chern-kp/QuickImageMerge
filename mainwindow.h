#pragma once

#include <QMainWindow>

// Forward declarations
namespace Ui {
class MainWindow;
}
class QEvent;
class QObject;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    // We are overriding this function to watch for events
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void on_deleteImageButton_clicked();     // This slot will be automatically called when 'deleteImageButton' is clicked.


private:
    // A helper function to keep the code clean
    void openFileDialog();

private:
    Ui::MainWindow *ui;
};