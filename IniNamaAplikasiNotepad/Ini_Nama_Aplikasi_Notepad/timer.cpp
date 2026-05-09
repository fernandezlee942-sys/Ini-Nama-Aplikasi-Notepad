#include "timer.h"
#include "ui_timer.h"

Timer::Timer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Timer)
    , seconds(0)
    , isRunning(false)
{
    ui->setupUi(this);

    timerStopwatch = new QTimer(this);
    timerStopwatch->setInterval(1000);

    connect(timerStopwatch, &QTimer::timeout,
            this, &Timer::updateStopwatch);

    updateDisplay();
}

Timer::~Timer()
{
    delete ui;
}

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
