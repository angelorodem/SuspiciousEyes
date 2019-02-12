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
    api->Init(NULL, "eng", tesseract::OEM_LSTM_ONLY);
    api->SetPageSegMode(tesseract::PSM_AUTO);
    QString str = QFileDialog::getOpenFileName(this,tr("Open Image"), "", tr("All (*.*)"));
    Mat input = imread(str.toStdString());

    if (api->Init(NULL, "eng")) {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }

    api->SetImage(input.data, input.cols, input.rows, 3, input.step);
    api->Recognize(0);
    tesseract::ResultIterator* ri = api->GetIterator();
    tesseract::PageIteratorLevel level = tesseract::RIL_WORD;

    if (ri != 0) {
        do {
            const char* word = ri->GetUTF8Text(level);
            float conf = ri->Confidence(level);
            int x1, y1, x2, y2;
            ri->BoundingBox(level, &x1, &y1, &x2, &y2);
            cv::rectangle(input,cv::Point(x1,y1), cv::Point(x2,y2),cv::Scalar(255,0,0));
            cv::putText(input,QString(word).toStdString()
                        ,cv::Point(x1,y1),FONT_HERSHEY_PLAIN,1
                        ,cv::Scalar(0,255,0),2);
            delete[] word;
        } while (ri->Next(level));
    }

    imshow("text",input);


    setlocale(LC_ALL, old_ctype);
    free(old_ctype);
}
