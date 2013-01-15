#include "matrixofpixels.h"



#include <iostream>
#include <stdio.h>
#include <fstream>

//#include <iomanip>
#include <qwt_legend.h>

#include <QMessageBox>
//#include <QFile>

/*
matrix of 256x256 pixels
*/



class ColorMap: public QwtLinearColorMap
{
public:
    ColorMap(int colormap);
};

ColorMap::ColorMap(int colormap)
{
//    enum ColorMapForPlot {bluered = 1, blackwhite = 2};
    switch (colormap)
    {
    case 1:
        setColorInterval(Qt::darkBlue, Qt::darkRed);
        addColorStop(0.2, Qt::blue);
        addColorStop(0.4, Qt::cyan);
        addColorStop(0.6, Qt::yellow);
        addColorStop(0.8, Qt::red);
        break;
    case 2:
        setColorInterval(Qt::black, Qt::white);
        break;
    case 3:
        setColorInterval(QColor(0,0,0),QColor(0,255,255));
        break;
    default:
        setColorInterval(Qt::darkBlue, Qt::darkRed);
    }
}

matrixofpixels::matrixofpixels(QWidget *parent):
QwtPlot( parent )
{
    MapPlot = new QwtPlotSpectrogram();
    his = new  Histogram("Hist",Qt::red);
    //###################ZoomInd = false;
    if (maskdatavector.size()>0) maskdatavector.clear();
    for (int i=0;i<Npix;i++)
    {
        for (int j=0;j<Npix;j++)
        {
            masked[i][j] = 1.;            
            maskdatavector.push_back(1.);
        }
    }
    numofnotmasked = Npixels;
}

matrixofpixels::~matrixofpixels()
{
    std::cout << " DESTRUCTOR blyaaaa " << std::endl;
    delete MapPlot;
    delete his;
    delete grid;
    //##############delete myzoom;
//    delete MapPlot;
    //delete rasterpixdata;
    //delete MainPlot;
    //HistogramData();

}

bool matrixofpixels::ifmatrixpixfile(QString namefilewithpixels)
{
    FILE *c;
    int i=0;
    float tmp;
    int ifnum;
    const char* charnamefilewithpixels = namefilewithpixels.toLocal8Bit().constData();
    c = fopen(charnamefilewithpixels,"r");
    while(feof(c) == 0 )
    {
        ifnum = fscanf(c,"%e",&tmp);
        if (!ifnum) {fclose(c); return false;}
        i++;
    }
    fclose(c);
    //std::cout << i << std::endl;
    if (i==Npixels+1) return true;
    else return false;    
}

bool matrixofpixels::ifmatrixmaskfile(QString namefilewithmask)
{
    FILE *c;
    int i=0;
    int tmp;
    int ifnum;
    const char* charnamefilewithmask = namefilewithmask.toLocal8Bit().constData();
    c = fopen(charnamefilewithmask,"r");
    while(feof(c) == 0 )
    {
        ifnum = fscanf(c,"%i",&tmp);
        if (!ifnum) {fclose(c); return false;}
        if (tmp!=0 && tmp!=1) {fclose(c); return false;}
        i++;
    }
    fclose(c);
    if (i==Npixels+1) return true;
    else return false;
}

int matrixofpixels::getmatrixofpixels(QString namefilewithpixels)
{
    if (!ifmatrixpixfile(namefilewithpixels))
    {
        QMessageBox::information(0,"Information","File does not contain information \nabout the pixels or is empty");
        return 0;
    }

    const char* charnamefilewithpixels = namefilewithpixels.toLocal8Bit().constData();

    float tmpfloat;
    std::ifstream inmat(charnamefilewithpixels);
    if (matdatavector.size()>0) matdatavector.clear();

    for (int i=0;i<Npix;i++)
    {
        for (int j=0;j<Npix;j++)
        {
            inmat >> tmpfloat;
            matrix[i][j] = tmpfloat;
            if (i==j && i==0) MaxElOfMat = tmpfloat;
            if (tmpfloat>MaxElOfMat) MaxElOfMat = tmpfloat;
            matdatavector.push_back((double)tmpfloat);
        }
    }
    inmat.close();
    return 1;
}

int matrixofpixels::getmaskedofpixels(QString namefilewithmask)
{    
    if (!ifmatrixmaskfile(namefilewithmask))
    {
        QMessageBox::information(0,"Information","File does not contain information \nabout the masked pixels or is empty");
        return 0;
    }
    const char* charnamefilewithmask = namefilewithmask.toLocal8Bit().constData();

    float tmpint;
    int tmpperc=0;
    std::ifstream inmask(charnamefilewithmask);

    if (maskdatavector.size()>0) maskdatavector.clear();

    for (int i=0;i<Npix;i++)
    {
        for (int j=0;j<Npix;j++)
        {
            inmask >> tmpint;
            masked[i][j] = tmpint;
            if (tmpint) tmpperc++;
            maskdatavector.push_back((double)tmpint);
        }
    }
//#######    if (ZoomInd) {delete myzoom; ZoomInd = false;}
    numofnotmasked = tmpperc;
    inmask.close();
    return 1;
}

float matrixofpixels::MaxElementOfMatrix()
{
    float tmpmax;
    for (int i=0;i<Npix;i++)
    {
        for (int j=0;j<Npix;j++)
        {
            if (i==j && i==0) tmpmax = matrix[i][j];
            if ((matrix[i][j]>tmpmax)&&(masked[i][j])) tmpmax = matrix[i][j];
        }
    }
    return tmpmax;
}

void matrixofpixels::flipudmasked()
{
    // flip matrix of mask
    int tmp;
    if (maskdatavector.size()!=Npixels) return;
    for (int i=0;i<Npix/2;i++)
    {
        for (int j=0;j<Npix;j++)
        {
            tmp = masked[i][j];
            masked[i][j] = masked[(Npix-1)-i][j];
            maskdatavector[Npix*i+j] = masked[(Npix-1)-i][j];
            masked[(Npix-1)-i][j] = tmp;
            maskdatavector[Npix*((Npix-1)-i)+j] = tmp;
        }
    }
}

void matrixofpixels::PixPlotSpect(int TypeOfSpec,QwtMatrixRasterData *rasterpixdata)
{
    changeitems();
    MapPlot->setData(rasterpixdata);
    MapPlot->setRenderThreadCount(0);
    setAxisAutoScale(this->xTop);
    setAxisAutoScale(this->xBottom);



    switch (TypeOfSpec)
    {
    case 1:
        MapPlot->setColorMap(new ColorMap(BLUERED));
        break;
    case 2:
        MapPlot->setColorMap(new ColorMap(BLACKWHITE));
        break;
    default:
        MapPlot->setColorMap(new ColorMap(BLUERED));
    }

    MapPlot->attach(/*MainPlot*/this);

    const QwtInterval zInterval = MapPlot->data()->interval(Qt::ZAxis);

    setAxisScale(QwtPlot::xBottom, 0, Npix);
    setAxisMaxMinor(QwtPlot::xBottom, 0);
    setAxisScale(QwtPlot::yLeft, 0, Npix);
    setAxisMaxMinor(QwtPlot::yLeft, 0);

    //rightAxis = new QwtScaleWidget();
    QwtScaleWidget *rightAxis = axisWidget(QwtPlot::yRight);

    //rightAxis = axisWidget(QwtPlot::yRight);
    rightAxis->setColorBarEnabled(true);
    rightAxis->setColorBarWidth(20);
    rightAxis->setColorMap(zInterval, new ColorMap(TypeOfSpec) );
    //ColorMap(TypeOfSpec);

    plotLayout()->setAlignCanvasToScales(true);

    setAxisScale(QwtPlot::yRight,zInterval.minValue(),zInterval.maxValue());
    enableAxis(QwtPlot::yRight);
    replot();
    setAutoFillBackground(true);

    /**
      setAutoFillBackground(true); - autozapolnenie, ctobi isklyuchit'
    nalozjeniya graphikov
    */

}

void matrixofpixels::createdatavector()
{
    if(datavector.size()) datavector.clear();
    int k=0;

    for (int i = 0;i<Npix;i++)
    {
        for (int j = 0;j<Npix;j++)
        {
            if (masked[i][j]) datavector.push_back(matrix[i][j]);
            else {if (masked[i][j] != matrix[i][j]) k++;}
        }
    }
    //std::cout << " datavector.size() " << datavector.size() << " k = " << k <<std::endl;
}

void matrixofpixels::createdatavector(double Xmin, double Xmax)
{
    if(datavector.size()) datavector.clear();
    int k=0;
    float NM,NON; // NON - number of noise NM - number of not masked
    NM = NON = 0;

    for (int i = 0;i<Npix;i++)
    {
        for (int j = 0;j<Npix;j++)
        {
            if (masked[i][j])
            {
                if(matrix[i][j]>=Xmin && matrix[i][j]<=Xmax)
                {
                    datavector.push_back(matrix[i][j]);
                }
                else
                {
                    if(matrix[i][j]<Xmin) {datavector.push_back((float)Xmin);/*std::cout<<"float xmin " << (float)Xmin;*/}
                    if(matrix[i][j]>Xmax) {datavector.push_back((float)Xmax);NON++;}
                }
                NM++;
            }
            else {if (masked[i][j] != matrix[i][j]) k++;}
        }
    }
    // 0 - number of noise 1 - number of not masked 2 - percent of noise
    numofnoise[0] = NON;
    numofnoise[1] = NM;
    numofnoise[2] = (NON/NM)*100.;
}

void matrixofpixels::createhistogram(double *limitsofvalues)
{
    //createdatavector();
    //HistogramData(limitsofvalues);
    createdatavector();
    double maxx = (double)MaxElementOfMatrix();
    double Ymax = his->createDataforHist(datavector,0,maxx,100);

    limitsofvalues[0]=0;
    limitsofvalues[1]=maxx;
    limitsofvalues[2]=100;

    HistogramPlotProperty(0,maxx,(Ymax+Ymax*0.1));
}

void matrixofpixels::createhistogram(double Xmin, double Xmax, int nbins)
{
    createdatavector(Xmin,Xmax);
    double Ymax = his->createDataforHist(datavector,Xmin,Xmax,nbins);
    HistogramPlotProperty(Xmin,Xmax,(Ymax+Ymax*0.1));
}

void matrixofpixels::HistogramPlotProperty(double Xmin, double Xmax, double Ymax)
{
    changeitems();
    setAxisAutoScale(this->xTop);
    setAxisAutoScale(this->xBottom);
    //setAxisFont(QwtPlot::xBottom,QFont("Helvetica",15,1));
    QwtText xlabel("Value");
    QwtText ylabel("Events");
    QColor col(Qt::red);
    xlabel.setColor(col);
    ylabel.setColor(col);
    xlabel.setFont(QFont("Helvetica",15,1));
    ylabel.setFont(QFont("Helvetica",15,1));
    setAxisTitle(QwtPlot::xBottom,xlabel);
    setAxisTitle(QwtPlot::yLeft,ylabel);

    setCanvasBackground(QColor(Qt::white));
    setAxisScale(QwtPlot::xBottom, Xmin, Xmax);
    setAxisMaxMinor(QwtPlot::xBottom, 0);
    setAxisScale(QwtPlot::yLeft, 0, Ymax);
    setAxisMaxMinor(QwtPlot::yLeft, 0);
    plotLayout()->setAlignCanvasToScales(true);

    his->attach(this);

    /*QwtPlotGrid **/grid = new QwtPlotGrid;
    grid->setMajPen(QPen(Qt::gray, 0, Qt::DotLine));
    grid->attach(this);

    replot();
}

void matrixofpixels::changeitems()
{
    QwtPlotItemList items = itemList(QwtPlotItem::Rtti_PlotItem);
     for ( int i = 0; i < items.size(); i++ )
     {
         items[i]->detach();
     }
     this->replot();
}
