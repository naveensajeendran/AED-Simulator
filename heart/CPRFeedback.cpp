#include "CPRFeedback.h"
#include <QPainter>
#include <QPaintEvent>
#include <QDebug>

CPRFeedback::CPRFeedback(QWidget *parent) : QWidget(parent), currentDepth(0), currentRate(0) {
    // Initialization, if needed
}

void CPRFeedback::updateCPRData(int compressionDepth, int compressionRate) {

    currentDepth = compressionDepth;
    currentRate = compressionRate;
    qDebug() << "currentDepth:"<<currentDepth;
    calculateFeedback();
    update();


}

void CPRFeedback::calculateFeedback() {
    if (currentDepth >= 50 && currentDepth <= 60) {
        if (currentRate >= 100 && currentRate <= 120) {
            feedbackMessage = "Good Compression Depth and Rate";
            qDebug() << "Good Compression Depth and Rate"<<currentDepth;
            emit feedbackUpdated(feedbackMessage);
        } else {
            feedbackMessage = "Good Depth, Adjust Rate";
            qDebug() << "Good Depth, Adjust Rate"<<currentDepth;
            qDebug() << "Emitting feedback signal with message:" << feedbackMessage;
            emit feedbackUpdated(feedbackMessage);
        }
    } else {
        if (currentRate >= 100 && currentRate <= 120) {
            feedbackMessage = "Adjust Depth, Good Rate";
            emit feedbackUpdated(feedbackMessage);
            qDebug() << "Adjust Depth, Good Rate. Fully extend your elbow, wrists and fingers"<<currentDepth;
            qDebug() << "Emitting feedback signal with message:" << feedbackMessage;
        } else {
            qDebug() << "Adjust Depth and Rate"<<currentDepth;
            feedbackMessage = "Adjust Depth and Rate. Perform at a Rate of 100 - 120/min";
            emit feedbackUpdated(feedbackMessage);
        }
    }
}
