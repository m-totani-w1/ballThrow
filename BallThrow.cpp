/* Include */
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "Leap.h"

using namespace Leap;
using namespace std;

/* フィールド生成用 */
#define W 15			/* フィールドの幅 */
#define	D 15			/* フィールドの長さ */
#define G (-9.8)		/* 重力加速度 */
#define V 15.0			/* 球の初速度 */
#define QX 0.0			/* 球の初期位置 x座標 */
#define QY 0.0			/* 球の初期位置 y座標 */
#define QZ (10.0)		/* 球の初期位置 z座標 */
#define KEY_ESCAPE 27		/*  */
#define DEFAULT_WIDTH 800	/* ウィンドウの幅 */
#define DEFAULT_HEIGHT 600	/* ウィンドウの高さ */
#define TSTEP 0.004	/* フレームごとの時間 */

double t = 0; /* 時間 */
double theta = PI/4;	/*　球の発射角Θ　*/
double omega = -PI / 8;/*　球の発射角Ω　*/

int window;
int screen_width = DEFAULT_WIDTH;
int screen_height = DEFAULT_HEIGHT;
int flag = 0;
void idle(void);
Controller controller;
Screen screen;

class SampleListener : public Listener {
        public:
                virtual void onInit(const Controller&);
                virtual void onConnect(const Controller&);
                virtual void onDisconnect(const Controller&);
                virtual void onExit(const Controller&);
                virtual void onFrame(const Controller&);
                virtual void onFocusGained(const Controller&);
				virtual void onFocusLost(const Controller&);
};

void SampleListener::onInit(const Controller& controller) {
	std::cout << "Initialized" << std::endl;
}

void SampleListener::onConnect(const Controller& controller) {
	std::cout << "Connected" << std::endl;
	/* 使用するジェスチャーをセットする */
  	controller.enableGesture(Gesture::TYPE_CIRCLE);
  	controller.enableGesture(Gesture::TYPE_KEY_TAP);
  	controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
  	controller.enableGesture(Gesture::TYPE_SWIPE);
}

void SampleListener::onDisconnect(const Controller& controller) {
	std::cout << "Disconnected" << std::endl;
}

void SampleListener::onExit(const Controller& controller) {
	std::cout << "Exited" << std::endl;
}

void SampleListener::onFrame(const Controller& controller) {
	Frame frame = controller.frame();// 最新のFrameを取得する
	HandList handList = frame.hands();// 全ての手の情報を取得する
	FingerList allFingerList = frame.fingers();// 全ての指の情報を取得する
	GestureList gesturesInFrame = frame.gestures();// 全てのジェスチャーの情報を取得する

	if (gesturesInFrame.count() != 0) {

		//それぞれの個数を表示する
		printf("\n\n\nhands:%d, fingers:%2d, gestures:%d\n",
			handList.count(), allFingerList.count(), gesturesInFrame.count());

		int i, j;



		//手と指の処理
		for (i = 0; i < handList.count(); i++) {
			Hand hand = handList[i];
			Vector handCenter = hand.palmPosition();
			FingerList fingerList = hand.fingers();// handの指の情報を取得する

			//個別の手の情報を出力する
			printf("  hand[%d] (%6.1f,%6.1f,%6.1f), fingers:%d\n",
				i, handCenter.x, handCenter.y, handCenter.z, fingerList.count());
			for (j = 0; j < fingerList.count(); j++) {
				Finger finger = fingerList[j];
				Vector currentPosition = finger.tipPosition();

				//個別の指の情報を出力する
				printf("    finger[%d] (%6.1f,%6.1f,%6.1f)\n",
					j, currentPosition.x, currentPosition.y, currentPosition.z);
			}
		}

		

		//ジェスチャーの処理
		GestureList gestures = frame.gestures();
		for (i = 0; i < gestures.count(); i++) {
			Gesture gesture = gestures[i];

			//ジェスチャーの種類の出力
			printf("  gesture[%d] ", i);
			switch (gesture.type()) {
			case Gesture::TYPE_CIRCLE:
				printf("CIRCLE\n");
				break;
			case Gesture::TYPE_SWIPE:
				printf("SWIPE\n");
				break;
			case Gesture::TYPE_KEY_TAP:
				printf("KEY_TAP\n");
			case Gesture::TYPE_SCREEN_TAP:
				printf("SCREEN_TAP\n");
				/*発射角の計算*/

				/*アニメーション開始*/
				glutIdleFunc(idle);
				flag = 1;
				break;
			default:
				printf("unknown\n");
				break;
			}
		}
	}
}

void SampleListener::onFocusGained(const Controller& controller) {
}

void SampleListener::onFocusLost(const Controller& controller) {
}

SampleListener listener;

void CleanupExit(){
	controller.removeListener(listener);
	exit (1);
}

/* OpenGL */
void idle(void){
        glutPostRedisplay();
}

/* 地面を描く */
void myGround(double height){
	static GLfloat ground[][4] = {
		{ 0.6, 0.6, 0.6, 1.0 },
		{ 0.3, 0.3, 0.3, 1.0 }
	};
	
	int i,j;
	
	glBegin(GL_QUADS);
	glNormal3d(0.0, 1.0, 0.0);
	for(j = -D / 2; j < D / 2; ++j){
		for(i = -W / 2; i < W / 2; ++i){
			glMaterialfv(GL_FRONT, GL_DIFFUSE, ground[(i+j) & 1]); //質感
			glVertex3d((GLdouble)i, height, (GLdouble)j);
			glVertex3d((GLdouble)i, height, (GLdouble)(j+1));
			glVertex3d((GLdouble)(i+1), height, (GLdouble)(j+1));
			glVertex3d((GLdouble)(i+1), height, (GLdouble)j);
		}
	}
	glEnd();
}

/* 画面表示 */
void display(void){
	
	/* 光源の位置 */
	static GLfloat lightpos[] = { 3.0, 4.0, 5.0, 1.0 };
	
	/* 球の色 */
	static GLfloat white[] = { 0.8, 0.8, 0.8, 1.0 };
	
	/* 球の位置 */
	static double px = QX, py = QY, pz = QZ;

	/*球の初速度*/
	static double initVx = 0;
	static double initVy = 0;
	static double initVz = 0;

	
	// pyv y軸の速度 pa 加速度 pe 跳ね上がり係数
	
	/* 物理演算 */
	if(flag) {
		pz = QZ;
		py = QY;
		t = 0;	/*初期時刻の設定*/
		initVx = -V * cos(theta) * sin(omega); /*x方向の初速度*/
		initVy = V * sin(theta);/*y方向の初速度*/
		initVz = -V * cos(theta) * cos(omega);/*z方向の初速度*/

		flag = 0;
	}
	
	double vx = initVx;	/*x方向の速度*/
	double vy = initVy;	/*y方向の速度*/
	double vz = initVz;	/*z方向の速度*/
	
	
	vy = G * t + initVy;
	
	
	px = vx * t + QX;
	py = vy * t + QY;
	pz = vz * t + QZ;
	t += TSTEP;
	
	printf("px:%f,py:%f,pz:%f,time:%f,vy:%f,initVy:%f\n", px, py, pz,t,vy,initVy);


	
	
	/* 画面クリア */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	/* モデルビュー変換行列の初期化 */
	glLoadIdentity();
	
	/* 光源の位置を設定 */
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
	
	/* 視点の移動(物体の方を奥に移す) */
	glTranslated(0.0, -1.5, -D);
	
	/* シーンの描画 */
	myGround(0.0);
	glTranslated(px, py, pz);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
	glutSolidSphere(0.1, 16, 8);
	
	glFlush();
}


void resize(int w, int h){
	/* ウインドウ全体をビューボート */
	glViewport(0, 0, w, h);
	
	/* 透視変換行列の設定 */
	glMatrixMode(GL_PROJECTION);
	
	/* 透視変換行列の初期化 */
	glLoadIdentity();
	gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);
	
	/* モデルビュー変換行列の指定 */
	glMatrixMode(GL_MODELVIEW);
}

void mouse(int button, int state, int x, int y){
	switch(button){
	case GLUT_LEFT_BUTTON:
		if(state == GLUT_DOWN){
			/*アニメーション開始*/
			glutIdleFunc(idle);
			flag = 1;
		}
		//else glutIdleFunc(0);
		break;
	case GLUT_RIGHT_BUTTON:
		if(state == GLUT_DOWN){
			/* コマ送り(１ステップだけ進める) */
			glutPostRedisplay();
		}
		break;
	//defaul:
	//	break;
	}
}

void keyboard(unsigned char key, int x, int y){
	/* ETCで終了 */
	if(key == '\033') 
		exit(0);
}

void init(void){
	/*初期設定*/
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

void ReSizeGLScene(int Width, int Height)
{
	if(Height==0)
		Height=1;
	screen_width = Width;
	screen_height = Height;
}

int main(int argc, char *argv[]){	
	const char *title = "Game";
 	SampleListener listener;
	//SampleListener listener = new SampleListener();
  	Controller controller;


	
	controller.addListener(listener);
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(screen_width, screen_height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow(title);
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	init();
	
	glutMainLoop();
	
	return 0;
}
