#include "FenetreOperations.h"
#include "data/DataManager.h"
#include <cmath>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QSpinBox>
#include <QScrollArea>
#include <QFrame>

FenetreOperations::FenetreOperations(CompteController* ctrl, QWidget* parent)
    : QWidget(parent), ctrl(ctrl) {

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(32, 28, 32, 28);
    mainLayout->setSpacing(20);

    // Titre
    QLabel* titre = new QLabel("Opérations Bancaires");
    titre->setStyleSheet("font-size: 22px; font-weight: bold; color: #2C3E50; background: transparent;");
    mainLayout->addWidget(titre);

    // Sélecteur de compte en haut
    mainLayout->addWidget(creerEnTeteCompte());

    // Onglets
    QTabWidget* tabs = new QTabWidget;
    tabs->addTab(creerOngletDepot(),    "Depot");
    tabs->addTab(creerOngletRetrait(),  "Retrait");
    tabs->addTab(creerOngletVirement(), "Virement");
    tabs->addTab(creerOngletPret(),     "Pret");
    mainLayout->addWidget(tabs, 1);

    // Label résultat
    lblResultat = new QLabel;
    lblResultat->setVisible(false);
    lblResultat->setAlignment(Qt::AlignCenter);
    lblResultat->setWordWrap(true);
    mainLayout->addWidget(lblResultat);

    mainLayout->addStretch();

    // Relier solde modifié
    if (ctrl)
        connect(ctrl, &CompteController::soldeModifie, this, &FenetreOperations::rafraichirSolde);
}

// ---------------------------------------------------------------------------
// En-tête compte

QWidget* FenetreOperations::creerEnTeteCompte() {
    QFrame* card = new QFrame;
    card->setStyleSheet(
        "QFrame { background: #FFFFFF; border: 1px solid #CFD8DC; border-radius: 12px; }"
    );
    QHBoxLayout* hl = new QHBoxLayout(card);
    hl->setContentsMargins(20, 14, 20, 14);
    hl->setSpacing(16);

    QLabel* icon = new QLabel;
    icon->setFixedSize(0, 0);

    QLabel* lbl = new QLabel("Compte actif :");
    lbl->setStyleSheet("color: #1C2A3A; font-weight: 600; background: transparent;");

    cmbComptes = new QComboBox;
    cmbComptes->setMinimumWidth(320);

    if (ctrl && ctrl->getClient()) {
        Client* cl = ctrl->getClient();

        // Toujours recharger depuis la BD pour etre sur d'avoir les comptes a jour
        if (cl->getComptes().isEmpty()) {
            auto comptesDB = DataManager::instance().chargerComptes(cl->getId());
            for (const auto& c : comptesDB)
                cl->ajouterCompte(c);
        }

        const auto& comptes = cl->getComptes();
        for (const auto& c : comptes)
            cmbComptes->addItem(c.getTypeStr() + "  |  " + c.getIBAN(), c.getIBAN());

        // Forcer le compte actif sur le premier si pas encore defini
        if (!comptes.isEmpty() && !ctrl->getCompteActif())
            ctrl->setCompteActif(comptes.first().getIBAN());

        // Synchroniser le combobox avec le compte actif du ctrl
        if (ctrl->getCompteActif()) {
            QString ibanActif = ctrl->getCompteActif()->getIBAN();
            for (int i = 0; i < cmbComptes->count(); ++i) {
                if (cmbComptes->itemData(i).toString() == ibanActif) {
                    cmbComptes->setCurrentIndex(i);
                    break;
                }
            }
        }
    }

    if (cmbComptes->count() == 0)
        cmbComptes->addItem("Aucun compte — contactez la banque");

    lblSoldeActif = new QLabel;
    lblSoldeActif->setStyleSheet(
        "color: #1A8A4A; font-size: 20px; font-weight: bold; background: transparent;"
    );

    if (ctrl && ctrl->getCompteActif())
        lblSoldeActif->setText(
            QString("%1 F CFA").arg(ctrl->getCompteActif()->getSolde(), 0, 'f', 0));
    else
        lblSoldeActif->setText("0 F CFA");

    connect(cmbComptes, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FenetreOperations::onCompteChange);

    hl->addWidget(icon);
    hl->addWidget(lbl);
    hl->addWidget(cmbComptes, 1);
    hl->addStretch();
    hl->addWidget(lblSoldeActif);

    return card;
}

// ---------------------------------------------------------------------------
// Helpers UI

static QWidget* creerCarte(const QString& couleurBord = "#2E6DB4") {
    QFrame* card = new QFrame;
    card->setStyleSheet(QString(
        "QFrame { background: #FFFFFF; border: 2px solid %1; border-radius: 12px; }"
    ).arg(couleurBord));
    return card;
}

// ---------------------------------------------------------------------------
// Onglet Dépôt

QWidget* FenetreOperations::creerOngletDepot() {
    QWidget* page = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(40, 32, 40, 32);
    layout->setSpacing(16);

    QWidget* carte = creerCarte("#27AE60");
    QVBoxLayout* cl = new QVBoxLayout(carte);
    cl->setContentsMargins(28, 24, 28, 24);
    cl->setSpacing(16);

    QLabel* info = new QLabel("💰 Effectuez un dépôt sur votre compte");
    info->setStyleSheet("color: #7A9CC6; font-size: 13px; background: transparent;");
    cl->addWidget(info);

    QFormLayout* form = new QFormLayout;
    form->setSpacing(14);
    form->setLabelAlignment(Qt::AlignRight);

    sbDepot = new QDoubleSpinBox;
    sbDepot->setRange(1.0, 50000000.0);
    sbDepot->setDecimals(0);
    sbDepot->setSuffix(" F CFA");
    sbDepot->setValue(10000.0);

    cmbCatDepot = new QComboBox;
    cmbCatDepot->addItems({"depot", "salaire", "remboursement", "autre"});

    leDescDepot = new QLineEdit;
    leDescDepot->setPlaceholderText("Ex: Salaire mensuel");

    form->addRow("Montant :", sbDepot);
    form->addRow("Catégorie :", cmbCatDepot);
    form->addRow("Description :", leDescDepot);
    cl->addLayout(form);

    QPushButton* btn = new QPushButton("✓  Déposer");
    btn->setProperty("type", "success");
    btn->setMinimumHeight(46);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, this, &FenetreOperations::effectuerDepot);
    cl->addWidget(btn);

    layout->addWidget(carte);
    layout->addStretch();
    return page;
}

// ---------------------------------------------------------------------------
// Onglet Retrait

QWidget* FenetreOperations::creerOngletRetrait() {
    QWidget* page = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(40, 32, 40, 32);
    layout->setSpacing(16);

    QWidget* carte = creerCarte("#C0392B");
    QVBoxLayout* cl = new QVBoxLayout(carte);
    cl->setContentsMargins(28, 24, 28, 24);
    cl->setSpacing(16);

    QLabel* info = new QLabel("💸 Retirez des fonds de votre compte");
    info->setStyleSheet("color: #7A9CC6; font-size: 13px; background: transparent;");
    cl->addWidget(info);

    QFormLayout* form = new QFormLayout;
    form->setSpacing(14);
    form->setLabelAlignment(Qt::AlignRight);

    sbRetrait = new QDoubleSpinBox;
    sbRetrait->setRange(1.0, 50000000.0);
    sbRetrait->setDecimals(0);
    sbRetrait->setSuffix(" F CFA");
    sbRetrait->setValue(5000.0);

    cmbCatRetrait = new QComboBox;
    cmbCatRetrait->addItems({"retrait", "alimentation", "transport", "loisirs", "logement", "sante", "autre"});

    leDescRetrait = new QLineEdit;
    leDescRetrait->setPlaceholderText("Ex: Courses supermarché");

    form->addRow("Montant :", sbRetrait);
    form->addRow("Catégorie :", cmbCatRetrait);
    form->addRow("Description :", leDescRetrait);
    cl->addLayout(form);

    QPushButton* btn = new QPushButton("✕  Retirer");
    btn->setProperty("type", "danger");
    btn->setMinimumHeight(46);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, this, &FenetreOperations::effectuerRetrait);
    cl->addWidget(btn);

    layout->addWidget(carte);
    layout->addStretch();
    return page;
}

// ---------------------------------------------------------------------------
// Onglet Virement

QWidget* FenetreOperations::creerOngletVirement() {
    QWidget* page = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(40, 32, 40, 32);
    layout->setSpacing(16);

    QWidget* carte = creerCarte("#2E6DB4");
    QVBoxLayout* cl = new QVBoxLayout(carte);
    cl->setContentsMargins(28, 24, 28, 24);
    cl->setSpacing(16);

    QLabel* info = new QLabel("↔ Effectuez un virement vers un autre compte");
    info->setStyleSheet("color: #7A9CC6; font-size: 13px; background: transparent;");
    cl->addWidget(info);

    QFormLayout* form = new QFormLayout;
    form->setSpacing(14);
    form->setLabelAlignment(Qt::AlignRight);

    leIbanVirement = new QLineEdit;
    leIbanVirement->setPlaceholderText("Ex: FR7610001000010000000000002");

    sbVirement = new QDoubleSpinBox;
    sbVirement->setRange(1.0, 50000000.0);
    sbVirement->setDecimals(0);
    sbVirement->setSuffix(" F CFA");
    sbVirement->setValue(10000.0);

    leDescVirement = new QLineEdit;
    leDescVirement->setPlaceholderText("Ex: Remboursement ami");

    form->addRow("IBAN destinataire :", leIbanVirement);
    form->addRow("Montant :", sbVirement);
    form->addRow("Description :", leDescVirement);
    cl->addLayout(form);

    // Avertissement validation 2 étapes
    QLabel* warning = new QLabel("⚠ Cette opération requiert une confirmation.");
    warning->setStyleSheet("color: #F39C12; font-size: 11px; background: transparent;");
    cl->addWidget(warning);

    QPushButton* btn = new QPushButton("→  Virer");
    btn->setProperty("type", "warning");
    btn->setMinimumHeight(46);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, this, &FenetreOperations::effectuerVirement);
    cl->addWidget(btn);

    layout->addWidget(carte);
    layout->addStretch();
    return page;
}

// ---------------------------------------------------------------------------
// Onglet Prêt

QWidget* FenetreOperations::creerOngletPret() {
    QWidget* page = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(40, 32, 40, 32);
    layout->setSpacing(16);

    QWidget* carte = creerCarte("#8E44AD");
    QVBoxLayout* cl = new QVBoxLayout(carte);
    cl->setContentsMargins(28, 24, 28, 24);
    cl->setSpacing(16);

    QLabel* info = new QLabel("🏦 Simulez et demandez un prêt bancaire");
    info->setStyleSheet("color: #7A9CC6; font-size: 13px; background: transparent;");
    cl->addWidget(info);

    QFormLayout* form = new QFormLayout;
    form->setSpacing(14);
    form->setLabelAlignment(Qt::AlignRight);

    sbMontantPret = new QDoubleSpinBox;
    sbMontantPret->setRange(10000.0, 50000000.0);
    sbMontantPret->setDecimals(0);
    sbMontantPret->setSuffix(" F CFA");
    sbMontantPret->setValue(100000.0);

    sbTauxPret = new QDoubleSpinBox;
    sbTauxPret->setRange(0.1, 20.0);
    sbTauxPret->setDecimals(2);
    sbTauxPret->setSuffix(" %");
    sbTauxPret->setValue(3.5);

    sbDureePret = new QSpinBox;
    sbDureePret->setRange(6, 360);
    sbDureePret->setSuffix(" mois");
    sbDureePret->setValue(60);

    form->addRow("Montant :", sbMontantPret);
    form->addRow("Taux annuel :", sbTauxPret);
    form->addRow("Durée :", sbDureePret);
    cl->addLayout(form);

    // Simulation
    lblSimul = new QLabel("Mensualité estimée : —");
    lblSimul->setStyleSheet("color: #1A3A7A; font-size: 14px; font-weight: bold; "
                            "background: #FFFFFF; border: 1px solid #CFD8DC; "
                            "border-radius: 8px; padding: 12px;");
    lblSimul->setAlignment(Qt::AlignCenter);
    cl->addWidget(lblSimul);

    QPushButton* btnSimul = new QPushButton("🔄  Simuler");
    connect(btnSimul, &QPushButton::clicked, this, &FenetreOperations::simulerPret);
    cl->addWidget(btnSimul);

    QPushButton* btnDemande = new QPushButton("📋  Soumettre la demande");
    btnDemande->setProperty("type", "primary");
    btnDemande->setMinimumHeight(46);
    btnDemande->setCursor(Qt::PointingHandCursor);
    connect(btnDemande, &QPushButton::clicked, this, &FenetreOperations::demanderPret);
    cl->addWidget(btnDemande);

    // Connexions pour simulation automatique
    connect(sbMontantPret, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &FenetreOperations::simulerPret);
    connect(sbTauxPret, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &FenetreOperations::simulerPret);
    connect(sbDureePret, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &FenetreOperations::simulerPret);

    simulerPret(); // Calcul initial

    layout->addWidget(carte);
    layout->addStretch();
    return page;
}

// ---------------------------------------------------------------------------
// Slots

void FenetreOperations::effectuerDepot() {
    if (!ctrl) return;
    double montant = sbDepot->value();
    QString desc   = leDescDepot->text().trimmed();
    QString cat    = cmbCatDepot->currentText();
    ctrl->deposer(montant, desc.isEmpty() ? "Dépôt" : desc, cat);
}

void FenetreOperations::effectuerRetrait() {
    if (!ctrl) return;
    double montant = sbRetrait->value();
    QString desc   = leDescRetrait->text().trimmed();
    QString cat    = cmbCatRetrait->currentText();
    ctrl->retirer(montant, desc.isEmpty() ? "Retrait" : desc, cat);
}

void FenetreOperations::effectuerVirement() {
    if (!ctrl) return;
    QString iban   = leIbanVirement->text().trimmed().remove(' ');
    double montant = sbVirement->value();
    QString desc   = leDescVirement->text().trimmed();

    if (iban.isEmpty()) {
        afficherResultat("Veuillez saisir l'IBAN du bénéficiaire.", false);
        return;
    }

    // Confirmation en 2 étapes
    QMessageBox confirm(this);
    confirm.setWindowTitle("Confirmer le virement");
    confirm.setText(QString("Confirmer le virement de <b>%1 F CFA</b> vers<br><b>%2</b> ?")
                    .arg(montant, 0, 'f', 2).arg(iban));
    confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirm.setDefaultButton(QMessageBox::No);
    confirm.setStyleSheet("QMessageBox { background: #FFFFFF; color: #2C3E50; }");

    if (confirm.exec() == QMessageBox::Yes)
        ctrl->virerVers(iban, montant, desc.isEmpty() ? "Virement" : desc);
}

void FenetreOperations::demanderPret() {
    if (!ctrl) return;
    double montant = sbMontantPret->value();
    double taux    = sbTauxPret->value();
    int    duree   = sbDureePret->value();
    ctrl->demanderPret(montant, taux, duree);
}

void FenetreOperations::simulerPret() {
    if (!lblSimul) return;
    double P = sbMontantPret->value();
    double t = sbTauxPret->value() / 100.0 / 12.0;
    int    n = sbDureePret->value();

    double mensualite, coutTotal, interets;
    if (t == 0.0) {
        mensualite = P / n;
    } else {
        double facteur = std::pow(1.0 + t, n);
        mensualite = P * (t * facteur) / (facteur - 1.0);
    }
    coutTotal = mensualite * n;
    interets  = coutTotal - P;

    lblSimul->setText(
        QString("Mensualité : <b>%1 F CFA</b>  |  Coût total : <b>%2 F CFA</b>  |  Intérêts : <b>%3 F CFA</b>")
        .arg(mensualite, 0, 'f', 2)
        .arg(coutTotal, 0, 'f', 2)
        .arg(interets, 0, 'f', 2)
    );
}

void FenetreOperations::afficherResultat(const QString& msg, bool succes) {
    if (!lblResultat) return;
    lblResultat->setText(msg);
    lblResultat->setObjectName(succes ? "successLabel" : "errorLabel");
    lblResultat->style()->unpolish(lblResultat);
    lblResultat->style()->polish(lblResultat);
    lblResultat->setVisible(true);
}

void FenetreOperations::onCompteChange(int /*index*/) {
    if (!ctrl || !cmbComptes) return;
    QString iban = cmbComptes->currentData().toString();
    if (!iban.isEmpty())
        ctrl->setCompteActif(iban);
    // Mettre à jour le solde affiché
    if (ctrl->getCompteActif() && lblSoldeActif)
        lblSoldeActif->setText(
            QString("%1 F CFA").arg(ctrl->getCompteActif()->getSolde(), 0, 'f', 2));
}

void FenetreOperations::rafraichirSolde(double solde) {
    if (!lblSoldeActif) return;
    if (solde < 0)
        lblSoldeActif->setStyleSheet("color: #E74C3C; font-size: 18px; font-weight: bold; background: transparent;");
    else
        lblSoldeActif->setStyleSheet("color: #27AE60; font-size: 18px; font-weight: bold; background: transparent;");
    lblSoldeActif->setText(QString("%1 F CFA").arg(solde, 0, 'f', 2));
}
