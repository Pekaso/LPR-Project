#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

int main(int argc, const char * argv[])
{
    double prate=0;
    cv::Mat src_img, src_gray, src_gray2, dst_img1, dst_img2, dst_img3, dst_img4, chr_tmp, chr_img[100];
    cv::Mat tmp_img;
    cv::Mat element;
    std::vector<std::vector<cv::Point>> strage;
    std::vector<std::vector<cv::Point>> chr_str;
    std::vector<std::vector<cv::Point>> tmp_str;
    std::vector<std::vector<cv::Point>>::iterator itc;
    
    //Load image
    src_img = cv::imread("/Users/Kousuke/Documents/OpenCV/OpenCV_PDetectCp/OpenCV_PDetectCp/DetectSample/detectsample4.bmp");
    if(src_img.empty()){
        return -1; //If image can't loaded
    }
    
    //Crop image (640x480 -> 320x240)
    src_img = src_img(cv::Rect(160,160,320,240));
    
    //Convert to grayscale
    cv::cvtColor(src_img, src_gray, CV_RGB2GRAY);
    
    //Gaussian filter (Noise reduction)
    cv::GaussianBlur(src_gray, src_gray2, cv::Size(5,5), 0);
    
    //Edge detection (Use canny method)
    cv::Canny(src_gray2, dst_img1, 150, 255);
    
    //Copy matrix (This is unnecessary code when porting on Android)
    dst_img2 = dst_img1.clone();
    
    //Find the contours in image
    cv::findContours(dst_img2, strage, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_KCOS);
    
    int offs = 4;
    
    //Search area of the plate with the correct aspect ratio
    for (int i = 0; i < strage.size(); i++) {
        size_t count = strage[i].size();
        if (count > 0) {
            cv::Mat pointr;
            cv::Rect rect;
            cv::Mat(strage[i]).convertTo(pointr, CV_32F);
            rect = cv::boundingRect(pointr);
            
            //Calculate aspect ratio
            prate = rect.width/rect.height;
            if (prate >=3 && prate <4.5 && rect.width < 250 && rect.width > 150){
                
                //Write rectangle to original image
                cv::rectangle(src_img, cv::Point(rect.x,rect.y), cvPoint (rect.x + rect.width, rect.y + rect.height), CV_RGB (0, 255, 0), 2);
                
                //Crop plate image
                dst_img3 = src_gray(cv::Rect(rect.x+offs,rect.y+offs,rect.width-2*offs,rect.height-2*offs));
                dst_img4 = dst_img1(cv::Rect(rect.x+offs,rect.y+offs,rect.width-2*offs,rect.height-2*offs));
                break;
            }
        }
    }
    
    //Show the image
    cv::namedWindow("src_img", CV_WINDOW_AUTOSIZE);
    cv::imshow("src_img", src_img);
    cv::namedWindow("src_gray", CV_WINDOW_AUTOSIZE);
    cv::imshow("src_gray", src_gray);
    cv::namedWindow("dst_img1", CV_WINDOW_AUTOSIZE);
    cv::imshow("dst_img1", dst_img1);
    
    //If detection has failed, stop the program
    if (dst_img3.empty()) {
        return -1;
    }
    cv::namedWindow("dst_img3", CV_WINDOW_AUTOSIZE);
    cv::imshow("dst_img3", dst_img3);
    cv::namedWindow("dst_img4", CV_WINDOW_AUTOSIZE);
    cv::imshow("dst_img4", dst_img4);
    
    //cv::waitKey(0);
    
    cv::destroyWindow("src_gray");
    cv::destroyWindow("dst_img1");
    cv::destroyWindow("dst_img3");
    
    cv::findContours(dst_img4, strage, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    
    int cnt = 0;
    char name[10];
    
    itc = strage.begin();
    while (itc!= strage.end()) {
        
                std::vector<cv::Point> pointr = *itc;
                cv::Mat rectMat = cv::Mat(pointr);
                
                cv::Rect rect = cv::boundingRect(rectMat);
                prate = (double)rect.width/rect.height;
                if (prate >0 && prate <1 && itc -> size() > 30){
                    //cv::rectangle(dst_img3, cv::Point(rect.x,rect.y), cvPoint (rect.x + rect.width, rect.y + rect.height), CV_RGB (0, 255, 0), 1);
                    
                    cnt++;
                    chr_tmp = dst_img3(rect);
                    chr_img[cnt] = cv::Mat::zeros(100, 60, CV_8U);
                    cv::resize(chr_tmp, chr_img[cnt], chr_img[cnt].size());
                    
                    std::sprintf(name, "chr_img%d",cnt);
                    std::string wname(name);
                    cv::namedWindow(wname, CV_WINDOW_AUTOSIZE);
                    cv::imshow(wname, chr_img[cnt]);
        
        }
        itc++;
    }
    cv::namedWindow("dst_img3", CV_WINDOW_AUTOSIZE);
    cv::imshow("dst_img3", dst_img3);
    cv::waitKey();
    
    /*
    cv::Mat tmp_chr[34];
    char tmpname[100];
    
    for (int i = 1; i < 35; i++) {
        std::sprintf(tmpname, "/Users/Kousuke/Documents/OpenCV/OpenCV_PDetectCp/OpenCV_PDetectCp/Template/%d.bmp",i);
        tmp_chr[i] = cv::imread(tmpname, CV_LOAD_IMAGE_GRAYSCALE);
        //cv::Canny(tmp_chr[i], tmp_chr[i], 0, 255);
        cv::adaptiveThreshold(tmp_chr[i], tmp_chr[i], 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 5, 5);
    }
    
    double result[cnt][34];
    char text[16];
    
    for (int j = 1; j < cnt+1; j++) {
        cv::findContours(chr_img[j], chr_str, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
        for (int i = 1; i < 35; i++) {
            cv::findContours(tmp_chr[i], tmp_str, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
            result[j][i] = cv::matchShapes(chr_str[0], chr_str[0], CV_CONTOURS_MATCH_I1, 0);
            //std::sprintf(text,"%.5f", result[j][i]);
            std::cout << j << "," << i << "," << result[j][i] << std::endl;
        }
        cv::waitKey();
    }
    
    cv::waitKey();
    */
}

