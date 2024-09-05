#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QTimer>
#include <QCheckBox>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>
#include <QtNetwork>
#include "grSim_Packet.pb.h"
#include "grSim_Commands.pb.h"
#include "grSim_Replacement.pb.h"
#include "ssl_vision_detection.pb.h"
#include "ssl_vision_wrapper.pb.h"

class MainWindow : public QDialog
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void reconnectUdp();
    void sendPacket();
    void sendBtnClicked();
    void resetBtnClicked();
    void disconnectUdp();
    void receiveBtnClicked();
    void processPendingDatagrams();
    void printRobotInfo(const SSL_DetectionRobot &robot);
    void updateRobotInfo(const SSL_DetectionRobot &robot, const QString &color);
    void stopReceiving();

private:
    QPushButton *btnReceive;
    bool sending, reseting;
    QUdpSocket udpsocket;
    QHostAddress _addr;
    quint16 _port;
    QLineEdit *edtIp;
    QLineEdit *edtPort;
    QLineEdit *edtId;
    QLineEdit *edtVx, *edtVy, *edtW;
    QLineEdit *edtV1, *edtV2, *edtV3, *edtV4;
    QLineEdit *edtKick, *edtChip;
    QLabel *lblIp;
    QLabel *lblPort;
    QLabel *lblId;
    QComboBox *cmbTeam;
    QLabel *lblVx, *lblVy, *lblW;
    QLabel *lblV1, *lblV2, *lblV3, *lblV4;
    QLabel *lblKick, *lblChip;
    QTextEdit *txtInfo;
    QCheckBox *chkVel, *chkSpin;
    QPushButton *btnSend;
    QPushButton *btnReset;
    QPushButton *btnConnect;
    QPushButton *btnStopReceive ;
    QTimer *timer;
    QLabel *lblStatus;
};

#endif // MAINWINDOW_H
