#include "mainwindow.h"
#include <QGridLayout>
#include <QDebug>
#include <QMessageBox>
#include "timer.h"

MainWindow::MainWindow(QWidget *parent)
    : QDialog(parent),
      udpsocket(this),
      udpsocket_rec(this) 
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
    btnStopReceive = new QPushButton("Stop Receiving", this); // interrupção de recebimento de dados
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
    layout->addWidget(btnReceive, 10, 1, 1, 2);     // atualizando o n° de colunas
    layout->addWidget(btnStopReceive, 10, 3, 1, 2); // Botão para parar o recebimento
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
    connect(btnStopReceive, SIGNAL(clicked()), this, SLOT(stopReceiving())); // Conectando o botão ao slot
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
    //QHostAddress multicastAddress("222.5.23.2");
    //quint16 port = 10006;
    QHostAddress multicastAddress("224.5.23.2");
    quint16 port = 10020;
    //  Fechar o socket atual se estiver aberto
    if (udpsocket_rec.isOpen())
    {
        udpsocket_rec.close();
    }

    // Configurar o socket para escutar no endereço multicast
    udpsocket_rec.bind(QHostAddress::AnyIPv4, port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    udpsocket_rec.joinMulticastGroup(multicastAddress);

    // Conectar o slot de leitura para processar os dados recebidos
    connect(&udpsocket_rec, &QUdpSocket::readyRead, this, &MainWindow::processPendingDatagrams);
}

void MainWindow::processPendingDatagrams()
{
    while (udpsocket_rec.hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpsocket_rec.pendingDatagramSize());
        udpsocket_rec.readDatagram(datagram.data(), datagram.size());

        // Variável para acumular as informações para o txtInfo
        QString info;

        // Criar um pacote SSL_WrapperPacket e tentar analisar os dados recebidos
        SSL_WrapperPacket packet;
        if (packet.ParseFromArray(datagram.data(), datagram.size()))
        {
            if (packet.has_detection())
            {
                SSL_DetectionFrame detection = packet.detection();
                double t_now = GetTimeSec();

                // Adiciona a informação de detecção no terminal e no txtInfo
                const char *header = "-[Detection Data]-------\n";
                printf("%s", header);
                info += "-[Detection Data]-------\n";

                // Frame info
                QString frameInfo = QString("Camera ID=%1 FRAME=%2 T_CAPTURE=%.4f ")
                                        .arg(detection.camera_id())
                                        .arg(detection.frame_number())
                                        .arg(detection.t_capture());
                printf("Camera ID=%d FRAME=%d T_CAPTURE=%.4f ", detection.camera_id(), detection.frame_number(), detection.t_capture());
                info += frameInfo;

                // SSL-Vision Processing Latency
                double sslLatency = (detection.t_sent() - detection.t_capture()) * 1000.0;
                printf("SSL-Vision Processing Latency                   %7.3fms\n", sslLatency);
                info += QString("SSL-Vision Processing Latency                   %1ms\n").arg(sslLatency, 0, 'f', 3);

                // Network Latency
                double networkLatency = (t_now - detection.t_sent()) * 1000.0;
                printf("Network Latency (assuming synched system clock) %7.3fms\n", networkLatency);
                info += QString("Network Latency (assuming synched system clock) %1ms\n").arg(networkLatency, 0, 'f', 3);

                // Total Latency
                double totalLatency = (t_now - detection.t_capture()) * 1000.0;
                printf("Total Latency   (assuming synched system clock) %7.3fms\n", totalLatency);
                info += QString("Total Latency   (assuming synched system clock) %1ms\n").arg(totalLatency, 0, 'f', 3);

                int balls_n = detection.balls_size();
                int robots_blue_n = detection.robots_blue_size();
                int robots_yellow_n = detection.robots_yellow_size();
                info += QString("Robots blue   %1\n").arg(robots_blue_n);
                info += QString("Robots Yellow   %1\n").arg(robots_yellow_n);
                // Ball info:
                for (int i = 0; i < balls_n; i++)
                {
                    SSL_DetectionBall ball = detection.balls(i);
                    QString ballInfo = QString("-Ball (%1/%2): CONF=%3 POS=<%4,%5> ")
                                           .arg(i + 1)
                                           .arg(balls_n)
                                           .arg(ball.confidence(), 0, 'f', 2)
                                           .arg(ball.x(), 0, 'f', 2)
                                           .arg(ball.y(), 0, 'f', 2);
                    printf("-Ball (%2d/%2d): CONF=%4.2f POS=<%9.2f,%9.2f> ", i + 1, balls_n, ball.confidence(), ball.x(), ball.y());
                    info += ballInfo;

                    // Z coordinate
                    if (ball.has_z())
                    {
                        QString zInfo = QString("Z=%1 ").arg(ball.z(), 0, 'f', 2);
                        printf("Z=%7.2f ", ball.z());
                        info += zInfo;
                    }
                    else
                    {
                        printf("Z=N/A   ");
                        info += "Z=N/A   ";
                    }

                    // RAW coordinates
                    QString rawInfo = QString("RAW=<%1,%2>\n")
                                          .arg(ball.pixel_x(), 0, 'f', 2)
                                          .arg(ball.pixel_y(), 0, 'f', 2);
                    printf("RAW=<%8.2f,%8.2f>\n", ball.pixel_x(), ball.pixel_y());
                    info += rawInfo;
                }

                // Blue robot info:
                for (int i = 0; i < robots_blue_n; i++)
                {
                    SSL_DetectionRobot robot = detection.robots_blue(i);
                    QString robotHeader = QString("-Robot(B) (%1/%2): ").arg(i + 1).arg(robots_blue_n);
                    printf("-Robot(B) (%2d/%2d): ", i + 1, robots_blue_n);
                    info += robotHeader;

                    // ID
                    QString idInfo = robot.has_robot_id() ? QString("Robot ID=%1 ").arg(robot.robot_id()) : QString("Robot ID=N/A ");
                    if (robot.has_robot_id())
                        printf("Robot ID=%d ", robot.robot_id());
                    else
                        printf("Robot ID=N/A ");
                    info += idInfo;

                    // CONF
                    QString confInfo = QString("CONF=%1 ").arg(robot.confidence(), 0, 'f', 2);
                    printf("CONF=%4.2f ", robot.confidence());
                    info += confInfo;

                    // POS
                    QString posInfo = QString("POS=<%1,%2> ")
                                          .arg(robot.x(), 0, 'f', 2)
                                          .arg(robot.y(), 0, 'f', 2);
                    printf("POS=<%9.2f,%9.2f> ", robot.x(), robot.y());
                    info += posInfo;

                    // ANGLE
                    if (robot.has_orientation())
                    {
                        QString angleInfo = QString("ANGLE=%1 ").arg(robot.orientation(), 0, 'f', 3);
                        printf("ANGLE=%7.3f ", robot.orientation());
                        info += angleInfo;
                    }
                    else
                    {
                        printf("ANGLE=N/A ");
                        info += "ANGLE=N/A ";
                    }

                    // RAW coordinates
                    QString rawRobotInfo = QString("RAW=<%1,%2>\n")
                                               .arg(robot.pixel_x(), 0, 'f', 2)
                                               .arg(robot.pixel_y(), 0, 'f', 2);
                    printf("RAW=<%8.2f,%8.2f>\n", robot.pixel_x(), robot.pixel_y());
                    info += rawRobotInfo;
                }

                // Yellow robot info:
                
                for (int i = 0; i < robots_yellow_n; i++)
                {
                    SSL_DetectionRobot robot = detection.robots_yellow(i);
                    QString robotHeader = QString("-Robot(Y) (%1/%2): ").arg(i + 1).arg(robots_yellow_n);
                    printf("-Robot(Y) (%2d/%2d): ", i + 1, robots_yellow_n);
                    info += robotHeader;

                    // ID
                    QString idInfo = robot.has_robot_id() ? QString("Robot ID=%1 ").arg(robot.robot_id()) : QString("Robot ID=N/A ");
                    if (robot.has_robot_id())
                        printf("Robot ID=%d ", robot.robot_id());
                    else
                        printf("Robot ID=N/A ");
                    info += idInfo;

                    // CONF
                    QString confInfo = QString("CONF=%1 ").arg(robot.confidence(), 0, 'f', 2);
                    printf("CONF=%4.2f ", robot.confidence());
                    info += confInfo;

                    // POS
                    QString posInfo = QString("POS=<%1,%2> ")
                                          .arg(robot.x(), 0, 'f', 2)
                                          .arg(robot.y(), 0, 'f', 2);
                    printf("POS=<%9.2f,%9.2f> ", robot.x(), robot.y());
                    info += posInfo;

                    // ANGLE
                    if (robot.has_orientation())
                    {
                        QString angleInfo = QString("ANGLE=%1 ").arg(robot.orientation(), 0, 'f', 3);
                        printf("ANGLE=%7.3f ", robot.orientation());
                        info += angleInfo;
                    }
                    else
                    {
                        printf("ANGLE=N/A ");
                        info += "ANGLE=N/A ";
                    }

                    // RAW coordinates
                    QString rawRobotInfo = QString("RAW=<%1,%2>\n")
                                               .arg(robot.pixel_x(), 0, 'f', 2)
                                               .arg(robot.pixel_y(), 0, 'f', 2);
                    printf("RAW=<%8.2f,%8.2f>\n", robot.pixel_x(), robot.pixel_y());
                    info += rawRobotInfo;
                }

                // Exibir uma linha de separação após cada pacote processado (opcional)
                const char *separator = "-------------------------\n";
                printf("%s", separator);
                info += "-------------------------\n";

                // Atualiza o QTextEdit (txtInfo) com as informações acumuladas
                txtInfo->append(info);
            }
        }
    }
}

void MainWindow::stopReceiving()
{   
    
    udpsocket_rec.close();  
                                                                  // Fecha o socket para interromper o recebimento
    disconnect(&udpsocket_rec, &QUdpSocket::readyRead, this, &MainWindow::processPendingDatagrams); // Desconecta o sinal
    txtInfo->append("Recebimento de dados interrompido.");                                      // Mensagem de log
    /*if (udpsocket.isOpen()) {

        udpsocket.close(); // Fecha o socket para interromper o recebimento
        disconnect(&udpsocket, &QUdpSocket::readyRead, this, &MainWindow::processPendingDatagrams); // Desconecta o sinal
        txtInfo->append("Recebimento de dados interrompido."); // Mensagem de log
    }*/
}
