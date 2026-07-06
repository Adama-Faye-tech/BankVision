#include "GraphiqueSolde.h"
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QFont>
#include <QFontMetrics>
#include <QPropertyAnimation>
#include <algorithm>
#include <cmath>

GraphiqueSolde::GraphiqueSolde(QWidget* parent) : QWidget(parent) {
    setMinimumHeight(200);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
}

void GraphiqueSolde::setValeurs(const QVector<double>& vals, const QStringList& lbls) {
    valeurs = vals;
    labels  = lbls;
    update();
}

void GraphiqueSolde::animer() {
    if (anim) { anim->stop(); delete anim; }
    anim = new QPropertyAnimation(this, "animProgress", this);
    anim->setDuration(1200);
    anim->setStartValue(0.0f);
    anim->setEndValue(1.0f);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
    anim = nullptr;
}

// ---------------------------------------------------------------------------
// Helpers

double GraphiqueSolde::xPt(int i, int W, int marge) const {
    if (valeurs.size() <= 1) return marge;
    return marge + (double)i * (W - 2.0 * marge) / (valeurs.size() - 1);
}

double GraphiqueSolde::yPt(double v, int H, int marge, double minV, double maxV) const {
    if (maxV == minV) return H / 2.0;
    return marge + (1.0 - (v - minV) / (maxV - minV)) * (H - 2.0 * marge);
}

// ---------------------------------------------------------------------------
// paintEvent

void GraphiqueSolde::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);

    int W = width(), H = height();
    const int MARGE_G  = 70;
    const int MARGE_DR = 20;
    const int MARGE_H  = 20;
    const int MARGE_B  = 36;

    // Fond arrondi
    QLinearGradient bg(0, 0, 0, H);
    bg.setColorAt(0.0, QColor(255, 255, 255));
    bg.setColorAt(1.0, QColor(245, 247, 250));
    QPainterPath bgPath;
    bgPath.addRoundedRect(rect(), 12, 12);
    p.fillPath(bgPath, bg);

    if (valeurs.size() < 2) {
        p.setPen(QColor(91, 163, 224, 150));
        p.setFont(QFont("Segoe UI", 11));
        p.drawText(rect(), Qt::AlignCenter, "Données insuffisantes");
        return;
    }

    double maxV = *std::max_element(valeurs.begin(), valeurs.end());
    double minV = *std::min_element(valeurs.begin(), valeurs.end());
    double marge = (maxV - minV) * 0.15;
    maxV += marge; minV -= marge;
    if (maxV == minV) { maxV += 100; minV -= 100; }

    // Adapter les marges effectives à l'espace de dessin
    int effW = W - MARGE_G - MARGE_DR;
    int effH = H - MARGE_H - MARGE_B;

    // Grille horizontale
    p.setPen(QPen(QColor(30, 58, 122, 80), 1, Qt::DashLine));
    QFont fontPetit("Segoe UI", 9);
    p.setFont(fontPetit);
    int nbLignes = 4;
    for (int i = 0; i <= nbLignes; ++i) {
        double v = minV + (maxV - minV) * i / nbLignes;
        double y = MARGE_H + (1.0 - (double)i / nbLignes) * effH;
        p.drawLine(MARGE_G, (int)y, W - MARGE_DR, (int)y);
        // Label valeur
        QString lbl = QString("%1 F CFA").arg((int)v);
        p.setPen(QColor(44, 62, 80, 180));
        p.drawText(0, (int)y - 8, MARGE_G - 4, 18, Qt::AlignRight | Qt::AlignVCenter, lbl);
        p.setPen(QPen(QColor(200, 210, 220, 120), 1, Qt::DashLine));
    }

    // Nombre de points à dessiner selon la progression de l'animation
    int nbPts = qMax(2, (int)(valeurs.size() * m_animProgress));

    // Zone remplie sous la courbe
    QPainterPath aire;
    aire.moveTo(xPt(0, W, MARGE_G), H - MARGE_B);
    for (int i = 0; i < nbPts; ++i)
        aire.lineTo(xPt(i, W, MARGE_G), yPt(valeurs[i], H, MARGE_H, minV, maxV));
    aire.lineTo(xPt(nbPts - 1, W, MARGE_G), H - MARGE_B);
    aire.closeSubpath();

    QLinearGradient fill(0, MARGE_H, 0, H - MARGE_B);
    fill.setColorAt(0.0, QColor(91, 163, 224, 100));
    fill.setColorAt(1.0, QColor(46, 109, 180, 0));
    p.fillPath(aire, fill);

    // Courbe principale
    QPainterPath courbe;
    courbe.moveTo(xPt(0, W, MARGE_G), yPt(valeurs[0], H, MARGE_H, minV, maxV));
    for (int i = 1; i < nbPts; ++i)
        courbe.lineTo(xPt(i, W, MARGE_G), yPt(valeurs[i], H, MARGE_H, minV, maxV));

    QPen penCourbe(QColor(91, 163, 224), 2.5);
    penCourbe.setCapStyle(Qt::RoundCap);
    p.setPen(penCourbe);
    p.setBrush(Qt::NoBrush);
    p.drawPath(courbe);

    // Points sur la courbe
    for (int i = 0; i < nbPts; ++i) {
        double px = xPt(i, W, MARGE_G);
        double py = yPt(valeurs[i], H, MARGE_H, minV, maxV);

        // Halo
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(91, 163, 224, 40));
        p.drawEllipse(QPointF(px, py), 8, 8);

        // Point
        p.setBrush(QColor(91, 163, 224));
        p.setPen(QPen(QColor(15, 30, 60), 1.5));
        p.drawEllipse(QPointF(px, py), 4, 4);
    }

    // Labels des mois
    if (!labels.isEmpty()) {
        p.setPen(QColor(122, 156, 198));
        p.setFont(QFont("Segoe UI", 9));
        int pas = qMax(1, (int)valeurs.size() / 6);
        for (int i = 0; i < nbPts; i += pas) {
            if (i < labels.size()) {
                double px = xPt(i, W, MARGE_G);
                p.drawText((int)px - 20, H - MARGE_B + 4, 40, MARGE_B - 4,
                           Qt::AlignCenter, labels[i]);
            }
        }
        // Toujours afficher le dernier label
        if (nbPts > 0 && (nbPts - 1) % pas != 0 && nbPts - 1 < labels.size()) {
            double px = xPt(nbPts - 1, W, MARGE_G);
            p.drawText((int)px - 20, H - MARGE_B + 4, 40, MARGE_B - 4,
                       Qt::AlignCenter, labels[nbPts - 1]);
        }
    }
}
