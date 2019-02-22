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



void janela::on_add_marker_clicked()
{
    //Add a marker (image to be found in other images)

    QString filename2 = QFileDialog::getOpenFileName(this,tr("Open Image"), "", tr("All (*.png *.jpg *.bmp *.jpeg *.tiff *.ppm *.pgm *.pbm *.sr *.ras *.jpe *.jp2 *.tif *.dib *.webp)"));
    if(filename2.isEmpty())return;

    Mat input_2 = imread(filename2.toStdString());
    cv::cvtColor(input_2,input_2,cv::COLOR_BGR2YCrCb);

    std::array<cv::Mat,3> channels;
    cv::split(input_2,channels);

    ui->markers_label->setText(ui->markers_label->text() + filename2 +"\n");

    //Find the feature points in the image and store them in an vector of touples
    cv::Ptr<cv::xfeatures2d::SIFT> detector = cv::xfeatures2d::SIFT::create();
    std::vector<cv::KeyPoint> keypoints1;
    cv::Mat descriptors1;

    detector->detectAndCompute( channels[0], noArray(), keypoints1, descriptors1 );

    markers.push_back(std::make_tuple(keypoints1,descriptors1,channels[0]));

}

void janela::on_template_match_clicked()
{
    //Template gives high precision, but it can't detect distorted or heavly rotated images.

    //Load image
    QString filename = QFileDialog::getOpenFileName(this,tr("Open Image"), "", tr("All (*.png *.jpg *.bmp *.jpeg *.tiff *.ppm *.pgm *.pbm *.sr *.ras *.jpe *.jp2 *.tif *.dib *.webp)"));
    if(filename.isEmpty())return;

    Mat input = imread(filename.toStdString());

    //Save colored image for result display
    cv::Mat img_display;
    input.copyTo(img_display);
    cv::cvtColor(input,input,cv::COLOR_BGR2YCrCb);

    std::array<cv::Mat,3> channels;
    cv::split(input,channels);

    cv::Mat result;

    //iterate all templates to scan for matches
    for (uint32_t c_markers = 0;c_markers < markers.size();++c_markers) {
        int result_cols =  channels[0].cols - std::get<2>(markers[c_markers]).cols + 1;
        int result_rows = channels[0].rows - std::get<2>(markers[c_markers]).rows + 1;
        result.create( result_rows, result_cols, CV_8UC1 );
        matchTemplate( channels[0], std::get<2>(markers[c_markers]), result, 1);

        //normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );
        double minVal; double maxVal; Point minLoc(0,0); Point maxLoc(0,0);
        minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );

        qDebug() << "min: " << minVal<< " max: " << maxVal;

        if(minVal < 0.2){

            rectangle( img_display, minLoc, Point( minLoc.x + std::get<2>(markers[c_markers]).cols , minLoc.y + std::get<2>(markers[c_markers]).rows ), cv::Scalar(0,0,255), 5, 8, 0 );
            rectangle( result, minLoc, Point( minLoc.x + std::get<2>(markers[c_markers]).cols , minLoc.y + std::get<2>(markers[c_markers]).rows ), Scalar(0,0,255), 5, 8, 0 );

            if(!img_display.empty()){
                cv::resize(img_display,img_display,cv::Size(),0.25,0.25,cv::INTER_CUBIC);
                imshow( "image_window: " + std::to_string(c_markers), img_display );
            }
        }

    }



}

void janela::on_text_match_clicked()
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

void janela::on_feature_match_clicked()
{
    //Feature match can find matches in distorted and noisy images, but it can give a high
    //quantity of false positives, even more for low res and low detailed templates

    QString filename = QFileDialog::getOpenFileName(this,tr("Open Image"), "", tr("All (*.png *.jpg *.bmp *.jpeg *.tiff *.ppm *.pgm *.pbm *.sr *.ras *.jpe *.jp2 *.tif *.dib *.webp)"));
    if(filename.isEmpty())return;
    Mat input = imread(filename.toStdString());
    cv::cvtColor(input,input,cv::COLOR_BGR2YCrCb);

    std::array<cv::Mat,3> channels;
    cv::split(input,channels);

    cv::Ptr<cv::xfeatures2d::SIFT> detector = cv::xfeatures2d::SIFT::create();
    std::vector<cv::KeyPoint> keypoints1;
    cv::Mat descriptors1;

    detector->detectAndCompute( channels[0], noArray(), keypoints1, descriptors1 );

    //feature matching
    for (unsigned c_markers = 0;c_markers < markers.size();++c_markers) {

        cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
        std::vector< std::vector<cv::DMatch> > knn_matches;
        matcher->knnMatch( descriptors1, std::get<1>(markers[c_markers]), knn_matches, 2 );

        const float ratio_thresh = 0.7f;
        std::vector<DMatch> good_matches;
        for (size_t i = 0; i < knn_matches.size(); i++)
        {
            if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance)
            {
                good_matches.push_back(knn_matches[i][0]);
            }
        }

        const unsigned thresh = ui->thresh_slider->value();
        cv::Mat img_matches;

        std::vector<DMatch> next_good_matches;
        unsigned count = 0;
        qDebug() << "Possible match! feature matches: " << good_matches.size();
        int32_t best_match_values = 0;
        while (true) {
            if(good_matches.size() > 8){
                //qDebug() << "Possible match! feature matches: " << good_matches.size();
                std::vector<Point2f> obj;
                std::vector<Point2f> scene;
                for( size_t i = 0; i < good_matches.size(); i++ )
                {
                    obj.push_back( std::get<0>(markers[c_markers])[ good_matches[i].trainIdx ].pt );
                    scene.push_back( keypoints1[ good_matches[i].queryIdx ].pt );
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
                std::vector<Point2f> obj_corners(4);
                std::vector<Point2f> scene_corners(4);
                if (this_good_matches.size() < thresh) {

                    qDebug() << "No good: " << this_good_matches.size();
                    goto end;
                }
                best_match_values = this_good_matches.size();

                drawMatches( channels[0], keypoints1, std::get<2>(markers[c_markers]), std::get<0>(markers[c_markers]), this_good_matches, img_matches, Scalar::all(-1),
                        Scalar::all(-1), std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );


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
                imshow("Good Matches & Object detection: " + std::to_string(c_markers+count), img_matches );
                ++count;
                good_matches.swap(next_good_matches);
                next_good_matches.clear();
            }else{
end:
                break;
            }

        }
    }
}
