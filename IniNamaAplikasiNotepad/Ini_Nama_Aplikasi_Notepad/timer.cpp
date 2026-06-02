#include "timer.h"
#include "ui_timer.h"
#include <QMessageBox>
#include <QFile>
#include <QDir>

Timer::Timer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Timer)
    , seconds(0)
    , isRunning(false)
    , remainingMinutes(0)
    , remainingSeconds(0)
    , remainingMs(0)
    , timerRunning(false)
    , alarmSet(false)
{
    ui->setupUi(this);

#ifdef HAS_MULTIMEDIA
    mediaPlayer = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    mediaPlayer->setAudioOutput(audioOutput);
#endif

    // Stopwatch
    timerStopwatch = new QTimer(this);
    timerStopwatch->setInterval(1000);
    connect(timerStopwatch, &QTimer::timeout,
            this, &Timer::updateStopwatch);
    updateDisplay();

    // Timer countdown
    timerCountdown = new QTimer(this);
    timerCountdown->setInterval(10);
    connect(timerCountdown, &QTimer::timeout,
            this, &Timer::updateTimer);

    // Alarm
    timerAlarm = new QTimer(this);
    timerAlarm->setInterval(500);
    connect(timerAlarm, &QTimer::timeout,
            this, &Timer::checkAlarm);
}

Timer::~Timer()
{
    delete ui;
}

// ── Stopwatch ──────────────────────────
void Timer::updateDisplay()
{
    int minute = seconds / 60;
    int second = seconds % 60;
    ui->labelStopwatch->setText(
        QString("%1:%2")
            .arg(minute, 2, 10, QChar('0'))
            .arg(second, 2, 10, QChar('0'))
        );
}

void Timer::updateStopwatch()
{
    seconds++;
    updateDisplay();
}

void Timer::on_btnStart_clicked()
{
    if (!isRunning) {
        timerStopwatch->start();
        isRunning = true;
    }
}

void Timer::on_btnStop_clicked()
{
    if (isRunning) {
        timerStopwatch->stop();
        isRunning = false;
    }
}

void Timer::on_btnReset_clicked()
{
    timerStopwatch->stop();
    isRunning = false;
    seconds = 0;
    updateDisplay();
}

// ── Timer Countdown ────────────────────
void Timer::updateTimerDisplay()
{
    ui->labelTimer->setText(
        QString("%1:%2:%3")
            .arg(remainingMinutes, 2, 10, QChar('0'))
            .arg(remainingSeconds, 2, 10, QChar('0'))
            .arg(remainingMs, 2, 10, QChar('0'))
        );
}

void Timer::updateTimer()
{
    if (remainingMinutes == 0 &&
        remainingSeconds == 0 &&
        remainingMs == 0)
    {
        timerCountdown->stop();
        timerRunning = false;
        putarRingtoneSakti();
        QMessageBox::information(this, "Timer", "Waktu habis!");
#ifdef HAS_MULTIMEDIA
        mediaPlayer->stop();
#endif
        return;
    }

    if (remainingMs > 0) {
        remainingMs--;
    } else if (remainingSeconds > 0) {
        remainingSeconds--;
        remainingMs = 99;
    } else {
        remainingMinutes--;
        remainingSeconds = 59;
        remainingMs = 99;
    }

    updateTimerDisplay();
}

void Timer::on_btnStartTimer_clicked()
{
    if (!timerRunning) {
        remainingMinutes = ui->spinMenit->value();
        remainingSeconds = ui->spinDetik->value();
        remainingMs = 0;

        if (remainingMinutes > 0 || remainingSeconds > 0) {
            timerCountdown->start();
            timerRunning = true;
        }
    }
}

void Timer::on_btnStopTimer_clicked()
{
    if (timerRunning) {
        timerCountdown->stop();
        timerRunning = false;
    }
}

void Timer::on_btnResetTimer_clicked()
{
    timerCountdown->stop();
    timerRunning = false;
    remainingMinutes = 0;
    remainingSeconds = 0;
    remainingMs = 0;
    ui->labelTimer->setText("00:00:00");
    ui->spinMenit->setValue(0);
    ui->spinDetik->setValue(0);
}

// ── Alarm ──────────────────────────────
void Timer::on_btnSetAlarm_clicked()
{
    alarmTime = ui->timeEditAlarm->time();
    alarmSet = true;
    timerAlarm->start();
    ui->labelAlarmStatus->setText(
        "Alarm diset: " + alarmTime.toString("hh:mm AP")
        );
}

void Timer::on_btnStopAlarm_clicked()
{
    timerAlarm->stop();
    alarmSet = false;
    ui->labelAlarmStatus->setText("Alarm belum diset");
#ifdef HAS_MULTIMEDIA
    mediaPlayer->stop();
#endif
}

void Timer::checkAlarm()
{
    if (!alarmSet) return;

    QTime now = QTime::currentTime();
    if (now.hour() == alarmTime.hour() &&
        now.minute() == alarmTime.minute() &&
        now.second() >= alarmTime.second())
    {
        timerAlarm->stop();
        alarmSet = false;
        ui->labelAlarmStatus->setText("Alarm belum diset");

        // 1. Hidupkan musik alarm
        putarRingtoneSakti();

        // 2. Berikan jeda super singkat agar thread audio stabil sebelum dikunci QMessageBox
        QTimer::singleShot(100, this, [this]() {
            QMessageBox::information(this, "Alarm", "Alarm berbunyi!");
#ifdef HAS_MULTIMEDIA
            mediaPlayer->stop();
#endif
        });
    }
}

void Timer::putarRingtoneSakti()
{
#ifdef HAS_MULTIMEDIA
    QString pathRingtone = ":/Mp3/Mp3/Ringtone.mp3";

    if (QFile::exists(pathRingtone)) {
        // SOLUSI TERBAIK QT 6: Buka file qrc langsung sebagai pencadangan data streaming biner
        QFile *audioFile = new QFile(pathRingtone, this);
        if (audioFile->open(QIODevice::ReadOnly)) {
            // Berikan device biner langsung ke MediaPlayer tanpa perlu copy-copy file lokal lagi
            mediaPlayer->setSourceDevice(audioFile, QUrl(pathRingtone));
        } else {
            // Fallback jika gagal open device
            mediaPlayer->setSource(QUrl(pathRingtone));
        }

        mediaPlayer->setLoops(QMediaPlayer::Infinite);
        audioOutput->setVolume(0.7);
        mediaPlayer->play();
    }
#endif
}