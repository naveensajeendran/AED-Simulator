// shockhandler.cpp
#include "ShockHandler.h"
#include <QTimer>
#include <QDebug>

ShockHandler::ShockHandler(ECGDisplay* ecgDisplay, QPushButton* stopStartButton, QTextBrowser* cprFeedbackBrowser, QLabel* shockCount, QLabel* elap, QLCDNumber* lcd,QObject* parent)
    : QObject(parent), ecgDisplay(ecgDisplay), stopStartButton(stopStartButton), cprFeedbackBrowser(cprFeedbackBrowser), shockCount(shockCount), elap(elap),lcd(lcd){
    shockCountdownTimer = new QTimer(this);
    connect(shockCountdownTimer, &QTimer::timeout, this, &ShockHandler::performShockCountdown);
    elapsedTimeTimer = new QTimer(this);
    connect(elapsedTimeTimer, &QTimer::timeout, this, &ShockHandler::updateElapsedTime);
    elapsedTime = 0;
    ShockCount = 0;
    shock = false;
    isHeartNormal = false;
    recheckArrhythmiaTimer = new QTimer(this);
    shockDeliveryCount  =0;
    connect(recheckArrhythmiaTimer, &QTimer::timeout, this, &ShockHandler::recheckArrhythmia);

//    shockCountdownTimer = new QTimer(this);
//    connect(shockCountdownTimer, &QTimer::timeout, this, &ShockHandler::handleNextShock);

}

void ShockHandler::deliverShoc() {
    deliverShock(currentScenario);
     qDebug() << "currentScenario:"<<currentScenario;
}

void ShockHandler::deliverShock(ShockHandler::ShockScenario scenario) {
    qDebug() << "DeliverShock is called:"<<scenario;
    currentScenario = scenario;
    switch (scenario) {
        case OneShock:
            QTimer::singleShot(2000, this, &ShockHandler::checkHeartAfterShock);
            break;
        case MultipleShocks:
            QTimer::singleShot(2000, this, &ShockHandler::checkHeartAfterShock);
            break;
        case Fibrillation:
            QTimer::singleShot(100, this, [this]() {
                    if(lcd->value() == 0){
                        currentScenario = Fibrillation;
                        deliverShoc();
                        cprFeedbackBrowser->setText("Analyzing.....");
                    } else {
                        ecgDisplay->setGraph(ECGDisplay::VentricularFibrillation);
                        stopStartButton->setText("Stop");
                        QTimer::singleShot(2000, this, &ShockHandler::checkHeartAfterShock);
                    }
                });
                break;
        case Tachycardia:
            QTimer::singleShot(100, this, [this]() {
                if(lcd->value() == 0){
                    currentScenario = Tachycardia;
                    deliverShoc();
                    cprFeedbackBrowser->setText("Analyzing.....");
                } else {
                    ecgDisplay->setGraph(ECGDisplay::VentricularTachycardia);
                    stopStartButton->setText("Stop");
                    QTimer::singleShot(2000, this, &ShockHandler::checkHeartAfterShock);
                }
            });
                break;
        case Asystole:
            stopStartButton->setText("Stop");
            ecgDisplay->setGraph(ECGDisplay::Asystole);
            cprFeedbackBrowser->setText("Heart rhythm normalized No Shock Needed");
            break;
        case PEA:
            stopStartButton->setText("Stop");
            ecgDisplay->setGraph(ECGDisplay::PEA);
            cprFeedbackBrowser->setText("Heart rhythm normalized No Shock Needed");
            break;
    }
}


void ShockHandler::handleNextShock() {
    if (ShockCount < 3) {
        cprFeedbackBrowser->setText("Delivering shock " + QString::number(shockDeliveryCount + 1));
        countdownValue = 5;
        shockCountdownTimer->start(1000);
        shockDeliveryCount++;
    } else {
        cprFeedbackBrowser->setText("All shocks delivered");
        isHeartNormal = true;
    }
}

void ShockHandler::checkHeartAfterShock() {
    heartRhythmIsNormal = QRandomGenerator::global()->bounded(2) == 0;

    if (currentScenario == Fibrillation) {
        countdownValue = 5;
        shockCountdownTimer->start(1000);
        isHeartNormal = true;
        ShockCount = 0;
        shockCount->setText(QString::number(ShockCount));
    }

    if (currentScenario == Tachycardia) {
        countdownValue = 5;
        shockCountdownTimer->start(1000);
        isHeartNormal = true;
        ShockCount = 0;
        shockCount->setText(QString::number(ShockCount));
    }
    if (currentScenario == MultipleShocks) {

        shockDeliveryCount = 0;
        handleNextShock();
    }


    shockCount->setText(QString::number(ShockCount));
}

void ShockHandler::updateElapsedTime() {
    elapsedTime++;
    elap->setText(QString::number(elapsedTime) + "sec");
}

void ShockHandler::performShockCountdown(){

    if (countdownValue > 0) {
        cprFeedbackBrowser->setText("Shock in " + QString::number(countdownValue) + "..." + " - Prepare for Shock dont touch patient");
        countdownValue--;
    } else {

        shockCountdownTimer->stop();
        cprFeedbackBrowser->setText("Shock Delivered");
        if(isHeartNormal == false){
            shock = true;
            QTimer::singleShot(2000, this, &ShockHandler::deliverShoc);
        }else{
            shock = false;
            isHeartNormal = false;
            cprFeedbackBrowser->setText("Heart rhythm normalized");
            ecgDisplay->setGraph(ECGDisplay::PEA);
            ShockCount = 0;
        }
        ShockCount++;
        shockCount->setText(QString::number(ShockCount));
        recheckArrhythmiaTimer->stop();
        elapsedTimeTimer->stop();

    }
}

void ShockHandler::recheckArrhythmia() {

    shockCountdownTimer->start(1000);
}
