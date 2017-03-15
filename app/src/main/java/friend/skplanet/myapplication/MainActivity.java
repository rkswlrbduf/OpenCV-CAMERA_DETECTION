package friend.skplanet.myapplication;

import android.app.Activity;
import android.content.DialogInterface;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.AlertDialog;
import android.util.Log;
import android.view.SurfaceView;
import android.view.WindowManager;
import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.android.Utils;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import android.os.Message;
import android.widget.ImageView;


public class MainActivity extends Activity implements CameraBridgeViewBase.CvCameraViewListener2 {

    ImageView DrawArea;

    private Mat img_input; //Image Input Memory Analyzer tools
    private Mat img_result; //Image Result Memory Analyzer tools
    private static final String TAG = "opencv"; // TAG = opencv
    private CameraBridgeViewBase mOpenCvCameraView; // Define CameraBridgeViewBase

    public native int convertNativeLib(long matAddrInput, long matAddrResult); // native-lib 에서 사용되는 함수이름과 매개변수 long형 matAddrInput과 long형 matAddrResult

    static final int PERMISSION_REQUEST_CODE = 1; // set PERMISSION_REQUEST_CODE to 1
    String[] PERMISSIONS  = {"android.permission.CAMERA"}; // PERMISSIONS = android.permission.CAMERA

    // 라이브러리 로드
    static {
        System.loadLibrary("opencv_java3"); // opencv_java3 라이브러리 로드
        System.loadLibrary("native-lib"); // native-lib 라이브러리 로드
    }

    // String을 입력받아서 퍼미션 허가 여부 확인(퍼미션 허가시 True, 퍼미션 미허가시 False 반환)
    private boolean hasPermissions(String[] permissions) {
        int ret = 0;

        for (String perms : permissions){ // 스트링 배열에 있는 퍼미션들의 허가 상태 여부 확인
            ret = checkCallingOrSelfPermission(perms);
            if (!(ret == PackageManager.PERMISSION_GRANTED)){ // 퍼미션 허가 안된 경우
                return false;
            }
        }
        return true; // 모든 퍼미션이 허가된 경우
    }

    // 마시멜로( API 23 )이상에서 런타임 퍼미션(Runtime Permission) 요청 onCreate 실행시 시작됨
    private void requestNecessaryPermissions(String[] permissions) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            requestPermissions(permissions, PERMISSION_REQUEST_CODE); // PERMISSION_REQUEST_CODE로 퍼미션 요청함
        }
    }

    // BaseLoaderCallback을 불러서 opencv 초기 세팅
    private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            switch (status) {
                case LoaderCallbackInterface.SUCCESS:
                {
                    mOpenCvCameraView.enableView(); // mOpenCvCameraView Enable 상태로 전환
                } break;
                default:
                {
                    super.onManagerConnected(status);
                } break;
            }
        }
    };

    // 퍼미션 허가 Result 과정( 퍼미션 다이얼로그 팝업 )
    @Override
    public void onRequestPermissionsResult(int permsRequestCode, String[] permissions, int[] grantResults){
        switch(permsRequestCode){

            case PERMISSION_REQUEST_CODE:
                if (grantResults.length > 0) {
                    boolean camreaAccepted = grantResults[0] == PackageManager.PERMISSION_GRANTED;

                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {

                        if (!camreaAccepted  )
                        {
                            showDialogforPermission("앱을 실행하려면 퍼미션을 허가하셔야합니다."); // 다이얼로그를 띄움
                            return;
                        }else
                        {
                            //이미 사용자에게 퍼미션 허가를 받음.
                        }
                    }
                }
                break;
        }
    }

    // 퍼미션 다이얼로그
    private void showDialogforPermission(String msg) {

        final AlertDialog.Builder myDialog = new AlertDialog.Builder(  MainActivity.this);
        myDialog.setTitle("알림");
        myDialog.setMessage(msg);
        myDialog.setCancelable(false);
        myDialog.setPositiveButton("예", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface arg0, int arg1) {
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                    requestPermissions(PERMISSIONS, PERMISSION_REQUEST_CODE);
                }

            }
        });
        myDialog.setNegativeButton("아니오", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface arg0, int arg1) {
                finish();
            }
        });
        myDialog.show();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        DrawArea = (ImageView) findViewById(R.id.imageview);

        getWindow().addFlags( WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON); // 화면 꺼짐 방지기능 활성화

        if (!hasPermissions(PERMISSIONS)) { //퍼미션 허가를 했었는지 여부를 확인
            requestNecessaryPermissions(PERMISSIONS);//퍼미션 허가안되어 있다면 사용자에게 요청
        } else {
            //이미 사용자에게 퍼미션 허가를 받음.
        }

        mOpenCvCameraView = (CameraBridgeViewBase)findViewById(R.id.activity_surface_view);
        mOpenCvCameraView.setVisibility(SurfaceView.VISIBLE); //SurfaceView의 일반 View 와 다른점은 일반 View는 한번 표출되면 생성된 모습 그대로이지만 SurfaceView는 화면의 변화가 실시간이다.
        mOpenCvCameraView.setCvCameraViewListener(this);
        mOpenCvCameraView.setCameraIndex(0); // front-camera(1),  back-camera(0)
        mLoaderCallback.onManagerConnected(LoaderCallbackInterface.SUCCESS); // openCV 시작
    }

    // Pause 동작
    @Override
    public void onPause() {
        super.onPause();
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
    }

    // Resume 동작
    @Override
    public void onResume() {
        super.onResume();

        if (!OpenCVLoader.initDebug()) {
            Log.d(TAG, "onResume :: Internal OpenCV library not found.");
            OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_3_1_0, this, mLoaderCallback); // 매개변수 Version, AppContext, Callback
        } else {
            Log.d(TAG, "onResum :: OpenCV library found inside package. Using it!");
            mLoaderCallback.onManagerConnected(LoaderCallbackInterface.SUCCESS); // openCV 시작
        }
    }

    // Destroy 동작
    public void onDestroy() {
        super.onDestroy();

        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
    }

    // enableView 다음에 실행된다. camera preview 시작시 실행 ( 매개변수 width, height -> Frame의 width, height )
    @Override
    public void onCameraViewStarted(int width, int height) {

    }

    // camera preview 정지시 실행된다.
    @Override
    public void onCameraViewStopped() {

    }

    // 아래 함수를 거친 프레임이 출력된다.
    @Override
    public Mat onCameraFrame(CameraBridgeViewBase.CvCameraViewFrame inputFrame) {

        if (inputFrame.rgba() != null) // inputFrame 포맷 형식이 rgba인지 확인한다.
        {
            img_input = inputFrame.rgba();

            if ( img_result != null ) img_result.release();
            img_result = new Mat( inputFrame.rgba().rows(), inputFrame.rgba().cols(), CvType.CV_8UC1);

            convertNativeLib(img_input.getNativeObjAddr(), img_result.getNativeObjAddr());
        }

        new Thread()

        {

            public void run()

            {

                Message msg = handler.obtainMessage();

                handler.sendMessage(msg);

            }

        }.start();

        return img_result;
    }

    final Handler handler = new Handler()

    {

        public void handleMessage(Message msg)

        {

            Bitmap bmp = Bitmap.createBitmap(img_result.cols(), img_result.rows(),
                    Bitmap.Config.ARGB_8888);
            Utils.matToBitmap(img_result, bmp);
            DrawArea.setImageBitmap(bmp);

        }

    };

}