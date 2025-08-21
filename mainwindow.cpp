#include "mainwindow.h"
#include "imageprocessor.h"
#include "ui_mainwindow.h"

#include <QEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setAcceptDrops(true);

    ui->fileSelectionZone->installEventFilter(this);

    connect(ui->optionOrientation, &QComboBox::currentIndexChanged, this, &MainWindow::on_optionOrientation_currentIndexChanged);
    on_optionOrientation_currentIndexChanged(ui->optionOrientation->currentIndex());

}

MainWindow::~MainWindow()
{
    delete ui;
}

// FUNC - Drag event (For Drag & Drop)
void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    // Check if the dragged data contains URLs (which files are)
    if (event->mimeData()->hasUrls()) {
        // If so, accept the proposed action (e.g., show a "+" cursor)
        event->acceptProposedAction();
    }
}

// FUNC - Drop event (For Drag & Drop)
void MainWindow::dropEvent(QDropEvent *event)
{
    // Get the list of URLs from the MIME data
    const QList<QUrl> urls = event->mimeData()->urls();
    QStringList filePaths;

    // Convert each QUrl to a local file path string
    for (const QUrl &url : urls) {
        filePaths.append(url.toLocalFile());
    }

    // Use our new helper function to process the files
    addFilesToList(filePaths);
}

// FUNC - Add Files to List
void MainWindow::addFilesToList(const QStringList &paths)
{
    QStringList supportedExtensions = {"png", "jpg", "jpeg", "bmp"};

    for (const QString &fullPath : paths) {
        QFileInfo fileInfo(fullPath);
        // Add only files with supported image extensions
        if (supportedExtensions.contains(fileInfo.suffix().toLower())) {
            auto *item = new QListWidgetItem(fileInfo.fileName());
            item->setData(Qt::UserRole, fullPath); // Store full path
            ui->fileListWidget->addItem(item);
        }
    }
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
        this, "Select Images", "", "Image Files (*.png *.jpg *.jpeg *.bmp)"
    );

    if (!filePaths.isEmpty()) {
        addFilesToList(filePaths);
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

// FUNC - Slot for the orientation combo box
void MainWindow::on_optionOrientation_currentIndexChanged(int index)
{
    ui->optionAlignment->clear(); // Remove old items
    QString currentOrientation = ui->optionOrientation->itemText(index);

    if (currentOrientation == "Vertical") {
        ui->optionAlignment->addItems({"Left", "Center", "Right"});
    } else { // Horizontal
        ui->optionAlignment->addItems({"Top", "Center", "Bottom"});
    }
}



// FUNC - Slot for the merge button.
void MainWindow::on_mergeButton_clicked()
{
    QStringList imagePaths;
    for (int i = 0; i < ui->fileListWidget->count(); ++i) {
        imagePaths.append(ui->fileListWidget->item(i)->data(Qt::UserRole).toString());
    }
    if (imagePaths.isEmpty()) {
        QMessageBox::warning(this, "No Images", "Please add images to the list first.");
        return;
    }

    // --- 1. Read options from ComboBoxes ---
    // Orientation
    Orientation orientation = (ui->optionOrientation->currentText() == "Vertical")
                                ? Orientation::Vertical
                                : Orientation::Horizontal;
    // Background Color
    QColor backgroundColor;
    QString bgColorText = ui->optionBackgroundColor->currentText();
    if (bgColorText == "Black")         backgroundColor = Qt::black;
    else if (bgColorText == "White")    backgroundColor = Qt::white;
    else                                backgroundColor = Qt::transparent; // Alpha Channel
    // Alignment
    Alignment alignment;
    QString alignText = ui->optionAlignment->currentText();
    if (alignText == "Left" || alignText == "Top")         alignment = Alignment::Left_Top;
    else if (alignText == "Center")                        alignment = Alignment::Center;
    else                                                   alignment = Alignment::Right_Bottom;


    // --- 2. Call the updated ImageProcessor ---
    ImageProcessor processor;
    QImage resultImage = processor.stitchImages(imagePaths, orientation, alignment, backgroundColor);

    if (resultImage.isNull()) {
        QMessageBox::critical(this, "Error", "Failed to create the merged image.");
        return;
    }

    // --- 3. Save the result (this part remains the same) ---
    QString savePath = QFileDialog::getSaveFileName(this, "Save Merged Image", "merged_image.png", "PNG Image (*.png);;JPEG Image (*.jpg *.jpeg)");
    if (savePath.isEmpty()) return;
    if (resultImage.save(savePath)) {
        QMessageBox::information(this, "Success", "Image saved successfully!");
    } else {
        QMessageBox::critical(this, "Error", "Failed to save the image.");
    }
}