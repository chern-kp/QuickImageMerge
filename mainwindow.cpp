#include "mainwindow.h"
#include "imageprocessor.h"
#include "ui_mainwindow.h"
#include <QEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

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

    // Check if the user selected any files
    if (!filePaths.isEmpty()) {
        // Iterate over each selected file path
        for (const QString &fullPath : filePaths) {
            QFileInfo fileInfo(fullPath); // Get file info for the path
            // Create a new list widget item with the file name
            QListWidgetItem *item = new QListWidgetItem(fileInfo.fileName());
            // Store the full file path in the item's user data
            item->setData(Qt::UserRole, fullPath);
            // Add the item to the file list widget
            ui->fileListWidget->addItem(item);
        }
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

// FUNC - Slot for the up button.
void MainWindow::on_upButton_clicked()
{
    // 1. Get the index of the currently selected row
    int currentRow = ui->fileListWidget->currentRow();

    // 2. Check if an item is selected and if it's not already at the top
    if (currentRow > 0) {
        // 3. Take the item out of the list. It's not deleted, we just hold a pointer to it.
        QListWidgetItem *currentItem = ui->fileListWidget->takeItem(currentRow);

        // 4. Insert the same item one position higher
        ui->fileListWidget->insertItem(currentRow - 1, currentItem);

        // 5. Re-select the item in its new position
        ui->fileListWidget->setCurrentRow(currentRow - 1);
    }
}

// FUNC - Slot for the down button.
void MainWindow::on_downButton_clicked()
{
    // 1. Get the index of the currently selected row
    int currentRow = ui->fileListWidget->currentRow();

    // 2. Check if an item is selected and if it's not already at the bottom
    // The last item's index is count() - 1
    if (currentRow != -1 && currentRow < ui->fileListWidget->count() - 1) {
        // 3. Take the item out of the list
        QListWidgetItem *currentItem = ui->fileListWidget->takeItem(currentRow);

        // 4. Insert the same item one position lower
        ui->fileListWidget->insertItem(currentRow + 1, currentItem);

        // 5. Re-select the item
        ui->fileListWidget->setCurrentRow(currentRow + 1);
    }
}

// FUNC - Slot for the merge button.
void MainWindow::on_mergeButton_clicked()
{
    // 1. Get the full paths from the list widget
    QStringList imagePaths;
    for (int i = 0; i < ui->fileListWidget->count(); ++i) {
        QListWidgetItem *item = ui->fileListWidget->item(i);
        // Retrieve the full path we stored earlier
        imagePaths.append(item->data(Qt::UserRole).toString());
    }

    if (imagePaths.isEmpty()) {
        QMessageBox::warning(this, "No Images", "Please add images to the list first.");
        return;
    }

    // 2. Call the ImageProcessor
    ImageProcessor processor;
    QImage resultImage = processor.stitchImagesVertically(imagePaths, Qt::black);

    if (resultImage.isNull()) {
        QMessageBox::critical(this, "Error", "Failed to create the merged image.");
        return;
    }

    // 3. Ask the user where to save the file
    QString savePath = QFileDialog::getSaveFileName(
        this,
        "Save Merged Image",
        "merged_image.png",
        "PNG Image (*.png);;JPEG Image (*.jpg *.jpeg)"
    );

    if (savePath.isEmpty()) {
        return; // User cancelled the dialog
    }

    // 4. Save the image and show a message
    if (resultImage.save(savePath)) {
        QMessageBox::information(this, "Success", "Image saved successfully to:\n" + savePath);
    } else {
        QMessageBox::critical(this, "Error", "Failed to save the image to:\n" + savePath);
    }
}