#include "histogram.h"

#include <iostream>

#include <qwt_plot_layout.h>


Histogram::Histogram(){}

Histogram::~Histogram(){

}

Histogram::Histogram(const QString &title, const QColor &symbolColor):
    QwtPlotHistogram(title)
{
    setStyle(QwtPlotHistogram::Columns);
    setColor(symbolColor);
}

void Histogram::setColor(const QColor &symbolColor)
{
//    Ustanovka stilya dlya Histogram
    QColor color = symbolColor;
    color.setAlpha(180);

    setPen(QPen(Qt::black));
    setBrush(QBrush(color));

    QwtColumnSymbol *symbol = new QwtColumnSymbol(QwtColumnSymbol::Box);
    symbol->setFrameStyle(QwtColumnSymbol::Raised);
    symbol->setLineWidth(2);
    symbol->setPalette(QPalette(color));
    setSymbol(symbol);
}


double Histogram::createDataforHist(QVector<double> &datavector, double minvalue,double maxvalue, int nbins)
{
//    Dannie dlya Histogram
    double delta = (maxvalue - minvalue)/nbins;
    QVector<QwtInterval> datainterval;
    QVector<QwtIntervalSample> samples;
    if(samples.size()) samples.clear();
    const int N = nbins;
    double values[N];
    double MaxNEvents = 0;
    double pos = minvalue;
    for (int i=0;i<nbins;i++)
    {

        datainterval.push_back(QwtInterval(pos,pos+delta));
        pos = pos + delta;
        values[i] = 0;
    }
    int count = 0;
    double tmp,left,right;
    for (int i=0;i<datavector.count();i++)
    {
        tmp = datavector[i];
        left = minvalue;
        right = left + delta;
        for (int j=0;j<nbins;j++)
        {
            if (((tmp>=left)&&(tmp<right)||(j==nbins-1)))
            {
                values[j]+=1;
                count++;
                break;
            }
            else
            {
                if ((tmp < minvalue)) { values[j]+=1; break;}
                left = right;
                right = right + delta;
            }
        }
    }
    for (int i=0;i<N;i++)
    {
        if (values[i]>MaxNEvents) MaxNEvents = values[i];
        samples.push_back(QwtIntervalSample(values[i], datainterval[i]));
    }

    setData(new QwtIntervalSeriesData(samples));

    return MaxNEvents;
}
