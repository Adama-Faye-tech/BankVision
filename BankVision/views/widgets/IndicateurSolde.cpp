#include "IndicateurSolde.h"
#include <QPainter>

IndicateurSolde::IndicateurSolde(QWidget *parent)
    : QWidget(parent), valeur(0) {}

void IndicateurSolde::setValeur(double v) {
    valeur = v;
    update();
}

void IndicateurSolde::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRect r = rect();

    p.setBrush(QColor(30, 60, 120));
    p.drawRoundedRect(r, 15, 15);

    p.setPen(Qt::white);
    p.setFont(QFont("Arial", 20, QFont::Bold));

    p.drawText(r, Qt::AlignCenter,
               QString::number(valeur, 'f', 2) + " FCFA");
}