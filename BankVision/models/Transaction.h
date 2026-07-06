#pragma once

#include <QDateTime>
#include <QString>

class Transaction {
private:
    int id;
    int compteId;
    QString type;
    double montant;
    QDateTime date;
    QString description;

public:
    Transaction();
    Transaction(int id, int compteId, QString type, double montant, QString description);

    int getCompteId() const;
    QString getType() const;
    double getMontant() const;
    QDateTime getDate() const;
};
