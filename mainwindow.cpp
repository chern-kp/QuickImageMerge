#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QEvent>
#include <QFileDialog>
#include <QFileInfo>

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
        // Create a new list that will hold just the file names
        QStringList fileNames;

        // Iterate through each full path provided by the dialog
        for (const QString &fullPath : filePaths) {
            // Use QFileInfo to easily extract just the file name
            QFileInfo fileInfo(fullPath);
            fileNames.append(fileInfo.fileName());
        }

        // Add all the extracted file names to our list widget in one go
        ui->fileListWidget->addItems(fileNames);
    }
}

// FUNC - Slot for the delete button.
void MainWindow::on_deleteImageButton_clicked()
{
    // QListWidget::selectedItems() returns a list of all selected items.
    // Even if we are in single-selection mode, it still returns a list (with one item).
    QList<QListWidgetItem *> selectedItems = ui->fileListWidget->selectedItems();

    // Check if the list is not empty (i.e., the user has selected something)
    if (!selectedItems.isEmpty()) {
        qDeleteAll(selectedItems);
    }
}