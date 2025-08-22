#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QMimeData>
#include <QMessageBox>
#include <QUrl>

const QStringList MainWindow::SUPPORTED_EXTENSIONS = {"png", "jpg", "jpeg", "bmp"};

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setAcceptDrops(true);
    ui->fileSelectionZone->installEventFilter(this);

    setupOptionControls();
    setupConnections();

    // Initialize last used directories
    m_lastOpenDirPath = QDir::homePath();
    m_lastSaveDirPath = QDir::homePath();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// SECTION - Setup Methods

void MainWindow::setupConnections()
{
    // Connect the orientation dropdown to our new slot
    connect(ui->optionOrientation, &QComboBox::currentIndexChanged, this, &MainWindow::on_optionOrientation_currentIndexChanged);
}

void MainWindow::setupOptionControls()
{
    // Populate orientation options, storing enum values in UserRole data
    ui->optionOrientation->addItem("Vertical", QVariant::fromValue(Orientation::Vertical));
    ui->optionOrientation->addItem("Horizontal", QVariant::fromValue(Orientation::Horizontal));

    // Populate background color options
    ui->optionBackgroundColor->addItem("White", QVariant::fromValue(QColor(Qt::white)));
    ui->optionBackgroundColor->addItem("Black", QVariant::fromValue(QColor(Qt::black)));
    ui->optionBackgroundColor->addItem("Transparent", QVariant::fromValue(QColor(Qt::transparent)));

    // Initial population of the alignment dropdown
    populateAlignmentOptions();
}

// !SECTION - Setup Methods
// SECTION - Event Handlers

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls()) {
        // If so, accept the proposed action (e.g., show a "+" cursor)
        event->acceptProposedAction();
    }
}

// FUNC - Drop event (For Drag & Drop)
void MainWindow::dropEvent(QDropEvent *event)
{
    QStringList filePaths;
    const QList<QUrl> urls = event->mimeData()->urls();
    for (const QUrl& url : urls) {
        if (url.isLocalFile()) {
            filePaths.append(url.toLocalFile());
        }
    }
    addFilesToList(filePaths);
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == ui->fileSelectionZone && event->type() == QEvent::MouseButtonPress) {
        openFileDialog();
        return true; // Event handled
    }
    return QMainWindow::eventFilter(watched, event);
}

// !SECTION - Event Handlers
// SECTION - UI Helper Methods

// FUNC - Add Files to List
void MainWindow::addFilesToList(const QStringList& paths)
{
    for (const QString& fullPath : paths) {
        QFileInfo fileInfo(fullPath);
        if (SUPPORTED_EXTENSIONS.contains(fileInfo.suffix().toLower())) {
            auto* item = new QListWidgetItem(fileInfo.fileName());
            item->setData(Qt::UserRole, fullPath); // Store full path
            item->setToolTip(fullPath); // Show full path on hover
            ui->fileListWidget->addItem(item);
        }
    }
}

// FUNC - Logic for opening the file dialog
void MainWindow::openFileDialog()
{
    QStringList filePaths = QFileDialog::getOpenFileNames(
        this, "Select Images", m_lastOpenDirPath, "Image Files (*.png *.jpg *.jpeg *.bmp)");
    if (!filePaths.isEmpty()) {
        // Update the last used directory for opening files
        QFileInfo fileInfo(filePaths.first());
        m_lastOpenDirPath = fileInfo.absoluteDir().absolutePath();
        addFilesToList(filePaths);
    }
}

// !SECTION - UI Helper Methods
// SECTION - Slots for UI actions

// FUNC - Slot for the delete button.
void MainWindow::on_deleteImageButton_clicked()
{
    qDeleteAll(ui->fileListWidget->selectedItems());
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
        QListWidgetItem* currentItem = ui->fileListWidget->takeItem(currentRow);
        ui->fileListWidget->insertItem(currentRow + 1, currentItem);
        ui->fileListWidget->setCurrentRow(currentRow + 1);
    }
}

void MainWindow::on_optionOrientation_currentIndexChanged(int index)
{
    // This slot handles changes to the orientation option.
    // It updates the available alignment options based on the selected orientation.
    populateAlignmentOptions();
}

void MainWindow::populateAlignmentOptions()
{
    ui->optionAlignment->clear();
    // Get the orientation enum directly from the item's data
    auto orientation = ui->optionOrientation->currentData().value<Orientation>();

    if (orientation == Orientation::Vertical) {
        ui->optionAlignment->addItem("Left", QVariant::fromValue(CrossAxisAlignment::Start));
        ui->optionAlignment->addItem("Center", QVariant::fromValue(CrossAxisAlignment::Center));
        ui->optionAlignment->addItem("Right", QVariant::fromValue(CrossAxisAlignment::End));
    } else { // Horizontal
        ui->optionAlignment->addItem("Top", QVariant::fromValue(CrossAxisAlignment::Start));
        ui->optionAlignment->addItem("Center", QVariant::fromValue(CrossAxisAlignment::Center));
        ui->optionAlignment->addItem("Bottom", QVariant::fromValue(CrossAxisAlignment::End));
    }
}

// !SECTION - UI Helper Methods
// SECTION - Core Logic Slot

void MainWindow::on_mergeButton_clicked()
{
    processAndSaveImages();
}

QStringList MainWindow::getImagePathsFromList() const
{
    QStringList imagePaths;
    for (int i = 0; i < ui->fileListWidget->count(); ++i) {
        imagePaths.append(ui->fileListWidget->item(i)->data(Qt::UserRole).toString());
    }
    return imagePaths;
}

void MainWindow::processAndSaveImages()
{
    const QStringList imagePaths = getImagePathsFromList();
    if (imagePaths.isEmpty()) {
        QMessageBox::warning(this, "No Images", "Please add images to the list first.");
        return;
    }

    // 1. Get options from UI controls
    const auto orientation = ui->optionOrientation->currentData().value<Orientation>();
    const auto alignment = ui->optionAlignment->currentData().value<CrossAxisAlignment>();
    const auto backgroundColor = ui->optionBackgroundColor->currentData().value<QColor>();

    // 2. Call the processor
    QImage resultImage = m_imageProcessor.stitchImages(imagePaths, orientation, alignment, backgroundColor);

    if (resultImage.isNull()) {
        QMessageBox::critical(this, "Error", "Failed to create the merged image. Check if all images are valid.");
        return;
    }

    // 3. Save the result
    if (!saveImageToFile(resultImage)) {
         QMessageBox::critical(this, "Error", "Failed to save the image.");
    } else {
        QMessageBox::information(this, "Success", "Image saved successfully!");
    }
}

bool MainWindow::saveImageToFile(const QImage& image)
{
    QString savePath = QFileDialog::getSaveFileName(this,
                                                    "Save Merged Image",
                                                    m_lastSaveDirPath + "/merged_image.png", // Use last save path
                                                    "PNG Image (*.png);;JPEG Image (*.jpg *.jpeg)");
    if (savePath.isEmpty()) {
        return false; // User cancelled
    }

    if (image.save(savePath)) {
        // Update the last used directory for saving files
        QFileInfo fileInfo(savePath);
        m_lastSaveDirPath = fileInfo.absoluteDir().absolutePath();
        return true;
    }
    return false;
}
// !SECTION - Core Logic Slot
