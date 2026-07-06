#include "Banque.h"
#include <QRandomGenerator>

Client* Banque::getClientParId(int id) {
    for (auto& c : clients)
        if (c.getId() == id) return &c;
    return nullptr;
}

Client* Banque::getClientParLogin(const QString& login) {
    for (auto& c : clients)
        if (c.getLogin() == login) return &c;
    return nullptr;
}

CompteBancaire* Banque::getCompteParIBAN(const QString& iban) {
    for (auto& c : clients) {
        CompteBancaire* cb = c.getCompteParIBAN(iban);
        if (cb) return cb;
    }
    return nullptr;
}

QString Banque::genererIBAN(const QString& prefix) {
    // Format simplifié : FRxx XXXX XXXX XXXX XXXX XXX
    QString num = QString::number(nextIbanNum++).rightJustified(10, '0');
    quint32 rand1 = QRandomGenerator::global()->bounded(10000);
    quint32 rand2 = QRandomGenerator::global()->bounded(10000);
    return QString("%1%2%3%4").arg(prefix).arg(num).arg(rand1, 4, 10, QChar('0')).arg(rand2, 4, 10, QChar('0'));
}
