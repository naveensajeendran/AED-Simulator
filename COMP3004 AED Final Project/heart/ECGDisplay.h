#ifndef ECGDISPLAY_H
#define ECGDISPLAY_H

#include <QWidget>
#include <QVector>
#include <QTimer>
#include <QGraphicsScene>
#include <QRandomGenerator>
#include <QPixmap>
#include <QLabel>
class ECGDisplay : public QWidget {
    Q_OBJECT

public:
    void addECGPoint(int point);
    QTimer *updateTimer;
    QVector<int> getECGData() const;
    QGraphicsScene *getScene() const;
    void updateScene();
    void simulateECG();
    enum GraphType {
            VentricularFibrillation,
            VentricularTachycardia,
            Asystole,
            PEA,
        };

    explicit ECGDisplay(QWidget *parent = nullptr, QLabel *displayLabel = nullptr);
    void setGraph(GraphType type);

private:
    QVector<int> ecgData;
    void updateDisplay();
    QGraphicsScene *scene;
    QLabel *displayLabel;
    QPixmap currentGraph;
    void updateGraphDisplay();
    static const int MaxDataPoints = 100;
};

#endif // ECGDISPLAY_H
