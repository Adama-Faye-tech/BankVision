#include "FenetreTableauBord.h"
#include "FenetreOperations.h"
#include "FenetreStatistiques.h"

#include <QVBoxLayout>
#include <QLabel>

FenetreTableauBord::FenetreTableauBord(QWidget *parent)
    : QWidget(parent) {

    auto *layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("TABLEAU DE BORD BANKVISION"));

    btnOperations = new QPushButton("Opérations");
    btnStats = new QPushButton("Statistiques");

    layout->addWidget(btnOperations);
    layout->addWidget(btnStats);

    connect(btnOperations, &QPushButton::clicked,
            this, &FenetreTableauBord::ouvrirOperations);

    connect(btnStats, &QPushButton::clicked,
            this, &FenetreTableauBord::ouvrirStats);
}

void FenetreTableauBord::ouvrirOperations() {
    auto *w = new FenetreOperations();
    w->show();
}

void FenetreTableauBord::ouvrirStats() {
    auto *w = new FenetreStatistiques();
    w->show();
}