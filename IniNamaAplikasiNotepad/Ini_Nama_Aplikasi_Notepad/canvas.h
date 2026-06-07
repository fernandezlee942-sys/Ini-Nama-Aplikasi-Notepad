#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QPixmap>
#include <QPoint>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QColor>
#include <QVector>

namespace Ui {
class Canvas;
}

class Canvas : public QWidget
{
    Q_OBJECT

public:
    explicit Canvas(QWidget *parent = nullptr, bool isDark = false);
    ~Canvas();

    bool isModified() const { return m_isModified; }
    void setupTheme(bool isDark);

    // PINDAHKAN KE SINI AGAR SHORTCUT DAN TOMBOL TERKONEKSI SEMPURNA
private slots:
    void clearCanvas();
    void saveImage();
    void openImage();
    void undo();
    void redo();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
private:
    Ui::Canvas *ui;
    QPixmap pixmap;
    QPoint lastPoint;
    QColor penColor;
    int penWidth;
    QColor canvasBgColor;
    bool m_isDark;
    bool m_isModified = false;

    QVector<QImage> undoStack;
    QVector<QImage> redoStack;
    const int MAX_HISTORY = 20;

    void handleMousePress(QMouseEvent *event);
    void handleMouseMove(QMouseEvent *event);
    void handleMouseRelease(QMouseEvent *event);
    void handlePaint();
    void saveCurrentState();
    void setupShortcuts();
    void updateBrushPreview();
};

#endif // CANVAS_H