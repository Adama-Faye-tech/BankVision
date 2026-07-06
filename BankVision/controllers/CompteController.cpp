#include "CompteController.h"
#include "data/DataManager.h"
#include <QSqlQuery>
#include <QDateTime>

CompteController::CompteController() {}

bool CompteController::depot(int compteId, double montant) {
    if(montant <= 0) return false;

    QSqlDatabase db = DataManager::instance().database();
    QSqlQuery q(db);

    q.prepare("UPDATE comptes SET solde = solde + ? WHERE id = ?");
    q.addBindValue(montant);
    q.addBindValue(compteId);

    if(q.exec()) {
        QSqlQuery t(db);
        t.prepare("INSERT INTO transactions(compte_id, type, montant, date) VALUES(?,?,?,?)");
        t.addBindValue(compteId);
        t.addBindValue("depot");
        t.addBindValue(montant);
        t.addBindValue(QDateTime::currentDateTime());

        t.exec();
        return true;
    }

    return false;
}

bool CompteController::retrait(int compteId, double montant) {
    if(montant <= 0) return false;

    QSqlDatabase db = DataManager::instance().database();
    QSqlQuery check(db);

    check.prepare("SELECT solde FROM comptes WHERE id = ?");
    check.addBindValue(compteId);

    if(check.exec() && check.next()) {
        double solde = check.value(0).toDouble();

        if(solde < montant) return false;

        QSqlQuery q(db);
        q.prepare("UPDATE comptes SET solde = solde - ? WHERE id = ?");
        q.addBindValue(montant);
        q.addBindValue(compteId);

        if(q.exec()) {
            QSqlQuery t(db);
            t.prepare("INSERT INTO transactions(compte_id, type, montant, date) VALUES(?,?,?,?)");
            t.addBindValue(compteId);
            t.addBindValue("retrait");
            t.addBindValue(montant);
            t.addBindValue(QDateTime::currentDateTime());

            t.exec();
            return true;
        }
    }

    return false;
}

bool CompteController::transfert(int fromId, int toId, double montant) {
    if(retrait(fromId, montant)) {
        return depot(toId, montant);
    }
    return false;
}