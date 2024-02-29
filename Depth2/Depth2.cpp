#include "GL/glut.h"
#include <iostream>
#include <fstream>

using namespace std;

double height = 0.0;
double width = 0.0;
double** matrix;
double rotateX = 0.0;
double rotateY = 0.0;
double scaleFactor = 1.1;

// Функция записи полученной матрицы в файл формата STL
void writeToFile(double** matrix, int height, int width) {
    ofstream f("output.stl");
    f << "solid model\n";
    for (int i = 1; i < (int)height; i++) {
        for (int j = 1; j < (int)width - 1; j++) {
            if ((matrix[i][j] > 0.0) && (matrix[i - 1][j] > 0.0) && (matrix[i - 1][j + 1] > 0.0) && (matrix[i][j + 1] > 0.0)) {
                f << "facet normal 0.0 0.0 0.0\nouter loop\n";
                f << "vertex " << float(i) << " " << float(j) << " " << float(matrix[i][j]) << "\n";
                f << "vertex " << float(i - 1) << " " << float(j) << " " << float(matrix[i - 1][j]) << "\n";
                f << "vertex " << float(i - 1) << " " << float(j + 1) << " " << float(matrix[i - 1][j + 1]) << "\n";
                f << "endloop\nendfacet\n";
                f << "facet normal 0.0 0.0 0.0\nouter loop\n";
                f << "vertex " << float(i) << " " << float(j) << " " << float(matrix[i][j]) << "\n";
                f << "vertex " << float(i) << " " << float(j + 1) << " " << float(matrix[i][j + 1]) << "\n";
                f << "vertex " << float(i - 1) << " " << float(j + 1) << " " << float(matrix[i - 1][j + 1]) << "\n";
                f << "endloop\nendfacet\n";
            }
        }
    }
    f << "\nendsolid model\n";
    f.close();
}

// Функция чтения данных из двоичного файла в матрицу
void ReadFromFile() {
    ifstream file("DepthMap_1.dat", ios::binary);
    if (file.is_open()) {
        file.read(reinterpret_cast<char*>(&height), sizeof(double));
        file.read(reinterpret_cast<char*>(&width), sizeof(double));
        matrix = new double* [static_cast<int>(height)];
        for (int i = 0; i < static_cast<int>(height); i++) {
            matrix[i] = new double[static_cast<int>(width)];
            for (int j = 0; j < static_cast<int>(width); j++) {
                file.read(reinterpret_cast<char*>(&matrix[i][j]), sizeof(double));
            }
        }
        file.close();
        writeToFile(matrix, static_cast<int>(height), static_cast<int>(width));
    }
}

// Функция отображения 3D объекта из матрицы 
void Object() {

    double avg_x = 0;
    double avg_y = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            avg_x += i;
            avg_y += j;
        }
    }
    avg_x /= height * width;
    avg_y /= height * width;
    glTranslatef(-avg_x * scaleFactor, -avg_y * scaleFactor, 0);
    glScalef(scaleFactor, scaleFactor, scaleFactor);

    for (int i = 1; i < height; i++) {
        for (int j = 0; j < width - 1; j++) {
            if ((matrix[i][j] > 0.0) && (matrix[i - 1][j] > 0.0) && (matrix[i - 1][j + 1] > 0.0) && (matrix[i][j + 1] > 0.0)) {
                glBegin(GL_TRIANGLE_FAN);
                glColor3f(0.4f, 0.4f, 0.5f);
                glVertex3d(i, j, matrix[i][j]);
                glVertex3d(i - 1, j, matrix[i - 1][j]);
                glVertex3d(i - 1, j + 1, matrix[i - 1][j + 1]);
                glEnd();
                glBegin(GL_TRIANGLE_FAN);
                glColor3f(0.4f, 0.4f, 0.5f);
                glVertex3d(i, j, matrix[i][j]);
                glVertex3d(i, j + 1, matrix[i][j + 1]);
                glVertex3d(i - 1, j + 1, matrix[i - 1][j + 1]);
                glEnd();
            }
        }
    }
    glFlush();
}


// Функция отрисовки оболочки 3D объекта
void Draw() {
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 700 / 500, 0.000001, 100000);
    gluLookAt(0, 0, 1000, 0, 0, 0, 0, 1, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotated(rotateX, 1.0, 0.0, 0.0);
    glRotated(rotateY, 0.0, 1.0, 0.0); 
    Object();
}

// Функция обработки нажатия клавиш для вращения и масштабирования объекта
void keyPressed(unsigned char key, int x, int y) {
    switch (key) {
    case 'w':
        rotateX += 5.0;
        break;
    case 's':
        rotateX -= 5.0;
        break;
    case 'a':
        rotateY -= 5.0;
        break;
    case 'd':
        rotateY += 5.0;
        break;
    case '=':
        scaleFactor += 0.1;
        break;
    case '-':
        scaleFactor -= 0.1;
        break;
    default:
        break;
    }

    glutPostRedisplay(); 
}

// Главная функция программы
int main(int argc, char** argv) {
    ReadFromFile();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(600, 600);
    glutCreateWindow("3D model");
    glutKeyboardFunc(keyPressed);   
    glutDisplayFunc(Draw); 
    glutMainLoop();
}

