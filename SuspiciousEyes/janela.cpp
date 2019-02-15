#include "janela.h"
#include "ui_janela.h"

janela::janela(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::janela)
{
    ui->setupUi(this);
}

janela::~janela()
{
    delete ui;
}

void janela::on_pushButton_clicked()
{
    QString str = QFileDialog::getOpenFileName(this,tr("Open Image"), "", tr("All (*.*)"));
    qDebug() << str;
    Mat input = imread(str.toStdString(),CV_32FC3);

    imshow("Janelao",input);

}

void janela::on_pushButton_2_clicked()
{
    char *old_ctype = strdup(setlocale(LC_ALL, NULL));
    setlocale(LC_ALL, "C");
    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    api->SetPageSegMode(tesseract::PSM_AUTO);
    QString str = QFileDialog::getOpenFileName(this,tr("Open Image"), "", tr("All (*.png *.jpg *.bmp *.jpeg *.tiff *.ppm *.pgm *.pbm *.sr *.ras *.jpe *.jp2 *.tif *.dib *.webp)"));
    Mat input = imread(str.toStdString());

    if (api->Init("../tess/", "eng", tesseract::OEM_LSTM_ONLY)) {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }

    api->SetImage(input.data, input.cols, input.rows, 3, input.step);
    api->Recognize(0);
    tesseract::ResultIterator* ri = api->GetIterator();
    tesseract::PageIteratorLevel level = tesseract::RIL_WORD;

    QString wow;
    if (ri != 0) {
        QRect last(0,0,0,0);
        do {
            const char* word = ri->GetUTF8Text(level);
            float conf = ri->Confidence(level)/100;
            int x1, y1, x2, y2;
            ri->BoundingBox(level, &x1, &y1, &x2, &y2);

            if (conf > 0.4f) {
                QRect this_rect(QPoint(x1,y1),QPoint(x2,y2));
                if (last.isNull()) {
                    last = this_rect;
                }
                if (last.top()-8 < this_rect.center().ry() && last.bottom()+8 > this_rect.center().ry() ) {
                    wow.append(" "+QString(word));
                    cv::line(input,cv::Point(this_rect.center().rx(),this_rect.center().ry()),
                             cv::Point(last.center().rx(),last.center().ry()),
                             cv::Scalar(0,0,255),2);
                }else{
                    wow.append("\n"+QString(word));
                }

                last = this_rect;
                cv::rectangle(input,cv::Point(x1,y1), cv::Point(x2,y2),cv::Scalar(0,conf*255,(1.0-conf)*255),2);
                cv::putText(input,QString(word).toStdString()
                            ,cv::Point(x1,y1),FONT_HERSHEY_PLAIN,1.2
                            ,cv::Scalar(0,conf*255,(1.0-conf)*255),1);
            }
            delete[] word;
        } while (ri->Next(level));
    }
    qDebug() << wow;

    imshow("text",input);

    delete api;

    setlocale(LC_ALL, old_ctype);
    free(old_ctype);
}
