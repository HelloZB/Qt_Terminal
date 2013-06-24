#include "widget.h"
#include "ui_widget.h"
#include <QtAddOnSerialPort/serialportinfo.h>
#include <QTextCodec> //qt4

#include <QMessageBox>
//#include <QDebug>
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    QTextCodec *utfcodec = QTextCodec::codecForName("UTF-8"); //qt4
    QTextCodec::setCodecForTr(utfcodec);
    QTextCodec::setCodecForCStrings(utfcodec);
    mySerialPort=new SerialPort(this);
    myTimer=new QTimer(this);
    connect(ui->toolButton, SIGNAL(clicked()), this, SLOT(refreshAvSerialPort()));
    connect(mySerialPort, SIGNAL(readyRead()), this , SLOT(readToPlainText()));
    //ui->groupBox->setHidden(ui->toolButton_3->isChecked());
    //ui->groupBox->hide();
    // fill data bits
    ui->comboBox_3->addItem(QLatin1String("8"), SerialPort::Data8);
    ui->comboBox_3->addItem(QLatin1String("7"), SerialPort::Data7);
    ui->comboBox_3->addItem(QLatin1String("6"), SerialPort::Data6);
    ui->comboBox_3->addItem(QLatin1String("5"), SerialPort::Data5);
    ui->comboBox_3->setCurrentIndex(0);

    // fill stop bits
    ui->comboBox_4->addItem(QLatin1String("2"), SerialPort::TwoStop);
    #ifdef Q_OS_WIN
    ui->comboBox_4->addItem(QLatin1String("1.5"), SerialPort::OneAndHalfStop);
    #endif
    ui->comboBox_4->addItem(QLatin1String("1"), SerialPort::OneStop);
    ui->comboBox_4->setCurrentIndex(ui->comboBox_4->count()-1);

    // fill parity
    ui->comboBox_5->addItem(QLatin1String("None"), SerialPort::NoParity);
    ui->comboBox_5->addItem(QLatin1String("Even"), SerialPort::EvenParity);
    ui->comboBox_5->addItem(QLatin1String("Odd"), SerialPort::OddParity);
    ui->comboBox_5->addItem(QLatin1String("Mark"), SerialPort::MarkParity);
    ui->comboBox_5->addItem(QLatin1String("Space"), SerialPort::SpaceParity);
    ui->comboBox_5->setCurrentIndex(0);

    // fill flow control
    ui->comboBox_6->addItem(QLatin1String("None"), SerialPort::NoFlowControl);
    ui->comboBox_6->addItem(QLatin1String("RTS/CTS"), SerialPort::HardwareControl);
    ui->comboBox_6->addItem(QLatin1String("XON/XOFF"), SerialPort::SoftwareControl);
    ui->comboBox_6->setCurrentIndex(0);
    refreshAvSerialPort();
    //currPortName="";
    loadSettings();
    if(complList.isEmpty())
        complList<<"ATID"<<"ATCH"<<"ATPL"<<"ATRE"<<"ATCN"<<"ATAC"<<"ATFR"<<"ATAD"<<"ATVR"<<"ATSH"<<"ATSL";

    myCompl=new QCompleter(complList, this);
        myCompl->setCaseSensitivity(Qt::CaseInsensitive);

    ui->lineEdit->setCompleter(myCompl);
    for(qint32 i=1; i<ui->comboBox_2->count(); i++){
        if(ui->comboBox_2->itemText(i)==currPortName){
            ui->comboBox_2->setCurrentIndex(i);
            break;
        }

    }
    dozvilNaOnovlPortiv=1;
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeIndxToPushButton(int)));
    connect(ui->comboBox_3, SIGNAL(currentIndexChanged(int)), this, SLOT(changeIndxToPushButton(int)));
    connect(ui->comboBox_4, SIGNAL(currentIndexChanged(int)), this, SLOT(changeIndxToPushButton(int)));
    connect(ui->comboBox_5, SIGNAL(currentIndexChanged(int)), this, SLOT(changeIndxToPushButton(int)));
    connect(ui->comboBox_6, SIGNAL(currentIndexChanged(int)), this, SLOT(changeIndxToPushButton(int)));

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(focusLineEdit(int)));
    connect(ui->comboBox_3, SIGNAL(currentIndexChanged(int)), this, SLOT(focusLineEdit(int)));
    connect(ui->comboBox_4, SIGNAL(currentIndexChanged(int)), this, SLOT(focusLineEdit(int)));
    connect(ui->comboBox_5, SIGNAL(currentIndexChanged(int)), this, SLOT(focusLineEdit(int)));
    connect(ui->comboBox_6, SIGNAL(currentIndexChanged(int)), this, SLOT(focusLineEdit(int)));
    connect(ui->comboBox_7, SIGNAL(currentIndexChanged(int)), this, SLOT(focusLineEdit(int)));
    connect(ui->comboBox_8, SIGNAL(currentIndexChanged(int)), this, SLOT(focusLineEdit(int)));
    connect(ui->cbTextSet, SIGNAL(currentIndexChanged(int)), this, SLOT(focusLineEdit(int)));
    connect(ui->toolButton_2,SIGNAL(clicked()), this , SLOT(focusLineEdit2()));
    connect(ui->toolButton_3,SIGNAL(clicked()), this , SLOT(focusLineEdit2()));
    connect(ui->toolButton_4,SIGNAL(clicked()), this , SLOT(focusLineEdit2()));

    connect(&checkPort, SIGNAL(portDisconnected()), this, SLOT(closeSerialPort()));

    //    onovlennyaTimer=new QTimer(this);
    //   connect(onovlennyaTimer, SIGNAL(timeout()), this, SLOT(onOnovlTimer()));
    //    onovlennyaTimer->start(1500);
}
//-----------------------------------------------------------------------------------------------------------
Widget::~Widget()
{
    saveSettings();
    delete ui;
}
//-----------------------------------------------------------------------------------------------------------
void Widget::on_lineEdit_2_returnPressed()
{

}

//-----------------------------------------------------------------------------------------------------------

bool Widget::openSerialPort()
{
    dozvilNaOnovlPortiv=1;
    currPortName=ui->comboBox_2->currentText();
    SerialPort::DataBits dataBits=static_cast<SerialPort::DataBits>(ui->comboBox_3->itemData((ui->comboBox_3->currentIndex())).toInt()); //currentText().toInt());
    SerialPort::Parity parity=static_cast<SerialPort::Parity>(ui->comboBox_5->itemData(ui->comboBox_5->currentIndex()).toInt());
    SerialPort::StopBits stopBits=static_cast<SerialPort::StopBits>(ui->comboBox_4->itemData(ui->comboBox_4->currentIndex()).toInt());
    SerialPort::FlowControl flowControl=static_cast<SerialPort::FlowControl>(ui->comboBox_6->itemData(ui->comboBox_6->currentIndex()).toInt());
    mySerialPort->setPort(ui->comboBox_2->currentText());
    if(mySerialPort->open(QIODevice::ReadWrite)) {
        if(mySerialPort->setRate(ui->comboBox->currentText().toInt()) && mySerialPort->setParity(parity) && mySerialPort->setDataBits(dataBits) && mySerialPort->setStopBits(stopBits) && mySerialPort->setFlowControl(flowControl)){

            ui->plainTextEdit->moveCursor(QTextCursor::End);
            QTextCharFormat textCharFormat=ui->plainTextEdit->currentCharFormat();
            textCharFormat.setFontItalic(1);
            textCharFormat.setFontWeight(0);
            ui->plainTextEdit->setCurrentCharFormat(textCharFormat);
            ui->plainTextEdit->appendPlainText(tr("Opened <---> %0, %1\r\n").arg(ui->comboBox_2->currentText()).arg(ui->comboBox_2->itemData(ui->comboBox_2->currentIndex(), Qt::ToolTipRole).toString()));
            ui->plainTextEdit->moveCursor(QTextCursor::End);
            textCharFormat.setFontItalic(0);
            ui->plainTextEdit->setCurrentCharFormat(textCharFormat);
            ui->lineEdit->setEnabled(1);
            if(ui->lineEdit->text()=="AT command")
                ui->lineEdit->clear();
            ui->pushButton->setText("Cl&ose");
            ui->lineEdit->setFocus();
            ui->pushButton_2->setEnabled(0);
            if(ui->groupBox->isHidden())
            ui->comboBox->setEnabled(0);
            ui->toolButton->setEnabled(0);
            checkPort.zapuskalka(ui->comboBox_2->currentText());
            return 1;
        }
        else {
            mySerialPort->close();
            QMessageBox::critical(this, tr("Error"),tr("Can't configure the serial port: %1,\nerror code: %2").arg(ui->comboBox_2->currentText()).arg(mySerialPort->error()));
            refreshAvSerialPort();
        }

    }
    else{
        QMessageBox::critical(this, tr("Error"),tr("Can't opened the serial port: %1,\nerror code: %2").arg(ui->comboBox_2->currentText()).arg(mySerialPort->error()));
        refreshAvSerialPort();
    }


    return 0;
}

//-----------------------------------------------------------------------------------------------------------
void Widget::closeSerialPort()
{
    if(checkPort.isRunning())
        checkPort.terminate();

    mySerialPort->close();
    ui->plainTextEdit->moveCursor(QTextCursor::End);
    QTextCharFormat textCharFormat=ui->plainTextEdit->currentCharFormat();
    textCharFormat.setFontItalic(1);
    textCharFormat.setFontWeight(0);
    ui->plainTextEdit->setCurrentCharFormat(textCharFormat);
    ui->plainTextEdit->appendPlainText(tr("Close >...<\r\n"));
    ui->plainTextEdit->moveCursor(QTextCursor::End);
    textCharFormat.setFontItalic(0);
    ui->plainTextEdit->setCurrentCharFormat(textCharFormat);
    ui->comboBox->setEnabled(1);
    ui->toolButton->setEnabled(1);
    if(ui->lineEdit->text().isEmpty())
        ui->lineEdit->setText("AT command");
    ui->lineEdit->setEnabled(0);
    ui->pushButton->setText("&Open");
}

//-----------------------------------------------------------------------------------------------------------
void Widget::refreshAvSerialPort()
{
    if(ui->pushButton->text()=="Cl&ose")
        closeSerialPort();
    dozvilNaOnovlPortiv=0;
    QStringList oldList;
    QStringList oldListInfo;
    QStringList newList;
    QStringList newListInfo;
    for(qint32 i=1; i<ui->comboBox_2->count(); i++) {
        oldList.append(ui->comboBox_2->itemText(i));
        oldListInfo.append(ui->comboBox_2->itemData(i, Qt::ToolTipRole).toString());
    }
    ui->comboBox_2->clear();
    ui->comboBox_2->addItem("REFRESH");
    foreach (const SerialPortInfo &info, SerialPortInfo::availablePorts()) {
        newList<<info.portName();
        newListInfo<< info.description()+",   "+ info.manufacturer() +",   "+ info.systemLocation();
    }
    qint32 currIndex=0;

    for(qint32 i=0; i<oldList.count(); i++) {
            bool vidsutnij=1;
            for(qint32 j=0; j<newList.count();j++) {
                if(oldList[i]==newList[j]) {
                    newList.removeAt(j);
                    newListInfo.removeAt(j);
                    vidsutnij=0;
                    break;
                }
            }
            if(vidsutnij) {
                oldList.removeAt(i);
                oldListInfo.removeAt(i);
                i--;
            }
        }

        for(qint32 i=0; i<newList.count(); i++) {
                ui->comboBox_2->addItem(newList[i]);
             ui->comboBox_2->setItemData(i+1, newListInfo[i], Qt::ToolTipRole);
        }
        for(qint32 i=0, j=ui->comboBox_2->count(); i<oldList.count();i++){
            ui->comboBox_2->addItem(oldList[i]);
            ui->comboBox_2->setItemData(i+j, oldListInfo[i], Qt::ToolTipRole);
            if(currPortName==oldList[i])
                currIndex=i+j;
        }
//        qDebug()<<QString::number(ui->comboBox_2->count());
        if(currIndex==0) {
            if(ui->comboBox_2->count()>1)
                ui->comboBox_2->setCurrentIndex(1);
            else
                ui->comboBox_2->setCurrentIndex(0);
        }
        else
            ui->comboBox_2->setCurrentIndex(currIndex);

}

//-----------------------------------------------------------------------------------------------------------
void Widget::loadSettings()
{
    QSettings setting ("Hello_ZB","Qt_Term");
    setting.beginGroup("SerialPort");
   qint32 pIndex= setting.value("baud").toInt();
   QRect rect = setting.value("position").toRect();
   if(rect.x()< 10)
       rect.setX(10);
   if(rect.y()<30)
       rect.setY(31);
    setGeometry(rect);
   if(pIndex)
       ui->comboBox->setCurrentIndex((--pIndex));
   else
          ui->comboBox->setCurrentIndex(1);

    currPortName=setting.value("curPortName").toString();

    pIndex= setting.value("databits").toInt();
    if(pIndex)
        ui->comboBox_3->setCurrentIndex(--pIndex);
    else
        ui->comboBox_3->setCurrentIndex(0);

    pIndex= setting.value("stopbits").toInt();
    if(pIndex)
        ui->comboBox_4->setCurrentIndex(--pIndex);
    else
        ui->comboBox_4->setCurrentIndex(ui->comboBox_4->count()-1);

    pIndex= setting.value("parity").toInt();
    if(pIndex)
        ui->comboBox_5->setCurrentIndex(--pIndex);
    else
        ui->comboBox_5->setCurrentIndex(0);

    pIndex= setting.value("flowcontrol").toInt();
    if(pIndex)
        ui->comboBox_6->setCurrentIndex(--pIndex);
    else
        ui->comboBox_6->setCurrentIndex(0);

    pIndex= setting.value("crlfN").toInt();
    if(pIndex)
        ui->comboBox_7->setCurrentIndex(--pIndex);
    else
        ui->comboBox_7->setCurrentIndex(0);

    pIndex= setting.value("crlfFN").toInt();
    if(pIndex)
        ui->comboBox_8->setCurrentIndex(--pIndex);
    else
        ui->comboBox_8->setCurrentIndex(3);
    QString str=setting.value("fnKey").toString();

    ui->cbTextSet->setCurrentIndex(setting.value("textmose", 0).toInt());

    if(!str.isEmpty())
        ui->lineEdit_2->setText(str);
    ui->checkBox->setChecked(!setting.value("atcc").toBool());
    //    setting.setValue("atcc", ui->checkBox->isChecked());

    ui->toolButton_3->setChecked(setting.value("vis").toBool());
    ui->groupBox->setHidden(!ui->toolButton_3->isChecked());
    ui->groupBox_2->setHidden(!ui->toolButton_3->isChecked());
//    setting.setValue("crlfN", ui->comboBox_7->currentIndex()+1);
//    setting.setValue("crlfFN", ui->comboBox_8->currentIndex()+1);
//    setting.setValue("fnKey", ui->lineEdit_2->text());
    complList=setting.value("list").toStringList();
    setting.endGroup();
}

//-----------------------------------------------------------------------------------------------------------
void Widget::saveSettings()
{
    QSettings setting ("Hello_ZB","Qt_Term");
    setting.beginGroup("SerialPort");
    setting.setValue("baud",(ui->comboBox->currentIndex()+1));
    setting.setValue("curPortName", currPortName);
    setting.setValue("position", this->geometry());
    setting.setValue("databits", (ui->comboBox_3->currentIndex()+1));
    setting.setValue("stopbits", (ui->comboBox_4->currentIndex()+1));
    setting.setValue("parity", (ui->comboBox_5->currentIndex()+1));
    setting.setValue("flowcontrol", (ui->comboBox_6->currentIndex()+1));
    setting.setValue("crlfN", ui->comboBox_7->currentIndex()+1);
    setting.setValue("crlfFN", ui->comboBox_8->currentIndex()+1);
    setting.setValue("fnKey", ui->lineEdit_2->text());
    setting.setValue("vis", ui->toolButton_3->isChecked());
    setting.setValue("atcc", !ui->checkBox->isChecked());
    setting.setValue("textmose", ui->cbTextSet->currentIndex());
    while(complList.count()>100)
        complList.removeLast();
    setting.setValue("list", complList);
    setting.endGroup();
}

//-----------------------------------------------------------------------------------------------------------

void Widget::on_toolButton_3_clicked(bool checked)
{
    ui->groupBox->setHidden(!checked);
    ui->groupBox_2->setHidden(!checked);
    if(checked) {

        if(ui->pushButton->text()=="Cl&ose"){
            ui->comboBox->setEnabled(1);

        }
        ui->plainTextEdit->moveCursor(QTextCursor::End);

    }
    else {
        if(ui->pushButton->text()=="Cl&ose")
            ui->comboBox->setEnabled(0);
    }



}

//-----------------------------------------------------------------------------------------------------------
void Widget::on_toolButton_4_clicked()
{
    QMessageBox::about(this, tr("About Terminalka"),
                       tr("<b>Simple serial port terminal.</b><br>"
                          "build date 2013-06-01<br>")+QString(QString::fromUtf8("©"))+tr(" Bogdan Zikranets, 2013"));
}

//-----------------------------------------------------------------------------------------------------------

void Widget::on_pushButton_clicked()
{
    if(ui->comboBox_2->currentText()=="REFRESH")
        refreshAvSerialPort();
    else{
        if(ui->pushButton->text()=="&Open")
            openSerialPort();
        else
            closeSerialPort();
    }
}

//-----------------------------------------------------------------------------------------------------------
void Widget::on_comboBox_2_highlighted(const QString &arg1)
{

    if(!arg1.isEmpty() && (arg1=="REFRESH" || dozvilNaOnovlPortiv))
    refreshAvSerialPort();
}

//-----------------------------------------------------------------------------------------------------------
void Widget::on_comboBox_2_activated(const QString &arg1)
{
    if(arg1=="REFRESH")
    {
         dozvilNaOnovlPortiv=0;
         refreshAvSerialPort();
    }
    else
    {
        if(ui->pushButton->text()=="&Open")
            openSerialPort();
        else
        {
            closeSerialPort();
            openSerialPort();
        }
    }
}

//-----------------------------------------------------------------------------------------------------------

void Widget::readToPlainText()
{
    QByteArray data=mySerialPort->readAll();

    if(!data.isEmpty()) {
        ui->plainTextEdit->moveCursor(QTextCursor::End);

        QTextCharFormat textCharFormat=ui->plainTextEdit->currentCharFormat();
        textCharFormat.setFontItalic(0);
        textCharFormat.setFontWeight(0);
        ui->plainTextEdit->setCurrentCharFormat(textCharFormat);
        if(ui->cbTextSet->currentIndex()>0){
            for(int i=1; i<1250 && mySerialPort->waitForReadyRead(25); i++){
                data.append(mySerialPort->readAll());
                if((i%100)==0)
                    update();
            }
            QString strNorm=data;

            for(int ff=0, ffMax=data.size(); ff<ffMax; ff+=16){

                QString hexStr=data.mid(ff,16).toHex().leftJustified(32, ' ');

                for(int i=30; i>1; i-=2)
                    hexStr=hexStr.insert(i, " ");

                ui->plainTextEdit->insertPlainText(QString("%1   %2\r\r").arg(hexStr.toUpper()).arg(strNorm.mid(ff,16).simplified()));
            }

        }else
        ui->plainTextEdit->insertPlainText(data/*.toHex().toUpper()*/);
//        ui->plainTextEdit->appendPlainText("");
        ui->plainTextEdit->moveCursor(QTextCursor::End);

    }

}

//-----------------------------------------------------------------------------------------------------------

void Widget::on_lineEdit_returnPressed()
{
    QByteArray data=ui->lineEdit->text().toLocal8Bit();
    if(ui->cbTextSet->currentIndex()>1){
        QString str=ui->lineEdit->text();
        str.remove(" ");
        bool ok=1;
        for(int i=0, iMax=str.size(); i<iMax; i+=7){
            str.mid(i,7).toInt(&ok, 16);
            if(!ok)
                break;
        }
        if(!ok){
            ui->lineEdit->setStyleSheet("QLineEdit{background: qlineargradient(spread:reflect, x1:0.486513, y1:0.339, x2:1, y2:0.346, stop:0.572193 rgba(255, 0, 0, 193), stop:1 rgba(255, 255, 255, 255));}");
            return;
        }else
            ui->lineEdit->setStyleSheet(ui->lineEdit_2->styleSheet());

        data=QByteArray::fromHex(str.remove(" ").toLocal8Bit());
    }

    if(!data.isEmpty() && data.left(3)!="+++" && data.left(3)!="&&&") {

        QString str=ui->lineEdit->text().toUpper();

        for(qint32 i=0; i<complList.count(); i++){
            if(str==complList[i].toUpper()){
                complList.removeAt(i);
                break;
            }

        }
    }


    ui->plainTextEdit->moveCursor(QTextCursor::End);
    QTextCharFormat textCharFormat=ui->plainTextEdit->currentCharFormat();
    textCharFormat.setFontItalic(0);
    textCharFormat.setFontWeight(QFont::Bold);
    ui->plainTextEdit->setCurrentCharFormat(textCharFormat);

    switch (ui->comboBox_7->currentIndex()) {
    case 0: { data+="\r\n"; break; }
    case 1: { data+="\r" ; break; }
    case 2: { data+="\n"; break; }
    }

    if(ui->cbTextSet->currentIndex()>1){
        QString strNorm=data;
        for(int ff=0, ffMax=data.size(); ff<ffMax; ff+=16){

            QString hexStr=data.mid(ff,16).toHex().leftJustified(32, ' ');

            for(int i=30; i>1; i-=2)
                hexStr=hexStr.insert(i, " ");

            ui->plainTextEdit->insertPlainText(QString("%1   %2\r").arg(hexStr.toUpper()).arg(strNorm.mid(ff,16).simplified()));

        }
    }else{
        if(ui->cbTextSet->currentIndex()>0){

            QString strNorm=data;
            for(int ff=0, ffMax=data.size(); ff<ffMax; ff+=16){

                QString hexStr=data.mid(ff,16).toHex().leftJustified(32, ' ');

                for(int i=30; i>1; i-=2)
                    hexStr=hexStr.insert(i, " ");

                ui->plainTextEdit->insertPlainText(QString("%1   %2\r").arg(hexStr.toUpper()).arg(strNorm.mid(ff,16).simplified()));
            }

        }else
            ui->plainTextEdit->insertPlainText(data.simplified()+"\r");
    }

    ui->plainTextEdit->moveCursor(QTextCursor::End);



    mySerialPort->write(data);
    selPosition=0;
    QTimer::singleShot(50, this, SLOT(clearLine()));

}

//-----------------------------------------------------------------------------------------------------------
void Widget::clearLine()
{
    if(ui->lineEdit->isEnabled()){
        if(!ui->lineEdit->text().isEmpty() && ui->lineEdit->text().left(3)!="+++" && ui->lineEdit->text().left(3)!="&&&"){
            delete myCompl;
            if(selPosition)
                complList.insert(0,ui->lineEdit->text().left(ui->lineEdit->text().count()-1));
            else
                complList.insert(0,ui->lineEdit->text());

            myCompl=new QCompleter(complList, this);
            myCompl->setCaseSensitivity(Qt::CaseInsensitive);
            ui->lineEdit->setCompleter(myCompl);
        }
        ui->lineEdit->clear();
        ui->lineEdit->setFocus();
    }

}

//-----------------------------------------------------------------------------------------------------------

void Widget::on_lineEdit_textEdited(const QString &arg1)
{
    if(ui->cbTextSet->currentIndex()>1){

        QString str=arg1;
        bool writeAfter=0;
        if(arg1.right(1)==ui->lineEdit_2->text()) {
            writeAfter=1;
           str=str.left(str.size()-1);
        }
        str.remove(' ');
        bool ok=1;
        for(int i=0, iMax=str.size(); i<iMax; i+=7){
            str.mid(i,7).toInt(&ok, 16);
            if(!ok)
                break;
        }
        if(!ok){
            ui->lineEdit->setStyleSheet("QLineEdit{background: qlineargradient(spread:reflect, x1:0.486513, y1:0.339, x2:1, y2:0.346, stop:0.572193 rgba(255, 0, 0, 193), stop:1 rgba(255, 255, 255, 255));}");
            writeAfter=0;
        }else
            ui->lineEdit->setStyleSheet(ui->lineEdit_2->styleSheet());

        if(writeAfter){
            ui->plainTextEdit->moveCursor(QTextCursor::End);
            QTextCharFormat textCharFormat=ui->plainTextEdit->currentCharFormat();
            textCharFormat.setFontItalic(0);
            textCharFormat.setFontWeight(QFont::Bold);
            ui->plainTextEdit->setCurrentCharFormat(textCharFormat);
            QByteArray data=QByteArray::fromHex(str.toLocal8Bit());;
            switch (ui->comboBox_8->currentIndex()) {
            case 0: { data+="\r\n"; break; }
            case 1: { data+="\r" ; break; }
            case 2: { data+="\n"; break; }
            }

            QString strNorm=data;
            for(int ff=0, ffMax=data.size(); ff<ffMax; ff+=16){

                QString hexStr=data.mid(ff,16).toHex().leftJustified(32, ' ');

                for(int i=30; i>1; i-=2)
                    hexStr=hexStr.insert(i, " ");

                ui->plainTextEdit->insertPlainText(QString("%1   %2\r").arg(hexStr.toUpper()).arg(strNorm.mid(ff,16).simplified()));
                ui->plainTextEdit->moveCursor(QTextCursor::End);
                mySerialPort->write(data);
                selPosition=1;
                QTimer::singleShot(50, this, SLOT(clearLine()));
            }
        }

    } else {

        if(arg1.right(1)==ui->lineEdit_2->text()) {
            QByteArray data=arg1.left(arg1.count()-1).toLocal8Bit();

            if(!data.isEmpty() && data.left(3)!="+++" && data.left(3)!="&&&") {

                QString str=arg1.left(arg1.count()-1).toUpper();
                for(qint32 i=0; i<complList.count(); i++){
                    if(str==complList[i].toUpper()){
                        complList.removeAt(i);
                        break;
                    }

                }
            }
            ui->plainTextEdit->moveCursor(QTextCursor::End);
            QTextCharFormat textCharFormat=ui->plainTextEdit->currentCharFormat();
            textCharFormat.setFontItalic(0);
            textCharFormat.setFontWeight(QFont::Bold);
            ui->plainTextEdit->setCurrentCharFormat(textCharFormat);
            switch (ui->comboBox_8->currentIndex()) {
            case 0: { data+="\r\n"; break; }
            case 1: { data+="\r" ; break; }
            case 2: { data+="\n"; break; }
            }

            if(ui->cbTextSet->currentIndex()>0){
                QString strNorm=data;
                for(int ff=0, ffMax=data.size(); ff<ffMax; ff+=16){

                    QString hexStr=data.mid(ff,16).toHex().leftJustified(32, ' ');

                    for(int i=30; i>1; i-=2)
                        hexStr=hexStr.insert(i, " ");

                    ui->plainTextEdit->insertPlainText(QString("%1   %2\r\r").arg(hexStr.toUpper()).arg(strNorm.mid(ff,16).simplified()));
                }

            }else
                ui->plainTextEdit->insertPlainText(data.simplified()+"\r");

            // ui->plainTextEdit->insertPlainText(data+"\r\n");
            ui->plainTextEdit->moveCursor(QTextCursor::End);



            mySerialPort->write(data);
            selPosition=1;
            QTimer::singleShot(50, this, SLOT(clearLine()));
        }
        else {

            if(ui->checkBox->isChecked() &&( arg1.left(3)=="+++" || arg1.left(3)=="&&&")){

                QByteArray data=arg1.toLocal8Bit();

                ui->plainTextEdit->moveCursor(QTextCursor::End);
                QTextCharFormat textCharFormat=ui->plainTextEdit->currentCharFormat();
                textCharFormat.setFontItalic(0);
                textCharFormat.setFontWeight(QFont::Bold);
                ui->plainTextEdit->setCurrentCharFormat(textCharFormat);
                data+="\r\n";
                if(ui->cbTextSet->currentIndex()>0){
                    QString strNorm=data;

                    for(int ff=0, ffMax=data.size(); ff<ffMax; ff+=16){

                        QString hexStr=data.mid(ff,16).toHex().leftJustified(32, ' ');

                        for(int i=30; i>1; i-=2)
                            hexStr=hexStr.insert(i, " ");

                        ui->plainTextEdit->insertPlainText(QString("%1   %2\r").arg(hexStr.toUpper()).arg(strNorm.mid(ff,16).simplified()));
                    }

                }else
                ui->plainTextEdit->insertPlainText(data);
                ui->plainTextEdit->moveCursor(QTextCursor::End);



                mySerialPort->write(data);
                ui->lineEdit->clear();
            }
        }
    }

}
//-----------------------------------------------------------------------------------------------------------
void Widget::on_pushButton_2_clicked()
{
    ui->pushButton_2->setEnabled(0);
    SerialPort::DataBits dataBits=static_cast<SerialPort::DataBits>(ui->comboBox_3->itemData((ui->comboBox_3->currentIndex())).toInt()); //currentText().toInt());
    SerialPort::Parity parity=static_cast<SerialPort::Parity>(ui->comboBox_5->itemData(ui->comboBox_5->currentIndex()).toInt());
    SerialPort::StopBits stopBits=static_cast<SerialPort::StopBits>(ui->comboBox_4->itemData(ui->comboBox_4->currentIndex()).toInt());
    SerialPort::FlowControl flowControl=static_cast<SerialPort::FlowControl>(ui->comboBox_6->itemData(ui->comboBox_6->currentIndex()).toInt());

        if(mySerialPort->setRate(ui->comboBox->currentText().toInt()) && mySerialPort->setParity(parity) && mySerialPort->setDataBits(dataBits) && mySerialPort->setStopBits(stopBits) && mySerialPort->setFlowControl(flowControl)){

            ui->plainTextEdit->moveCursor(QTextCursor::End);
            QTextCharFormat textCharFormat=ui->plainTextEdit->currentCharFormat();
            textCharFormat.setFontItalic(1);
            textCharFormat.setFontWeight(0);
            ui->plainTextEdit->setCurrentCharFormat(textCharFormat);
            ui->plainTextEdit->appendPlainText(tr("Changed!\r\n"));
            ui->plainTextEdit->moveCursor(QTextCursor::End);
            textCharFormat.setFontItalic(0);
            ui->plainTextEdit->setCurrentCharFormat(textCharFormat);
            ui->lineEdit->setFocus();
        }
        else {
            mySerialPort->close();
            QMessageBox::critical(this, tr("Error"),tr("Can't configure the serial port: %1,\nerror code: %2").arg(ui->comboBox_2->currentText()).arg(mySerialPort->error()));
            refreshAvSerialPort();
        }
}

//-----------------------------------------------------------------------------------------------------------
void Widget::changeIndxToPushButton(int indx)
{
    if(ui->pushButton->text()=="Cl&ose"){
        ui->pushButton_2->setEnabled(1);
    }
}

//-----------------------------------------------------------------------------------------------------------
void Widget::focusLineEdit(int indx)
{
    if(ui->lineEdit->isEnabled())
        ui->lineEdit->setFocus();
}

//-----------------------------------------------------------------------------------------------------------
void Widget::focusLineEdit2()
{
    if(ui->lineEdit->isEnabled())
        ui->lineEdit->setFocus();
}
//-----------------------------------------------------------------------------------------------------------

void Widget::on_cbTextSet_currentIndexChanged(int index)
{
    if(index>1){
        ui->checkBox->setEnabled(0);
        QString str=ui->lineEdit->text();

        bool ok=1;
        for(int i=0, iMax=str.size(); i<iMax; i+=7){
            str.mid(i,7).toInt(&ok, 16);
            if(!ok)
                break;
        }
        if(!ok){
            if(ui->lineEdit->isEnabled())
            ui->lineEdit->setStyleSheet("QLineEdit{background: qlineargradient(spread:reflect, x1:0.486513, y1:0.339, x2:1, y2:0.346, stop:0.572193 rgba(255, 0, 0, 193), stop:1 rgba(255, 255, 255, 255));}");
            return;
        }else
            ui->lineEdit->setStyleSheet(ui->lineEdit_2->styleSheet());
    }else{
        ui->lineEdit->setStyleSheet(ui->lineEdit_2->styleSheet());
        ui->checkBox->setEnabled(1);
    }
}
