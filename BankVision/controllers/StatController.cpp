#include "StatController.h"
#include "data/DataManager.h"
#include <QSqlQuery>

StatController::StatController() {}

QVector<double> StatController::getSoldesEvolution(int compteId) {
    QVector<double> data;

    QSqlQuery q(DataManager::instance().database());

    q.prepare("SELECT solde FROM transactions WHERE compte_id = ? ORDER BY date ASC");
    q.addBindValue(compteId);

    if(q.exec()) {
        while(q.next()) {
            data.append(q.value(0).toDouble());
        }
    }

    return data;
}

double StatController::getTotalDepenses(int compteId) {
    QSqlQuery q(DataManager::instance().database());

    q.prepare("SELECT SUM(montant) FROM transactions WHERE compte_id=? AND type='retrait'");
    q.addBindValue(compteId);

    if(q.exec() && q.next())
        return q.value(0).toDouble();

    return 0;
}

double StatController::getTotalDepots(int compteId) {
    QSqlQuery q(DataManager::instance().database());

    q.prepare("SELECT SUM(montant) FROM transactions WHERE compte_id=? AND type='depot'");
    q.addBindValue(compteId);

    if(q.exec() && q.next())
        return q.value(0).toDouble();

    return 0;
}