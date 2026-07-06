#include "FenetreConnexion.h"
#include "FenetreTableauBord.h"
#include "controllers/AuthController.h"
#include "models/Banque.h"
#include "data/DataManager.h"
#include <QDialog>
#include <QFormLayout>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPainter>
#include <QLinearGradient>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QKeyEvent>
#include <QTimer>
#include <QApplication>
#include <QScreen>

FenetreConnexion::FenetreConnexion(QWidget* parent) : QWidget(parent) {
    setWindowTitle("BankVision — Connexion");
    setMinimumSize(900, 600);

    // Centrer la fenêtre
    QScreen* screen = QApplication::primaryScreen();
    if (screen) {
        QRect geo = screen->availableGeometry();
        resize(qMin(1100, geo.width() - 100), qMin(700, geo.height() - 100));
        move((geo.width() - width()) / 2, (geo.height() - height()) / 2);
    }

    construireUI();

    // Connecter les signaux du AuthController
    connect(&AuthController::instance(), &AuthController::connexionReussie,
            this, [this](Client*) { onConnexionReussie(); });
    connect(&AuthController::instance(), &AuthController::connexionEchouee,
            this, &FenetreConnexion::onConnexionEchouee);

    animerEntree();
}

// ---------------------------------------------------------------------------
// Construction UI

void FenetreConnexion::construireUI() {
    // Layout principal
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // --- Panneau gauche (branding) ---
    QWidget* panneauGauche = new QWidget;
    panneauGauche->setObjectName("panneauGauche");
    panneauGauche->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout* lgLayout = new QVBoxLayout(panneauGauche);
    lgLayout->setAlignment(Qt::AlignCenter);
    lgLayout->setSpacing(16);

    // Logo image au lieu d'un emoji
    lblLogo = new QLabel;
    QPixmap pixLogo(":/images/logo.png");
    if (!pixLogo.isNull()) {
        lblLogo->setPixmap(pixLogo.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        lblLogo->setText("BankVision");
        lblLogo->setStyleSheet("font-size: 28px; font-weight: bold; color: #1A3A7A; background: transparent;");
    }
    lblLogo->setAlignment(Qt::AlignCenter);

    lblTitre = new QLabel("BankVision");
    lblTitre->setObjectName("appTitle");
    lblTitre->setAlignment(Qt::AlignCenter);
    lblTitre->setStyleSheet("font-size: 38px; font-weight: bold; color: #1A3A7A; background: transparent; letter-spacing: 3px;");

    lblSousTitre = new QLabel("Gestion Bancaire Intelligente");
    lblSousTitre->setObjectName("appSubtitle");
    lblSousTitre->setAlignment(Qt::AlignCenter);
    lblSousTitre->setStyleSheet("font-size: 13px; color: #5A738E; background: transparent; letter-spacing: 1px;");

    // Séparateur
    QFrame* sep = new QFrame;
    sep->setFrameShape(QFrame::HLine);
    sep->setFixedWidth(200);
    sep->setStyleSheet("background: #CFD8DC; border: none; max-height: 1px;");

    // Tagline
    QLabel* tagline = new QLabel("Sécurisé · Fiable · Performant");
    tagline->setAlignment(Qt::AlignCenter);
    tagline->setStyleSheet("color: #78909C; font-size: 11px; background: transparent;");

    lgLayout->addStretch();
    lgLayout->addWidget(lblLogo);
    lgLayout->addWidget(lblTitre);
    lgLayout->addWidget(lblSousTitre);
    lgLayout->addSpacing(12);
    lgLayout->addWidget(sep, 0, Qt::AlignCenter);
    lgLayout->addSpacing(12);
    lgLayout->addWidget(tagline);
    lgLayout->addStretch();

    // --- Panneau droit (formulaire) ---
    QWidget* panneauDroit = new QWidget;
    panneauDroit->setFixedWidth(420);
    panneauDroit->setStyleSheet("background-color: #ECEFF1;");

    QVBoxLayout* pdLayout = new QVBoxLayout(panneauDroit);
    pdLayout->setAlignment(Qt::AlignCenter);
    pdLayout->setContentsMargins(48, 48, 48, 48);
    pdLayout->setSpacing(0);

    // Carte de connexion
    carte = new QWidget;
    carte->setObjectName("loginCard");
    carte->setStyleSheet(
        "QWidget#loginCard {"
        "  background-color: #FFFFFF;"
        "  border: 1px solid #CFD8DC;"
        "  border-radius: 16px;"
        "}"
    );

    QVBoxLayout* carteLayout = new QVBoxLayout(carte);
    carteLayout->setContentsMargins(36, 36, 36, 36);
    carteLayout->setSpacing(20);

    // Titre de la carte
    QLabel* lblConnexion = new QLabel("Connexion");
    lblConnexion->setStyleSheet("font-size: 22px; font-weight: bold; color: #2C3E50; background: transparent;");

    QLabel* lblDesc = new QLabel("Entrez vos identifiants pour accéder à votre espace");
    lblDesc->setWordWrap(true);
    lblDesc->setStyleSheet("color: #5A7A9A; font-size: 12px; background: transparent;");

    // Champ Login
    QLabel* lblLogin = new QLabel("Identifiant");
    lblLogin->setStyleSheet("color: #7A9CC6; font-size: 12px; font-weight: 600; background: transparent;");
    champLogin = new QLineEdit;
    champLogin->setPlaceholderText("Votre login");
    champLogin->setMinimumHeight(44);

    // Champ Mot de passe
    QLabel* lblMdp = new QLabel("Mot de passe");
    lblMdp->setStyleSheet("color: #7A9CC6; font-size: 12px; font-weight: 600; background: transparent;");
    champMdp = new QLineEdit;
    champMdp->setEchoMode(QLineEdit::Password);
    champMdp->setPlaceholderText("••••••••");
    champMdp->setMinimumHeight(44);

    // Message d'erreur / info
    lblMessage = new QLabel;
    lblMessage->setObjectName("errorLabel");
    lblMessage->setWordWrap(true);
    lblMessage->setVisible(false);
    lblMessage->setMinimumHeight(36);
    lblMessage->setAlignment(Qt::AlignCenter);

    // Indicateur de verrouillage
    lblVerrou = new QLabel;
    lblVerrou->setObjectName("warningLabel");
    lblVerrou->setVisible(false);
    lblVerrou->setAlignment(Qt::AlignCenter);

    // Bouton de connexion
    btnConnecter = new QPushButton("Se connecter");
    btnConnecter->setProperty("type", "primary");
    btnConnecter->setMinimumHeight(48);
    btnConnecter->setCursor(Qt::PointingHandCursor);

    // Bouton de création de compte
    QPushButton* btnCreerCompte = new QPushButton("Créer un compte");
    btnCreerCompte->setMinimumHeight(40);
    btnCreerCompte->setCursor(Qt::PointingHandCursor);
    btnCreerCompte->setStyleSheet("QPushButton { background-color: transparent; border: 1px solid #3498DB; color: #3498DB; } QPushButton:hover { background-color: #3498DB; color: white; }");

    // Aide
    QLabel* lblAide = new QLabel("Demo: admin / admin123  ·  alice / password");
    lblAide->setAlignment(Qt::AlignCenter);
    lblAide->setStyleSheet("color: #7A9CC6; font-size: 10px; background: transparent;");

    carteLayout->addWidget(lblConnexion);
    carteLayout->addWidget(lblDesc);
    carteLayout->addSpacing(8);
    carteLayout->addWidget(lblLogin);
    carteLayout->addWidget(champLogin);
    carteLayout->addWidget(lblMdp);
    carteLayout->addWidget(champMdp);
    carteLayout->addWidget(lblMessage);
    carteLayout->addWidget(lblVerrou);
    carteLayout->addSpacing(8);
    carteLayout->addWidget(btnConnecter);
    carteLayout->addWidget(btnCreerCompte);
    carteLayout->addSpacing(8);
    carteLayout->addWidget(lblAide);

    pdLayout->addWidget(carte);

    mainLayout->addWidget(panneauGauche, 1);
    mainLayout->addWidget(panneauDroit);

    // Connexions
    connect(btnConnecter, &QPushButton::clicked, this, &FenetreConnexion::tenterConnexion);
    connect(btnCreerCompte, &QPushButton::clicked, this, [this]() {
        // Dialogue de création de compte
        QDialog diag(this);
        diag.setWindowTitle("Nouveau Client");
        diag.setMinimumWidth(360);
        diag.setStyleSheet("QDialog { background-color: #FFFFFF; } QLabel { color: #2C3E50; } QLineEdit { background-color: #F4F6F9; color: #2C3E50; border: 1px solid #CFD8DC; }");

        QFormLayout form(&diag);
        QLineEdit leLogin;
        QLineEdit leMdp;
        leMdp.setEchoMode(QLineEdit::Password);
        QLineEdit lePrenom;
        QLineEdit leNom;
        QLineEdit leEmail;

        form.addRow("Identifiant :", &leLogin);
        form.addRow("Mot de passe :", &leMdp);
        form.addRow("Prénom :", &lePrenom);
        form.addRow("Nom :", &leNom);
        form.addRow("Email :", &leEmail);

        QPushButton btnValider("Valider & Créer");
        btnValider.setProperty("type", "primary");
        form.addRow(&btnValider);

        connect(&btnValider, &QPushButton::clicked, &diag, &QDialog::accept);

        if (diag.exec() == QDialog::Accepted) {
            QString logStr = leLogin.text().trimmed();
            QString mdpStr = leMdp.text();
            QString preStr = lePrenom.text().trimmed();
            QString nomStr = leNom.text().trimmed();
            QString mailStr = leEmail.text().trimmed();

            if (logStr.isEmpty() || mdpStr.isEmpty()) {
                QMessageBox::warning(this, "Erreur", "L'identifiant et le mot de passe sont requis.");
                return;
            }

            if (AuthController::instance().inscrireClient(logStr, mdpStr, preStr, nomStr, mailStr)) {
                // Créer automatiquement un compte courant à la création du client
                Client cTmp = DataManager::instance().chargerClient(logStr);
                if (cTmp.getId() > 0) {
                    // Création compte courant avec 0 F CFA par défaut
                    QString iban = Banque::instance().genererIBAN();
                    DataManager::instance().creerCompte(cTmp.getId(), iban, TypeCompte::COURANT, 10000.0); // 10 000 F CFA offerts
                }
                QMessageBox::information(this, "Succès", "Client créé avec succès ! Un compte courant avec 10 000 F CFA a été ouvert.");
                champLogin->setText(logStr);
                champMdp->setFocus();
            } else {
                QMessageBox::warning(this, "Erreur", "L'identifiant existe déjà.");
            }
        }
    });
}

// ---------------------------------------------------------------------------
// Dessin du fond

void FenetreConnexion::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Charger l'icône de la fenêtre
    setWindowIcon(QIcon(":/images/icon.jpg"));

    // Fond gradient clair
    QLinearGradient bg(0, 0, width(), height());
    bg.setColorAt(0.0, QColor(245, 247, 250));
    bg.setColorAt(1.0, QColor(224, 230, 238));
    p.fillRect(rect(), bg);

    // Cercles décoratifs plus doux
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(52, 152, 219, 12));
    p.drawEllipse(QPoint(150, 150), 200, 200);
    p.setBrush(QColor(52, 152, 219, 8));
    p.drawEllipse(QPoint(100, 500), 300, 300);
    p.setBrush(QColor(46, 204, 113, 6));
    p.drawEllipse(QPoint(450, 300), 250, 250);
}

// ---------------------------------------------------------------------------
// Animations

void FenetreConnexion::animerEntree() {
    if (!carte) return;
    QGraphicsOpacityEffect* opacite = new QGraphicsOpacityEffect(carte);
    carte->setGraphicsEffect(opacite);
    opacite->setOpacity(0.0);

    QPropertyAnimation* anim = new QPropertyAnimation(opacite, "opacity", this);
    anim->setDuration(800);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void FenetreConnexion::animerEchec() {
    // Animation de tremblement
    QPropertyAnimation* anim = new QPropertyAnimation(carte, "pos", this);
    anim->setDuration(400);
    QPoint pos = carte->pos();
    anim->setKeyValueAt(0.0, pos);
    anim->setKeyValueAt(0.1, pos + QPoint(-8, 0));
    anim->setKeyValueAt(0.3, pos + QPoint(8, 0));
    anim->setKeyValueAt(0.5, pos + QPoint(-6, 0));
    anim->setKeyValueAt(0.7, pos + QPoint(6, 0));
    anim->setKeyValueAt(0.9, pos + QPoint(-3, 0));
    anim->setKeyValueAt(1.0, pos);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

// ---------------------------------------------------------------------------
// Slots

void FenetreConnexion::tenterConnexion() {
    if (AuthController::instance().estVerrouille()) return;

    QString login = champLogin->text().trimmed();
    QString mdp   = champMdp->text();

    if (login.isEmpty() || mdp.isEmpty()) {
        lblMessage->setText("Veuillez remplir tous les champs.");
        lblMessage->setVisible(true);
        return;
    }

    lblMessage->setVisible(false);
    btnConnecter->setEnabled(false);
    btnConnecter->setText("Connexion en cours…");

    AuthController::instance().connecter(login, mdp);

    QTimer::singleShot(300, this, [this] {
        if (!AuthController::instance().estConnecte()) {
            btnConnecter->setEnabled(true);
            btnConnecter->setText("Se connecter");
        }
    });
}

void FenetreConnexion::onConnexionReussie() {
    btnConnecter->setText("✓ Connecté !");
    btnConnecter->setProperty("type", "success");
    btnConnecter->style()->unpolish(btnConnecter);
    btnConnecter->style()->polish(btnConnecter);

    QTimer::singleShot(600, this, &FenetreConnexion::ouvrirTableauBord);
}

void FenetreConnexion::onConnexionEchouee(const QString& raison) {
    lblMessage->setText(raison);
    lblMessage->setVisible(true);
    btnConnecter->setEnabled(true);
    btnConnecter->setText("Se connecter");
    animerEchec();

    if (AuthController::instance().estVerrouille()) {
        btnConnecter->setEnabled(false);
        lblVerrou->setVisible(true);
        timerVerrou = new QTimer(this);
        connect(timerVerrou, &QTimer::timeout, this, &FenetreConnexion::mettreAJourVerrouillage);
        timerVerrou->start(1000);
    }
}

void FenetreConnexion::mettreAJourVerrouillage() {
    int restant = AuthController::instance().secondesVerrouillage();
    if (restant > 0) {
        lblVerrou->setText(QString("🔒 Compte verrouillé — %1 sec.").arg(restant));
    } else {
        lblVerrou->setVisible(false);
        btnConnecter->setEnabled(true);
        lblMessage->setText("Vous pouvez réessayer.");
        if (timerVerrou) { timerVerrou->stop(); timerVerrou->deleteLater(); timerVerrou = nullptr; }
    }
}

void FenetreConnexion::ouvrirTableauBord() {
    FenetreTableauBord* tdb = new FenetreTableauBord(AuthController::instance().getClientActif());
    tdb->show();
    close();
}

void FenetreConnexion::keyPressEvent(QKeyEvent* e) {
    if (e->key() == Qt::Key_Escape) close();
    else QWidget::keyPressEvent(e);
}
