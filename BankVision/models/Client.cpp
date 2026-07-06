#include "Client.h"

Client::Client() : id(-1) {}

Client::Client(int id, QString nom, QString prenom, QString email)
    : id(id), nom(nom), prenom(prenom), email(email) {}

int Client::getId() const { return id; }
QString Client::getNom() const { return nom; }
QString Client::getPrenom() const { return prenom; }
QString Client::getEmail() const { return email; }

void Client::setNom(const QString& n) { nom = n; }
void Client::setPrenom(const QString& p) { prenom = p; }
void Client::setEmail(const QString& e) { email = e; }