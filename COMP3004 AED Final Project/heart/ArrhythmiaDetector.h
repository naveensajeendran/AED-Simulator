#ifndef ARRHYTHMIADETECTOR_H
#define ARRHYTHMIADETECTOR_H

#include <QObject>
#include <QVector>
#include <QPushButton>
#include <QLabel>
#include <QTextBrowser>
#include "ECGDisplay.h" // Include the ECGDisplay class
#include <QLCDNumber>
#include "CPRFeedback.h"

class ArrhythmiaDetector : public QObject {
    Q_OBJECT

public:
    void analyzeECG(const QVector<int> &ecgData); // Analyzes ECG data for arrhythmias
    int calculateHeartRate(const QVector<int> &ecgData);
    void simulateRandomOutcome();
    void on_automaticProgression();


    enum ShockScenario {
        OneShock,
        MultipleShocks,
        Fibrillation,
        Tachycardia,
        Asystole,
        PEA
    };

    ArrhythmiaDetector(ECGDisplay* ecgDisplay, QPushButton* stopStartButton, QTextBrowser* cprFeedbackBrowser, QLabel* shockCount, QLabel* elap, QLCDNumber* lcd, CPRFeedback* cprFeedback, QObject* parent = nullptr);
    void deliverShock(ShockScenario scenario);
    void deliverShoc();


public slots:
     void handleNextShock();
     void shocking();



signals:
    void arrhythmiaDetected(const QString &type);
    void heartRateCalculated(int rate);
    void stopElapsedTimeTimer();




private:

    //heart new ------
    int dotCount;
    int currentStep;
    int depth;
    int rate;
    int TotalCprSteps;
    QTimer *dotTimer;
    QTimer *cprtime;
    CPRFeedback* cprFeedback;
    ECGDisplay* ecgDisplay;
    QPushButton* stopStartButton;
    QTextBrowser* cprFeedbackBrowser;
    QLabel* shockCount;
    QLabel* elap;
    QLCDNumber* lcd;
    bool heartRhythmIsNormal;
    int countdownValue;
    QTimer *shockCountdownTimer;
    void nextCpr();
    void checkHeartAfterShock();
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
    void updateAnalyzingText();
    //void on_automaticProgression();


//------

    // Thresholds and limits for arrhythmia detection
    int fibrillationThreshold;           // Threshold for fibrillation detection
    int fibrillationIrregularityLimit;   // Limit for irregularity count in fibrillation
    int qrsThreshold;                    // Threshold for QRS complex detection in tachycardia
    int qrsRefractoryPeriod;             // Refractory period to avoid double counting QRS
    int tachycardiaRateThreshold;        // Rate threshold for tachycardia detection
};

#endif // ARRHYTHMIADETECTOR_H
