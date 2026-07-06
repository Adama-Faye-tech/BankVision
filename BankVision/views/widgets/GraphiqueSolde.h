#pragma once

#include <QWidget>
#include <QVector>

class GraphiqueSolde : public QWidget {
    Q_OBJECT

public:
    explicit GraphiqueSolde(QWidget *parent = nullptr);

    void setValeurs(const QVector<double>& v);

protected:
    void paintEvent(QPaintEvent*) override;

private:
    QVector<double> valeurs;
};