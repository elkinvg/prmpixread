#ifndef MATRIXOFPIXELS_H
#define MATRIXOFPIXELS_H
#include <QString>
#include <QVector>
#include <QPainter>
#include <iostream>

#include <qwt_plot.h>
#include <qwt_color_map.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_matrix_raster_data.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_zoomer.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_histogram.h>
#include <qwt_painter.h>
#include <QtGui/QGridLayout>
#include "histogram.h"


const int Npixels = 65536;
const int Npix = 256;

class MyZoomer: public QwtPlotZoomer
{
    Q_OBJECT
public:
    MyZoomer(QwtPlotCanvas *canvas):
        QwtPlotZoomer(canvas)
    {
        setTrackerMode(AlwaysOn);
        ZM = 3;

        //setRubberBand(RectRubberBand);
        setRubberBand(CrossRubberBand);
        setRubberBandPen(QColor(Qt::yellow));
    }

    ~MyZoomer(){}
    int ZM;
//    enum ZoomMode {HORIZONTALZOOM = 1, VERTICALZOOM = 2, RECTZOOM = 3};


    virtual QwtText trackerTextF(const QPointF &pos) const
    {
        QColor bg(Qt::white);
        bg.setAlpha(200);

        QwtText text = QwtPlotZoomer::trackerTextF(pos);
        text.setBackgroundBrush( QBrush( bg ));
        return text;
    }

    virtual void zoom(const QRectF &selrect)
    {
        //int ZM
        // {HORIZONTALZOOM = 1, VERTICALZOOM = 2, RECTZOOM = 3};
        switch (ZM)
        {
        case 1:
            QwtPlotZoomer::zoom(QRectF (selrect.x(),QwtPlotZoomer::zoomRect().y(),selrect.width(),QwtPlotZoomer::zoomRect().height()));
            break;
        case 2:
            QwtPlotZoomer::zoom(QRectF /*newrect*/(QwtPlotZoomer::zoomRect().x(),selrect.y(),QwtPlotZoomer::zoomRect().width(),selrect.height()));
            break;
        case 3:
            QwtPlotZoomer::zoom(selrect);
            break;
        default:
            QwtPlotZoomer::zoom(selrect);
        }

    }

    void ZoomMode(int ZoomMode)
    {
        ZM = ZoomMode;
    }

    virtual void drawRubberBand(QPainter *dr) const
    {
        // Ustanovka rezima dlya zoom
        // {HORIZONTALZOOM = 1, VERTICALZOOM = 2, RECTZOOM = 3};
        const QRectF &pRect = QwtPlotZoomer::pickRect();
        const QPolygon &pa = QwtPlotZoomer::selection();

        if ( pa.count() < 1 )  return;
        if ( pa.count() < 2 )   return;
        QwtPainter::drawRect(dr,QRectF(pa[0],pa[int(pa.count() - 1)]));

        switch (ZM)
        {
        case 1:
            QwtPainter::drawRect(dr,QRectF(QPointF(pa[0].x(),pRect.topLeft().y()),QPointF(pa[int(pa.count() - 1)].x(),pRect.bottomRight().y())));
//                        this->setRubberBandPen(Qt::red);
            break;
        case 2:
            QwtPainter::drawRect(dr,QRectF(QPointF(pRect.topLeft().x(),pa[0].y()),QPointF(pRect.bottomRight().x(),pa[int(pa.count() - 1)].y())));
            break;
        case 3:
            QwtPainter::drawRect(dr,QRectF(pa[0],pa[int(pa.count() - 1)]));
            break;
        default:
            QwtPainter::drawRect(dr,QRectF(pa[0],pa[int(pa.count() - 1)]));
        }
    }
};

class matrixofpixels: public QwtPlot
{
    Q_OBJECT
public:

    matrixofpixels(QWidget *parent = NULL);
    ~matrixofpixels();

    int getmatrixofpixels(QString namefilewithpixels);
    int getmaskedofpixels(QString namefilewithmask);
    float MaxElementOfMatrix(); // maximum element of matrix
    void flipudmasked(); // povorot masked matrix po central oss

    enum ColorMapForPlot {BLUERED = 1, BLACKWHITE = 2};

    float matrix[Npix][Npix];
    float MaxElOfMat;
    float masked[Npix][Npix];
    float numofnotmasked;
    float numofnoise[3]; // 0 - number of noise 1 - number of not masked 2 - percent of noise

    QVector<double> matdatavector;
    QVector<double> maskdatavector;
    QVector<double> datavector;


    bool ifmatrixpixfile(QString namefilewithpixels);
    bool ifmatrixmaskfile(QString namefilewithmask);

    QwtPlotSpectrogram *MapPlot;
    QwtPlotHistogram *HistData;
    Histogram *his;
     QwtPlotGrid *grid;
    //QwtPlotZoomer *myzoom;
//    MyZoomer *myzoom;
//    bool ZoomInd;

    void PixPlotSpect(int TypeOfSpec,QwtMatrixRasterData *rasterpixdata);
    void changeitems();
//    void HistogramData(double *limitsofvalues);
    void createhistogram(double *limitsofvalues);
    void createhistogram(double Xmin,double Xmax,int nbins);



private:
    void createdatavector();
    void createdatavector(double Xmin,double Xmax);
    void HistogramPlotProperty(double Xmin,double Xmax,double Ymax);


};

#endif // MATRIXOFPIXELS_H
