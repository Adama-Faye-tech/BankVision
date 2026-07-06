#include "Banque.h"

Banque::Banque() {}

void Banque::ajouterClient(const Client& c) {
    clients.push_back(c);
}

void Banque::ajouterCompte(const CompteBancaire& c) {
    comptes.push_back(c);
}

int Banque::nombreClients() const {
    return clients.size();
}

int Banque::nombreComptes() const {
    return comptes.size();
}