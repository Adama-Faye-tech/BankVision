#include "IndicateurSolde.h"
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>

IndicateurSolde::IndicateurSolde(QWidget* parent) : QWidget(parent) {
    setFixedHeight(12);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void IndicateurSolde::setSolde(double s, double obj) {
    solde    = s;
    objectif = obj;
    update();
}

void IndicateurSolde::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int W = width(), H = height();

    // Fond de la barre
    QPainterPath bg;
    bg.addRoundedRect(0, 0, W, H, H / 2, H / 2);
    p.fillPath(bg, QColor(15, 30, 60));

    if (objectif <= 0) return;

    // Calcul du remplissage
    double ratio = solde < 0 ? 0.0 : qMin(1.0, solde / objectif);

    // Couleur selon le niveau
    QColor couleur;
    if (solde < 0)            couleur = QColor(231, 76, 60);   // Rouge
    else if (ratio < 0.3)    couleur = QColor(230, 126, 34);  // Orange
    else if (ratio < 0.7)    couleur = QColor(241, 196, 15);  // Jaune
    else                     couleur = QColor(46, 204, 113);  // Vert

    // Barre de progression
    int fillW = qMax(H, (int)(W * ratio));
    QPainterPath bar;
    bar.addRoundedRect(0, 0, fillW, H, H / 2, H / 2);

    QLinearGradient grad(0, 0, fillW, 0);
    grad.setColorAt(0.0, couleur.darker(120));
    grad.setColorAt(1.0, couleur.lighter(120));
    p.fillPath(bar, grad);

    // Reflet lumineux
    QLinearGradient reflet(0, 0, 0, H);
    reflet.setColorAt(0.0, QColor(255, 255, 255, 60));
    reflet.setColorAt(0.5, QColor(255, 255, 255, 0));
    QPainterPath refletPath;
    refletPath.addRoundedRect(0, 0, fillW, H / 2, H / 2, H / 2);
    p.fillPath(refletPath, reflet);
}
