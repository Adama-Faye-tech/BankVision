#pragma once
#include <QWidget>
#include <QVector>
#include <QStringList>
#include <QPropertyAnimation>

/**
 * @brief Widget QPainter affichant la courbe d'évolution du solde sur N mois.
 *        Supporte une animation d'entrée progressive.
 */
class GraphiqueSolde : public QWidget {
    Q_OBJECT
    Q_PROPERTY(float animProgress READ animProgress WRITE setAnimProgress)
public:
    explicit GraphiqueSolde(QWidget* parent = nullptr);

    void setValeurs(const QVector<double>& vals, const QStringList& labels = {});
    void animer();

    float animProgress() const { return m_animProgress; }
    void  setAnimProgress(float p) { m_animProgress = p; update(); }

protected:
    void paintEvent(QPaintEvent*) override;

private:
    QVector<double> valeurs;
    QStringList     labels;
    float           m_animProgress = 1.0f;
    QPropertyAnimation* anim = nullptr;

    void dessinerGrille(QPainter& p, int W, int H, int marge,
                        double minV, double maxV);
    void dessinerAire(QPainter& p, int W, int H, int marge,
                      double minV, double maxV, int nbPts);
    void dessinerCourbe(QPainter& p, int W, int H, int marge,
                        double minV, double maxV, int nbPts);
    void dessinerPoints(QPainter& p, int W, int H, int marge,
                        double minV, double maxV);
    void dessinerLabels(QPainter& p, int W, int H, int marge,
                        double minV, double maxV);

    double xPt(int i, int W, int marge) const;
    double yPt(double v, int H, int marge, double minV, double maxV) const;
};
