#include "FenetreStatistiques.h"
#include "controllers/StatController.h"
#include "widgets/GraphiqueSolde.h"

#include <QVBoxLayout>

FenetreStatistiques::FenetreStatistiques(QWidget *parent)
    : QWidget(parent) {

    auto *layout = new QVBoxLayout(this);

    StatController stat;

    GraphiqueSolde *graph = new GraphiqueSolde();
    graph->setValeurs(stat.getSoldesEvolution(1));

    layout->addWidget(graph);
}