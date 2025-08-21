#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QEvent>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->fileSelectionZone->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// FUNC - Event filter function
bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    // 1. Check if the event is coming from our specific label
    if (watched == ui->fileSelectionZone) {
        // 2. Check if the event is a mouse button press
        if (event->type() == QEvent::MouseButtonPress) {
            // If it's a click on our zone, open the file dialog.
            openFileDialog();
            return true; // We handled the event, no one else should.
        }
    }

    // For all other events, pass them to the base class implementation
    return QMainWindow::eventFilter(watched, event);
}

// FUNC - Logic for opening the file dialog
void MainWindow::openFileDialog()
{
    QStringList filePaths = QFileDialog::getOpenFileNames(
        this,
        "Select Images",
        "", // Start in the default directory
        "Image Files (*.png *.jpg *.jpeg *.bmp)"
    );

    if (!filePaths.isEmpty()) {
        // TODO
    }
}
