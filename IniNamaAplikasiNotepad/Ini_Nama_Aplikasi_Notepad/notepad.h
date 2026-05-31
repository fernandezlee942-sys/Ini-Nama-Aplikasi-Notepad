#ifndef NOTEPAD_H
#define NOTEPAD_H

// Bawaan Qt & C++ Library
#include <QMainWindow>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QDesktopServices>
#include <QKeyEvent>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QPrinter>
#include <QPrintDialog>
#include <QLabel>
#include <QTextEdit>
#include <QDir>
#include <QFileInfoList>
#include <QUrl>

// 1. Kondisional Include untuk Player Musik
#ifdef HAS_MULTIMEDIA
#include <QMediaPlayer>
#include <QAudioOutput>
#endif

QT_BEGIN_NAMESPACE
namespace Ui {
class Notepad;
}
QT_END_NAMESPACE

class Notepad : public QMainWindow
{
    Q_OBJECT // Mengaktifkan fitur Signal & Slot lewat MOC (Meta Object Compiler)

public:
    explicit Notepad(QWidget *parent = nullptr);
    ~Notepad() override; // Dekonstruktor untuk membersihkan Heap memori
    void updateAllIcons(bool isDark);

private slots:
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_as_triggered();
    void on_actionPrint_triggered();
    void on_actionExit_triggered();
    void on_actionCopy_triggered();
    void on_actionCut_triggered();
    void on_actionPaste_triggered();
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();
    void on_actionTimer_triggered();
    void on_actionCalculator_triggered();
    void on_actionCalendar_triggered();
    void on_actionLight_triggered();
    void on_actionDark_triggered();
    void on_tabWidget_tabCloseRequested(int index);
    void on_tabWidget_currentChanged(int index);
    void updateTabTitle();
    void eksekusiPasteGambarSakti();
    void on_actionInsert_Image_triggered();
    void on_shortcutCloseTab_triggered();
    void on_actionPlaylist_1_triggered();

    void on_actionStop_Music_triggered();

    void on_actionClose_Tab_triggered();

    void on_actionFind_triggered();

    void on_actionFind_Next_triggered();

    void on_actionFind_Previous_triggered();

    void on_actionStatus_Bar_triggered();

    void updateStatusBarData();

private:
    Ui::Notepad *ui; // Pointer menuju UI (Pimpl Idiom / Bridge Pattern)
    QTextEdit* getActiveEditor();
    void ekstrakMusicResource(const QString &resourcePath, const QString &targetFileName);

    bool isStatusBarActive = false; // Status awal = false (mati)
    bool isFindActive = false;      // Status fitur Find aktif/tidak
    QLabel *findStatusLabel = nullptr; // Widget label khusus untuk menampung info Find di kanan

    // Variabel untuk melacak status pencarian teks terakhir
    QString lastSearchText = "";

// 2. Kondisional Deklarasi Variabel Musik (Wajib berada di dalam Class)
#ifdef HAS_MULTIMEDIA
    QMediaPlayer *mediaPlayer = nullptr;
    QAudioOutput *audioOutput = nullptr;
    QStringList musicPlaylist;
    int currentTrackIndex = 0;
    void playNextTrack();
#endif

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // NOTEPAD_H