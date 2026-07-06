#include "CamembertDepenses.h"
#include <QPainter>
#include <QtMath>

CamembertDepenses::CamembertDepenses(QWidget *parent)
    : QWidget(parent) {}

void CamembertDepenses::setValeurs(const QVector<double>& v) {
    valeurs = v;
    update();
}

void CamembertDepenses::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    if(valeurs.isEmpty()) return;

    double total = 0;
    for(double v : valeurs) total += v;
    if(total == 0) return;

    QRectF rect(20, 20, width()-40, height()-40);

    double startAngle = 0;

    QVector<QColor> colors = {
        QColor(52,152,219),
        QColor(46,204,113),
        QColor(231,76,60),
        QColor(155,89,182),
        QColor(241,196,15)
    };

    for(int i=0;i<valeurs.size();i++) {
        double angle = (valeurs[i] / total) * 360.0;

        p.setBrush(colors[i % colors.size()]);
        p.drawPie(rect, startAngle * 16, angle * 16);

        startAngle += angle;
    }
}