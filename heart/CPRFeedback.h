#ifndef CPRFEEDBACK_H
#define CPRFEEDBACK_H

#include <QWidget>
#include <QString>

class CPRFeedback : public QWidget {
    Q_OBJECT

public:
    CPRFeedback(QWidget *parent = nullptr);
    void updateCPRData(int compressionDepth, int compressionRate);

signals:
      void feedbackUpdated(const QString& message);

protected:
    void calculateFeedback();

private:

    QString feedbackMessage;  // Stores the feedback message
    int currentDepth;         // Current compression depth
    int currentRate;          // Current compression rate

};

#endif // CPRFEEDBACK_H
