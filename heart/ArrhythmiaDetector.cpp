#include "ArrhythmiaDetector.h"
#include <QDebug>
#include <QTimer>

ArrhythmiaDetector::ArrhythmiaDetector(ECGDisplay* ecgDisplay, QPushButton* stopStartButton, QTextBrowser* cprFeedbackBrowser, QLabel* shockCount, QLabel* elap, QLCDNumber* lcd, CPRFeedback* cprFeedback, QObject* parent)
    : QObject(parent), ecgDisplay(ecgDisplay), stopStartButton(stopStartButton), cprFeedbackBrowser(cprFeedbackBrowser), shockCount(shockCount), elap(elap), lcd(lcd), cprFeedback(cprFeedback){
    shockCountdownTimer = new QTimer(this);
    connect(shockCountdownTimer, &QTimer::timeout, this, &ArrhythmiaDetector::performShockCountdown);
    elapsedTimeTimer = new QTimer(this);
    connect(elapsedTimeTimer, &QTimer::timeout, this, &ArrhythmiaDetector::updateElapsedTime);
    elapsedTime = 0;
    ShockCount = 0;
    currentStep = 0;
    depth = 0;
    rate = 0;

    shock = false;
    isHeartNormal = false;
    recheckArrhythmiaTimer = new QTimer(this);
    shockDeliveryCount  =0;
    connect(recheckArrhythmiaTimer, &QTimer::timeout, this, &ArrhythmiaDetector::recheckArrhythmia);

    dotTimer = new QTimer(this);
    connect(dotTimer, &QTimer::timeout, this, &ArrhythmiaDetector::updateAnalyzingText);

    cprtime = new QTimer(this);
    connect(cprtime, &QTimer::timeout, this, &ArrhythmiaDetector::on_automaticProgression);

}


void ArrhythmiaDetector::deliverShoc() {
    deliverShock(currentScenario);
     qDebug() << "currentScenario:"<<currentScenario;
}


//animation

void ArrhythmiaDetector::updateAnalyzingText() {
    QString text = "Analyzing";
    for (int i = 0; i < dotCount; ++i) {
        text += ".";
    }
    cprFeedbackBrowser->setText(text);

    dotCount = (dotCount + 1) % 4;
}

//-------------


void ArrhythmiaDetector::deliverShock(ArrhythmiaDetector::ShockScenario scenario) {
    qDebug() << "DeliverShock is called:"<<scenario;
    currentScenario = scenario;
    switch (scenario) {
        case OneShock:
            QTimer::singleShot(2000, this, &ArrhythmiaDetector::checkHeartAfterShock);
            break;
        case MultipleShocks:
            QTimer::singleShot(2000, this, &ArrhythmiaDetector::checkHeartAfterShock);
            break;
        case Fibrillation:
            QTimer::singleShot(100, this, [this]() {
                    if(lcd->value() == 0){
                        currentScenario = Fibrillation;
                        deliverShoc();
                          updateAnalyzingText();
                          dotTimer->start(600);
                    } else {
                        cprFeedbackBrowser->setText("Shock advised");

                        dotCount = 0;
                        dotTimer->stop();
                        ecgDisplay->setGraph(ECGDisplay::VentricularFibrillation);
                        stopStartButton->setText("Stop");
                        QTimer::singleShot(2000, this, &ArrhythmiaDetector::checkHeartAfterShock);
                    }
                });
                break;
        case Tachycardia:
            QTimer::singleShot(100, this, [this]() {
                if(lcd->value() == 0){
                    currentScenario = Tachycardia;
                    deliverShoc();
                    updateAnalyzingText();
                    dotTimer->start(600);
                } else {
                    cprFeedbackBrowser->setText("Shock advised");
                    dotCount = 0;
                    dotTimer->stop();
                    ecgDisplay->setGraph(ECGDisplay::VentricularTachycardia);
                    stopStartButton->setText("Stop");
                    QTimer::singleShot(2000, this, &ArrhythmiaDetector::checkHeartAfterShock);
                }
            });
                break;
        case Asystole:

            QTimer::singleShot(100, this, [this]() {
                if(lcd->value() == 0){
                    currentScenario = Asystole;
                    deliverShoc();
                      updateAnalyzingText();
                      dotTimer->start(600);
                } else {
                    dotCount = 0;
                    dotTimer->stop();
                    ecgDisplay->setGraph(ECGDisplay::Asystole);
                    stopStartButton->setText("Stop");
                    cprFeedbackBrowser->setText("Heart rhythm normalized No Shock Needed");
                    emit stopElapsedTimeTimer();
                }
            });
            break;
        case PEA:
            QTimer::singleShot(100, this, [this]() {
                if(lcd->value() == 0){
                    currentScenario = PEA;
                    deliverShoc();
                      updateAnalyzingText();
                      dotTimer->start(600);
                } else {
                    dotCount = 0;
                    dotTimer->stop();
                    ecgDisplay->setGraph(ECGDisplay::PEA);
                    stopStartButton->setText("Stop");
                    cprFeedbackBrowser->setText("Heart rhythm normalized No Shock Needed");
                    emit stopElapsedTimeTimer();
                }
            });
            break;
    }
}


void ArrhythmiaDetector::on_automaticProgression() {
    if (currentStep < 5) {
        nextCpr();
    } else {
        cprtime->stop();
    }
    if(currentStep == 4 && shock == true){
        handleNextShock();
    }
}

void ArrhythmiaDetector::nextCpr(){
    switch (currentStep) {
        case 0:
            depth = 5;
            rate = 90;
            cprFeedback->updateCPRData(depth, rate);

            break;
        case 1:
            depth = 5;
            rate = 110;
            cprFeedback->updateCPRData(depth, rate);
            break;
        case 2:
            depth = 50;
            rate = 90;
            cprFeedback->updateCPRData(depth, rate);
            break;
        case 3:
            depth = 50;
            rate = 110;
            cprFeedback->updateCPRData(depth, rate);
            break;
        case 4:
            break;

    }
     currentStep++;
}

void ArrhythmiaDetector::handleNextShock() {
    if (ShockCount < 3) {
        TotalCprSteps = 4;
        if (currentStep < TotalCprSteps) {
            cprtime->start(3000);
        }else{
            cprFeedbackBrowser->setText("Shock Is Advised");
            //cprFeedbackBrowser->setText("Delivering shock " + QString::number(ShockCount + 1));
//            countdownValue = 5;
//            shockCountdownTimer->start(1000);
            shockDeliveryCount++;

            cprtime->start(3000);
        }

    } else {
        cprtime->stop();
        elapsedTimeTimer->stop();
        ecgDisplay->setGraph(ECGDisplay::PEA);
        cprFeedbackBrowser->setText("All shocks delivered");
        isHeartNormal = true;
    }
}


void ArrhythmiaDetector::checkHeartAfterShock() {
    heartRhythmIsNormal = QRandomGenerator::global()->bounded(2) == 0;

    if (currentScenario == Fibrillation) {
//        countdownValue = 5;
//        shockCountdownTimer->start(1000);
        isHeartNormal = true;
        ShockCount = 0;
        shockCount->setText(QString::number(ShockCount));
    }

    if (currentScenario == Tachycardia) {
//        countdownValue = 5;
//        shockCountdownTimer->start(1000);
        isHeartNormal = true;
        ShockCount = 0;
        shockCount->setText(QString::number(ShockCount));
    }
    if (currentScenario == MultipleShocks) {

        elapsedTimeTimer->start(1000);
        shock = true;
        ecgDisplay->setGraph(ECGDisplay::VentricularTachycardia);
        shockDeliveryCount = 0;
        currentStep = 0;
        handleNextShock();
    }


    shockCount->setText(QString::number(ShockCount));
}


void ArrhythmiaDetector::updateElapsedTime() {
    elapsedTime++;
    elap->setText(QString::number(elapsedTime) + "sec");
}

void ArrhythmiaDetector::performShockCountdown(){

    if (countdownValue > 0) {
        cprFeedbackBrowser->setText("Stand Clear Shock in " + QString::number(countdownValue) + "..." + " - Prepare for Shock dont touch patient");
        countdownValue--;
    } else {

        shockCountdownTimer->stop();
        cprFeedbackBrowser->setText("Shock Delivered");
        if(isHeartNormal == false){
            shock = true;
            QTimer::singleShot(2000, this, &ArrhythmiaDetector::deliverShoc);
        }else{
            shock = false;
            isHeartNormal = false;
            cprFeedbackBrowser->setText("Heart rhythm normalized");
            ecgDisplay->setGraph(ECGDisplay::PEA);
            ShockCount = 0;
            elapsedTimeTimer->stop();
            //
        }
        currentStep = 0;
        cprtime->start(3000);
        ShockCount++;
        shockCount->setText(QString::number(ShockCount));
        recheckArrhythmiaTimer->stop();
        emit stopElapsedTimeTimer();

    }
}


void ArrhythmiaDetector::shocking(){
    countdownValue = 5;
    shockCountdownTimer->start(1000);

}
void ArrhythmiaDetector::recheckArrhythmia() {

    shockCountdownTimer->start(1000);
}

//--------------


void ArrhythmiaDetector::analyzeECG(const QVector<int> &ecgData) {
    int heartRate = calculateHeartRate(ecgData);
    emit heartRateCalculated(heartRate);
    qDebug() << "arr:" << heartRate;

}


int ArrhythmiaDetector::calculateHeartRate(const QVector<int> &ecgData) {
    int peakThreshold = 50;
    int rPeakCount = 0;
    bool isPeak = false;

    for (int i = 1; i < ecgData.size() - 1; ++i) {
        if (ecgData[i] > peakThreshold && ecgData[i] > ecgData[i - 1] && ecgData[i] > ecgData[i + 1] && !isPeak) {
            rPeakCount++;
            isPeak = true;
        } else if (ecgData[i] < ecgData[i - 1] && isPeak) {
            isPeak = false;
        }
    }

    int secondsRecorded = 10;
    int heartRate = (rPeakCount * 60) / secondsRecorded;

    return heartRate;
}

