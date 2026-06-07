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
#include <QCloseEvent>
#include <QMessageBox>


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
    // 4. Konfigurasi dan Hubungkan Horizontal Slider untuk Ukuran Brush
    ui->horizontalSlider->setRange(1, 100);
    ui->horizontalSlider->setValue(4);

    ui->brushSizeLabel->setText(QString("Size: %1px").arg(ui->horizontalSlider->value()));

    // Panggil sekali saat start agar preview langsung muncul
    updateBrushPreview();

    connect(ui->horizontalSlider, &QSlider::valueChanged, this, [this](int value) {
        this->penWidth = value;
        ui->brushSizeLabel->setText(QString("Size: %1px").arg(value));
        this->updateBrushPreview(); // <--- UPDATE VISUAL LINGKARAN
    });

    // 5. Hubungkan CheckBox untuk Fitur Penghapus (Eraser)
    connect(ui->checkBox, &QCheckBox::toggled, this, [this](bool checked) {
        if (checked) {
            ui->horizontalSlider->setValue(20);
        } else {
            ui->horizontalSlider->setValue(4);
        }
        this->updateBrushPreview(); // <--- UPDATE VISUAL LINGKARAN SAAT BERUBAH JADI ERASER
        ui->drawingArea->update();
    });
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
        m_isModified = true; // Tandai bahwa kanvas sudah berubah
    }
}

void Canvas::handleMouseMove(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        // Hitung rasio
        double scaleX = (double)pixmap.width() / ui->drawingArea->width();
        double scaleY = (double)pixmap.height() / ui->drawingArea->height();

        // Map koordinat mouse ke koordinat pixmap
        QPoint currentPoint = QPoint(event->pos().x() * scaleX, event->pos().y() * scaleY);
        QPoint prevPoint = QPoint(lastPoint.x() * scaleX, lastPoint.y() * scaleY);

        QPainter painter(&pixmap);

        // ... (sisanya sama, gunakan activeColor)
        QColor activeColor = ui->checkBox->isChecked() ? canvasBgColor : penColor;
        QPen pen(activeColor, penWidth * scaleX, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin); // Scale penWidth juga agar konsisten

        painter.setPen(pen);
        painter.drawLine(prevPoint, currentPoint);

        lastPoint = event->pos(); // Simpan koordinat asli mouse untuk next iteration
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
    // Gunakan IgnoreAspectRatio agar gambar memenuhi seluruh widget
    painter.drawPixmap(0, 0, pixmap.scaled(ui->drawingArea->size(),
                                           Qt::IgnoreAspectRatio,
                                           Qt::SmoothTransformation));
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
    m_isModified = false; // Reset setelah dibersihkan
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
    m_isModified = false;
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

    // Jika user menekan Cancel, filePath akan kosong
    if (filePath.isEmpty()) return;

    if (pixmap.save(filePath)) {
        m_isModified = false;

        QMessageBox::information(this, "Success", "Gambar coretan berhasil disimpan!");

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

void Canvas::updateBrushPreview()
{
    // Buat gambar kosong seukuran komponen preview (misal kita pakai komponen QLabel bernama brushPreview)
    QPixmap pix(ui->brushPreview->size());

    // Warnai background preview agar sama dengan warna dasar aplikasi (transparan/mengikuti sistem)
    pix.fill(Qt::transparent);

    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing); // Biar lingkarannya halus/ga patah-patah

    // Tentukan warna lingkaran preview
    QColor previewColor = ui->checkBox->isChecked() ? QColor(128, 128, 128, 100) : penColor;
    painter.setBrush(QBrush(previewColor));
    painter.setPen(Qt::NoPen);

    // Gambar lingkaran tepat di tengah-tengah komponen preview
    int centerX = pix.width() / 2;
    int centerY = pix.height() / 2;
    int radius = penWidth / 2;

    // Batasi radius agar tidak luber keluar dari kotak preview jika ukuran 100
    if (radius > centerX) radius = centerX - 2;

    painter.drawEllipse(QPoint(centerX, centerY), radius, radius);

    // Tempelkan hasil gambar lingkaran ke QLabel ui->brushPreview
    ui->brushPreview->setPixmap(pix);
}

void Canvas::closeEvent(QCloseEvent *event)
{
    // Jika tidak ada perubahan, langsung tutup saja
    if (!m_isModified) {
        event->accept();
        return;
    }

    // Hanya muncul jika m_isModified == true
    QMessageBox::StandardButton resBtn = QMessageBox::question(this, "Canvas",
                                                               "Ada perubahan yang belum disimpan. Ingin menyimpannya?",
                                                               QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                               QMessageBox::Yes);
    if (resBtn == QMessageBox::Yes) {
        saveImage();
        // Cek lagi: jika setelah save masih modified (user cancel di dialog), jangan tutup
        if (m_isModified) {
            event->ignore();
        } else {
            event->accept();
        }
    } else if (resBtn == QMessageBox::No) {
        event->accept();
    } else {
        event->ignore();
    }
}