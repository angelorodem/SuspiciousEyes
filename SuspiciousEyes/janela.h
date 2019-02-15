#ifndef JANELA_H
#define JANELA_H

#include <QWidget>
#include <QFileDialog>
#include <QDebug>

#include "opencv2/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "tesseract/baseapi.h"
#include "tesseract/helpers.h"
#include "tesseract/resultiterator.h"


using namespace cv;

namespace Ui {
class janela;
}

class janela : public QWidget
{
    Q_OBJECT

public:
    explicit janela(QWidget *parent = nullptr);
    ~janela();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::janela *ui;
};

#endif // JANELA_H
