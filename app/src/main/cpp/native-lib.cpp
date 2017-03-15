/*
#include <jni.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

extern "C" {

int process(Mat img_input, Mat img_result) {
    cvtColor(img_input, img_result, CV_RGBA2GRAY);

    return (0);
}
    JNIEXPORT jint JNICALL
    Java_friend_skplanet_myapplication_MainActivity_convertNativeLib(JNIEnv*, jobject, jlong addrInput, jlong addrResult)
    {



        */
/*jbyte* _yuv  = env->GetByteArrayElements(yuv, 0);
        jint*  _bgra = env->GetIntArrayElements(bgra, 0);

        Mat myuv(height + height/2, width, CV_8UC1, (unsigned char *)_yuv);
        Mat mbgra(height, width, CV_8UC4, (unsigned char *)_bgra);
        //_bgra int형 배열로부터 8bit 4채널로 바꿔 mbgra라는 Mat 구조체
        Mat mgray(height, width, CV_8UC1, (unsigned char *)_yuv);
        //_bgra int형 배열로부터 8bit 1채널로 바꿔 mbgra라는 Mat 구조체
        //Please make attention about BGRA byte order
        //ARGB stored in java as int array becomes BGRA at native level
        cvtColor(myuv, mbgra, CV_YUV420sp2BGR, 4); //YUV로 들어온 색상체계를 BGR로 바꾼다.
        //일부 CV_YUV420sp2BGR이 안먹힌다. sp를 i로 바꿔서 해도 무방할듯..?
        vector<KeyPoint> v;
        FastFeatureDetector detector(50); //특징점 추출 함수 호출
        detector.detect(mgray, v);  //특징점 추출 함수 호출
        for( size_t i = 0; i < v.size(); i++ ) //특징점이 저장된 v 벡터를 중점으로 CIRCLE을 그려준다.
            circle(mbgra, Point(v[i].pt.x, v[i].pt.y), 10, Scalar(0,0,255,255));


//픽셀데이터 접근 추가사항(본인이 추가한 내용이다.)
        for(int co=0; co<mbgra.cols; co++)
        {
            for(int ro=0; ro<mbgra.rows; ro++)
            {
                mbgra.at<Vec4b>(ro,co)[0] = 255; // 블루(Blue)값을 255로 바꿔준다.
                // mbgra.at<Vec4b>(ro,co)[1] = 255; //이것들까지 바꾸면 하얀색 화면을 보게될것이다.
                // mbgra.at<Vec4b>(ro,co)[2] = 255;
            }
        }


//메모리 해제
        env->ReleaseIntArrayElements(bgra, _bgra, 0);
        env->ReleaseByteArrayElements(yuv, _yuv, 0);*//*


        Mat &img_input = *(Mat *) addrInput;
        Mat &img_result = *(Mat *) addrResult;

        int conv = process(img_input, img_result);
        int ret = (jint) conv;

        return ret;

    }
}*/
#include <jni.h>
#include <opencv2/opencv.hpp>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <string>


using namespace cv;
using namespace std;

extern "C" {

float resize(Mat img_src, Mat &img_resize, int resize_width){

    float scale = resize_width / (float)img_src.cols ;
    if (img_src.cols > resize_width) {
        int new_height = cvRound(img_src.rows * scale);
        resize(img_src, img_resize, Size(resize_width, new_height));
    }
    else {
        img_resize = img_src;
    }
    return scale;
}


JNIEXPORT void JNICALL
Java_friend_skplanet_myapplication_MainActivity_detect(JNIEnv *, jobject,
                                                                          jlong cascadeClassifier_face,
                                                                          jlong cascadeClassifier_eye,
                                                                          jlong addrInput,
                                                                          jlong addrResult) {

    Mat &img_input = *(Mat *) addrInput;
    Mat &img_result = *(Mat *) addrResult;

    img_result = img_input.clone();

    std::vector<Rect> faces;
    Mat img_gray;

    cvtColor(img_input, img_gray, COLOR_BGR2GRAY);
    equalizeHist(img_gray, img_gray);

    Mat img_resize;
    float resizeRatio = resize(img_gray, img_resize, 640);

    //-- Detect faces
    ((CascadeClassifier *) cascadeClassifier_face)->detectMultiScale( img_resize, faces, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(30, 30) );


    __android_log_print(ANDROID_LOG_DEBUG, (char *) "native-lib :: ",
                        (char *) "face %d found ", faces.size());

    for (int i = 0; i < faces.size(); i++) {
        double real_facesize_x = faces[i].x / resizeRatio;
        double real_facesize_y = faces[i].y / resizeRatio;
        double real_facesize_width = faces[i].width / resizeRatio;
        double real_facesize_height = faces[i].height / resizeRatio;

        Point center( real_facesize_x + real_facesize_width / 2, real_facesize_y + real_facesize_height/2);
        ellipse(img_result, center, Size( real_facesize_width / 2, real_facesize_height / 2), 0, 0, 360,
                Scalar(255, 0, 255), 30, 8, 0);


        Rect face_area(real_facesize_x, real_facesize_y, real_facesize_width,real_facesize_height);
        Mat faceROI = img_gray( face_area );
        std::vector<Rect> eyes;

        //-- In each face, detect eyes
        ((CascadeClassifier *) cascadeClassifier_eye)->detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CASCADE_SCALE_IMAGE, Size(30, 30) );

        for ( size_t j = 0; j < eyes.size(); j++ )
        {
            Point eye_center( real_facesize_x + eyes[j].x + eyes[j].width/2, real_facesize_y + eyes[j].y + eyes[j].height/2 );
            int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
            circle( img_result, eye_center, radius, Scalar( 255, 0, 0 ), 30, 8, 0 );
        }
    }

}


JNIEXPORT jlong JNICALL
Java_friend_skplanet_myapplication_MainActivity_loadCascade(JNIEnv *env, jobject,
                                                                               jstring cascadeFileName) {

    const char *nativeFileNameString = env->GetStringUTFChars(cascadeFileName, JNI_FALSE);

    string baseDir("/storage/emulated/0/");
    baseDir.append(nativeFileNameString);
    const char *pathDir = baseDir.c_str();

    jlong ret = 0;
    ret = (jlong) new CascadeClassifier(pathDir);
    if (((CascadeClassifier *) ret)->empty()) {
        __android_log_print(ANDROID_LOG_DEBUG, "native-lib :: ",
                            "CascadeClassifier로 로딩 실패  %s", nativeFileNameString);
    }
    else
        __android_log_print(ANDROID_LOG_DEBUG, "native-lib :: ",
                            "CascadeClassifier로 로딩 성공 %s", nativeFileNameString);

    return ret; }
}