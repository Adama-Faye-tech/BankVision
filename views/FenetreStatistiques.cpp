#include "FenetreStatistiques.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QFrame>

FenetreStatistiques::FenetreStatistiques(StatController* statCtrl, QWidget* parent)
    : QWidget(parent), statCtrl(statCtrl) {

    QScrollArea* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    QWidget* page = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(24);

    // Titre
    QLabel* titre = new QLabel("Statistiques et Analyses");
    titre->setStyleSheet("font-size: 22px; font-weight: bold; color: #2C3E50; background: transparent;");
    layout->addWidget(titre);

    // KPIs
    layout->addWidget(creerKPIs());

    // Courbe du solde
    QLabel* titreGraphe = new QLabel("Evolution du solde sur 12 mois");
    titreGraphe->setStyleSheet("font-size: 15px; font-weight: 600; color: #1A3A7A; background: transparent;");
    layout->addWidget(titreGraphe);

    graphSolde = new GraphiqueSolde;
    graphSolde->setMinimumHeight(230);
    layout->addWidget(graphSolde);

    // Comparaison Recettes / Dépenses
    QLabel* titreHisto = new QLabel("Recettes vs Depenses (6 derniers mois)");
    titreHisto->setStyleSheet("font-size: 15px; font-weight: 600; color: #1A3A7A; background: transparent;");
    layout->addWidget(titreHisto);

    histogramme = new GraphiqueSolde; // réutilisé pour afficher recettes vs dépenses
    histogramme->setMinimumHeight(200);
    layout->addWidget(histogramme);

    // Camembert des dépenses
    QLabel* titreCamem = new QLabel("Repartition des depenses par categorie (3 mois)");
    titreCamem->setStyleSheet("font-size: 15px; font-weight: 600; color: #8AB8E8; background: transparent;");
    layout->addWidget(titreCamem);

    camembert = new CamembertDepenses;
    camembert->setMinimumHeight(260);
    layout->addWidget(camembert);

    layout->addStretch();

    scroll->setWidget(page);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(scroll);

    rafraichir();
}

QWidget* FenetreStatistiques::creerKPIs() {
    QWidget* w = new QWidget;
    QGridLayout* grid = new QGridLayout(w);
    grid->setSpacing(16);

    auto creerCarteKPI = [&](const QString& valeur, const QString& libelle,
                              const QString& icone, const QString& couleur) -> QWidget* {
        QFrame* card = new QFrame;
        card->setStyleSheet(QString(
            "QFrame { background: #F8FAFC; border: 1px solid %1; border-radius: 12px; }"
        ).arg(couleur));

        QVBoxLayout* cl = new QVBoxLayout(card);
        cl->setContentsMargins(20, 16, 20, 16);
        cl->setAlignment(Qt::AlignCenter);

        // Barre de couleur en haut de la carte
        QFrame* barre = new QFrame;
        barre->setFixedHeight(4);
        barre->setStyleSheet(QString("background: %1; border-radius: 2px;").arg(couleur));

        QLabel* val = new QLabel(valeur);
        val->setObjectName("kpiValue");
        val->setStyleSheet(QString("color: %1; font-size: 22px; font-weight: bold; background: transparent;").arg(couleur));
        val->setAlignment(Qt::AlignCenter);

        // Masquer l'icone (labels texte simples)
        Q_UNUSED(icone);

        QLabel* lbl = new QLabel(libelle);
        lbl->setObjectName("kpiLabel");
        lbl->setStyleSheet("color: #455A64; font-size: 11px; background: transparent;");
        lbl->setAlignment(Qt::AlignCenter);

        cl->addWidget(barre);
        cl->addSpacing(8);
        cl->addWidget(val);
        cl->addWidget(lbl);

        return card;
    };

    // Créer les cartes
    auto cardSoldeMoyen = creerCarteKPI("—", "Solde moyen (30j)", "~", "#5BA3E0");
    lblSoldeMoyen = cardSoldeMoyen->findChild<QLabel*>("kpiValue");

    auto cardDepMax = creerCarteKPI("—", "Depense maximale", "v", "#E74C3C");
    lblDepMax = cardDepMax->findChild<QLabel*>("kpiValue");

    auto cardEpargne = creerCarteKPI("—", "Taux d'epargne", "+", "#2ECC71");
    lblTauxEpargne = cardEpargne->findChild<QLabel*>("kpiValue");

    auto cardTotalDep = creerCarteKPI("—", "Total depenses (1 mois)", "-", "#E67E22");
    lblTotalDep = cardTotalDep->findChild<QLabel*>("kpiValue");

    grid->addWidget(cardSoldeMoyen, 0, 0);
    grid->addWidget(cardDepMax,     0, 1);
    grid->addWidget(cardEpargne,    0, 2);
    grid->addWidget(cardTotalDep,   0, 3);

    return w;
}

void FenetreStatistiques::rafraichir() {
    if (!statCtrl) return;

    // KPIs
    double soldeMoyen  = statCtrl->getSoldeMoyen(30);
    double depMax      = statCtrl->getDepenseMax(1);
    double tauxEpargne = statCtrl->getTauxEpargne(1);
    double totalDep    = statCtrl->getTotalDepenses(1);

    if (lblSoldeMoyen)  lblSoldeMoyen->setText(QString("%1 F CFA").arg(soldeMoyen, 0, 'f', 2));
    if (lblDepMax)       lblDepMax->setText(QString("%1 F CFA").arg(depMax, 0, 'f', 2));
    if (lblTauxEpargne) lblTauxEpargne->setText(QString("%1%").arg(tauxEpargne, 0, 'f', 1));
    if (lblTotalDep)     lblTotalDep->setText(QString("%1 F CFA").arg(totalDep, 0, 'f', 2));

    // Courbe du solde
    if (graphSolde) {
        auto valeurs = statCtrl->getSoldesMensuels(12);
        auto labels  = statCtrl->getLabelsMois(12);
        graphSolde->setValeurs(valeurs, labels);
        graphSolde->animer();
    }

    // Histogramme recettes vs dépenses (simplifié : on montre les dépenses mensuelles)
    if (histogramme) {
        auto depenses = statCtrl->getDepensesMensuelles(6);
        auto labels6  = statCtrl->getLabelsMois(6);
        histogramme->setValeurs(depenses, labels6);
        histogramme->animer();
    }

    // Camembert
    if (camembert) {
        auto cats = statCtrl->getDepensesParCategorie(3);
        camembert->setDonnees(cats);
    }
}
