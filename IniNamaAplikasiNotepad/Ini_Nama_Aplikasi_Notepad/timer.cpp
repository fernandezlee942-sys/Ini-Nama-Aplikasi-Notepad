#include "timer.h"
#include "ui_timer.h"
#include <QMessageBox>

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
    timerAlarm->setInterval(1000);
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
        QMessageBox::information(this, "Timer", "Waktu habis!");
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
}

void Timer::checkAlarm()
{
    if (!alarmSet) return;

    QTime now = QTime::currentTime();
    if (now.hour() == alarmTime.hour() &&
        now.minute() == alarmTime.minute() &&
        now.second() == alarmTime.second())
    {
        timerAlarm->stop();
        alarmSet = false;
        ui->labelAlarmStatus->setText("Alarm belum diset");
        QMessageBox::information(this, "Alarm", "Alarm berbunyi!");
    }
}