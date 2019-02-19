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

void janela::on_add_rule_clicked()
{
    QString rule = ui->rule_edit->text();
    ui->rule_edit->clear();
    QRegularExpression re;
    re.setPattern(rule);
    re.setPatternOptions(QRegularExpression::CaseInsensitiveOption);

    ui->rules_label->setText(ui->rules_label->text() + "RULE: [" +rule+ "]\n");
    res.push_back(re);

}

void janela::on_gogo_clicked()
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

    check_dlp(wow);

    delete api;

    setlocale(LC_ALL, old_ctype);
    free(old_ctype);
}

bool janela::check_dlp(QString& str)
{
    QRegularExpressionMatch match;
    QString message;
    for (unsigned i = 0;i < res.size();++i) {
        match = res[i].match(str);
        if(match.hasMatch()){
            message.append("The rule [" + res[i].pattern() + "] returned positive!\n");
        }
    }

    if (!message.isEmpty()) {
        QMessageBox msgBox;
        msgBox.setText("Oh shit boiii");
        msgBox.setInformativeText(message);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
    }

    return true;
}

void janela::on_gogo_image_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("Open Image"), "", tr("All (*.png *.jpg *.bmp *.jpeg *.tiff *.ppm *.pgm *.pbm *.sr *.ras *.jpe *.jp2 *.tif *.dib *.webp)"));
    Mat input = imread(filename.toStdString());
    cv:cvtColor(input,input,cv::COLOR_BGR2GRAY);

    QString filename2 = QFileDialog::getOpenFileName(this,tr("Open Image"), "", tr("All (*.png *.jpg *.bmp *.jpeg *.tiff *.ppm *.pgm *.pbm *.sr *.ras *.jpe *.jp2 *.tif *.dib *.webp)"));
    Mat input_2 = imread(filename2.toStdString());
    cv::cvtColor(input_2,input_2,cv::COLOR_BGR2GRAY);

    int minHessian = 400;
    cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create(minHessian);
    std::vector<cv::KeyPoint> keypoints1, keypoints2;
    cv::Mat descriptors1, descriptors2;

    detector->detectAndCompute( input, noArray(), keypoints1, descriptors1 );
    detector->detectAndCompute( input_2, noArray(), keypoints2, descriptors2 );

    cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
    std::vector< std::vector<cv::DMatch> > knn_matches;
    matcher->knnMatch( descriptors1, descriptors2, knn_matches, 2 );

    const float ratio_thresh = 0.45f;
    std::vector<DMatch> good_matches;
    for (size_t i = 0; i < knn_matches.size(); i++)
    {
        if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance)
        {
            good_matches.push_back(knn_matches[i][0]);
        }
    }

    cv::Mat img_matches;
    drawMatches( input, keypoints1, input_2, keypoints2, good_matches, img_matches, Scalar::all(-1),
                 Scalar::all(-1), std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
    //-- Show detected matches
    imshow("Good Matches", img_matches );

}

void janela::on_add_marker_clicked()
{

}
