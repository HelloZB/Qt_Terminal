#include "checkcurrport.h"
#include <QtAddOnSerialPort/serialportinfo.h>

//#include <QTime>
//#include <QDebug>

CheckCurrPort::CheckCurrPort(QObject *parent) :
    QThread(parent)
{
}

void CheckCurrPort::zapuskalka(QString currPort)
{
    this->currPort=currPort;   
    this->start();
}

void CheckCurrPort::run()
{
//    qDebug()<<QTime::currentTime().toString()<<"startt";
    while(true){
        msleep(1500);
        bool found=0;
        foreach (const SerialPortInfo &info, SerialPortInfo::availablePorts()){  //SerialPortInfo &info, SerialPortInfo::availablePorts())

            if(info.portName()==currPort){
                found=1;
                SerialPort testConn;//(currPort);
                testConn.setPort(currPort);
                if(testConn.open(QIODevice::ReadWrite)){
                   //???? testConn.close();
                    emit portDisconnected();
                    //closeSerialPort(1);
//                    qDebug()<<QTime::currentTime().toString()<<"opened!";
                    //return;
                } /*else {
                    qDebug()<<QTime::currentTime().toString()<<"can not open";
                }*/
                break;
            }
       }
        if(!found){
//            closeSerialPort(1);            
//             qDebug()<<QTime::currentTime().toString()<<" not found!";
             emit portDisconnected();
             //return;
        }
    }
//    qDebug()<<QTime::currentTime().toString()<<"exitttt";
}
