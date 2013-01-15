#ifndef PRMPIXREAD_H
#define PRMPIXREAD_H

#include <QMainWindow>
#include <QLayout>
#include "matrixofpixels.h"


namespace Ui {
    class PrMpixRead;
}


class PrMpixRead : public QMainWindow//, matrixofpixels
{
    Q_OBJECT

public:
    //PrMpixRead
    explicit PrMpixRead(QWidget *parent = 0);
    ~PrMpixRead();
    matrixofpixels *MPix;

    void matrixspectrogram(int TypeOfSpec);

    QHBoxLayout *layout;
    //QGraphicsScene *scene;
    //enum ColorMapForPlot {BLUERED = 1, BLACKWHITE = 2};
    enum TypeOfSpecs {WORKPIXELS = 1, MASKPIXELS = 2};
    enum ModePlot {OPENNEWMATRIX = 0, SPECTROGRAM = 1, HISTOGRAM = 2};
    enum ZoomMode {HORIZONTALZOOM = 1, VERTICALZOOM = 2, RECTZOOM = 3};
    int MaskOrPix; // pixels or masked ColorMapForPlot

    MyZoomer *myzoom;
    //QwtPickerTrackerMachine *statemach;
    QwtPlotPicker *pick;
    QwtPlotPanner *panner;
    bool ZoomInd;

private slots:
    void statusbarinfo(const QPointF &pos);
    void statusbarinfohist(const QPointF &pos);
    void spectr_ScrollB_down_valueChanged(int value);
    void spectr_ScrollB_up_valueChanged(int value);

    void on_actionOpen_pixel_triggered();
    void on_actionOpen_mask_triggered();
    void on_actionSave_canv_As_triggered();
    void on_verScrollB_down_valueChanged(int value);

    void on_verScrollB_up_valueChanged(int value);

    void on_Button_graph_clicked();

    void on_actionSpectrogramma_of_Working_Pixels_triggered();

    void on_actionSpectrogramma_of_Masking_Pixels_triggered();

    void on_actionFlip_matrix_of_mask_triggered();

    void on_radioButton_HORIZ_clicked();

    void on_radioButton_VERT_clicked();

    void on_radioButton_RECT_clicked();

private:
    Ui::PrMpixRead *ui;
    void ZoomIndcheck(int index);
};



#endif // PRMPIXREAD_H
