#include "canvas.h"
#include "ui_canvas.h"
#include <QPainter>
#include <QPen>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QEvent>
#include <QShortcut>
#include <QKeySequence>

Canvas::Canvas(QWidget *parent, bool isDark)
    : QWidget(parent)
    , ui(new Ui::Canvas)
    , m_isDark(isDark)
{
    ui->setupUi(this);

    ui->drawingArea->installEventFilter(this);
    ui->drawingArea->setAttribute(Qt::WA_OpaquePaintEvent, true);

    penWidth = 4;

    // 1. Atur tema dan buat buffer QPixmap pertama kali
    setupTheme(m_isDark);

    // 2. Hubungkan Tombol Fisik UI ke Fungsi C++
    connect(ui->clearButton, &QPushButton::clicked, this, &Canvas::clearCanvas);
    connect(ui->saveAsButton, &QPushButton::clicked, this, &Canvas::saveImage);
    connect(ui->undoButton, &QPushButton::clicked, this, &Canvas::undo);
    connect(ui->redoButton, &QPushButton::clicked, this, &Canvas::redo);
    connect(ui->OpenButton, &QPushButton::clicked, this, &Canvas::openImage);

    // 3. Aktifkan sistem Shortcut Keyboard (Menggunakan gaya modern, bukan SLOT)
    setupShortcuts();

    // NOTED: saveCurrentState() di sini didelete karena sudah dipanggil di dalam setupTheme()
}

Canvas::~Canvas()
{
    delete ui;
}

void Canvas::setupShortcuts()
{
    // Gunakan koneksi pointer fungsi modern agar terhindar dari bug MOC Qt
    QShortcut *shortSave = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_S), this);
    connect(shortSave, &QShortcut::activated, this, &Canvas::saveImage);

    QShortcut *shortOpen = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_O), this);
    connect(shortOpen, &QShortcut::activated, this, &Canvas::openImage);

    QShortcut *shortUndo = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Z), this);
    connect(shortUndo, &QShortcut::activated, this, &Canvas::undo);

    QShortcut *shortRedo = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Y), this);
    connect(shortRedo, &QShortcut::activated, this, &Canvas::redo);
}

void Canvas::setupTheme(bool isDark)
{
    m_isDark = isDark;

    if (m_isDark) {
        canvasBgColor = QColor(25, 25, 25);
        penColor = Qt::yellow;
        this->setStyleSheet("Canvas { background-color: #282828; }");
    } else {
        canvasBgColor = Qt::white;
        penColor = QColor(30, 30, 30);
        this->setStyleSheet("Canvas { background-color: #F5F5F5; }");
    }

    ui->drawingArea->setStyleSheet(QString("background-color: %1;").arg(canvasBgColor.name()));

    // Alokasikan ulang buffer gambar agar ukurannya pas dan bersih sewarna tema
    pixmap = QPixmap(800, 600);
    pixmap.fill(canvasBgColor);

    // Reset history agar tidak membawa sampah coretan dari tema sebelumnya
    undoStack.clear();
    redoStack.clear();
    saveCurrentState(); // Ambil snapshot pertama kondisi kanvas kosong

    ui->drawingArea->update();
}

void Canvas::saveCurrentState()
{
    if (undoStack.size() >= MAX_HISTORY) {
        undoStack.removeFirst();
    }
    undoStack.append(pixmap.toImage());
}

bool Canvas::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->drawingArea) {
        if (event->type() == QEvent::MouseButtonPress) {
            handleMousePress(static_cast<QMouseEvent*>(event));
            return true;
        }
        else if (event->type() == QEvent::MouseMove) {
            handleMouseMove(static_cast<QMouseEvent*>(event));
            return true;
        }
        else if (event->type() == QEvent::MouseButtonRelease) {
            handleMouseRelease(static_cast<QMouseEvent*>(event));
            return true;
        }
        else if (event->type() == QEvent::Paint) {
            handlePaint();
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void Canvas::handleMousePress(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        lastPoint = event->pos();
        redoStack.clear();
    }
}

void Canvas::handleMouseMove(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        QPainter painter(&pixmap);
        QPen pen(penColor, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);

        painter.drawLine(lastPoint, event->pos());
        lastPoint = event->pos();
        ui->drawingArea->update();
    }
}

void Canvas::handleMouseRelease(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        saveCurrentState();
    }
}

void Canvas::handlePaint()
{
    QPainter painter(ui->drawingArea);
    painter.drawPixmap(0, 0, pixmap);
}

void Canvas::undo()
{
    if (undoStack.size() > 1) {
        redoStack.append(undoStack.takeLast());
        pixmap = QPixmap::fromImage(undoStack.last());
        ui->drawingArea->update();
    }
}

void Canvas::redo()
{
    if (!redoStack.isEmpty()) {
        QImage nextState = redoStack.takeLast();
        undoStack.append(nextState);
        pixmap = QPixmap::fromImage(nextState);
        ui->drawingArea->update();
    }
}

void Canvas::clearCanvas()
{
    pixmap.fill(canvasBgColor);
    saveCurrentState();
    ui->drawingArea->update();
}

void Canvas::openImage()
{
    QString defaultPath = "C:/Ini_Nama_Aplikasi_Notepad/Canvas";

    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Open Drawing"),
        defaultPath,
        tr("Images (*.png *.jpg *.jpeg);;All Files (*)")
        );

    if (!filePath.isEmpty()) {
        QPixmap loadedPixmap;
        if (loadedPixmap.load(filePath)) {
            pixmap = loadedPixmap.scaled(800, 600, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

            undoStack.clear();
            redoStack.clear();
            saveCurrentState();

            ui->drawingArea->update();
            QMessageBox::information(this, "Success", "Gambar berhasil dimuat ke Kanvas!");

            // =========================================================================
            // INTEGRASI LOG: CATAT AKTIVITAS OPEN GAMBAR KANVAS
            // =========================================================================
            QString logPath = "C:/Ini_Nama_Aplikasi_Notepad/Files/savelog.txt";
            QFile logFile(logPath);
            if (logFile.open(QFile::WriteOnly | QFile::Append | QFile::Text)) {
                QTextStream logOut(&logFile);
                QString waktuSekarang = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
                QFileInfo fileInfo(filePath);

                logOut << "[" << waktuSekarang << "] [Canvas] Berhasil membuka gambar: " << fileInfo.fileName() << "\n";
                logFile.close();
            }

        } else {
            QMessageBox::warning(this, "Error", "Gagal membuka file gambar.");
        }
    }
}

void Canvas::saveImage()
{
    QString defaultPath = "C:/Ini_Nama_Aplikasi_Notepad/Canvas";
    QDir dir(defaultPath);
    if (!dir.exists()) dir.mkpath(".");

    QString filePath = QFileDialog::getSaveFileName(
        this, tr("Save Drawing"), defaultPath,
        tr("PNG Files (*.png);;JPEG Files (*.jpg *.jpeg);;All Files (*)")
        );

    if (!filePath.isEmpty()) {
        if (pixmap.save(filePath)) {
            QMessageBox::information(this, "Success", "Gambar coretan berhasil disimpan!");

            // =========================================================================
            // INTEGRASI LOG: CATAT AKTIVITAS SAVE GAMBAR KANVAS
            // =========================================================================
            QString logPath = "C:/Ini_Nama_Aplikasi_Notepad/Files/savelog.txt";
            QFile logFile(logPath);
            if (logFile.open(QFile::WriteOnly | QFile::Append | QFile::Text)) {
                QTextStream logOut(&logFile);
                QString waktuSekarang = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
                QFileInfo fileInfo(filePath);

                logOut << "[" << waktuSekarang << "] [Canvas] Berhasil menyimpan gambar: " << fileInfo.fileName() << "\n";
                logFile.close();
            }

        } else {
            QMessageBox::warning(this, "Error", "Gagal menyimpan gambar.");
        }
    }
}