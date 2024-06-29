#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ArrhythmiaDetector.h"
#include "CPRFeedback.h"
#include "ECGDisplay.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    enum ShockScenario {
           OneShock,
           MultipleShocks,
           Fibrillation,
           Tachycardia,
           Asystole,
           PEA,

       };



private slots:
    void on_stopstart_clicked();
    void updateECG();
    void on_electrodeButton_clicked();
    void on_automaticProgression();
    void startECGAnalysis();
    void updateHeartRateDisplay(int rate);
    void refreshHeartRate();
    void updateBattery();
    void handleOneShockScenario();
    void handleMultipleShockScenario();
    void updateElapsedTime();
    void fabill();
    void tach();
    void testBattry();
    void selffail();
    void pea();
    void asy();
    void randomizeAndHandleScenario();
    void stopTimer();
    void Shock();


private:

    //test

    QTimer *cprtime;
    //
    Ui::MainWindow *ui;
    ArrhythmiaDetector *arrhythmiaDetector;
    CPRFeedback *cprFeedback;
    ECGDisplay *ecgDisplay;
    QTimer *ecgUpdateTimer;
    int currentStep;
    void proceedToNextStep();
//    void updateUIForCurrentStep();
    QTimer *automaticProgressionTimer;
    QTimer *shockCountdownTimer;
    int countdownValue;
    QTimer *heartRateUpdateTimer;
    QTimer *batteryUpdateTimer;
    ArrhythmiaDetector::ShockScenario currentScenario;
    bool istimer;
    bool isShock;
    ShockScenario currentShockScenario;
    QTimer *elapsedTimeTimer;
    int elapsedTime;
    bool batteryCheck;


};

#endif // MAINWINDOW_H
