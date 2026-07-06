#include "GraphiqueSolde.h"
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <algorithm>

GraphiqueSolde::GraphiqueSolde(QWidget *parent)
    : QWidget(parent) {}

void GraphiqueSolde::setValeurs(const QVector<double>& v) {
    valeurs = v;
    update();
}

void GraphiqueSolde::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int W = width();
    int H = height();
    int M = 40;

    QLinearGradient bg(0,0,0,H);
    bg.setColorAt(0, QColor(18,36,74));
    bg.setColorAt(1, QColor(10,20,40));
    p.fillRect(rect(), bg);

    if (valeurs.size() < 2) return;

    double maxV = *std::max_element(valeurs.begin(), valeurs.end());
    double minV = *std::min_element(valeurs.begin(), valeurs.end());
    if (maxV == minV) maxV += 1;

    auto x = [&](int i){
        return M + (double)i * (W - 2*M) / (valeurs.size()-1);
    };

    auto y = [&](double v){
        return M + (1 - (v-minV)/(maxV-minV)) * (H - 2*M);
    };

    QPainterPath path;
    path.moveTo(x(0), y(valeurs[0]));

    for (int i=1;i<valeurs.size();i++)
        path.lineTo(x(i), y(valeurs[i]));

    p.setPen(QPen(QColor(100,180,255),2));
    p.drawPath(path);

    p.setBrush(QColor(100,180,255));
    for (int i=0;i<valeurs.size();i++)
        p.drawEllipse(QPointF(x(i), y(valeurs[i])), 4,4);
}