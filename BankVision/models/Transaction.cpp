#include "Transaction.h"

Transaction::Transaction()
    : id(-1), compteId(-1), montant(0) {}

Transaction::Transaction(int id, int compteId, QString type, double montant, QString description)
    : id(id), compteId(compteId), type(type), montant(montant),
      date(QDateTime::currentDateTime()), description(description) {}

int Transaction::getCompteId() const { return compteId; }
QString Transaction::getType() const { return type; }
double Transaction::getMontant() const { return montant; }
QDateTime Transaction::getDate() const { return date; }