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
    if(ui->rule_edit->text().isEmpty()){
        return;
    }
    QString rule = ui->rule_edit->text();
    ui->rule_edit->clear();
    QRegularExpression re;
    re.setPattern(rule);
    re.setPatternOptions(QRegularExpression::CaseInsensitiveOption);

    ui->rules_label->setText(ui->rules_label->text() + "RULE: [" +rule+ "]\n");
    res.push_back(re);

}


bool janela::check_text_dlp(QString& str)
{
    QRegularExpressionMatch match;
    QString message;
    for (unsigned i = 0;i < res.size();++i) {
        match = res[i].match(str);
        if(match.hasMatch()){
            message.append("The rule [" + res[i].pattern() + "] returned positive!\n");
        }
    }

    warning(message);


    return true;
}

void janela::warning(QString &str)
{
    if (!str.isEmpty()) {
        QMessageBox msgBox;
        msgBox.setText("Oops");
        msgBox.setInformativeText(str);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
    }else{
        QMessageBox msgBox;
        msgBox.setText("Oops");
        msgBox.setInformativeText("Got a match!");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
    }
}



void janela::on_add_marker_clicked()
{
    //Add a marker (image to be found in other images)

    QStringList filenames = QFileDialog::getOpenFileNames(this,tr("Open Image"), "", tr("All (*.png *.jpg *.bmp *.jpeg *.tiff *.ppm *.pgm *.pbm *.sr *.ras *.jpe *.jp2 *.tif *.dib *.webp)"));
    if(filenames.isEmpty())return;

    foreach (QString file, filenames) {
        Mat input_2 = imread(file.toStdString());
        cv::cvtColor(input_2,input_2,cv::COLOR_BGR2YCrCb);

        std::array<cv::Mat,3> channels;
        cv::split(input_2,channels);

        ui->markers_label->setText(ui->markers_label->text() + file +"\n");

        //Find the feature points in the image and store them in an vector of touples
        cv::Ptr<cv::xfeatures2d::SIFT> detector = cv::xfeatures2d::SIFT::create();
        std::vector<cv::KeyPoint> keypoints1;
        cv::Mat descriptors1;

        detector->detectAndCompute( channels[0], noArray(), keypoints1, descriptors1 );

        markers.push_back(std::make_tuple(keypoints1,descriptors1,channels[0]));

    }

}


void janela::on_text_match_clicked()
{
    char *old_ctype = strdup(setlocale(LC_ALL, NULL));
    setlocale(LC_ALL, "C");
    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    api->SetPageSegMode(tesseract::PSM_AUTO);

    QStringList files = QFileDialog::getOpenFileNames(this,tr("Open Image"),
                                               "", tr("All (*.png *.jpg *.bmp *.jpeg *.tiff *.ppm *.pgm *.pbm *.sr *.ras *.jpe *.jp2 *.tif *.dib *.webp)"));

    if (api->Init("../tess/", "eng", tesseract::OEM_LSTM_ONLY)) {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }

    foreach (QString str, files) {
        Mat input = imread(str.toStdString());

        api->SetImage(input.data, input.cols, input.rows, 3, input.step);
        api->Recognize(0);
        tesseract::ResultIterator* ri = api->GetIterator();
        tesseract::PageIteratorLevel level = tesseract::RIL_WORD;

        QString text_assembly;
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
                        text_assembly.append(" "+QString(word));
                        if(debug){
                            cv::line(input,cv::Point(this_rect.center().rx(),this_rect.center().ry()),
                                     cv::Point(last.center().rx(),last.center().ry()),
                                     cv::Scalar(0,0,255),2);
                        }

                    }else{
                        text_assembly.append("\n"+QString(word));
                    }

                    last = this_rect;
                    if(debug){
                        cv::rectangle(input,cv::Point(x1,y1), cv::Point(x2,y2),cv::Scalar(0,conf*255,(1.0-conf)*255),2);
                        cv::putText(input,QString(word).toStdString()
                                    ,cv::Point(x1,y1),FONT_HERSHEY_PLAIN,1.2
                                    ,cv::Scalar(0,conf*255,(1.0-conf)*255),1);
                    }

                }
                delete[] word;
            } while (ri->Next(level));
        }
        if(debug){
            if(input.cols > 1000 || input.rows > 800){
                cv::resize(input,input,cv::Size(),0.2,0.2);
            }

                imshow("Result" + std::to_string(i),input);
        }


        check_text_dlp(text_assembly);
    }

    delete api;

    setlocale(LC_ALL, old_ctype);
    free(old_ctype);
}

void janela::on_feature_match_clicked()
{
    //Feature match can find matches in distorted and noisy images, but it can give a high
    //quantity of false positives, even more for low res and low detailed templates


    QStringList filenames = QFileDialog::getOpenFileNames(this,tr("Open Image"), "", tr("All (*.png *.jpg *.bmp *.jpeg *.tiff *.ppm *.pgm *.pbm *.sr *.ras *.jpe *.jp2 *.tif *.dib *.webp)"));
    if(filenames.isEmpty())return;

    cv::Ptr<cv::xfeatures2d::SIFT> detector = cv::xfeatures2d::SIFT::create();
    cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
    const unsigned thresh = ui->thresh_slider->value();

    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;
    unsigned count = 0;
    foreach (QString filename, filenames) {
        qDebug() << filename ;
        Mat input = imread(filename.toStdString());
        cv::cvtColor(input,input,cv::COLOR_BGR2YCrCb);

        std::array<cv::Mat,3> channels;
        cv::split(input,channels);

        keypoints.clear();
        detector->detectAndCompute( channels[0], noArray(), keypoints, descriptors );

        //feature matching
        for (unsigned c_markers = 0;c_markers < markers.size();++c_markers) {

            std::vector< std::vector<cv::DMatch> > knn_matches;
            matcher->knnMatch( descriptors, std::get<1>(markers[c_markers]), knn_matches, 2 );

            std::vector<DMatch> good_matches;
            for (size_t i = 0; i < knn_matches.size(); i++)
            {
                if (knn_matches[i][0].distance < 0.7f * knn_matches[i][1].distance)
                {
                    good_matches.push_back(knn_matches[i][0]);
                }
            }


            cv::Mat img_matches;

            std::vector<DMatch> next_good_matches;


            while (true) {
                if(good_matches.size() > 8){
                    //qDebug() << "Possible match! feature matches: " << good_matches.size();
                    std::vector<Point2f> obj;
                    std::vector<Point2f> scene;
                    for( size_t i = 0; i < good_matches.size(); i++ )
                    {
                        obj.push_back( std::get<0>(markers[c_markers])[ good_matches[i].trainIdx ].pt );
                        scene.push_back( keypoints[ good_matches[i].queryIdx ].pt );
                    }

                    Mat inliers;
                    Mat H = findHomography( obj, scene, RANSAC,3,inliers,2000,0.995);
                    std::vector<DMatch> this_good_matches;
                    for (int i=0; i<inliers.rows; ++i)
                    {
                        if (inliers.at<uchar>(i,0) != 0)
                        {
                            this_good_matches.push_back(good_matches[i]);
                        }else{
                            next_good_matches.push_back(good_matches[i]);
                        }
                    }

                    if (this_good_matches.size() < thresh) {
                        goto end;
                    }
                    if(debug){
                        drawMatches( channels[0], keypoints, std::get<2>(markers[c_markers]), std::get<0>(markers[c_markers]), this_good_matches, img_matches, Scalar::all(-1),
                                Scalar::all(-1), std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

                        std::vector<Point2f> obj_corners(4);
                        std::vector<Point2f> scene_corners(4);
                        //-- Get the corners from the image_1 ( the object to be "detected" )

                        obj_corners[0] = Point2f(0, 0);
                        obj_corners[1] = Point2f( (float)std::get<2>(markers[c_markers]).cols, 0 );
                        obj_corners[2] = Point2f( (float)std::get<2>(markers[c_markers]).cols, (float)std::get<2>(markers[c_markers]).rows );
                        obj_corners[3] = Point2f( 0, (float)std::get<2>(markers[c_markers]).rows );

                        perspectiveTransform( obj_corners, scene_corners, H);
                        //-- Draw lines between the corners (the mapped object in the scene - image_2 )
                        line( img_matches, scene_corners[0],
                                scene_corners[1], Scalar(0, 0, 255), 4 );
                        line( img_matches, scene_corners[1] ,
                                scene_corners[2], Scalar( 0, 0, 255), 4 );
                        line( img_matches, scene_corners[2],
                                scene_corners[3], Scalar( 0, 0, 255), 4 );
                        line( img_matches, scene_corners[3],
                                scene_corners[0], Scalar( 0, 0, 255), 4 );
                        //-- Show detected matches
                        cv::resize(img_matches,img_matches,cv::Size(),0.2,0.2);
                        imshow("Good Matches & Object detection: " + std::to_string(c_markers) + ":" + std::to_string(count), img_matches );
                        ++count;
                    }

                    good_matches.swap(next_good_matches);
                    next_good_matches.clear();
                }else{
    end:
                    break;
                }

            }
        }
    }

}
