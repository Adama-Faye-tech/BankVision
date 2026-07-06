#pragma once

#include <QWidget>
#include <QVector>

class CamembertDepenses : public QWidget {
    Q_OBJECT

public:
    explicit CamembertDepenses(QWidget *parent = nullptr);

    void setValeurs(const QVector<double>& v);

protected:
    void paintEvent(QPaintEvent*) override;

private:
    QVector<double> valeurs;
};