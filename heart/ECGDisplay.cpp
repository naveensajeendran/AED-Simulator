#include "ECGDisplay.h"
#include <QPainter>
#include <QPaintEvent>
#include <QPainterPath>
ECGDisplay::ECGDisplay(QWidget *parent, QLabel *displayLabel)
    : QWidget(parent), displayLabel(displayLabel) {
    ecgData.reserve(MaxDataPoints);
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &ECGDisplay::updateDisplay);
    updateTimer->start(100);
    scene = new QGraphicsScene(this);
}

void ECGDisplay::setGraph(GraphType type) {
    switch (type) {
        case VentricularFibrillation:
            currentGraph.load(":/images/VF.png");
            break;
        case VentricularTachycardia:
            currentGraph.load(":/images/VT.png");
            break;
        case Asystole:
            currentGraph.load(":/images/Asystole.png");
            break;
        case PEA:
            currentGraph.load(":/images/PEA.png");
            break;
    }
    updateGraphDisplay();
}

void ECGDisplay::updateGraphDisplay() {
    if (displayLabel) {
        displayLabel->setPixmap(currentGraph);
    }
}



QVector<int> ECGDisplay::getECGData() const {
    return ecgData;
}

QGraphicsScene *ECGDisplay::getScene() const {
    return scene;
}

void ECGDisplay::updateScene() {
    scene->clear();
    QPainterPath path;
    path.moveTo(0, 50);
    for (int i = 0; i < ecgData.size(); ++i) {
        qreal x = static_cast<qreal>(i) / MaxDataPoints * 100;
        qreal y = 50 - (ecgData[i] * 50 / 100);
        path.lineTo(x, y);
    }
    scene->addPath(path);
}


void ECGDisplay::simulateECG() {
    int simulatedPoint = QRandomGenerator::global()->bounded(100);
    addECGPoint(simulatedPoint);
    updateScene();

}

void ECGDisplay::addECGPoint(int point) {
    if (ecgData.size() >= MaxDataPoints) {
        ecgData.pop_front();
    }
    ecgData.push_back(point);
    updateScene();
}


void ECGDisplay::updateDisplay() {
    update();
}


