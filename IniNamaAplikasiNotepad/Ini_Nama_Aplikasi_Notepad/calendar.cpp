#include "calendar.h"
#include "ui_calendar.h"
#include <QTextCharFormat>
#include <QBrush>
#include <QMenu>
#include <QAction>
#include <QCursor>
#include <QPalette>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QMessageBox>
#include <utility>   // Diperlukan untuk std::as_const
#include <algorithm> // Diperlukan untuk std::sort

Calendar::Calendar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Calendar)
{
    ui->setupUi(this);

    // Mengatur agar textEdit_2 bisa memunculkan menu klik kanan kustom
    ui->textEdit_2->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->textEdit_2, &QTextEdit::customContextMenuRequested, this, &Calendar::setupContextMenu);

    // Hubungkan signal internal calendarWidget agar saat ganti halaman bulan/tahun, warna & list ikut ter-update
    connect(ui->calendarWidget, &QCalendarWidget::currentPageChanged, this, &Calendar::on_calendarWidget_currentPageChanged);

    // Otomatis muat data dari file txt saat jendela kalender pertama kali dibuka
    loadTasksFromFile();
}

Calendar::~Calendar()
{
    delete ui;
}

// ==================== PATH & MANAJEMEN FILE ====================

// Mengubah arah path default ke folder Files dengan nama file Task.txt
QString Calendar::getSaveFilePath()
{
    QString folderPath = "C:/Ini_Nama_Aplikasi_Notepad/Files";
    QDir dir(folderPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    return folderPath + "/Task.txt";
}

// Menyimpan isi QVector m_taskList ke file Task.txt
void Calendar::saveTasksToFile()
{
    QFile file(getSaveFilePath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Gagal menyimpan data tugas ke berkas lokal.");
        return;
    }

    QTextStream out(&file);
    for (const Task &task : std::as_const(m_taskList)) {
        // Format simpanan: Nama Tugas|dd,M,yyyy|1 atau 0 (Urgent)
        out << task.name << "|"
            << task.deadline.toString("d,M,yyyy") << "|"
            << (task.isUrgent ? "1" : "0") << "\n";
    }
    file.close();
}

// Membaca file Task.txt dan memasukkannya kembali ke QVector m_taskList saat init
void Calendar::loadTasksFromFile()
{
    m_taskList.clear();
    QFile file(getSaveFilePath());

    // Jika file belum ada, tidak masalah (berarti belum ada tugas yang disimpan)
    if (!file.exists()) {
        updateCalendarColors();
        updateTaskListView();
        updateCalendarTextEdit();
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        // Memisah data berdasarkan karakter pipa '|'
        QStringList tokens = line.split("|");
        if (tokens.size() >= 3) {
            Task task;
            task.name = tokens[0];
            task.deadline = QDate::fromString(tokens[1], "d,M,yyyy");
            task.isUrgent = (tokens[2] == "1");

            if (task.deadline.isValid()) {
                m_taskList.append(task);
            }
        }
    }
    file.close();

    // Urutkan data secara kronologis berdasarkan tenggat waktu terdekat
    std::sort(m_taskList.begin(), m_taskList.end(), [](const Task &a, const Task &b) {
        return a.deadline < b.deadline;
    });

    // Refresh visual antarmuka
    updateCalendarColors();
    updateTaskListView();
    updateCalendarTextEdit();
}

// ==================== TAB 2: ADD TASK (SUBMIT) ====================
void Calendar::on_pushButton_clicked()
{
    if (ui->lineEdit->text().trimmed().isEmpty()) return;

    Task newTask;
    newTask.name = ui->lineEdit->text().trimmed();

    QString dateInput = ui->lineEdit_2->text().trimmed();
    newTask.deadline = QDate::fromString(dateInput, "d,M,yyyy");

    if (!newTask.deadline.isValid()) {
        newTask.deadline = QDate::currentDate();
    }

    newTask.isUrgent = ui->checkBox->isChecked();

    m_taskList.append(newTask);

    std::sort(m_taskList.begin(), m_taskList.end(), [](const Task &a, const Task &b) {
        return a.deadline < b.deadline;
    });

    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->checkBox->setChecked(false);

    // Otomatis simpan perubahan setiap kali menambah tugas baru
    saveTasksToFile();

    updateCalendarColors();
    updateTaskListView();
    updateCalendarTextEdit();
}

// ==================== TAB 3: DELETE & SAVE MANUAL ====================

// pushButton_2: Tombol Menghapus Tugas berdasarkan Nama (lineEdit_3)
void Calendar::on_pushButton_2_clicked()
{
    QString targetName = ui->lineEdit_3->text().trimmed();
    if (targetName.isEmpty()) return;

    bool found = false;

    // Iterasi mundur untuk menghapus data dari vector secara aman
    for (int i = m_taskList.size() - 1; i >= 0; --i) {
        if (m_taskList[i].name.compare(targetName, Qt::CaseInsensitive) == 0) {
            m_taskList.removeAt(i);
            found = true;
        }
    }

    if (found) {
        // Otomatis sinkronisasi ke file Task.txt
        saveTasksToFile();

        // Refresh UI secara instan
        updateCalendarColors();
        updateTaskListView();
        updateCalendarTextEdit();

        ui->lineEdit_3->clear();
        QMessageBox::information(this, "Sukses", "Tugas '" + targetName + "' berhasil dihapus.");
    } else {
        QMessageBox::warning(this, "Gagal", "Tugas dengan nama '" + targetName + "' tidak ditemukan.");
    }
}

// pushButton_3: Tombol SAVE TASK (Instan tanpa lineEdit_4)
void Calendar::on_pushButton_3_clicked()
{
    // Langsung timpa data terbaru ke C:/Ini_Nama_Aplikasi_Notepad/Files/Task.txt
    saveTasksToFile();

    // Re-render UI untuk memastikan data benar-benar sinkron
    updateCalendarColors();
    updateTaskListView();
    updateCalendarTextEdit();

    QMessageBox::information(this, "Sukses", "Seluruh data tugas berhasil diperbarui ke Task.txt!");
}

// ==================== TAB 1: CALENDAR LOGIC & HIGHLIGHT ====================
void Calendar::updateCalendarColors()
{
    if (!ui->calendarWidget) return;

    QColor defaultTextColor = this->palette().color(QPalette::Text);

    QTextCharFormat weekendFormat;
    weekendFormat.setForeground(QBrush(defaultTextColor));

    ui->calendarWidget->setWeekdayTextFormat(Qt::Saturday, weekendFormat);
    ui->calendarWidget->setWeekdayTextFormat(Qt::Sunday, weekendFormat);

    ui->calendarWidget->setDateTextFormat(QDate(), QTextCharFormat());

    QDate today = QDate::currentDate();

    QTextCharFormat orangeFormat;
    orangeFormat.setForeground(QBrush(QColor(255, 140, 0)));

    QTextCharFormat redFormat;
    redFormat.setForeground(QBrush(Qt::red));

    for (const Task &task : std::as_const(m_taskList)) {
        if (task.deadline >= today) {
            ui->calendarWidget->setDateTextFormat(task.deadline, orangeFormat);
        } else {
            ui->calendarWidget->setDateTextFormat(task.deadline, redFormat);
        }
    }
}

void Calendar::updateCalendarTextEdit()
{
    ui->textEdit->clear();
    QDate today = QDate::currentDate();

    QString content = "DAFTAR TUGAS AKTIF\n";
    bool hasActive = false;

    for (const Task &task : std::as_const(m_taskList)) {
        if (task.deadline >= today) {
            QString urgentStr = task.isUrgent ? "[URGENT] " : "";
            content += QString("- [%1] %2%3\n")
                           .arg(task.deadline.toString("dd/MM/yyyy"), urgentStr, task.name);
            hasActive = true;
        }
    }
    if (!hasActive) content += "(Tidak ada tugas aktif)\n";

    content += "\nEWAT DEADLINE\n";
    bool hasOverdue = false;

    for (const Task &task : std::as_const(m_taskList)) {
        if (task.deadline < today) {
            QString urgentStr = task.isUrgent ? "[URGENT] " : "";
            content += QString("- [%1] %2%3\n")
                           .arg(task.deadline.toString("dd/MM/yyyy"), urgentStr, task.name);
            hasOverdue = true;
        }
    }
    if (!hasOverdue) content += "(Tidak ada tugas lewat deadline)\n";

    ui->textEdit->setText(content);
}

void Calendar::on_calendarWidget_clicked(const QDate &date)
{
    Q_UNUSED(date);
    updateCalendarTextEdit();
    updateTaskListView();
}

void Calendar::on_calendarWidget_currentPageChanged(int year, int month)
{
    Q_UNUSED(year);
    Q_UNUSED(month);
    updateCalendarColors();
    updateTaskListView();
    updateCalendarTextEdit();
}

void Calendar::updateTaskListView()
{
    ui->textEdit_2->clear();
    QDate today = QDate::currentDate();

    QString normalTasks = "";
    QString overdueTasks = "Lewat deadline:\n";
    bool hasOverdue = false;

    for (const Task &task : std::as_const(m_taskList)) {
        int daysLeft = today.daysTo(task.deadline);
        QString urgentStatus = task.isUrgent ? "Urgent" : "Not Urgent";

        if (daysLeft >= 0) {
            QString line = QString("%1, %2, sisa %3 hari\n")
            .arg(task.name, urgentStatus, QString::number(daysLeft));
            normalTasks += line;
        } else {
            overdueTasks += QString("- %1 (lewat %2 hari)\n")
            .arg(task.name, QString::number(qAbs(daysLeft)));
            hasOverdue = true;
        }
    }

    QString finalReport = normalTasks;
    if (hasOverdue) {
        finalReport += "\n" + overdueTasks;
    }

    ui->textEdit_2->setText(finalReport);
}

void Calendar::setupContextMenu()
{
    QMenu contextMenu(this);
    QAction *clearAction = contextMenu.addAction("Hapus Semua Task");

    connect(clearAction, &QAction::triggered, this, [=]() {
        m_taskList.clear();

        // Bersihkan berkas fisik Task.txt
        saveTasksToFile();

        ui->textEdit->clear();
        ui->textEdit_2->clear();

        updateCalendarColors();
        updateCalendarTextEdit();
    });

    contextMenu.exec(QCursor::pos());
}

void Calendar::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::PaletteChange) {
        updateCalendarColors();
    }
    QWidget::changeEvent(event);
}