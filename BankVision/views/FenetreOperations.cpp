#include "FenetreOperations.h"
#include "controllers/CompteController.h"

#include <QVBoxLayout>
#include <QMessageBox>

FenetreOperations::FenetreOperations(QWidget *parent)
    : QWidget(parent) {

    auto *layout = new QVBoxLayout(this);

    montant = new QLineEdit();
    montant->setPlaceholderText("Montant");

    btnDepot = new QPushButton("Déposer");
    btnRetrait = new QPushButton("Retirer");

    layout->addWidget(montant);
    layout->addWidget(btnDepot);
    layout->addWidget(btnRetrait);

    connect(btnDepot, &QPushButton::clicked, this, &FenetreOperations::depot);
    connect(btnRetrait, &QPushButton::clicked, this, &FenetreOperations::retrait);
}

void FenetreOperations::depot() {
    CompteController c;
    c.depot(1, montant->text().toDouble());
}

void FenetreOperations::retrait() {
    CompteController c;
    c.retrait(1, montant->text().toDouble());
}