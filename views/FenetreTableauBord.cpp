#include "FenetreTableauBord.h"
#include "FenetreOperations.h"
#include "FenetreStatistiques.h"
#include "FenetreConnexion.h"
#include "controllers/AuthController.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollArea>
#include <QApplication>
#include <QScreen>
#include <QFrame>

// ---------------------------------------------------------------------------

FenetreTableauBord::FenetreTableauBord(Client* client, QWidget* parent)
    : QMainWindow(parent), client(client) {

    setWindowTitle("BankVision — " + (client ? client->getNomComplet() : "Tableau de bord"));
    setWindowIcon(QIcon(":/images/icon.jpg"));
    setMinimumSize(1100, 700);

    QScreen* screen = QApplication::primaryScreen();
    if (screen) {
        QRect geo = screen->availableGeometry();
        resize(qMin(1280, geo.width() - 60), qMin(820, geo.height() - 60));
        move((geo.width() - width()) / 2, (geo.height() - height()) / 2);
    }

    // Contrôleurs
    comptCtrl = new CompteController(client, this);
    CompteBancaire* compteActif = comptCtrl->getCompteActif();
    statCtrl  = new StatController(compteActif, this);

    // Widget central = sidebar + pages
    QWidget* central = new QWidget;
    QHBoxLayout* ml  = new QHBoxLayout(central);
    ml->setContentsMargins(0, 0, 0, 0);
    ml->setSpacing(0);

    pages = new QStackedWidget;
    pages->addWidget(creerPageAccueil());
    pages->addWidget(creerPageOperations());
    pages->addWidget(creerPageStatistiques());
    pages->addWidget(creerPagePrets());

    ml->addWidget(creerSidebar());
    ml->addWidget(pages, 1);

    setCentralWidget(central);

    // Signaux
    connect(comptCtrl, &CompteController::soldeModifie, this, &FenetreTableauBord::rafraichirSolde);
    connect(comptCtrl, &CompteController::operationReussie, this, [this](const QString& msg) {
        rafraichirHistorique();
        QMessageBox::information(this, "Opération réussie", msg);
    });
    connect(comptCtrl, &CompteController::operationEchouee, this, [this](const QString& msg) {
        QMessageBox::warning(this, "Opération impossible", msg);
    });

    rafraichirSolde(compteActif ? compteActif->getSolde() : 0.0);
    rafraichirHistorique();
    mettreAJourGraphique();
}

// ---------------------------------------------------------------------------
// Sidebar

QWidget* FenetreTableauBord::creerSidebar() {
    QWidget* side = new QWidget;
    side->setObjectName("sidebar");
    side->setFixedWidth(220);

    QVBoxLayout* layout = new QVBoxLayout(side);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // En-tête sidebar
    QWidget* header = new QWidget;
    header->setStyleSheet("background: #040A18; padding: 24px 16px 16px 16px;");
    QVBoxLayout* hLayout = new QVBoxLayout(header);
    hLayout->setContentsMargins(16, 24, 16, 16);

    QLabel* logoImage = new QLabel;
    QPixmap pixLogo(":/images/logo.png");
    if (!pixLogo.isNull()) {
        logoImage->setPixmap(pixLogo.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        logoImage->setFixedSize(0,0);
    }
    logoImage->setStyleSheet("background: transparent;");

    QLabel* logoTexte = new QLabel("BankVision");
    logoTexte->setStyleSheet("color: #FFFFFF; font-size: 16px; font-weight: bold; background: transparent;");

    QLabel* user = new QLabel(client ? client->getNomComplet() : "");
    user->setStyleSheet("color: #C5D8F0; font-size: 12px; background: transparent;");

    hLayout->addWidget(logoImage, 0, Qt::AlignCenter);
    hLayout->addWidget(logoTexte, 0, Qt::AlignCenter);
    hLayout->addWidget(user, 0, Qt::AlignCenter);

    layout->addWidget(header);

    // Séparateur
    QFrame* sep = new QFrame;
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("background: #1A3A7A; border: none; max-height: 1px;");
    layout->addWidget(sep);

    layout->addSpacing(8);

    // Boutons navigation
    QStringList labels = {"Accueil", "Operations", "Statistiques", "Prets"};
    for (int i = 0; i < labels.size(); ++i) {
        QPushButton* btn = new QPushButton(labels[i]);
        btn->setObjectName("sidebarBtn");
        btn->setFlat(true);
        btn->setCursor(Qt::PointingHandCursor);
        connect(btn, &QPushButton::clicked, this, [this, i] { afficherPage(i); });
        layout->addWidget(btn);
        navBtns.append(btn);
    }

    layout->addStretch();

    // Bouton déconnexion
    QFrame* sep2 = new QFrame;
    sep2->setFrameShape(QFrame::HLine);
    sep2->setStyleSheet("background: #1A3A7A; border: none; max-height: 1px;");
    layout->addWidget(sep2);

    QPushButton* btnDeco = new QPushButton("Deconnexion");
    btnDeco->setObjectName("sidebarBtn");
    btnDeco->setFlat(true);
    btnDeco->setCursor(Qt::PointingHandCursor);
    btnDeco->setStyleSheet(
        "QPushButton { color: #C0392B; text-align: left; padding: 14px 20px; font-size: 14px; font-weight: 600; background: transparent; border: none; }"
        "QPushButton:hover { background: #1A0A0A; }"
    );
    connect(btnDeco, &QPushButton::clicked, this, &FenetreTableauBord::deconnecter);
    layout->addWidget(btnDeco);
    layout->addSpacing(8);

    // Sélectionner le premier bouton
    afficherPage(0);

    return side;
}

// ---------------------------------------------------------------------------
// Page Accueil

QWidget* FenetreTableauBord::creerPageAccueil() {
    QScrollArea* scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    QWidget* page = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(20);

    // --- En-tête ---
    QLabel* titre = new QLabel("Tableau de bord");
    titre->setObjectName("sectionTitle");
    titre->setStyleSheet("font-size: 22px; font-weight: bold; color: #2C3E50; "
                          "border: none; padding: 0; background: transparent;");
    layout->addWidget(titre);

    // Sélecteur de compte
    QHBoxLayout* compteRow = new QHBoxLayout;
    QLabel* lblCmpte = new QLabel("Compte :");
    lblCmpte->setStyleSheet("color: #1C2A3A; font-weight: 600; background: transparent;");
    cmbComptes = new QComboBox;
    if (client) {
        for (const auto& c : client->getComptes())
            cmbComptes->addItem(c.getTypeStr() + " — " + c.getIBAN(), c.getIBAN());
    }
    connect(cmbComptes, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FenetreTableauBord::onCompteChange);

    QPushButton* btnExport = new QPushButton("Export CSV");
    connect(btnExport, &QPushButton::clicked, this, &FenetreTableauBord::exporterCSV);

    compteRow->addWidget(lblCmpte);
    compteRow->addWidget(cmbComptes, 1);
    compteRow->addStretch();
    compteRow->addWidget(btnExport);
    layout->addLayout(compteRow);

    // --- Carte Solde ---
    QFrame* carteSolde = new QFrame;
    carteSolde->setStyleSheet(
        "QFrame { background: qlineargradient(x1:0,y1:0,x2:1,y2:0,"
        "stop:0 #FFFFFF, stop:1 #ECEFF1);"
        "border: 1px solid #CFD8DC; border-radius: 16px; }"
    );
    QVBoxLayout* csl = new QVBoxLayout(carteSolde);
    csl->setContentsMargins(28, 24, 28, 24);

    lblCompteType = new QLabel("Compte Courant");
    lblCompteType->setStyleSheet("color: #455A64; font-size: 13px; background: transparent;");

    lblSolde = new QLabel("0,00 F CFA");
    lblSolde->setObjectName("balancePositive");
    lblSolde->setStyleSheet("font-size: 40px; font-weight: bold; color: #1A8A4A; background: transparent;");

    lblIBAN = new QLabel("IBAN : —");
    lblIBAN->setStyleSheet("color: #5A738E; font-size: 11px; background: transparent;");

    indicateur = new IndicateurSolde;

    QLabel* lblObjLabel = new QLabel("Objectif : 5 000 000 F CFA");
    lblObjLabel->setStyleSheet("color: #455A64; font-size: 10px; background: transparent;");

    csl->addWidget(lblCompteType);
    csl->addWidget(lblSolde);
    csl->addWidget(lblIBAN);
    csl->addSpacing(12);
    csl->addWidget(indicateur);
    csl->addWidget(lblObjLabel);
    layout->addWidget(carteSolde);

    // --- Courbe ---
    QLabel* titreCourbe = new QLabel("Evolution du solde (12 derniers mois)");
    titreCourbe->setStyleSheet("font-size: 15px; font-weight: 600; color: #1A3A7A; background: transparent;");
    layout->addWidget(titreCourbe);

    graphique = new GraphiqueSolde;
    graphique->setMinimumHeight(220);
    layout->addWidget(graphique);

    // --- Historique transactions ---
    QLabel* titreHisto = new QLabel("Dernieres transactions");
    titreHisto->setStyleSheet("font-size: 15px; font-weight: 600; color: #1A3A7A; background: transparent;");
    layout->addWidget(titreHisto);

    tableTransact = new QTableWidget(0, 5);
    tableTransact->setHorizontalHeaderLabels({"Date", "Type", "Description", "Montant", "Solde après"});
    tableTransact->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableTransact->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    tableTransact->setAlternatingRowColors(true);
    tableTransact->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableTransact->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableTransact->verticalHeader()->setVisible(false);
    tableTransact->setMinimumHeight(240);
    layout->addWidget(tableTransact);

    scroll->setWidget(page);
    return scroll;
}

// ---------------------------------------------------------------------------
// Page Opérations (délègue à FenetreOperations en mode widget)

QWidget* FenetreTableauBord::creerPageOperations() {
    FenetreOperations* ops = new FenetreOperations(comptCtrl);
    return ops;
}

// ---------------------------------------------------------------------------
// Page Statistiques

QWidget* FenetreTableauBord::creerPageStatistiques() {
    FenetreStatistiques* stats = new FenetreStatistiques(statCtrl);
    return stats;
}

// ---------------------------------------------------------------------------
// Page Prêts

QWidget* FenetreTableauBord::creerPagePrets() {
    QScrollArea* scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    QWidget* page = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(20);

    QLabel* titre = new QLabel("Mes Prets");
    titre->setStyleSheet("font-size: 22px; font-weight: bold; color: #2C3E50; background: transparent;");
    layout->addWidget(titre);

    // Tableau des prêts
    auto prets = comptCtrl->getPrets();
    QTableWidget* tbl = new QTableWidget(prets.size(), 6);
    tbl->setHorizontalHeaderLabels({"Montant", "Taux", "Durée", "Mensualité", "Coût total", "Statut"});
    tbl->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tbl->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tbl->setAlternatingRowColors(true);
    tbl->verticalHeader()->setVisible(false);

    for (int i = 0; i < prets.size(); ++i) {
        const Pret& p = prets[i];
        tbl->setItem(i, 0, new QTableWidgetItem(QString("%1 F CFA").arg(p.getMontant(), 0, 'f', 2)));
        tbl->setItem(i, 1, new QTableWidgetItem(QString("%1%").arg(p.getTaux(), 0, 'f', 2)));
        tbl->setItem(i, 2, new QTableWidgetItem(QString("%1 mois").arg(p.getDuree())));
        tbl->setItem(i, 3, new QTableWidgetItem(QString("%1 F CFA").arg(p.getMensualite(), 0, 'f', 2)));
        tbl->setItem(i, 4, new QTableWidgetItem(QString("%1 F CFA").arg(p.getCoutTotal(), 0, 'f', 2)));
        QTableWidgetItem* statutItem = new QTableWidgetItem(p.getStatutStr());
        if (p.getStatut() == StatutPret::APPROUVE)   statutItem->setForeground(QColor(46, 204, 113));
        else if (p.getStatut() == StatutPret::REFUSE) statutItem->setForeground(QColor(231, 76, 60));
        else                                           statutItem->setForeground(QColor(241, 196, 15));
        tbl->setItem(i, 5, statutItem);
    }

    if (prets.isEmpty()) {
        tbl->setRowCount(1);
        QTableWidgetItem* it = new QTableWidgetItem("Aucun prêt en cours");
        it->setTextAlignment(Qt::AlignCenter);
        tbl->setItem(0, 0, it);
        tbl->setSpan(0, 0, 1, 6);
    }

    layout->addWidget(tbl);
    layout->addStretch();

    scroll->setWidget(page);
    return scroll;
}

// ---------------------------------------------------------------------------
// Slots

void FenetreTableauBord::afficherPage(int index) {
    pages->setCurrentIndex(index);
    for (int i = 0; i < navBtns.size(); ++i) {
        navBtns[i]->setProperty("active", i == index ? "true" : "false");
        navBtns[i]->style()->unpolish(navBtns[i]);
        navBtns[i]->style()->polish(navBtns[i]);
    }
    // Si on affiche la page des statistiques (index 2), on la rafraîchit
    if (index == 2) {
        FenetreStatistiques* fs = qobject_cast<FenetreStatistiques*>(pages->widget(2));
        if (fs) {
            fs->rafraichir();
        }
    }
}

void FenetreTableauBord::onCompteChange(int index) {
    if (!client || index < 0 || index >= client->getComptes().size()) return;
    QString iban = cmbComptes->currentData().toString();
    comptCtrl->setCompteActif(iban);
    statCtrl->setCompte(comptCtrl->getCompteActif());

    CompteBancaire* c = comptCtrl->getCompteActif();
    if (c) {
        rafraichirSolde(c->getSolde());
        rafraichirHistorique();
        mettreAJourGraphique();
        // Mettre à jour également les statistiques
        FenetreStatistiques* fs = qobject_cast<FenetreStatistiques*>(pages->widget(2));
        if (fs) {
            fs->rafraichir();
        }
    }
}

void FenetreTableauBord::rafraichirSolde(double solde) {
    QString fmt = QString("%L1 F CFA").arg(solde, 0, 'f', 2);
    if (lblSolde) {
        lblSolde->setText(fmt);
        if (solde < 0)
            lblSolde->setStyleSheet("font-size: 40px; font-weight: bold; color: #E74C3C; background: transparent;");
        else
            lblSolde->setStyleSheet("font-size: 40px; font-weight: bold; color: #2ECC71; background: transparent;");
    }

    CompteBancaire* c = comptCtrl->getCompteActif();
    if (c && lblCompteType) lblCompteType->setText(c->getTypeStr());
    if (c && lblIBAN)       lblIBAN->setText("IBAN : " + c->getIBAN());
    if (indicateur)          indicateur->setSolde(solde, 5000000.0);
    mettreAJourIndicateur();
}

void FenetreTableauBord::rafraichirHistorique() {
    if (!tableTransact) return;
    CompteBancaire* c = comptCtrl->getCompteActif();
    if (!c) return;

    auto transactions = c->getHistorique(30);
    tableTransact->setRowCount(transactions.size());

    for (int i = 0; i < transactions.size(); ++i) {
        const Transaction& t = transactions[i];

        tableTransact->setItem(i, 0, new QTableWidgetItem(t.date.toString("dd/MM/yyyy HH:mm")));

        // Type avec icône
        QString typeStr;
        QColor  couleurType;
        if      (t.type == "depot")              { typeStr = "⬆ Dépôt";    couleurType = QColor(46, 204, 113); }
        else if (t.type == "retrait")             { typeStr = "⬇ Retrait";  couleurType = QColor(231, 76, 60); }
        else if (t.type == "virement_envoi")      { typeStr = "↗ Virement"; couleurType = QColor(241, 196, 15); }
        else if (t.type == "virement_reception")  { typeStr = "↙ Reçu";    couleurType = QColor(91, 163, 224); }
        else                                       { typeStr = t.type;      couleurType = QColor(176, 200, 232); }

        QTableWidgetItem* typeItem = new QTableWidgetItem(typeStr);
        typeItem->setForeground(couleurType);
        tableTransact->setItem(i, 1, typeItem);

        tableTransact->setItem(i, 2, new QTableWidgetItem(t.description));

        // Montant (rouge si débit, vert si crédit)
        bool debit = (t.type == "retrait" || t.type == "virement_envoi");
        QString montantStr = (debit ? "- " : "+ ") + QString("%1 F CFA").arg(t.montant, 0, 'f', 2);
        QTableWidgetItem* mItem = new QTableWidgetItem(montantStr);
        mItem->setForeground(debit ? QColor(231, 76, 60) : QColor(46, 204, 113));
        mItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        tableTransact->setItem(i, 3, mItem);

        QTableWidgetItem* sItem = new QTableWidgetItem(QString("%1 F CFA").arg(t.soldeApres, 0, 'f', 2));
        sItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        tableTransact->setItem(i, 4, sItem);

        tableTransact->setRowHeight(i, 36);
    }
}

void FenetreTableauBord::mettreAJourGraphique() {
    if (!graphique || !statCtrl) return;
    auto valeurs = statCtrl->getSoldesMensuels(12);
    auto labels  = statCtrl->getLabelsMois(12);
    graphique->setValeurs(valeurs, labels);
    graphique->animer();
}

void FenetreTableauBord::mettreAJourIndicateur() {
    if (!indicateur || !comptCtrl->getCompteActif()) return;
    indicateur->setSolde(comptCtrl->getCompteActif()->getSolde(), 5000000.0);
}

void FenetreTableauBord::exporterCSV() {
    QString fichier = QFileDialog::getSaveFileName(this, "Exporter le relevé CSV",
                      "releve_" + (client ? client->getLogin() : "compte") + ".csv",
                      "Fichiers CSV (*.csv)");
    if (fichier.isEmpty()) return;
    if (comptCtrl->exporterCSV(fichier))
        QMessageBox::information(this, "Export réussi", "Relevé exporté avec succès.");
    else
        QMessageBox::warning(this, "Erreur d'export", "Impossible d'écrire le fichier.");
}

void FenetreTableauBord::deconnecter() {
    AuthController::instance().deconnecter();
    FenetreConnexion* login = new FenetreConnexion;
    login->show();
    close();
}
