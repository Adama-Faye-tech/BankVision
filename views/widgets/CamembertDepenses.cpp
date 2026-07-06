#include "CamembertDepenses.h"
#include <QPainter>
#include <QPainterPath>
#include <QFont>
#include <cmath>

CamembertDepenses::CamembertDepenses(QWidget* parent) : QWidget(parent) {
    setMinimumHeight(220);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Palette de couleurs harmonieuse
    couleurs = {
        QColor(91, 163, 224),   // Bleu
        QColor(46, 204, 113),   // Vert
        QColor(231, 76, 60),    // Rouge
        QColor(241, 196, 15),   // Jaune
        QColor(155, 89, 182),   // Violet
        QColor(230, 126, 34),   // Orange
        QColor(26, 188, 156),   // Turquoise
        QColor(189, 195, 199),  // Gris
    };
}

void CamembertDepenses::setDonnees(const QMap<QString, double>& cats) {
    donnees = cats;
    update();
}

void CamembertDepenses::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int W = width(), H = height();

    // Fond
    QLinearGradient bg(0, 0, 0, H);
    bg.setColorAt(0.0, QColor(255, 255, 255));
    bg.setColorAt(1.0, QColor(245, 247, 250));
    QPainterPath bgPath;
    bgPath.addRoundedRect(rect(), 12, 12);
    p.fillPath(bgPath, bg);

    if (donnees.isEmpty()) {
        p.setPen(QColor(52, 152, 219, 150));
        p.setFont(QFont("Segoe UI", 11));
        p.drawText(rect(), Qt::AlignCenter, "Aucune dépense");
        return;
    }

    double total = 0.0;
    for (auto v : donnees) total += v;
    if (total <= 0) return;

    // Zone camembert (à gauche)
    int legendeW = 160;
    int camW     = W - legendeW - 20;
    int camH     = H - 20;
    int diametre = qMin(camW, camH) - 20;
    int cx       = 10 + camW / 2;
    int cy       = H / 2;
    QRect camRect(cx - diametre / 2, cy - diametre / 2, diametre, diametre);

    // Dessiner les tranches
    double angle = 90.0; // Commencer à 12h
    int i = 0;
    for (auto it = donnees.constBegin(); it != donnees.constEnd(); ++it, ++i) {
        double span = it.value() / total * 360.0;
        QColor col  = couleurs[i % couleurs.size()];

        // Ombre légère
        p.setPen(Qt::NoPen);
        p.setBrush(col.darker(120));
        p.drawPie(camRect.adjusted(2, 2, 2, 2), (int)(angle * 16), -(int)(span * 16));

        // Tranche principale
        p.setBrush(col);
        p.drawPie(camRect, (int)(angle * 16), -(int)(span * 16));

        angle -= span;
    }

    // Cercle central (donut)
    int innerD = diametre / 2;
    p.setBrush(QColor(255, 255, 255));
    p.setPen(Qt::NoPen);
    p.drawEllipse(QPoint(cx, cy), innerD / 2, innerD / 2);

    // Texte central
    p.setPen(QColor(44, 62, 80));
    p.setFont(QFont("Segoe UI", 10, QFont::Bold));
    p.drawText(cx - innerD / 2, cy - 12, innerD, 14, Qt::AlignCenter,
               QString("%1 F CFA").arg((int)total));
    p.setFont(QFont("Segoe UI", 8));
    p.setPen(QColor(127, 140, 141));
    p.drawText(cx - innerD / 2, cy + 2, innerD, 14, Qt::AlignCenter, "Total dépenses");

    // Légende à droite
    int legX = W - legendeW;
    int legY = 20;
    i = 0;
    for (auto it = donnees.constBegin(); it != donnees.constEnd() && legY < H - 20; ++it, ++i) {
        QColor col = couleurs[i % couleurs.size()];
        double pct = it.value() / total * 100.0;

        // Carré coloré
        p.setBrush(col);
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(legX, legY, 12, 12, 3, 3);

        // Label
        p.setPen(QColor(176, 200, 232));
        p.setFont(QFont("Segoe UI", 9));
        QString label = it.key();
        if (label.length() > 12) label = label.left(12) + "…";
        p.drawText(legX + 18, legY - 1, legendeW - 22, 14, Qt::AlignLeft | Qt::AlignVCenter, label);

        // Pourcentage
        p.setPen(QColor(91, 163, 224));
        p.setFont(QFont("Segoe UI", 9, QFont::Bold));
        p.drawText(legX + 18, legY + 13, legendeW - 22, 13, Qt::AlignLeft,
                   QString("%1%").arg(pct, 0, 'f', 1));

        legY += 38;
    }
}
