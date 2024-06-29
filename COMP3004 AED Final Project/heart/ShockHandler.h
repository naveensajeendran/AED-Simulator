// shockhandler.h
#ifndef SHOCKHANDLER_H
#define SHOCKHANDLER_H

#include <QObject>
#include <QPushButton>
#include <QLabel>
#include "ECGDisplay.h" // Include the ECGDisplay class
#include <QTextBrowser>
#include <QLCDNumber>
class ShockHandler : public QObject {
    Q_OBJECT

public:
    enum ShockScenario {
        OneShock,
        MultipleShocks,
        Fibrillation,
        Tachycardia,
        Asystole,
        PEA
    };

    ShockHandler(ECGDisplay* ecgDisplay, QPushButton* stopStartButton, QTextBrowser* cprFeedbackBrowser, QLabel* shockCount, QLabel* elap, QLCDNumber* lcd,QObject* parent = nullptr);

    void deliverShock(ShockScenario scenario);
    void deliverShoc();



public slots:
     void handleNextShock();

private:
    ECGDisplay* ecgDisplay;
    QPushButton* stopStartButton;
    QTextBrowser* cprFeedbackBrowser;
    QLabel* shockCount;
    QLabel* elap;
    QLCDNumber* lcd;
    bool heartRhythmIsNormal;
    int countdownValue;
    QTimer *shockCountdownTimer;

    void checkHeartAfterShock(); // Assuming this method exists
    void performShockCountdown();
    QTimer *elapsedTimeTimer;
    void updateElapsedTime();
    int elapsedTime;
    ShockScenario currentScenario;
    int ShockCount;
    bool isHeartNormal;
    bool shock;
    QTimer *recheckArrhythmiaTimer;
    void recheckArrhythmia();
    int shockDeliveryCount;
    bool healthy;
};


#endif // SHOCKHANDLER_H
