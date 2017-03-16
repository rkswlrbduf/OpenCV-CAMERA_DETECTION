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

JNIEXPORT jint JNICALL
Java_friend_skplanet_myapplication_MainActivity_detect(JNIEnv *, jobject,
                                                                        jlong addrRgba,
                                                                        jlong addrGray) {

    Mat& img = *(Mat*)addrRgba;
    Mat& gray = *(Mat*)addrGray;

    cvtColor(img, gray, CV_BGR2GRAY);
    // smooth it, otherwise a lot of false circles may be detected
    GaussianBlur( gray, gray, Size(9, 9), 2, 2 );
    vector<Vec3f> circles;
    HoughCircles(gray, circles, CV_HOUGH_GRADIENT,
                 2, gray.rows/4, 200, 100 );
    for( size_t i = 0; i < circles.size(); i++ )
    {
        Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);
        // draw the circle center
        circle( img, center, 3, Scalar(0,255,0), -1, 8, 0 );
        // draw the circle outline
        circle( img, center, radius, Scalar(0,0,255), 3, 8, 0 );
    }

    circles.erase(circles.begin(), circles.end());

    return 0;

    }

}

/*

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
}*/
