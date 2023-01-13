#include <QColorDialog>
#include <QSettings>
#include <QTextCodec>
#include "ColorPalette.h"
#include "ui_ColorPalette.h"

ColorPalette &ColorPalette::instance()
{
    static ColorPalette colorPalette;
    return colorPalette;
}

ColorPalette::ColorPalette(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColorPalette)
{
    ui->setupUi(this);

    setMaximumSize(QSize(385,280));
    setMinimumSize(QSize(385,280));
    setWindowFlags(windowFlags()& ~Qt::WindowMinMaxButtonsHint);
    setWindowFlags(windowFlags()& Qt::WindowStaysOnTopHint);

    on_reset_clicked();

    loadConfig();
    showParameters();
}

ColorPalette::~ColorPalette()
{
    delete ui;
}

QColor ColorPalette::getColor(const QColor &color)
{
    QColor colorNew = QColorDialog::getColor(color,this);
    //如果颜色无效
    if (!colorNew.isValid()) {
        return color;
    } else {
        return colorNew;
    }
}

void ColorPalette::setPen(QPen &pen, const QColor &color, float width)
{
    pen.setColor(color);
    pen.setWidthF(width);
}

void ColorPalette::setLabelColor(QLabel *label, const QColor &color)
{
    QPalette label_palette;
    label_palette.setColor(QPalette::Background, color);
    label->setAutoFillBackground(true);
    label->setPalette(label_palette);
}

void ColorPalette::setLineEditFont(QLineEdit *lineEdit, int size)
{
    QFont font = lineEdit->font();
    font.setPointSize(size);
    lineEdit->setFont(font);
}

void ColorPalette::loadConfig()
{
    QSettings settings("systemColor.ini", QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("UTF8"));
    if (settings.value("markPenColor_R").isNull()) {
        on_reset_clicked();
        saveConfig();
    } else {
        QColor color = QColor(settings.value("markPenColor_R").toInt(), settings.value("markPenColor_G").toInt(), settings.value("markPenColor_B").toInt());
        double width = settings.value("markPenWidth").toDouble();
        setPen(m_markPen, color, width);

        color = QColor(settings.value("rectPenColor_R").toInt(), settings.value("rectPenColor_G").toInt(), settings.value("rectPenColor_B").toInt());
        width = settings.value("rectPenWidth").toDouble();
        setPen(m_rectPen, color, width);

        color = QColor(settings.value("cursorPenColor_R").toInt(), settings.value("cursorPenColor_G").toInt(), settings.value("cursorPenColor_B").toInt());
        width = settings.value("cursorPenWidth").toDouble();
        setPen(m_cursorPen, color, width);

        color = QColor(settings.value("rulerPenColor_R").toInt(), settings.value("rulerPenColor_G").toInt(), settings.value("rulerPenColor_B").toInt());
        width = settings.value("rulerPenWidth").toDouble();
        setPen(m_rulerPen, color, width);

        color = QColor(settings.value("rulerFontColor_R").toInt(), settings.value("rulerFontColor_G").toInt(), settings.value("rulerFontColor_B").toInt());
        width = settings.value("rulerFontWidth").toDouble();
        setPen(m_rulerFont, color, width);

        color = QColor(settings.value("withinThresholdPenColor_R").toInt(), settings.value("withinThresholdPenColor_G").toInt(), settings.value("withinThresholdPenColor_B").toInt());
        width = settings.value("withinThresholdPenWidth").toDouble();
        setPen(m_withinThresholdPen, color, width);

        color = QColor(settings.value("beyondThresholdPenColor_R").toInt(), settings.value("beyondThresholdPenColor_G").toInt(), settings.value("beyondThresholdPenColor_B").toInt());
        width = settings.value("beyondThresholdPenWidth").toDouble();
        setPen(m_beyondThresholdPen, color, width);

        m_rulerFontSize = settings.value("rulerFontSize").toFloat();
        m_withinThresholdFontSize = settings.value("withinThresholdFontSize").toFloat();
        m_beyondThresholdFontSize = settings.value("beyondThresholdFontSize").toFloat();
    }
}

void ColorPalette::saveConfig()
{
    QSettings settings("systemColor.ini", QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("UTF8"));

    settings.setValue("markPenColor_R", m_markPen.color().red());
    settings.setValue("markPenColor_G", m_markPen.color().green());
    settings.setValue("markPenColor_B", m_markPen.color().blue());
    settings.setValue("markPenWidth", m_markPen.widthF());

    settings.setValue("rectPenColor_R", m_rectPen.color().red());
    settings.setValue("rectPenColor_G", m_rectPen.color().green());
    settings.setValue("rectPenColor_B", m_rectPen.color().blue());
    settings.setValue("rectPenWidth", m_rectPen.widthF());

    settings.setValue("cursorPenColor_R", m_cursorPen.color().red());
    settings.setValue("cursorPenColor_G", m_cursorPen.color().green());
    settings.setValue("cursorPenColor_B", m_cursorPen.color().blue());
    settings.setValue("cursorPenWidth", m_cursorPen.widthF());

    settings.setValue("rulerPenColor_R", m_rulerPen.color().red());
    settings.setValue("rulerPenColor_G", m_rulerPen.color().green());
    settings.setValue("rulerPenColor_B", m_rulerPen.color().blue());
    settings.setValue("rulerPenWidth", m_rulerPen.widthF());

    settings.setValue("rulerFontColor_R", m_rulerFont.color().red());
    settings.setValue("rulerFontColor_G", m_rulerFont.color().green());
    settings.setValue("rulerFontColor_B", m_rulerFont.color().blue());
    settings.setValue("rulerFontWidth", m_rulerFont.widthF());

    settings.setValue("withinThresholdPenColor_R", m_withinThresholdPen.color().red());
    settings.setValue("withinThresholdPenColor_G", m_withinThresholdPen.color().green());
    settings.setValue("withinThresholdPenColor_B", m_withinThresholdPen.color().blue());
    settings.setValue("withinThresholdPenWidth", m_withinThresholdPen.widthF());

    settings.setValue("beyondThresholdPenColor_R", m_beyondThresholdPen.color().red());
    settings.setValue("beyondThresholdPenColor_G", m_beyondThresholdPen.color().green());
    settings.setValue("beyondThresholdPenColor_B", m_beyondThresholdPen.color().blue());
    settings.setValue("beyondThresholdPenWidth", m_beyondThresholdPen.widthF());

    settings.setValue("rulerFontSize", (double)m_rulerFontSize);
    settings.setValue("withinThresholdFontSize", (double)m_withinThresholdFontSize);
    settings.setValue("beyondThresholdFontSize", (double)m_beyondThresholdFontSize);
}

void ColorPalette::on_rulerFontSize_valueChanged(int arg1)
{
    m_rulerFontSize = arg1;
    setLineEditFont(ui->rulerFont, m_rulerFontSize);
}

void ColorPalette::on_withinThresholdFontSize_valueChanged(int arg1)
{
    m_withinThresholdFontSize = arg1;
    setLineEditFont(ui->withinThresholdFont, m_withinThresholdFontSize);
}

void ColorPalette::on_beyondThresholdFontSize_valueChanged(int arg1)
{
    m_beyondThresholdFontSize = arg1;
    setLineEditFont(ui->beyondThresholdFont, m_beyondThresholdFontSize);
}

void ColorPalette::on_Save_clicked()
{
    saveConfig();
    QDialog::close();
}

void ColorPalette::on_close_clicked()
{
    QDialog::close();
}

void ColorPalette::on_reset_clicked()
{
    m_markPen                    = QPen(QColor(0, 255, 0), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    m_rectPen                    = QPen(QColor(255, 59,33), 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    m_cursorPen                  = QPen(Qt::magenta, 2, Qt::DashLine);
    m_rulerPen                   = QPen(QColor(255, 19, 113), 1, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin);
    m_rulerFont                  = QPen(QColor(255, 19, 113), 1, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin);
    m_withinThresholdPen         = QPen(QColor(46, 84, 255), 20, Qt::DotLine);
    m_beyondThresholdPen         = QPen(Qt::red, 20, Qt::DotLine);

    m_rulerFontSize              = 9;
    m_withinThresholdFontSize    = 12;
    m_beyondThresholdFontSize    = 12;

    showParameters();
}

void ColorPalette::showParameters()
{
    setLabelColor(ui->markColor, m_markPen.color());
    setLabelColor(ui->rectColor, m_rectPen.color());
    setLabelColor(ui->cursorColor, m_cursorPen.color());
    setLabelColor(ui->rulerColor, m_rulerPen.color());
    setLabelColor(ui->rulerFontColor, m_rulerFont.color());
    setLabelColor(ui->withinThresholdFontColor, m_withinThresholdPen.color());
    setLabelColor(ui->beyondThresholdFontColor, m_beyondThresholdPen.color());

    setLineEditFont(ui->rulerFont, m_rulerFontSize);
    setLineEditFont(ui->withinThresholdFont, m_withinThresholdFontSize);
    setLineEditFont(ui->beyondThresholdFont, m_beyondThresholdFontSize);

    ui->makeWidth->setValue(m_markPen.width());
    ui->rectWidth->setValue(m_rectPen.width());
    ui->cursorWidth->setValue(m_cursorPen.width());
    ui->rulerWidth->setValue(m_rulerPen.width());

    ui->rulerFontSize->setValue(m_rulerFontSize);
    ui->withinThresholdFontSize->setValue(m_withinThresholdFontSize);
    ui->beyondThresholdFontSize->setValue(m_beyondThresholdFontSize);
}

void ColorPalette::on_markColorChange_clicked()
{
    QColor color = getColor(m_markPen.color());
    setPen(m_markPen, color, ui->makeWidth->value());
    setLabelColor(ui->markColor, color);
}

void ColorPalette::on_makeWidth_valueChanged(double arg1)
{
    setPen(m_markPen, m_markPen.color(), arg1);
}

void ColorPalette::on_rectColorChange_clicked()
{
    QColor color = getColor(m_rectPen.color());
    setPen(m_rectPen, color, ui->rectWidth->value());
    setLabelColor(ui->rectColor, color);
}

void ColorPalette::on_rectWidth_valueChanged(double arg1)
{
    setPen(m_rectPen, m_rectPen.color(), arg1);
}

void ColorPalette::on_cursorColorChange_clicked()
{
    QColor color = getColor(m_cursorPen.color());
    setPen(m_cursorPen, color, ui->cursorWidth->value());
    setLabelColor(ui->cursorColor, color);
}

void ColorPalette::on_cursorWidth_valueChanged(double arg1)
{
    setPen(m_cursorPen, m_cursorPen.color(), arg1);
}

void ColorPalette::on_rulerColorChange_clicked()
{
    QColor color = getColor(m_rulerPen.color());
    setPen(m_rulerPen, color, ui->rulerWidth->value());
    setLabelColor(ui->rulerColor, color);
}

void ColorPalette::on_rulerWidth_valueChanged(double arg1)
{
    setPen(m_rulerPen, m_rulerPen.color(), arg1);
}

void ColorPalette::on_rulerFontColorChange_clicked()
{
    QColor color = getColor(m_rulerFont.color());
    setPen(m_rulerFont, color, m_rulerFont.widthF());
    setLabelColor(ui->rulerFontColor, color);
}

void ColorPalette::on_withinThresholdFontColorChange_clicked()
{
    QColor color = getColor(m_withinThresholdPen.color());
    setPen(m_withinThresholdPen, color, m_withinThresholdPen.widthF());
    setLabelColor(ui->withinThresholdFontColor, color);
}

void ColorPalette::on_beyondThresholdFontColorChange_clicked()
{
    QColor color = getColor(m_beyondThresholdPen.color());
    setPen(m_beyondThresholdPen, color, m_beyondThresholdPen.widthF());
    setLabelColor(ui->beyondThresholdFontColor, color);
}
