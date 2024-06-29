#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QThread>
#include <QRandomGenerator>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      cprFeedback(new CPRFeedback(this)),
      ecgDisplay(new ECGDisplay(this)),
      ecgUpdateTimer(new QTimer(this)),
      automaticProgressionTimer(new QTimer(this)),
      shockCountdownTimer(new QTimer(this))


{
      ui->setupUi(this);
      ecgDisplay = new ECGDisplay(this, ui->graph);
      isShock = false;
      connect(cprFeedback, &CPRFeedback::feedbackUpdated, this, [this](const QString& message) {
          qDebug() << "arr:" << message;
          ui->cprfeedback->setText(message);
      });
      arrhythmiaDetector = new ArrhythmiaDetector(ecgDisplay, ui->stopstart, ui->cprfeedback, ui->shockcount, ui->shockcount_2, ui->lcdNumber, cprFeedback, this);
      connect(arrhythmiaDetector, &ArrhythmiaDetector::stopElapsedTimeTimer, this, &MainWindow::stopTimer);

      QPixmap pix(":/images/Aed-electrode-placement-removebg-preview.png");
      ui->label_7->setPixmap(pix);
      QPixmap check (":/images/pngtree-stickman-doctor-and-patient-in-hospital-png-image_2365319-transformed-removebg-preview (1).png");
      ui->label_6->setPixmap(check);
      QPixmap amb (":/images/7346905 (1).png");
      ui->label->setPixmap(amb);
      QPixmap air (":/images/2231138 (1).png");
      ui->label_9->setPixmap(air);
      QPixmap rise (":/images/683080-200.png");
      ui->label_8->setPixmap(rise);

      // LED light

      QPixmap step1 (":/images/ledoff.png");
      ui->label_11->setPixmap(step1);

      QPixmap step2 (":/images/ledoff.png");
      ui->label_12->setPixmap(step1);

      QPixmap step3 (":/images/ledoff.png");
      ui->label_13->setPixmap(step1);

      QPixmap step4 (":/images/ledoff.png");
      ui->label_14->setPixmap(step1);

      QPixmap step5 (":/images/ledoff.png");
      ui->label_15->setPixmap(step1);

        //------------------------------




       istimer = false;
       batteryCheck = false;


      elapsedTimeTimer = new QTimer(this);
      connect(elapsedTimeTimer, &QTimer::timeout, this, &MainWindow::updateElapsedTime);
      elapsedTime = 0;

      //buttons
      connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::handleMultipleShockScenario);
      connect(ui->oneshock_2, &QPushButton::clicked, this, &MainWindow::tach);
      connect(ui->oneshock_3, &QPushButton::clicked, this, &MainWindow::fabill);
      connect(ui->batttest, &QPushButton::clicked, this, &MainWindow::testBattry);
      connect(ui->selffail, &QPushButton::clicked, this, &MainWindow::selffail);
      connect(ui->asystole, &QPushButton::clicked, this, &MainWindow::asy);
      connect(ui->pea, &QPushButton::clicked, this, &MainWindow::pea);

      connect(ui->shockbttn, &QPushButton::clicked, this, &MainWindow::Shock);
      //test

      cprtime = new QTimer(this);
      connect(cprtime, &QTimer::timeout, arrhythmiaDetector, &ArrhythmiaDetector::on_automaticProgression);
      //

      connect(automaticProgressionTimer, &QTimer::timeout, this, &MainWindow::on_automaticProgression);
      currentStep = 0;
      connect(arrhythmiaDetector, &ArrhythmiaDetector::arrhythmiaDetected,
                  this, [this](const QString &type) {
                      ui->cprfeedback->setText(type);
                  });
      connect(ecgUpdateTimer, &QTimer::timeout, this, &MainWindow::updateECG);
      ui->eleStat->setText("Disconnected");
      connect(ui->electrode, &QPushButton::clicked, this, &MainWindow::on_electrodeButton_clicked);
      connect(arrhythmiaDetector, &ArrhythmiaDetector::heartRateCalculated,
                  this, &MainWindow::updateHeartRateDisplay);
      heartRateUpdateTimer = new QTimer(this);
          connect(heartRateUpdateTimer, &QTimer::timeout, this, &MainWindow::refreshHeartRate);
          heartRateUpdateTimer->start(5000);
      //battery
      batteryUpdateTimer = new QTimer(this);
      connect(batteryUpdateTimer, &QTimer::timeout, this, &MainWindow::updateBattery);
      batteryUpdateTimer->start(60000); // Update every minute (60000 milliseconds)
      ui->battery->setValue(100);

}

MainWindow::~MainWindow()
{
    delete ui;
}


// test senarios

void MainWindow::selffail(){

    ui->electrode->setEnabled(false);

    if(ui->selffail->text() == "Self Fail Test Off"){
        ui->electrode->setEnabled(false);
        ui->selffail->setText("Self Fail Test On");
         ui->cprfeedback->setText("Self Test Fail IS ON \n Electrode: Fail \n Electrode wont Function Replace AED");
         ui->eleStat->setText("FAIL");
         ui->eleStat->setStyleSheet("QLabel {"
                                          "  color: #FF0000;"
                                          "font: 14pt DejaVu Sans;"
                                          "background-color: rgb(246, 245, 244);"
                                          "border-bottom-left-radius: 15px;"
                                          "border-bottom-right-radius: 15px;"
                                          "border: 2px solid #7ea5a9;"

                                          "}");

      }
    else{
        ui->electrode->setEnabled(true);
        ui->selffail->setText("Self Fail Test Off");
        ui->eleStat->setText("Disconnected");
        ui->cprfeedback->setText("");
        ui->eleStat->setStyleSheet("QLabel {"
                                         "  color: #000000;"
                                         "font: 14pt DejaVu Sans;"
                                         "background-color: rgb(246, 245, 244);"
                                         "border-bottom-left-radius: 15px;"
                                         "border-bottom-right-radius: 15px;"
                                         "border: 2px solid #7ea5a9;"

                                         "}");

    }

}

void MainWindow::updateElapsedTime() {
    elapsedTime++;
    ui->shockcount_2->setText(QString::number(elapsedTime) + "sec");
}


void MainWindow::handleOneShockScenario() {
    startECGAnalysis();
    currentShockScenario = OneShock;
}

void MainWindow::stopTimer() {
    qDebug() << "call to stop:";
    elapsedTimeTimer->stop();
}

void MainWindow::fabill() {
    ui->eleStat->setText("Connected");
    elapsedTime = 0;
    ecgUpdateTimer->start(100);
    heartRateUpdateTimer->start();
    ui->stopstart->setText("Stop");
    arrhythmiaDetector->deliverShock(ArrhythmiaDetector::Fibrillation);
    elapsedTimeTimer->start(1000);
}


void MainWindow::tach() {
    ui->eleStat->setText("Connected");
    elapsedTimeTimer->start(1000);
    elapsedTime = 0;
    ecgUpdateTimer->start(100);
    heartRateUpdateTimer->start();
    ui->stopstart->setText("Stop");
    arrhythmiaDetector->deliverShock(ArrhythmiaDetector::Tachycardia);
}


void MainWindow::pea() {
    ui->eleStat->setText("Connected");
    elapsedTime = 0;
    elapsedTimeTimer->start(1000);
    ecgUpdateTimer->start(100);
    heartRateUpdateTimer->start();
    arrhythmiaDetector->deliverShock(ArrhythmiaDetector::PEA);
}



void MainWindow::asy() {
    ui->eleStat->setText("Connected");
    elapsedTime = 0;
    elapsedTimeTimer->start(1000);
    ecgUpdateTimer->start(100);
    heartRateUpdateTimer->start();
    arrhythmiaDetector->deliverShock(ArrhythmiaDetector::Asystole);
}




void MainWindow::handleMultipleShockScenario() {
    ui->eleStat->setText("Connected");
    ecgUpdateTimer->start(100);
    heartRateUpdateTimer->start();
    ui->stopstart->setText("Stop");
    arrhythmiaDetector->deliverShock(ArrhythmiaDetector::MultipleShocks);
}

// -------------------

void MainWindow::Shock(){
    isShock = true;
    arrhythmiaDetector->shocking();
    currentStep = 6;
    automaticProgressionTimer->start(10000);

}

void MainWindow::randomizeAndHandleScenario() {
    int scenario = QRandomGenerator::global()->bounded(4); // 0 to 3 for four scenarios

    switch (scenario) {
        case 0:
            currentScenario = ArrhythmiaDetector::Tachycardia;
            break;
        case 1:
            currentScenario = ArrhythmiaDetector::Fibrillation;
            break;
        case 2:
            currentScenario = ArrhythmiaDetector::Asystole;
            break;
        case 3:
            currentScenario = ArrhythmiaDetector::PEA;
            break;
    }
    //currentScenario = ArrhythmiaDetector::PEA;

    arrhythmiaDetector->deliverShock(currentScenario);

}



void MainWindow::testBattry() {
     ui->battery->setValue(29);
     int currentLevel =  ui->battery->value();
     batteryCheck = true;
     if(currentLevel > 40 && currentLevel <= 100 && batteryCheck == true){
         ui->cprfeedback->setText("Battery Is Good");
         batteryCheck = false;
     }
     updateBattery();
}



void MainWindow::updateBattery() {
    int currentLevel = ui->battery->value();
    if (currentLevel > 0) {
        ui->battery->setValue(currentLevel - 1); // Decrease by 1%

    }

    if(currentLevel > 40 && batteryCheck == true){
        ui->cprfeedback->setText("Battery Is Good " + QString::number(currentLevel) + "%");
        batteryCheck = false;
    }

    if(currentLevel < 30){
        ui->cprfeedback->setText("ALERT - BATTERY IS LOW");
    }
    else {
        batteryUpdateTimer->stop(); // Stop the timer if battery is drained
    }
}


void MainWindow::refreshHeartRate() {
    QVector<int> ecgData = ecgDisplay->getECGData();
    int currentHeartRate = arrhythmiaDetector->calculateHeartRate(ecgData);
    updateHeartRateDisplay(currentHeartRate);
}


void MainWindow::updateHeartRateDisplay(int rate) {

    ui->lcdNumber->display(rate);
    qDebug() << "Heart rate to display:" << rate;

    ui->lcdNumber->update();
}




void MainWindow::startECGAnalysis() {

    QVector<int> ecgData = ecgDisplay->getECGData();
    arrhythmiaDetector->analyzeECG(ecgData);


}


void MainWindow::proceedToNextStep() {
    switch (currentStep) {
        case 0:
            batteryCheck = true;
            updateBattery();
            break;
        case 1:
            ui->cprfeedback->setText("Checking If Electrode Is Connected");
            ui->label_15->setPixmap(QPixmap(":/images/ledon.png"));
            break;
        case 2:
            ui->cprfeedback->setText("Self Test Is Done");
            break;
        case 3:
            ui->label_15->setPixmap(QPixmap(":/images/ledoff.png"));
            ui->label_11->setPixmap(QPixmap(":/images/ledon.png"));
            ui->cprfeedback->setText("Check Responsiveness");
            break;
        case 4:

            ui->label_12->setPixmap(QPixmap(":/images/ledon.png"));
            ui->cprfeedback->setText("Call for Help");
            break;
        case 5:

            ui->label_15->setPixmap(QPixmap(":/images/ledon.png"));
            ui->cprfeedback->setText("Place Electrode");
            break;
        case 6:
            ui->label_13->setPixmap(QPixmap(":/images/ledon.png"));
            cprtime->start(1000);
            ui->cprfeedback->setText("Open Airway");
            break;
        case 7:
            ui->label_14->setPixmap(QPixmap(":/images/ledon.png"));
            ui->cprfeedback->setText("Look for rise and fall of the victim's chest");
            break;
        case 8:
            if (ecgUpdateTimer->isActive()) {
                ui->stopstart->setText("Start");
            } else {
                ui->stopstart->setText("Stop");
            }
            break;
    }
    if (currentStep == 5) {
        automaticProgressionTimer->stop();
        if(currentScenario == ArrhythmiaDetector::PEA ||currentScenario == ArrhythmiaDetector::Asystole ){
            currentStep = 5;
            automaticProgressionTimer->start(10000);
        }
        ui->label_15->setPixmap(QPixmap(":/images/ledon.png"));
        ui->cprfeedback->setText("Place Electrode");
         QString electrodeStatusText = ui->eleStat->text();
            if (electrodeStatusText != "Connected") {
                ui->cprfeedback->setText("Place Electrode");

            } else {
                ui->stopstart->setText("Stop");
                //randomizeAndHandleScenario();
            }
        }
    currentStep++;
}



void MainWindow::on_electrodeButton_clicked() {
    if(istimer == false){
        elapsedTime = 0;
        elapsedTimeTimer->start(1000);
    }
    ecgUpdateTimer->start();
    heartRateUpdateTimer->start();

    ui->eleStat->setText("Connected");
    ui->stopstart->setText("Stop");
    randomizeAndHandleScenario();
    currentStep = 5;
    proceedToNextStep();
}


void MainWindow::on_automaticProgression() {
    if (currentStep < 8) {
        proceedToNextStep();
    }else if(currentStep == 5){
        automaticProgressionTimer->stop();
    }
    else {
        automaticProgressionTimer->stop();
    }
}


void MainWindow::on_stopstart_clicked() {

    if(ui->stopstart->text() == "Start"){
        ui->stopstart->setText("Stop");
        int battLevel = ui->battery->value();
        if (battLevel > 30) {
            elapsedTime = 0;
            elapsedTimeTimer->start(1000);
            istimer = true;
            currentStep = 0;
            proceedToNextStep();
            automaticProgressionTimer->start(10000);
        }else{
            ui->cprfeedback->setText("Battery IS LOW. PLEASE CHANGE BATTERY NOW");
        }

      }
    else{
        ui->eleStat->setText("Disconnected");
        ui->stopstart->setText("Start");
        ui->battery->setValue(100);
        automaticProgressionTimer->stop();
        ecgUpdateTimer->stop();
        heartRateUpdateTimer->stop();

        ui->cprfeedback->setText("");
        ui->lcdNumber->display(0);
        ui->graph->clear();
        QPixmap step1 (":/images/ledoff.png");
        ui->label_11->setPixmap(step1);

        QPixmap step2 (":/images/ledoff.png");
        ui->label_12->setPixmap(step1);

        QPixmap step3 (":/images/ledoff.png");
        ui->label_13->setPixmap(step1);

        QPixmap step4 (":/images/ledoff.png");
        ui->label_14->setPixmap(step1);

        QPixmap step5 (":/images/ledoff.png");
        ui->label_15->setPixmap(step1);
    }
}
void MainWindow::updateECG() {

    ecgDisplay->simulateECG();
}

