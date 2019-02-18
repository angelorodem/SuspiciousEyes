#ifndef JANELA_H
#define JANELA_H

#include <QWidget>
#include <QFileDialog>
#include <QDebug>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QMessageBox>

#include <vector>

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
    void on_add_rule_clicked();

    void on_gogo_clicked();

private:
    std::vector<QRegularExpression> res;
    QRegularExpressionMatch match;
    QString massager;
    bool check_dlp(QString &str);
    Ui::janela *ui;
};

#endif // JANELA_H
