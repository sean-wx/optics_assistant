#ifndef COLORPALETTE_H
#define COLORPALETTE_H

#include <QDialog>
#include <QColor>
#include <QLabel>
#include <QLineEdit>

namespace Ui {
class ColorPalette;
}

class ColorPalette : public QDialog
{
    Q_OBJECT

public:
    static ColorPalette &instance();
    QPen getMarkPen() { return m_markPen; }
    void setMarkPen(const QPen &pen) { m_markPen = pen; }
    QPen getRectPen() { return m_rectPen; }
    void setRectPen(const QPen &pen) { m_rectPen = pen; }
    QPen getRulerPen() { return m_rulerPen; }
    void setRulerPen(const QPen &pen) { m_rulerPen = pen; }
    QPen getRulerFontPen() { return m_rulerFont; }
    void setRulerFontPen(const QPen &pen) { m_rulerFont = pen; }
    QPen getCursorPen() { return m_cursorPen; }
    void setCursorPen(const QPen &pen) { m_cursorPen = pen; }
    QPen getWithinThresholdPen() { return m_withinThresholdPen; }
    void setWithinThresholdPen(const QPen &pen) { m_withinThresholdPen = pen; }
    QPen getBeyondThresholdPen() { return m_beyondThresholdPen; }
    void setBeyondThresholdPen(const QPen &pen) { m_beyondThresholdPen = pen; }

    float getRulerFontSize() { return m_rulerFontSize; }
    void  setRulerFontSize(float size) { m_rulerFontSize = size; }
    float getWithinThresholdFontSize() { return m_withinThresholdFontSize; }
    void  setWithinThresholdFontSize(float size) { m_withinThresholdFontSize = size; }
    float getBeyondThresholdFontSize() { return m_beyondThresholdFontSize; }
    void  setBeyondThresholdFontSize(float size) { m_beyondThresholdFontSize = size; }

private:
    explicit ColorPalette(QWidget *parent = 0);
    ~ColorPalette();

    QColor getColor(const QColor &color);
    void setPen(QPen &pen, const QColor &color, float width);
    void setLabelColor(QLabel *label, const QColor &color);
    void setLineEditFont(QLineEdit *lineEdit, int size);
    void loadConfig();
    void saveConfig();
    void showParameters();

private slots:
    void on_rulerFontSize_valueChanged(int arg1);
    void on_withinThresholdFontSize_valueChanged(int arg1);
    void on_beyondThresholdFontSize_valueChanged(int arg1);
    void on_Save_clicked();
    void on_close_clicked();
    void on_reset_clicked();
    void on_markColorChange_clicked();
    void on_makeWidth_valueChanged(double arg1);
    void on_rectColorChange_clicked();
    void on_rectWidth_valueChanged(double arg1);
    void on_cursorColorChange_clicked();
    void on_cursorWidth_valueChanged(double arg1);
    void on_rulerColorChange_clicked();
    void on_rulerWidth_valueChanged(double arg1);
    void on_rulerFontColorChange_clicked();
    void on_withinThresholdFontColorChange_clicked();
    void on_beyondThresholdFontColorChange_clicked();

private:
    Ui::ColorPalette *ui;

    QPen    m_markPen;                    // 标注框的画笔
    QPen    m_rectPen;                    // 矩形框的画笔
    QPen    m_rulerPen;                   // 标尺的画笔
    QPen    m_rulerFont;                  // 标尺字体的画笔
    QPen    m_cursorPen;                  // 十字光标的画笔
    QPen    m_withinThresholdPen;         // 低于阈值的画笔
    QPen    m_beyondThresholdPen;         // 超出阈值的画笔

    float   m_rulerFontSize               = 9;  // 标尺字体大小
    float   m_withinThresholdFontSize     = 12; // 阈值内字体大小
    float   m_beyondThresholdFontSize     = 12; // 阈值外字体大小
};

#endif // COLORPALETTE_H
