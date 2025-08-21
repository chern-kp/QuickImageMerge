#pragma once

#include <QMainWindow>

// For drag and drop events
class QDragEnterEvent;
class QDropEvent;

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
    // Called when a drag operation enters the widget
    void dragEnterEvent(QDragEnterEvent *event) override;
    // Called when a drop is performed
    void dropEvent(QDropEvent *event) override;
    // Called when an event is filtered
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void on_deleteImageButton_clicked(); // Slot for 'deleteImageButton' button
    void on_upButton_clicked(); // Slot for 'upButton' button
    void on_downButton_clicked(); // Slot for 'downButton' button
    void on_mergeButton_clicked(); // Slot for 'mergeButton' button
    void on_optionOrientation_currentIndexChanged(int index); // Slot for 'optionOrientation' combo box (dropdown menu)



private:
    // A helper function to keep the code clean
    void openFileDialog();

    void addFilesToList(const QStringList &paths);


private:
    Ui::MainWindow *ui;
};