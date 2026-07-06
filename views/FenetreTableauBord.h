#pragma once
#include <QMainWindow>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QStackedWidget>
#include "models/Client.h"
#include "controllers/CompteController.h"
#include "controllers/StatController.h"
#include "views/widgets/GraphiqueSolde.h"
#include "views/widgets/CamembertDepenses.h"
#include "views/widgets/IndicateurSolde.h"

/**
 * @brief Fenêtre principale après connexion : tableau de bord, navigation, opérations.
 */
class FenetreTableauBord : public QMainWindow {
    Q_OBJECT
public:
    explicit FenetreTableauBord(Client* client, QWidget* parent = nullptr);

private slots:
    void afficherPage(int index);
    void onCompteChange(int index);
    void rafraichirSolde(double solde);
    void rafraichirHistorique();
    void exporterCSV();
    void deconnecter();

private:
    Client*            client     = nullptr;
    CompteController*  comptCtrl  = nullptr;
    StatController*    statCtrl   = nullptr;

    // Navigation
    QStackedWidget*    pages      = nullptr;
    QList<QPushButton*> navBtns;

    // Widgets tableau de bord
    QLabel*          lblNomClient   = nullptr;
    QLabel*          lblSolde       = nullptr;
    QLabel*          lblCompteType  = nullptr;
    QLabel*          lblIBAN        = nullptr;
    IndicateurSolde* indicateur     = nullptr;
    QComboBox*       cmbComptes     = nullptr;
    QTableWidget*    tableTransact  = nullptr;
    GraphiqueSolde*  graphique      = nullptr;

    // Pages
    QWidget* creerPageAccueil();
    QWidget* creerPageOperations();
    QWidget* creerPageStatistiques();
    QWidget* creerPagePrets();

    // Sidebar
    QWidget* creerSidebar();

    void mettreAJourGraphique();
    void mettreAJourIndicateur();
    QString formaterMontant(double montant) const;
};
