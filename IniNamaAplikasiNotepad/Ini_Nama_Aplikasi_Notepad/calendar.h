#ifndef CALENDAR_H
#define CALENDAR_H

#include <QWidget>
#include <QVector>
#include <QString>
#include <QDate>

struct Task {
    QString name;
    QDate deadline;
    bool isUrgent;
};

namespace Ui {
class Calendar;
}

class Calendar : public QWidget
{
    Q_OBJECT

public:
    explicit Calendar(QWidget *parent = nullptr);
    ~Calendar();

private slots:
    void on_pushButton_clicked();
    void on_calendarWidget_clicked(const QDate &date);
    void on_calendarWidget_currentPageChanged(int year, int month);
    void on_pushButton_2_clicked(); // Slot untuk Delete Task
    void on_pushButton_3_clicked(); // Slot untuk Save Task (Instan)

private:
    Ui::Calendar *ui;
    QVector<Task> m_taskList;

    void updateCalendarColors();
    void updateTaskListView();
    void setupContextMenu();
    void updateCalendarTextEdit();

    void saveTasksToFile();
    void loadTasksFromFile();
    QString getSaveFilePath(); // Akan mengarah ke folder Files/Task.txt

protected:
    void changeEvent(QEvent *event) override;
};

#endif // CALENDAR_H