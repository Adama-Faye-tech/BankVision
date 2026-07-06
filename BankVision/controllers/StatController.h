#pragma once

#include <QVector>

class StatController {
public:
    StatController();
    QVector<double> getSoldesEvolution(int compteId);
    double getTotalDepenses(int compteId);
    double getTotalDepots(int compteId);
};
