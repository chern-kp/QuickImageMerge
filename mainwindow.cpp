#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCoreApplication>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QMimeData>
#include <QMessageBox>
#include <QUrl>
#include <QDir>
#include <QSettings>

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
    QSettings settings("MySoft", "QuickImageMerge");
    m_lastOpenDirPath = settings.value("lastOpenDir", QDir::homePath()).toString();
    m_lastSaveDirPath = settings.value("lastSaveDir", QDir::homePath()).toString();

    // Allow multi-selection with Ctrl/Shift
    ui->fileListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

    // Enable drag and drop for reordering
    ui->fileListWidget->setDragDropMode(QAbstractItemView::InternalMove);
    ui->fileListWidget->setDropIndicatorShown(true);
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

        // Save the path for future sessions
        QSettings settings("MySoft", "QuickImageMerge");
        settings.setValue("lastOpenDir", m_lastOpenDirPath);

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
    QList<QListWidgetItem*> selection = ui->fileListWidget->selectedItems();
    if (selection.isEmpty()) {
        return;
    }

    // Get all selected rows and sort them
    QList<int> selectedRows;
    for (QListWidgetItem* item : selection) {
        selectedRows.append(ui->fileListWidget->row(item));
    }
    std::sort(selectedRows.begin(), selectedRows.end());

    // If the top-most item is already at the top, we can't move up
    if (selectedRows.first() == 0) {
        return;
    }

    // Disable signals to avoid performance issues and unwanted slot calls
    ui->fileListWidget->blockSignals(true);

    // Move each selected item up by one position
    for (int row : selectedRows) {
        QListWidgetItem* item = ui->fileListWidget->takeItem(row);
        ui->fileListWidget->insertItem(row - 1, item);
        item->setSelected(true);
    }

    // Re-enable signals
    ui->fileListWidget->blockSignals(false);

    // Ensure the first moved item is visible, without changing the selection
    QListWidgetItem* firstMovedItem = ui->fileListWidget->item(selectedRows.first() - 1);
    if (firstMovedItem) {
        ui->fileListWidget->scrollToItem(firstMovedItem);
    }
}

// FUNC - Slot for the down button.
void MainWindow::on_downButton_clicked()
{
    QList<QListWidgetItem*> selection = ui->fileListWidget->selectedItems();
    if (selection.isEmpty()) {
        return;
    }

    // Get all selected rows and sort them
    QList<int> selectedRows;
    for (QListWidgetItem* item : selection) {
        selectedRows.append(ui->fileListWidget->row(item));
    }
    // Sort descending for moving down
    std::sort(selectedRows.begin(), selectedRows.end(), std::greater<int>());

    // If the bottom-most item is already at the bottom, we can't move down
    if (selectedRows.first() == ui->fileListWidget->count() - 1) {
        return;
    }

    ui->fileListWidget->blockSignals(true);

    // Move each selected item down by one position
    for (int row : selectedRows) {
        QListWidgetItem* item = ui->fileListWidget->takeItem(row);
        ui->fileListWidget->insertItem(row + 1, item);
        item->setSelected(true);
    }

    ui->fileListWidget->blockSignals(false);

    // Ensure the last moved item is visible, without changing the selection
    // `selectedRows` is sorted descending, so `first()` gives us the original bottom-most row
    QListWidgetItem* lastMovedItem = ui->fileListWidget->item(selectedRows.first() + 1);
    if (lastMovedItem) {
        ui->fileListWidget->scrollToItem(lastMovedItem);
    }
}

//FUNC - Slot for the "Add to Context Menu" button.
void MainWindow::on_addToContextMenuButton_clicked()
{
    const QString appPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    const QString menuName = "Add to Quick Image Merge";
    bool success = true;

    // Check if the context menu item exists for the first supported extension. (We assume that if it exists for one, it exists for all.)
    QString checkKeyPath = QString("HKEY_CLASSES_ROOT\\SystemFileAssociations\\.%1\\shell\\%2").arg(SUPPORTED_EXTENSIONS.first(), menuName);
    QSettings checkSettings(checkKeyPath, QSettings::NativeFormat);

    if (checkSettings.childGroups().isEmpty() && checkSettings.allKeys().isEmpty()) {
        // Add context menu items
        for (const QString& ext : SUPPORTED_EXTENSIONS) {
            QString keyPath = QString("HKEY_CLASSES_ROOT\\SystemFileAssociations\\.%1\\shell\\%2").arg(ext, menuName);
            QSettings settings(keyPath, QSettings::NativeFormat);

            settings.setValue("Icon", appPath);
            settings.setValue("FriendlyAppName", "Quick Image Merge");

            QString commandKeyPath = keyPath + "\\command";
            QSettings commandSettings(commandKeyPath, QSettings::NativeFormat);
            commandSettings.setValue(".", QString("\"%1\" \"%2\"").arg(appPath, "%1"));

            if (settings.status() != QSettings::NoError || commandSettings.status() != QSettings::NoError) {
                success = false;
            }
        }

        if (success) {
            QMessageBox::information(this, "Success", "Context menu item added successfully!");
        } else {
            QMessageBox::critical(this, "Error", "Failed to add context menu item. Please try running as administrator.");
        }
    } else {
        // Remove context menu items
        QMessageBox msgBox;
        msgBox.setText("The context menu item already exists.");
        msgBox.setInformativeText("Do you want to remove it?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        msgBox.setIcon(QMessageBox::Question);

        if (msgBox.exec() == QMessageBox::Yes) {
            for (const QString& ext : SUPPORTED_EXTENSIONS) {
                QString keyPath = QString("HKEY_CLASSES_ROOT\\SystemFileAssociations\\.%1\\shell\\%2").arg(ext, menuName);
                QSettings settings(keyPath, QSettings::NativeFormat);
                settings.remove("");

                if (settings.status() != QSettings::NoError) {
                    success = false;
                }
            }

            if (success) {
                QMessageBox::information(this, "Success", "Context menu item removed successfully!");
            } else {
                QMessageBox::critical(this, "Error", "Failed to remove context menu item. Please try running as administrator.");
            }
        }
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
    const bool quickSave = ui->optionQuickSaveCheckBox->isChecked();
    if (saveImageToFile(resultImage, quickSave)) {
        if (!quickSave) { // Show success message only if not quick saving
            QMessageBox::information(this, "Success", "Image saved successfully!");
        }
    }
}

bool MainWindow::saveImageToFile(const QImage& image, bool quickSave)
{
    QString savePath;
    if (quickSave) {
        // Automatic saving to the last used directory
        QString baseName = "merged_image";
        QString suffix = ".png";
        int counter = 0;
        do {
            savePath = m_lastSaveDirPath + "/" + baseName;
            if (counter > 0) {
                savePath += QString(" (%1)").arg(counter);
            }
            savePath += suffix;
            counter++;
        } while (QFile::exists(savePath)); // Check if a file with this name already exists
    } else {
        savePath = QFileDialog::getSaveFileName(
            this,
            "Save Merged Image",
            m_lastSaveDirPath + "/merged_image.png", // Use last save path
            "PNG Image (*.png);;JPEG Image (*.jpg *.jpeg)");
    }

    if (savePath.isEmpty()) {
        return false; // User cancelled, do nothing.
    }

    if (image.save(savePath)) {
        // Update the last used directory for saving files
        QFileInfo fileInfo(savePath);
        m_lastSaveDirPath = fileInfo.absoluteDir().absolutePath();

        // Save the path for future sessions
        QSettings settings("MySoft", "QuickImageMerge");
        settings.setValue("lastSaveDir", m_lastSaveDirPath);
        return true;
    } else {
        // Real I/O error, inform the user.
        QMessageBox::critical(this, "Error", "Failed to save the image to disk.");
        return false;
    }
}
// !SECTION - Core Logic Slot
