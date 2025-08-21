#pragma once

#include <QMainWindow>
#include "imageprocessor.h"

// Forward declarations
namespace Ui {
class MainWindow;
}
class QDragEnterEvent;
class QDropEvent;
class QEvent;
class QObject;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void on_deleteImageButton_clicked();
    void on_upButton_clicked();
    void on_downButton_clicked();
    void on_mergeButton_clicked();
    void on_optionOrientation_currentIndexChanged(int index);

private:
    // Setup methods
    void setupConnections();
    void setupOptionControls();

    // Helper methods for UI logic
    void openFileDialog();
    void addFilesToList(const QStringList& paths);
    void populateAlignmentOptions();
    QStringList getImagePathsFromList() const;

    // Helper method for core logic
    void processAndSaveImages();
    bool saveImageToFile(const QImage& image);

private:
    Ui::MainWindow* ui;
    ImageProcessor m_imageProcessor;

    static const QStringList SUPPORTED_EXTENSIONS;
};
