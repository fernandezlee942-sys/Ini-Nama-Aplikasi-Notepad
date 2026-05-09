#ifndef TIMER_H
#define TIMER_H
#include <QWidget>
#include <QTimer>

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
    void updateStopwatch();
    void on_btnStart_clicked();
    void on_btnStop_clicked();
    void on_btnReset_clicked();
    void on_btnTimer_clicked();      // tambah
    void on_btnAlarm_clicked();      // tambah
    void on_btnStopWatch_clicked();  // tambah

private:
    Ui::Timer *ui;
    QTimer *timerStopwatch;
    int seconds;
    bool isRunning;

    void updateDisplay();
};

#endif // TIMER_H