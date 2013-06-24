#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtCore/QtGlobal>
#include <QtAddOnSerialPort/serialport-global.h>
#include <QtAddOnSerialPort/serialport.h>
#include <QSettings>
#include <QTimer>
#include <QCompleter>
#include "checkcurrport.h"


namespace Ui {
class Widget;
}

QT_BEGIN_NAMESPACE_SERIALPORT
class SerialPort;
QT_END_NAMESPACE_SERIALPORT

QT_USE_NAMESPACE_SERIALPORT

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    
private slots:
    void on_lineEdit_2_returnPressed();
    bool openSerialPort();
    void closeSerialPort();
    void refreshAvSerialPort();
    void loadSettings();
    void saveSettings();
    void readToPlainText();
    void clearLine();
    void changeIndxToPushButton(int indx);
    void focusLineEdit(int indx);
    void focusLineEdit2();

    void on_toolButton_3_clicked(bool checked);

    void on_toolButton_4_clicked();

    void on_pushButton_clicked();

    void on_comboBox_2_highlighted(const QString &arg1);

    void on_comboBox_2_activated(const QString &arg1);

    void on_lineEdit_returnPressed();

    void on_lineEdit_textEdited(const QString &arg1);

    void on_pushButton_2_clicked();

    void on_cbTextSet_currentIndexChanged(int index);

private:
    Ui::Widget *ui;
    SerialPort *mySerialPort;
    QTimer *myTimer;
    bool dozvilNaOnovlPortiv;
    QString currPortName;
    QStringList complList;
    QCompleter *myCompl;
    bool selPosition;
    CheckCurrPort checkPort;

};

#endif // WIDGET_H
