#include "Client.h"

Client::Client(int id, const QString& login, const QString& prenom,
               const QString& nom, const QString& email)
    : id(id), login(login), prenom(prenom), nom(nom), email(email)
{}

void Client::ajouterCompte(const CompteBancaire& c) {
    comptes.append(c);
}

CompteBancaire* Client::getCompteParIBAN(const QString& iban) {
    for (auto& c : comptes) {
        if (c.getIBAN() == iban) return &c;
    }
    return nullptr;
}
