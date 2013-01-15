#ifndef HISTOGRAM_H
#define HISTOGRAM_H
#include <qwt_plot_histogram.h>

#include <QObject>

class Histogram : public QwtPlotHistogram
{
//    Q_OBJECT
public:
    Histogram();
    ~Histogram();
    Histogram(const QString &, const QColor &);

    void setColor(const QColor &);
    double createDataforHist(QVector<double> &datavector, double minvalue,double maxvalue, int nbins);

signals:

public slots:

};

#endif // HISTOGRAM_H
