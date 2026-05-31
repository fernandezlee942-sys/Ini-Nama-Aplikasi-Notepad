#ifndef TIMER_H
#define TIMER_H
#include <QWidget>
#include <QTimer>
#include <QTime>

#ifdef HAS_MULTIMEDIA
#include <QMediaPlayer>
#include <QAudioOutput>
#endif

namespace Ui {
class Timer;
}

class Timer : public QWidget
{
    Q_OBJECT

public:
    explicit Timer(QWidget *parent = nullptr);
    ~Timer();

private slots:
    // Stopwatch
    void updateStopwatch();
    void on_btnStart_clicked();
    void on_btnStop_clicked();
    void on_btnReset_clicked();

    // Timer
    void updateTimer();
    void on_btnStartTimer_clicked();
    void on_btnStopTimer_clicked();
    void on_btnResetTimer_clicked();

    // Alarm
    void on_btnSetAlarm_clicked();
    void on_btnStopAlarm_clicked();
    void checkAlarm();

private:
    Ui::Timer *ui;

    // Stopwatch
    QTimer *timerStopwatch;
    int seconds;
    bool isRunning;
    void updateDisplay();

    // Timer
    QTimer *timerCountdown;
    int remainingMinutes;
    int remainingSeconds;
    int remainingMs;
    bool timerRunning;
    void updateTimerDisplay();

    // Alarm
    QTimer *timerAlarm;
    QTime alarmTime;
    bool alarmSet;

#ifdef HAS_MULTIMEDIA
    QMediaPlayer *mediaPlayer;
    QAudioOutput *audioOutput;
#endif

    void putarRingtoneSakti();
};

#endif // TIMER_H