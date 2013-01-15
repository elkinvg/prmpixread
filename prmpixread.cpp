#include "prmpixread.h"
#include "ui_prmpixread.h"

#include <QFileDialog>
#include <QFile>
#include <qmath.h>
#include <iostream>

PrMpixRead::PrMpixRead(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PrMpixRead)
{
    MPix = new matrixofpixels ;
    ZoomInd = true;
      ui->setupUi(this);
//    QHBoxLayout *layout = new QHBoxLayout(ui->graph_frame);
//    layout->setContentsMargins( 0, 0, 0, 0 );
    layout = new QHBoxLayout(ui->graph_frame);
    layout->setContentsMargins( 0, 0, 0, 0 );
    ui->scrool_frame->setVisible(false);
    ui->Button_graph->setEnabled(false);
    QDoubleValidator *doublevalidator = new QDoubleValidator;
    doublevalidator->setNotation(QDoubleValidator::ScientificNotation);
    ui->lineEdit_xmin->setValidator(doublevalidator);
    ui->lineEdit_xmax->setValidator(doublevalidator);

    QRegExp rx("[1-9]\\d{0,3}");
    QValidator *v = new QRegExpValidator(rx, this);
    ui->lineEdit_nbins->setValidator(v);
//    QObject *df = new QObject;
    //scene = new QGraphicsScene;
}



PrMpixRead::~PrMpixRead()
{
    delete MPix;
    delete ui;
    //delete scene;
}

void PrMpixRead::on_actionOpen_pixel_triggered()
{
    QString fn = QFileDialog::getOpenFileName(this, tr("File..."),QString(), tr("txt-files (*.txt);;All Files (*)"));
    if (fn.isEmpty())
    {
        return;
    }
    if (MPix->getmatrixofpixels(fn))
    {
        ui->InfooutTextEdit->appendPlainText( "Open file with pixel \n" + fn + "\n\n");
        ui->spinBox_down->setValue(ui->spinBox_down->minimum());
        ui->spinBox_up->setValue(ui->spinBox_up->maximum());
        ui->scrool_frame->setVisible(true);
        ui->Button_graph->setEnabled(true);
        ui->frame_lineedit->setEnabled(false);
        MaskOrPix = WORKPIXELS;
//        if(!ZoomInd)
//        {
//            delete myzoom;
//            delete pick;
//        }
//        ZoomInd = true;

        ZoomIndcheck(OPENNEWMATRIX);
        connect(ui->spinBox_down,SIGNAL(valueChanged(int)),SLOT(spectr_ScrollB_down_valueChanged(int)));
        matrixspectrogram(MaskOrPix);        
        ui->InfooutTextEdit->insertPlainText("maximum element of the matrix\n" + QString::number(MPix->MaxElOfMat) + "\n");
    }
}

void PrMpixRead::on_actionOpen_mask_triggered()
{
//    delete MPix;
    QString fn = QFileDialog::getOpenFileName(this, tr("File..."),QString(), tr("txt-files (*.txt);;All Files (*)"));
    if (fn.isEmpty())
    {
        return;
    }
    if(MPix->getmaskedofpixels(fn))
    {
        ui->InfooutTextEdit->appendPlainText( "Open file with mask \n" + fn + "\n\n");
        ui->spinBox_down->setValue(ui->spinBox_down->minimum());
        ui->spinBox_up->setValue(ui->spinBox_up->maximum());
        ui->scrool_frame->setVisible(false);
        MaskOrPix = MASKPIXELS;
//        if(!ZoomInd)
//        {
//            delete myzoom;
//            delete pick;
//            //delete statemach;
//        }
//        ZoomInd = true;
        ZoomIndcheck(OPENNEWMATRIX);
        matrixspectrogram(MaskOrPix);
        ui->InfooutTextEdit->insertPlainText("number of not masked pixels\n" + QString::number(MPix->numofnotmasked) + "\n");
        ui->InfooutTextEdit->moveCursor(QTextCursor::End);
        ui->frame_lineedit->setEnabled(false);
    }
}

void PrMpixRead::matrixspectrogram(int TypeOfSpec)
{
    // TypeOfSpec = {WORKPIXELS = 1, MASKPIXELS = 2}
    QLayoutItem * item = ui->graph_frame->layout()->itemAt(0);

    while ( (item = ui->graph_frame->layout()->itemAt(0)) )
    {
        ui->graph_frame->layout()->removeItem(item);
        ui->graph_frame->layout()->removeWidget(item->widget());
        ui->graph_frame->layout()->update();
    }
    QwtMatrixRasterData *rasterpixdata = new QwtMatrixRasterData();
    double MinZRasterdata,MaxZRasterdata;

    switch (TypeOfSpec)
    {
    case 1:
        rasterpixdata->setValueMatrix(MPix->matdatavector,Npix);
        rasterpixdata->setInterval(Qt::XAxis,QwtInterval(0,Npix,QwtInterval::ExcludeMaximum));
        rasterpixdata->setInterval(Qt::YAxis,QwtInterval(0,Npix,QwtInterval::ExcludeMaximum));
        MinZRasterdata = (double)MPix->MaxElOfMat*((double)(ui->spinBox_down->value()/100.));
        MaxZRasterdata = (double)MPix->MaxElOfMat*((double)(ui->spinBox_up->value()/100.));
        rasterpixdata->setInterval( Qt::ZAxis, QwtInterval(MinZRasterdata, MaxZRasterdata) );
        break;
    case 2:
        rasterpixdata->setValueMatrix(MPix->maskdatavector,Npix);
        rasterpixdata->setInterval(Qt::XAxis,QwtInterval(-0.5,Npix-0.5,QwtInterval::ExcludeMaximum));
        rasterpixdata->setInterval(Qt::YAxis,QwtInterval(-0.5,Npix-0.5,QwtInterval::ExcludeMaximum));
        rasterpixdata->setInterval( Qt::ZAxis, QwtInterval(0., 1.) );
        break;
    //default:
    }



    MPix->PixPlotSpect(TypeOfSpec,rasterpixdata);

    ZoomIndcheck(SPECTROGRAM);

    ui->graph_frame->layout()->addWidget(MPix);
}

void PrMpixRead::on_actionSave_canv_As_triggered()
{
    QString fn = QFileDialog::/*getSaveFileName(this,"Save as",tr("Image-Files (*.jpg *.jpeg *.png)"));*/getSaveFileName(this, tr("Save current Icon File"),"", "*.png;; *.jpg *.jpeg");
    if (fn.isEmpty()){
        return;
    }
    QPixmap pixmap = QPixmap::grabWidget(ui->graph_frame);
    pixmap.save(fn);
}

void PrMpixRead::on_verScrollB_down_valueChanged(int value)
{
    int vd = ui->verScrollB_down->value();
    int vu = ui->verScrollB_up->value();
    if (vd>vu) ui->verScrollB_up->setValue(vd);
    matrixspectrogram(MaskOrPix);
}

void PrMpixRead::on_verScrollB_up_valueChanged(int value)
{
    int vd = ui->verScrollB_down->value();
    int vu = ui->verScrollB_up->value();
    if (vu<vd) ui->verScrollB_down->setValue(vu);
    matrixspectrogram(MaskOrPix);
    //maskedspectrogram();
}


void PrMpixRead::statusbarinfo(const QPointF &pos)
{
    float elementofmatrix;
    int Xpos = (int)floor(pos.x());
    //int df = (int)MPix->myzoom->trackerPosition()
    int Ypos = (int)floor(pos.y());
    if (Xpos <= 0 ) Xpos = 0; if (Ypos <= 0 ) Ypos = 0;
    if (Xpos >= 255) Xpos = 255; if (Ypos >= 255) Ypos = 255;


    switch (MaskOrPix)
    {
    case 1:
        elementofmatrix = MPix->matrix[Ypos][Xpos];
        break;
    case 2:
        elementofmatrix = MPix->masked[Ypos][Xpos];
        break;
    }

//    std::cout << Xpos << "  " << Ypos << "  " << MPix->matrix[Ypos][Xpos] <<std::endl;

    QString statusmessage("X=\t\t" + QString::number(Xpos) + "\t\t" + "Y=\t\t" + QString::number(Ypos) + "\t\t" + "Value =\t\t" + QString::number(elementofmatrix));
    ui->statusBar->showMessage(statusmessage);
}

void PrMpixRead::on_Button_graph_clicked()
{

//    MPix->clearFocus();
    if (ui->frame_lineedit->isEnabled())
    {
        if(!ZoomInd)
        {
            std::cout<< " ASSSAA " << std::endl;
            delete myzoom;
            delete pick;
        }
        ZoomInd = true;
        double Xmax = ui->lineEdit_xmax->displayText().toDouble();
        double Xmin = ui->lineEdit_xmin->displayText().toDouble();
        int Nbins = ui->lineEdit_nbins->displayText().toInt();
        MPix->createhistogram(Xmin,Xmax,Nbins);
        ui->frame_lineedit->setEnabled(true);
        ZoomIndcheck(HISTOGRAM);
        ui->InfooutTextEdit->insertPlainText("\n");
        ui->InfooutTextEdit->insertPlainText("Xmin= " + QString::number(Xmin) + "   Xmax = " + QString::number(Xmax) + "\n");
        ui->InfooutTextEdit->insertPlainText("Number of noise = " + QString::number(MPix->numofnoise[0]) + "   Percent of noise =" + QString::number(MPix->numofnoise[2]) + "%\n");
        ui->InfooutTextEdit->moveCursor(QTextCursor::End);
    }
    else
    {
        if(!ZoomInd)
        {
            std::cout<< " ASSSAA " << std::endl;
            delete myzoom;
            delete pick;
        }
        ZoomInd = true;

//        MPix->createdatavector();
        double limitsofvalues[3]; // limit values for histogram on xleft,xright,ytop
//        MPix->HistogramData(limitsofvalues);
        MPix->createhistogram(limitsofvalues);

        ui->lineEdit_xmin->setText(QString::number(limitsofvalues[0]));
        ui->lineEdit_xmax->setText(QString::number(limitsofvalues[1]));
        ui->lineEdit_nbins->setText(QString::number(limitsofvalues[2]));
        ui->frame_lineedit->setEnabled(true);

        ZoomIndcheck(HISTOGRAM);
    }

}

void PrMpixRead::spectr_ScrollB_down_valueChanged(int value)
{
    int vd = ui->verScrollB_down->value();
    int vu = ui->verScrollB_up->value();
    if (vd>vu) ui->verScrollB_up->setValue(vd);
    matrixspectrogram(MaskOrPix);
}

void PrMpixRead::spectr_ScrollB_up_valueChanged(int value)
{
}

void PrMpixRead::ZoomIndcheck(int index)
{
    // index is mode
    // enum ModePlot {OPENNEWMATRIX = 0, SPECTROGRAM = 1, HISTOGRAM = 2};
    switch (index)
    {
    case 0:
        if(!ZoomInd)
        {
            std::cout<< " DELETE() " << std::endl;
            delete pick;
            delete panner;
            delete myzoom;
        }
        ZoomInd = true;
        break;
    case 1:
        if (ZoomInd)
        {
            std::cout<< " ZoomIndcheck() " << std::endl;
            myzoom = new MyZoomer(MPix->canvas());

            myzoom->setMousePattern( QwtEventPattern::MouseSelect2,
                Qt::RightButton, Qt::ControlModifier );
            myzoom->setMousePattern( QwtEventPattern::MouseSelect3,
                Qt::RightButton );

            panner = new QwtPlotPanner( MPix->canvas() );
            panner->setMouseButton( Qt::MidButton );
            panner->setAxisEnabled(QwtPlot::yRight,false);
            pick = new QwtPlotPicker(MPix->canvas());
            pick->setStateMachine(/*statemach*/new QwtPickerTrackerMachine());
            connect(pick,SIGNAL( moved(QPointF)/*selected(QPointF)*/),SLOT(statusbarinfo(QPointF)));

            if (ui->radioButton_HORIZ->isChecked()) myzoom->ZoomMode(HORIZONTALZOOM);
            if (ui->radioButton_VERT->isChecked()) myzoom->ZoomMode(VERTICALZOOM);
            if (ui->radioButton_RECT->isChecked()) myzoom->ZoomMode(RECTZOOM);

            MPix->setAxisTitle(QwtPlot::xBottom,"");
            MPix->setAxisTitle(QwtPlot::yLeft,"");
            //statemach = new QwtPickerTrackerMachine();


            ZoomInd = false;
        }
        break;
    case 2:
        if (ZoomInd)
        {
            myzoom = new MyZoomer(MPix->canvas());

            if (ui->radioButton_HORIZ->isChecked()) {myzoom->ZoomMode(HORIZONTALZOOM);}
            if (ui->radioButton_VERT->isChecked()) {myzoom->ZoomMode(VERTICALZOOM);}
            if (ui->radioButton_RECT->isChecked()) {myzoom->ZoomMode(RECTZOOM);}

            myzoom->setMousePattern( QwtEventPattern::MouseSelect2,
                Qt::RightButton, Qt::ControlModifier );
            myzoom->setMousePattern( QwtEventPattern::MouseSelect3,
                Qt::RightButton );

            panner = new QwtPlotPanner( MPix->canvas() );
            panner->setMouseButton( Qt::MidButton );

            pick = new QwtPlotPicker(MPix->canvas());
            //statemach = new QwtPickerTrackerMachine();
            pick->setStateMachine(/*statemach*/new QwtPickerTrackerMachine());
            connect(pick,SIGNAL( moved(QPointF)/*selected(QPointF)*/),SLOT(statusbarinfohist(QPointF)));
            ZoomInd = false;
            ui->menuGraph->setEnabled(true);
            ui->scrool_frame->setVisible(false);
            MPix->enableAxis(QwtPlot::yRight,false);
        }
        break;
    }
}

void PrMpixRead::on_actionSpectrogramma_of_Working_Pixels_triggered()
{
    ZoomIndcheck(OPENNEWMATRIX);
    ui->spinBox_down->setValue(ui->spinBox_down->minimum());
    ui->spinBox_up->setValue(ui->spinBox_up->maximum());
    ui->scrool_frame->setVisible(true);
    connect(ui->spinBox_down,SIGNAL(valueChanged(int)),SLOT(spectr_ScrollB_down_valueChanged(int)));
    MaskOrPix = WORKPIXELS;
    matrixspectrogram(MaskOrPix);
}

void PrMpixRead::on_actionSpectrogramma_of_Masking_Pixels_triggered()
{
    ZoomIndcheck(OPENNEWMATRIX);
    ui->scrool_frame->setVisible(false);
    MaskOrPix = MASKPIXELS;
    matrixspectrogram(MaskOrPix);
}

void PrMpixRead::on_actionFlip_matrix_of_mask_triggered()
{
    ui->frame_lineedit->setEnabled(false);
    MPix->flipudmasked();
}

void PrMpixRead::on_radioButton_HORIZ_clicked()
{
    myzoom->ZoomMode(HORIZONTALZOOM);
}

void PrMpixRead::on_radioButton_VERT_clicked()
{
    myzoom->ZoomMode(VERTICALZOOM);
}

void PrMpixRead::on_radioButton_RECT_clicked()
{
    myzoom->ZoomMode(RECTZOOM);
}

void PrMpixRead::statusbarinfohist(const QPointF &pos)
{
    int Xpos = floor(pos.x());
    int Ypos = (int)floor(pos.y());
    QString statusmessage("Value=\t\t" + QString::number(Xpos) + "\t\t" + "Yposition=\t\t" +  QString::number(Ypos));
    ui->statusBar->showMessage(statusmessage);
}
