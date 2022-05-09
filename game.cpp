/* Include */
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "Leap.h"

using namespace Leap;
using namespace std;

/* フィールド生成用 */
#define W 10                    /* フィールドの幅 */
#define D 10                    /* フィールドの長さ */
#define G (-9.8)                /* 重力加速度 */
#define V 25.0                  /* 球の初速度 */
#define QX 0.0                  /* 球の初期位置 x座標 */
#define QY 1.5                  /* 球の初期位置 y座標 */
#define QZ (10.0)               /* 球の初期位置 z座標 */
#define KEY_ESCAPE 27           /*  */
#define DEFAULT_WIDTH 800       /* ウィンドウの幅 */
#define DEFAULT_HEIGHT 600      /* ウィンドウの高さ */
#define TSTEP 0.01              /* フレームごとの時間 */

int window;
int screen_width = DEFAULT_WIDTH;
int screen_height = DEFAULT_HEIGHT;
int flag = 0;
Controller controller;
Screen screen;

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

        // pyv y軸の速度 pa 加速度 pe 跳ね上がり係数

        /* 物理演算 */
        if(flag) {
                pz = 10;
                flag = 0;
        }
        px += 0.000;
        py += 0.000;
        pz -= 0.01;



        /* 画面クリア */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* モデルビュー変換行列の初期化 */
        glLoadIdentity();

        /* 光源の位置を設定 */
        glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

        /* 視点の移動(物体の方を奥に移す) */
        glTranslated(0.0, -QY, -D);

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
        //      break;
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
        //SampleListener listener;
        //Controller controller;

        //controller.addListener(listener);

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