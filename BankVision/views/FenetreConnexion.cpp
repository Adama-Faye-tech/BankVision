#include "FenetreConnexion.h"
#include "controllers/AuthController.h"
#include "FenetreTableauBord.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>

FenetreConnexion::FenetreConnexion(QWidget *parent)
    : QWidget(parent) {

    auto *layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("BANKVISION - CONNEXION"));

    username = new QLineEdit();
    username->setPlaceholderText("Username");

    password = new QLineEdit();
    password->setPlaceholderText("Password");
    password->setEchoMode(QLineEdit::Password);

    btnLogin = new QPushButton("Se connecter");

    layout->addWidget(username);
    layout->addWidget(password);
    layout->addWidget(btnLogin);

    connect(btnLogin, &QPushButton::clicked,
            this, &FenetreConnexion::onLoginClicked);
}

void FenetreConnexion::onLoginClicked() {
    AuthController auth;

    if(auth.login(username->text(), password->text())) {
        auto *dash = new FenetreTableauBord();
        dash->show();
        this->close();
    } else {
        QMessageBox::warning(this, "Erreur", "Login incorrect");
    }
}