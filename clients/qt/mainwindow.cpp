#include "mainwindow.h"
#include <QGridLayout>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QDialog(parent),
      udpsocket(this)
{
    QGridLayout *layout = new QGridLayout(this);
    edtIp = new QLineEdit("127.0.0.1", this);
    edtPort = new QLineEdit("20011", this);
    edtId = new QLineEdit("0", this);
    edtVx = new QLineEdit("0", this);
    edtVy = new QLineEdit("0", this);
    edtW = new QLineEdit("0", this);
    edtV1 = new QLineEdit("0", this);
    edtV2 = new QLineEdit("0", this);
    edtV3 = new QLineEdit("0", this);
    edtV4 = new QLineEdit("0", this);
    edtChip = new QLineEdit("0", this);
    edtKick = new QLineEdit("0", this);
    txtInfo = new QTextEdit(this);
    txtInfo->setReadOnly(true);

    this->setWindowTitle(QString("grSim Sample Client - v 2.0"));

    lblIp = new QLabel("Simulator Address", this);
    lblPort = new QLabel("Simulator Port", this);
    lblId = new QLabel("Id", this);
    lblVx = new QLabel("Velocity X (m/s)", this);
    lblVy = new QLabel("Velocity Y (m/s)", this);
    lblW = new QLabel("Velocity W (rad/s)", this);
    lblV1 = new QLabel("Wheel1 (rad/s)", this);
    lblV2 = new QLabel("Wheel2 (rad/s)", this);
    lblV3 = new QLabel("Wheel3 (rad/s)", this);
    lblV4 = new QLabel("Wheel4 (rad/s)", this);
    cmbTeam = new QComboBox(this);
    cmbTeam->addItem("Yellow");
    cmbTeam->addItem("Blue");
    lblChip = new QLabel("Chip (m/s)", this);
    lblKick = new QLabel("Kick (m/s)", this);
    // txtInfo = new QTextEdit(this);
    chkVel = new QCheckBox("Send Velocity? (or wheels)", this);
    chkSpin = new QCheckBox("Spin", this);
    btnSend = new QPushButton("Send", this);
    btnReset = new QPushButton("Reset", this);
    btnConnect = new QPushButton("Connect", this);
    btnReceive = new QPushButton("Receive", this);
    // txtInfo->setReadOnly(true);
    // txtInfo->setHtml("This program is part of <b>grSim RoboCup SSL Simulator</b> package.<br />For more information please refer to <a href=\"http://eew.aut.ac.ir/~parsian/grsim/\">http://eew.aut.ac.ir/~parsian/grsim</a><br /><font color=\"gray\">This program is free software under the terms of GNU General Public License Version 3.</font>");
    // txtInfo->setFixedHeight(70);
    layout->addWidget(lblIp, 1, 1, 1, 1);
    layout->addWidget(edtIp, 1, 2, 1, 1);
    layout->addWidget(lblPort, 1, 3, 1, 1);
    layout->addWidget(edtPort, 1, 4, 1, 1);
    layout->addWidget(lblId, 2, 1, 1, 1);
    layout->addWidget(edtId, 2, 2, 1, 1);
    layout->addWidget(cmbTeam, 2, 3, 1, 2);
    layout->addWidget(lblVx, 3, 1, 1, 1);
    layout->addWidget(edtVx, 3, 2, 1, 1);
    layout->addWidget(lblVy, 4, 1, 1, 1);
    layout->addWidget(edtVy, 4, 2, 1, 1);
    layout->addWidget(lblW, 5, 1, 1, 1);
    layout->addWidget(edtW, 5, 2, 1, 1);
    layout->addWidget(chkVel, 6, 1, 1, 1);
    layout->addWidget(edtKick, 6, 2, 1, 1);
    layout->addWidget(lblV1, 3, 3, 1, 1);
    layout->addWidget(edtV1, 3, 4, 1, 1);
    layout->addWidget(lblV2, 4, 3, 1, 1);
    layout->addWidget(edtV2, 4, 4, 1, 1);
    layout->addWidget(lblV3, 5, 3, 1, 1);
    layout->addWidget(edtV3, 5, 4, 1, 1);
    layout->addWidget(lblV4, 6, 3, 1, 1);
    layout->addWidget(edtV4, 6, 4, 1, 1);
    layout->addWidget(lblChip, 7, 1, 1, 1);
    layout->addWidget(edtChip, 7, 2, 1, 1);
    layout->addWidget(lblKick, 7, 3, 1, 1);
    layout->addWidget(edtKick, 7, 4, 1, 1);
    layout->addWidget(chkSpin, 8, 1, 1, 4);
    layout->addWidget(btnConnect, 9, 1, 1, 2);
    layout->addWidget(btnSend, 9, 3, 1, 1);
    layout->addWidget(btnReset, 9, 4, 1, 1);
    layout->addWidget(btnReceive, 10, 1, 1, 4);
    layout->addWidget(txtInfo, 11, 1, 1, 4);
    // layout->addWidget(txtInfo, 10, 1, 1, 4);
    timer = new QTimer(this);
    timer->setInterval(20);
    connect(edtIp, SIGNAL(textChanged(QString)), this, SLOT(disconnectUdp()));
    connect(edtPort, SIGNAL(textChanged(QString)), this, SLOT(disconnectUdp()));
    connect(timer, SIGNAL(timeout()), this, SLOT(sendPacket()));
    connect(btnConnect, SIGNAL(clicked()), this, SLOT(reconnectUdp()));
    connect(btnSend, SIGNAL(clicked()), this, SLOT(sendBtnClicked()));
    connect(btnReset, SIGNAL(clicked()), this, SLOT(resetBtnClicked()));
    connect(btnReceive, SIGNAL(clicked()), this, SLOT(receiveBtnClicked()));
    btnSend->setDisabled(true);
    chkVel->setChecked(true);
    sending = false;
    reseting = false;
}

MainWindow::~MainWindow()
{
}

void MainWindow::disconnectUdp()
{
    udpsocket.close();
    sending = false;
    btnSend->setText("Send");
    btnSend->setDisabled(true);
}

void MainWindow::sendBtnClicked()
{
    sending = !sending;
    if (!sending)
    {
        timer->stop();
        btnSend->setText("Send");
    }
    else
    {
        timer->start();
        btnSend->setText("Pause");
    }
}

void MainWindow::resetBtnClicked()
{
    reseting = true;
    edtVx->setText("0");
    edtVy->setText("0");
    edtW->setText("0");
    edtV1->setText("0");
    edtV2->setText("0");
    edtV3->setText("0");
    edtV4->setText("0");
    edtChip->setText("0");
    edtKick->setText("0");
    chkVel->setChecked(true);
    chkSpin->setChecked(false);
}

void MainWindow::reconnectUdp()
{
    _addr = QHostAddress(edtIp->text());
    _port = edtPort->text().toUShort();
    btnSend->setDisabled(false);
}

void MainWindow::sendPacket()
{
    if (reseting)
    {
        sendBtnClicked();
        reseting = false;
    }
    grSim_Packet packet;
    bool yellow = false;
    if (cmbTeam->currentText() == "Yellow")
        yellow = true;
    packet.mutable_commands()->set_isteamyellow(yellow);
    packet.mutable_commands()->set_timestamp(0.0);
    grSim_Robot_Command *command = packet.mutable_commands()->add_robot_commands();
    command->set_id(edtId->text().toInt());

    command->set_wheelsspeed(!chkVel->isChecked());
    command->set_wheel1(edtV1->text().toDouble());
    command->set_wheel2(edtV2->text().toDouble());
    command->set_wheel3(edtV3->text().toDouble());
    command->set_wheel4(edtV4->text().toDouble());
    command->set_veltangent(edtVx->text().toDouble());
    command->set_velnormal(edtVy->text().toDouble());
    command->set_velangular(edtW->text().toDouble());

    command->set_kickspeedx(edtKick->text().toDouble());
    command->set_kickspeedz(edtChip->text().toDouble());
    command->set_spinner(chkSpin->isChecked());

    QByteArray dgram;
    dgram.resize(packet.ByteSize());
    packet.SerializeToArray(dgram.data(), dgram.size());
    udpsocket.writeDatagram(dgram, _addr, _port);
}

void MainWindow::receiveBtnClicked()
{
    // Configurar o endereço e porta de multicast
    QHostAddress multicastAddress("222.5.23.2");
    quint16 port = 10006;

    // Fechar o socket atual se estiver aberto
    if (udpsocket.isOpen())
    {
        udpsocket.close();
    }

    // Configurar o socket para escutar no endereço multicast
    udpsocket.bind(QHostAddress::AnyIPv4, port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    udpsocket.joinMulticastGroup(multicastAddress);

    // Conectar o slot de leitura para processar os dados recebidos
    connect(&udpsocket, &QUdpSocket::readyRead, this, &MainWindow::processPendingDatagrams);
}

void MainWindow::printRobotInfo(const SSL_DetectionRobot &robot)
{
    printf("CONF=%4.2f ", robot.confidence());
    if (robot.has_robot_id())
    {
        printf("ID=%3d ", robot.robot_id());
    }
    else
    {
        printf("ID=N/A ");
    }
    printf(" HEIGHT=%6.2f POS=<%9.2f,%9.2f> ", robot.height(), robot.x(), robot.y());
    if (robot.has_orientation())
    {
        printf("ANGLE=%6.3f ", robot.orientation());
    }
    else
    {
        printf("ANGLE=N/A    ");
    }
    printf("RAW=<%8.2f,%8.2f>\n", robot.pixel_x(), robot.pixel_y());
}

void MainWindow::updateRobotInfo(const SSL_DetectionRobot &robot, const QString &color)
{
    QString info;

    info += color;

    // Adiciona o nome do robô e ID
    if (robot.has_robot_id())
    {
        info += QString("Robot ID=%1 ").arg(robot.robot_id());
    }
    else
    {
        info += QString("Robot ID=N/A ");
    }

    // Adiciona as informações de confiança e posição
    info += QString("CONF=%1 ").arg(robot.confidence(), 0, 'f', 2);
    info += QString("HEIGHT=%1 ").arg(robot.height(), 0, 'f', 2);
    info += QString("POS=<%1,%2> ").arg(robot.x(), 0, 'f', 2).arg(robot.y(), 0, 'f', 2);
    if (robot.has_orientation())
    {
        info += QString("ANGLE=%1 ").arg(robot.orientation(), 0, 'f', 3);
    }
    else
    {
        info += QString("ANGLE=N/A ");
    }
    info += QString("RAW=<%1,%2>").arg(robot.pixel_x(), 0, 'f', 2).arg(robot.pixel_y(), 0, 'f', 2);

    // Adiciona o texto ao QTextEdit
    txtInfo->append(info);
}

void MainWindow::processPendingDatagrams()
{
    while (udpsocket.hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpsocket.pendingDatagramSize());
        udpsocket.readDatagram(datagram.data(), datagram.size());

        // Criar um pacote SSL_WrapperPacket e tentar analisar os dados recebidos
        SSL_WrapperPacket packet;
        if (packet.ParseFromArray(datagram.data(), datagram.size()))
        {
            if (packet.has_detection())
            {
                SSL_DetectionFrame detection = packet.detection();

                // Exibir informações sobre os robôs detectados
                printf("-[Detection Data]-------\n");
                printf("Camera ID=%d FRAME=%d T_CAPTURE=%.4f\n",
                       detection.camera_id(), detection.frame_number(), detection.t_capture());

                printf("SSL-Vision Processing Latency %7.3fms\n",
                       (detection.t_sent() - detection.t_capture()) * 1000.0);

                // Iterar sobre todos os robôs detectados
                for (int i = 0; i < detection.robots_yellow_size(); ++i)
                {
                    QString color = "Yellow Robot ";
                    const SSL_DetectionRobot &robot = detection.robots_yellow(i);
                    printf("Yellow Robot [%d]: ", i);
                    printRobotInfo(robot);
                    updateRobotInfo(robot, color);
                }
                for (int i = 0; i < detection.robots_blue_size(); ++i)
                {
                    QString color = "Blue Robot ";
                    const SSL_DetectionRobot &robot = detection.robots_blue(i);
                    printf("Blue Robot [%d]: ", i);
                    printRobotInfo(robot);
                    updateRobotInfo(robot, color);
                }
            }
        }
    }
}
