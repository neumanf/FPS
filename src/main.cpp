#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <SOIL/SOIL.h>
#include <math.h>

#define SKY_FRONT 0
#define SKY_RIGHT 1
#define SKY_LEFT 2
#define SKY_BACK 3
#define SKY_UP 4
#define SKY_DOWN 5

#define FPS 60
#define TO_RADIANS 3.14 / 180.0

#define LOGLENGTH 6
#define LOGQUANTITY 6
#define ROOF0 4.8, 5, 7
#define ROOF1 1.2, 1.5, 3.5
#define ROOF2 7.7, 1.5, 3.5
#define ROOF3 7.7, 8.4, 3.5
#define ROOF4 1.2, 8.4, 3.5

#define BLADE0 -1, -.08, 1.2
#define BLADE1 1, -.08, 1.2
#define BLADE2 1, -.08, 5.5
#define BLADE3 -1, -.08, 5.5
#define BLADE4 -1, .08, 1.2
#define BLADE5 1, .08, 1.2
#define BLADE6 1, .08, 5.5
#define BLADE7 -1, .08, 5.5

// Vetor de texturas dos objetos
GLuint texture[15];

// Veetor de texturas do céu
int skybox[6];

// Dimensões da janela
const int width = 1280; // 16 * 50;
const int height = 720; // 9 * 50;

float pitch = 0.0, yaw = 0.0;
float camX = 0.0, camZ = 0.0;

// Ângulo de rotação das hélices do moinho
float millAngle = 0;

// Estrutura da câmera, avançar, voltar, ir para esquerda e para direita
struct Motion {
    bool Forward, Backward, Left, Right;
};

// Inicializa valores como False para não ter um movimento antes do
// usuario passar eles
Motion motion = {false, false, false, false};

void init() {
    // Retira o cursor do mouse
    glutSetCursor(GLUT_CURSOR_NONE);
    // Habilita Teste de profundidade
    // Se o teste falhar, o fragmento será descartado. Se o teste for aprovado,
    // o buffer de profundidade será atualizado com a profundidade de saída do
    // fragmento
    glEnable(GL_DEPTH_TEST);
    // Especifica o valor de profundidade para comparações profundas
    // Com o LEQUAL ele permite o valor de entrada se for >= ao valor
    // profundidade
    glDepthFunc(GL_LEQUAL);
    // Move o ponteiro do mouse para as coordenadas do centro da janela
    glutWarpPointer(width / 2, height / 2);
}

// Desenha feno
void drawHay(GLUquadricObj *cylinder) {
    // Faz com que a matriz no topo da pilha seja igual
    glPushMatrix();
    // Dimensões
    // slices ---- Especifica o número de subdivisões ao redor do eixo z
    // stacks ----Especifica o número de subdivisões ao longo do eixo z
    const float BASE = 1.5, TOP = 1.5, HEIGHT = 2.5, SLICES = 20.0,
                STACKS = 20.0;
    // Habilita a textura
    glBindTexture(GL_TEXTURE_2D, texture[9]);
    // Desenha o cilindro
    gluCylinder(cylinder, BASE, TOP, HEIGHT, SLICES, STACKS);
    // Rotaciona 180 graus em torno de x
    glRotatef(180, 1, 0, 0);
    // Desenha um disco
    gluDisk(cylinder, 0.0f, BASE, SLICES, 1);
    // Rotaciona 180 graus em torno de x
    glRotatef(180, 1, 0, 0);
    // Multiplica a matriz atual por uma de translação
    // Translada em Z para a altura do cilindro
    glTranslatef(0.0f, 0.0f, HEIGHT);
    // Adiciona um disco
    gluDisk(cylinder, 0.0f, TOP, SLICES, 1);
    // Insere ele na parte inferior do cilindro
    glTranslatef(0.0f, 0.0f, -HEIGHT);
    // exibe a pilha da matriz atual substituindo a matriz atual pela que esta
    // abaixo dela na pilha
    glPopMatrix();
}

// Desenha moinho
void drawMill(GLUquadricObj *cylinder, GLUquadricObj *disk,
              GLfloat material_specular[], GLfloat material1_specular[]) {
    // Faz com que a matriz no topo da pilha seja igual
    glPushMatrix();
    // Moinho
    // Translada -5 em x e y
    glTranslatef(-5, -5, 0);
    // Habilita a textura do corpo do moinho
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    // Desenha um cilindro
    gluCylinder(cylinder, 2.35, 1.7, 7, 50, 50);
    // Translada em Z
    glTranslatef(0, 0, 6.9);
    // Teto do moinho
    // Habilita a textura
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    // Desenha um cilindro
    gluCylinder(cylinder, 1.8, 0, 2, 50, 50);
    // Especifica os parâmetros do material para o modelo de iluminação
    // Faces a serem atualizadas
    // Iluminação especular. As superficies brilhantes mudam de aparencia
    // de acordo com a posicao de visualizacao, da efeito de suavidade
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material1_specular);
    // Textura de madeira
    glBindTexture(GL_TEXTURE_2D, texture[4]);
    // Transalada para baixo e para fora
    glTranslatef(2, 2, -1);
    // Rotaciona em 90 em x
    glRotatef(90, 1, 0, 0);
    // Em -45 em y
    glRotatef(-45, 0, 1, 0);

    // Controle de rotação das elices
    glRotatef(millAngle, 0, 0, 1);
    // Desenha o cilindro que segura as elices
    gluCylinder(cylinder, .15, .15, 1.5, 10, 10);
    // Põe um disco
    gluDisk(disk, 0, .15, 10, 10);
    // Translada pra cima para por as elicas
    glTranslatef(0, 0, .2);
    glPushMatrix();
    // Rotaciona 90 em x
    glRotatef(90, 1, 0, 0);

    // 4 Lâminas
    for (int i = 0; i < 4; i++) {
        // Cilindro que segura a lamina
        gluCylinder(cylinder, .1, 0, 3, 10, 10);
        // Inclinar
        glPushMatrix();
        // Rotaciona 15 em Z, curva na palheta
        glRotatef(15, 0, 0, 1);
        // polígonos voltados para frente e para trás
        glFrontFace(GL_CW);

        glBegin(GL_QUADS);
        // Desenha a lamina
        glNormal3f(0, 1, 0);
        glTexCoord2f(0, 0);
        glVertex3f(BLADE0);
        glTexCoord2f(1, 0);
        glVertex3f(BLADE1);
        glTexCoord2f(1, 3);
        glVertex3f(BLADE2);
        glTexCoord2f(0, 3);
        glVertex3f(BLADE3);

        glNormal3f(0, -1, 0);
        glTexCoord2f(0, 0);
        glVertex3f(BLADE7);
        glTexCoord2f(1, 0);
        glVertex3f(BLADE6);
        glTexCoord2f(1, 3);
        glVertex3f(BLADE5);
        glTexCoord2f(0, 3);
        glVertex3f(BLADE4);

        glNormal3f(1, 0, 0);
        glTexCoord2f(0, 0);
        glVertex3f(BLADE0);
        glTexCoord2f(1, 0);
        glVertex3f(BLADE3);
        glTexCoord2f(1, 3);
        glVertex3f(BLADE7);
        glTexCoord2f(0, 3);
        glVertex3f(BLADE4);

        glNormal3f(-1, 0, 0);
        glTexCoord2f(0, 0);
        glVertex3f(BLADE1);
        glTexCoord2f(1, 0);
        glVertex3f(BLADE5);
        glTexCoord2f(1, 3);
        glVertex3f(BLADE6);
        glTexCoord2f(0, 3);
        glVertex3f(BLADE2);

        glNormal3f(0, 0, 1);
        glTexCoord2f(0, 0);
        glVertex3f(BLADE0);
        glTexCoord2f(1, 0);
        glVertex3f(BLADE4);
        glTexCoord2f(1, 3);
        glVertex3f(BLADE5);
        glTexCoord2f(0, 3);
        glVertex3f(BLADE1);

        glNormal3f(0, 0, -1);
        glTexCoord2f(0, 0);
        glVertex3f(BLADE3);
        glTexCoord2f(1, 0);
        glVertex3f(BLADE2);
        glTexCoord2f(1, 3);
        glVertex3f(BLADE6);
        glTexCoord2f(0, 3);
        glVertex3f(BLADE7);
        glEnd();
        // polígonos voltados para frente e para trás
        glFrontFace(GL_CCW);

        glPopMatrix();

        glRotatef(90, 0, 1, 0);
    }

    glPopMatrix();
    glPopMatrix();

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_specular);
}

// Desenha a casa
void drawHouse(GLUquadricObj *cylinder, GLUquadricObj *disk,
               GLfloat material_specular[], GLfloat material1_specular[]) {
    // Casa
    glTranslatef(2, 0.5, 0);
    // Define iluminação especular
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material1_specular);

    // Parede esquerda
    glPushMatrix();
    for (int i = 0; i < LOGQUANTITY + 1; i++) {
        // Textura da parte fora do tronco
        glBindTexture(GL_TEXTURE_2D, texture[1]);
        // Cilindro (tronco de madeira)
        gluCylinder(cylinder, .3, .3, LOGLENGTH, 30, 30);
        // Textura de dentro do tronco
        glBindTexture(GL_TEXTURE_2D, texture[2]);
        // Insere o disco para tapar o tronco
        gluDisk(disk, 0, .3, 30, 30);
        // Translada para mais
        glTranslatef(0, 0, LOGLENGTH);
        // Poe o outro disco
        gluDisk(disk, 0, .3, 30, 30);
        // Volta para a posição
        glTranslatef(0, 0, -LOGLENGTH);
        glTranslatef(0, -.6, 0);
    }
    glPopMatrix();
    glPushMatrix();

    // Parede direita
    glTranslatef(5, 0, 0);
    for (int i = 0; i < LOGQUANTITY + 1; i++) {
        glBindTexture(GL_TEXTURE_2D, texture[1]);
        gluCylinder(cylinder, .3, .3, LOGLENGTH, 30, 30);
        glBindTexture(GL_TEXTURE_2D, texture[2]);
        gluDisk(disk, 0, .3, 30, 30);
        glTranslatef(0, 0, LOGLENGTH);
        gluDisk(disk, 0, .3, 30, 30);
        glTranslatef(0, 0, -LOGLENGTH);
        glTranslatef(0, -.6, 0);
    }
    glPopMatrix();
    glTranslatef(-.5, 0, .5);
    glTranslatef(0, -.3, 0);
    glRotatef(90, 0, 1, 0);
    glPushMatrix();

    // Parede frontal
    // Desenhando o tronco pela metade para a parte central vazia para ser a
    // porta
    for (int i = 0; i < LOGQUANTITY; i++) {
        float LOG_LENGTH = i == 0 ? 6 : 2.1;
        // Tronco
        glBindTexture(GL_TEXTURE_2D, texture[1]);
        gluCylinder(cylinder, .3, .3, LOG_LENGTH, 30, 30);
        glBindTexture(GL_TEXTURE_2D, texture[2]);
        gluDisk(disk, 0, .3, 30, 30);
        glTranslatef(0, 0, LOG_LENGTH);
        gluDisk(disk, 0, .3, 30, 30);
        glTranslatef(0, 0, -LOG_LENGTH);
        glTranslatef(0, -.6, 0);
    }

    glTranslatef(0, 3, 3.9);
    // A outra parte do tronco
    for (int i = 0; i < LOGQUANTITY; i++) {
        if (i == 0)
            continue;

        float LOG_LENGTH = 2.1;
        // Tronco
        glBindTexture(GL_TEXTURE_2D, texture[1]);
        gluCylinder(cylinder, .3, .3, LOG_LENGTH, 30, 30);

        glBindTexture(GL_TEXTURE_2D, texture[2]);
        gluDisk(disk, 0, .3, 30, 30);
        glTranslatef(0, 0, LOG_LENGTH);
        gluDisk(disk, 0, .3, 30, 30);
        glTranslatef(0, 0, -LOG_LENGTH);
        glTranslatef(0, -.6, 0);
    }
    glPopMatrix();
    glPushMatrix();

    // Parede traseira
    glTranslatef(-5, 0, 0);
    for (int i = 0; i < LOGQUANTITY; i++) {
        glBindTexture(GL_TEXTURE_2D, texture[1]);
        gluCylinder(cylinder, .3, .3, LOGLENGTH, 30, 30);
        glBindTexture(GL_TEXTURE_2D, texture[2]);
        gluDisk(disk, 0, .3, 30, 30);
        glTranslatef(0, 0, LOGLENGTH);
        gluDisk(disk, 0, .3, 30, 30);
        glTranslatef(0, 0, -LOGLENGTH);
        glTranslatef(0, -.6, 0);
    }
    glPopMatrix();
    glPopMatrix();
    // Seta iluminação especular no material
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_specular);

    // Teto
    glTranslatef(-7, -3, 8);
    glRotatef(-90, 1, 0, 0);
    glFrontFace(GL_CW);
    // Textura do teto
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    // Teto em formato de triangulo
    glBegin(GL_TRIANGLES);

    // Frente
    glNormal3f(1, 0, -1);
    glTexCoord2f(0, 0);
    glVertex3f(ROOF1);
    glTexCoord2f(1, 0);
    glVertex3f(ROOF2);
    glTexCoord2f(.5, 1);
    glVertex3f(ROOF0);

    // Direita
    glNormal3f(0, -1, 1);
    glTexCoord2f(0, 0);
    glVertex3f(ROOF2);
    glTexCoord2f(1, 0);
    glVertex3f(ROOF3);
    glTexCoord2f(.5, 1);
    glVertex3f(ROOF0);

    // Traseira
    glNormal3f(-1, 0, -1);
    glTexCoord2f(0, 0);
    glVertex3f(ROOF3);
    glTexCoord2f(1, 0);
    glVertex3f(ROOF4);
    glTexCoord2f(.5, 1);
    glVertex3f(ROOF0);

    // Esquerda
    glNormal3f(0, 1, 1);
    glTexCoord2f(0, 0);
    glVertex3f(ROOF4);
    glTexCoord2f(1, 0);
    glVertex3f(ROOF1);
    glTexCoord2f(.5, 1);
    glVertex3f(ROOF0);
    glEnd();
    glFrontFace(GL_CCW);

    // Parte de baixo do teto
    glBindTexture(GL_TEXTURE_2D, texture[4]);
    // Repete a textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Cria um objeto do tipo quadrado
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(ROOF4);
    glTexCoord2f(3, 0);
    glVertex3f(ROOF3);
    glTexCoord2f(3, 3);
    glVertex3f(ROOF2);
    glTexCoord2f(0, 3);
    glVertex3f(ROOF1);
    glEnd();

    glDisable(GL_LIGHT1);
    glDisable(GL_LIGHT2);

    // Chaminé
    glPushMatrix();
    glTranslatef(3.3, 5, 5);
    glRotatef(45, 0, 0, 1);
    // Textura da chamine
    glBindTexture(GL_TEXTURE_2D, texture[5]);
    // Seta um cilindro, parte de fora
    gluCylinder(cylinder, .5, .5, 2, 4, 4);
    // Parte de dentro
    gluCylinder(cylinder, .3, .3, 2, 4, 4);
    glTranslatef(0, 0, 2);
    // Parte inferior da chaminé
    gluDisk(disk, .3, .5, 4, 4);
    glPopMatrix();
}

// Desenha a grama
void drawFloor() {
    // Textura da grama
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    // Quadrilatero
    glBegin(GL_QUADS);

    glTexCoord2f(0.0, 0.0);
    glVertex3f(-50.0, -5.0, -50.0);
    glTexCoord2f(25.0, 0.0);
    glVertex3f(50.0, -5.0, -50.0);
    glTexCoord2f(25.0, 25.0);
    glVertex3f(50.0, -5.0, 50.0);
    glTexCoord2f(0.0, 25.0);
    glVertex3f(-50.0, -5.0, 50.0);

    glEnd();
    // Repete a textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

// Desenha o poste
void drawPole(GLUquadricObj *cylinder, GLUquadricObj *sphere,
              GLfloat material_emission[]) {
    glPushMatrix();
    glTranslatef(-5, -5, 0);
    glRotatef(-90, 1, 0, 0);
    // Textura do corpo do poste
    // Multiplos cilindros na base do poste
    glBindTexture(GL_TEXTURE_2D, texture[8]);

    gluCylinder(cylinder, .2, .15, .25, 20, 20);
    glTranslatef(0, 0, .25);
    gluCylinder(cylinder, .15, .15, 3, 20, 20);
    glTranslatef(0, 0, 3);
    gluCylinder(cylinder, .15, .2, .1, 20, 20);
    gluCylinder(cylinder, .03, .03, .2, 10, 5);
    glPopMatrix();

    glTranslatef(-5, -1.5, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    GLfloat material_emission9[] = {.7, .6, .3, 1};
    // Emite luz
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material_emission9);
    gluSphere(sphere, .2, 40, 40);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material_emission);
    // Desabilita ilumininação
    glDisable(GL_LIGHTING);
    glColor4f(.7, .7, .7, .2);
    // Desenha a lampada
    gluSphere(sphere, .32, 40, 40);
    gluSphere(sphere, .35, 40, 40);
    glColor4f(1, 1, 1, 1);
    // Habilita a luz
    glEnable(GL_LIGHTING);
}

// Desenha o céu
void drawSkybox(double D) {
    float white[] = {1, 1, 1, 1};
    glColor3fv(white);

    // Lados
    glBindTexture(GL_TEXTURE_2D, skybox[SKY_RIGHT]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(-D, -D, -D);
    glTexCoord2f(1, 0);
    glVertex3f(+D, -D, -D);
    glTexCoord2f(1, 1);
    glVertex3f(+D, +D, -D);
    glTexCoord2f(0, 1);
    glVertex3f(-D, +D, -D);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, skybox[SKY_FRONT]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(+D, -D, -D);
    glTexCoord2f(1, 0);
    glVertex3f(+D, -D, +D);
    glTexCoord2f(1, 1);
    glVertex3f(+D, +D, +D);
    glTexCoord2f(0, 1);
    glVertex3f(+D, +D, -D);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, skybox[SKY_LEFT]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(+D, -D, +D);
    glTexCoord2f(1, 0);
    glVertex3f(-D, -D, +D);
    glTexCoord2f(1, 1);
    glVertex3f(-D, +D, +D);
    glTexCoord2f(0, 1);
    glVertex3f(+D, +D, +D);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, skybox[SKY_BACK]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(-D, -D, +D);
    glTexCoord2f(1, 0);
    glVertex3f(-D, -D, -D);
    glTexCoord2f(1, 1);
    glVertex3f(-D, +D, -D);
    glTexCoord2f(0, 1);
    glVertex3f(-D, +D, +D);
    glEnd();

    // Topo e fundo
    glBindTexture(GL_TEXTURE_2D, skybox[SKY_UP]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(-D, +D, -D);
    glTexCoord2f(1, 0);
    glVertex3f(+D, +D, -D);
    glTexCoord2f(1, 1);
    glVertex3f(+D, +D, +D);
    glTexCoord2f(0, 1);
    glVertex3f(-D, +D, +D);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, skybox[SKY_DOWN]);
    glBegin(GL_QUADS);
    glTexCoord2f(1, 1);
    glVertex3f(+D, -D, -D);
    glTexCoord2f(0, 1);
    glVertex3f(-D, -D, -D);
    glTexCoord2f(0, 0);
    glVertex3f(-D, -D, +D);
    glTexCoord2f(1, 0);
    glVertex3f(+D, -D, +D);
    glEnd();
}

void draw() {
    glEnable(GL_TEXTURE_2D);

    // Textura do céu
    drawSkybox(35);

    // Iluminação global
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    GLfloat ambient0[] = {0.4, 0.4, 0.4, 1};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient0);

    // Lua
    GLfloat pos1[] = {1, -1, -1, 0};
    GLfloat light1_diffuse[] = {.7, .7, .7, 1};
    GLfloat light1_specular[] = {.2, .2, .2, 1};
    glLightfv(GL_LIGHT1, GL_POSITION, pos1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);

    // Lâmpada
    GLfloat light2_position[] = {-5, 5, 3.64, 1};
    GLfloat light2_diffuse[] = {.9, .7, .5, 1};
    GLfloat light2_specular[] = {.3, .2, .1, 1};
    GLfloat light2_linear_attenuation[] = {.015};
    glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_diffuse);
    glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
    glLightfv(GL_LIGHT2, GL_SPECULAR, light2_specular);
    glLightfv(GL_LIGHT2, GL_LINEAR_ATTENUATION, light2_linear_attenuation);

    GLfloat material_emission[] = {0, 0, 0, 1};
    GLfloat material_diffuse[] = {.8, .8, .8, 1.0};
    GLfloat material_ambient[] = {.5, .5, .5, 1.0};
    GLfloat material_specular[] = {.1, .1, .1, 1.0};
    GLfloat material1_specular[] = {0, 0, 0, 1.0};
    // Difusa+ambiente+especular = Phong Reflexion
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material_emission);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_specular);

    // gluNewQuadric cria e retorna um ponteiro para um novo objeto Quadrics.
    // Este objeto deve ser referenciado ao chamar funções de renderização e
    // controle de quádricas.
    GLUquadricObj *cylinder;
    cylinder = gluNewQuadric();
    GLUquadricObj *disk;
    disk = gluNewQuadric();
    GLUquadricObj *sphere;
    sphere = gluNewQuadric();

    // Diz se o objeto Quadric deve ser texturado
    gluQuadricTexture(cylinder, GLU_TRUE);
    // Uma normal é gerada para cada vértice
    // Phong Shading
    gluQuadricNormals(cylinder, GLU_SMOOTH);
    // Especifica o estilo de desenho. Desenha com polígonos
    gluQuadricDrawStyle(cylinder, GLU_FILL);
    gluQuadricTexture(disk, GLU_TRUE);
    gluQuadricNormals(disk, GLU_SMOOTH);
    gluQuadricDrawStyle(disk, GLU_FILL);

    // Grama
    drawFloor();

    // Poste de iluminação
    drawPole(cylinder, sphere, material_emission);

    // Casa
    drawHouse(cylinder, disk, material_specular, material1_specular);

    // Moinho
    drawMill(cylinder, disk, material_specular, material1_specular);

    // Fenos
    glTranslatef(3, -4, -1);
    drawHay(cylinder);
    glTranslatef(-1, -3, 0);
    drawHay(cylinder);
    glTranslatef(3, .5, 0);
    drawHay(cylinder);
    glTranslatef(-2, 0, 2);
    drawHay(cylinder);

    gluDeleteQuadric(cylinder);
    gluDeleteQuadric(disk);
    gluDeleteQuadric(sphere);

    glDisable(GL_TEXTURE_2D);
}

/*
Caso alguma dessas direções seja verdadeira, indica que a tecla que corresponde
a essa direção foi pressionada e portanto as posições da câmera nos eixos X e Z,
expressas pelas variáveis camX e camZ, devem ser alteradas. Por fim, são
realizadas as rotações e translações pertinentes
*/
void camera() {
    const float speed = 5.0;

    if (motion.Forward) {
        camX += cos((yaw + 90) * TO_RADIANS) / speed;
        camZ -= sin((yaw + 90) * TO_RADIANS) / speed;
    }
    if (motion.Backward) {
        camX += cos((yaw + 90 + 180) * TO_RADIANS) / speed;
        camZ -= sin((yaw + 90 + 180) * TO_RADIANS) / speed;
    }
    if (motion.Left) {
        camX += cos((yaw + 90 + 90) * TO_RADIANS) / speed;
        camZ -= sin((yaw + 90 + 90) * TO_RADIANS) / speed;
    }
    if (motion.Right) {
        camX += cos((yaw + 90 - 90) * TO_RADIANS) / speed;
        camZ -= sin((yaw + 90 - 90) * TO_RADIANS) / speed;
    }

    if (pitch >= 70)
        pitch = 70;
    if (pitch <= -60)
        pitch = -60;

    glRotatef(-pitch, 1.0, 0.0, 0.0); // Rotaciona ao longo do eixo X
    glRotatef(-yaw, 0.0, 1.0, 0.0);   // Rotaciona ao longo do eixo Y

    glTranslatef(-camX, 0.0, -camZ);
}

/*
Determina o que será mostrado na janela. Nela são limpados os buffers,
redefinida a matriz atual de volta ao seu estado padrão,  chamadas as
funções camera() e draw() e, por fim, trocados os conteúdos dos buffers de trás
e frente.
*/
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    camera();
    draw();

    glutSwapBuffers();
}
/*
Garante que ao modificar o tamanho da janela, o conteúdo seja exibido de forma
correta, para tanto, modifica a perspectiva da cena para se adequar aos novos
valores de altura e largura
*/
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 16.0 / 9.0, 1, 75);
    glMatrixMode(GL_MODELVIEW);
}

// Chama a função display periodicamente de acordo com a taxa de FPS
void timer(int) {
    glutPostRedisplay();
    glutWarpPointer(width / 2, height / 2);
    glutTimerFunc(1000 / FPS, timer, 0);
}

// Controla a velocidade do movimento da câmera
void passive_motion(int x, int y) {
    int dev_x, dev_y;
    dev_x = (width / 2) - x;
    dev_y = (height / 2) - y;

    const float look_speed = 50.0;
    yaw += (float)dev_x / look_speed;
    pitch += (float)dev_y / look_speed;
}

// Controla o movimento da câmera quando uma tecla é pressionada
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'q':
        exit(0);
    case 'W':
    case 'w':
        motion.Forward = true;
        break;
    case 'A':
    case 'a':
        motion.Left = true;
        break;
    case 'S':
    case 's':
        motion.Backward = true;
        break;
    case 'D':
    case 'd':
        motion.Right = true;
        break;
    }
}

// Controla o movimento da câmera quando uma tecla é solta
void keyboard_up(unsigned char key, int x, int y) {
    switch (key) {
    case 'W':
    case 'w':
        motion.Forward = false;
        break;
    case 'A':
    case 'a':
        motion.Left = false;
        break;
    case 'S':
    case 's':
        motion.Backward = false;
        break;
    case 'D':
    case 'd':
        motion.Right = false;
        break;
    }
}

/*
Recebe input de teclas especiais do teclado
como F1, setas, page up, etc.
*/
void specialkeyboard(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        millAngle--;
        break;
    case GLUT_KEY_RIGHT:
        millAngle++;
        break;
    }
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    // Configura o modo de display, sendo Double, RGB ou Depth
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    // Dimensões da janela
    glutInitWindowSize(width, height);
    // Nome da janela
    glutCreateWindow("Farm Scene");

    init();
    glutDisplayFunc(display);
    // Redimensiona o ambiente conforme a janela é redimensionada
    glutReshapeFunc(reshape);
    // Seta velocidade de movimento
    glutPassiveMotionFunc(passive_motion);
    glutTimerFunc(0, timer, 0);
    // Captura de teclas de movimento
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboard_up);
    glutSpecialFunc(specialkeyboard);

    // Carrega as texturas
    texture[0] = SOIL_load_OGL_texture(
        "assets/images/biggrass.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB |
            SOIL_FLAG_COMPRESS_TO_DXT);
    texture[1] = SOIL_load_OGL_texture(
        "assets/images/log1.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB |
            SOIL_FLAG_COMPRESS_TO_DXT);
    texture[2] = SOIL_load_OGL_texture(
        "assets/images/cut.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB |
            SOIL_FLAG_COMPRESS_TO_DXT);
    texture[3] = SOIL_load_OGL_texture(
        "assets/images/roof.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB |
            SOIL_FLAG_COMPRESS_TO_DXT);
    texture[4] = SOIL_load_OGL_texture(
        "assets/images/planks.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB |
            SOIL_FLAG_COMPRESS_TO_DXT);
    texture[5] = SOIL_load_OGL_texture(
        "assets/images/bricks.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB |
            SOIL_FLAG_COMPRESS_TO_DXT);
    texture[6] = SOIL_load_OGL_texture(
        "assets/images/grain.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB |
            SOIL_FLAG_COMPRESS_TO_DXT);
    texture[7] = SOIL_load_OGL_texture(
        "assets/images/darkgrain.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB |
            SOIL_FLAG_COMPRESS_TO_DXT);
    texture[8] = SOIL_load_OGL_texture(
        "assets/images/metal.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB |
            SOIL_FLAG_COMPRESS_TO_DXT);
    texture[9] = SOIL_load_OGL_texture(
        "assets/images/hay.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB |
            SOIL_FLAG_COMPRESS_TO_DXT);

    // Carrega texturas do céu
    skybox[SKY_FRONT] = SOIL_load_OGL_texture(
        "assets/images/nightsky_west.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB |
            SOIL_FLAG_COMPRESS_TO_DXT);
    skybox[SKY_RIGHT] = SOIL_load_OGL_texture(
        "assets/images/nightsky_south.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB |
            SOIL_FLAG_COMPRESS_TO_DXT);
    skybox[SKY_LEFT] = SOIL_load_OGL_texture(
        "assets/images/nightsky_north.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB |
            SOIL_FLAG_COMPRESS_TO_DXT);
    skybox[SKY_BACK] = SOIL_load_OGL_texture(
        "assets/images/nightsky_east.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB |
            SOIL_FLAG_COMPRESS_TO_DXT);
    skybox[SKY_UP] = SOIL_load_OGL_texture(
        "assets/images/nightsky_up.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB |
            SOIL_FLAG_COMPRESS_TO_DXT);
    skybox[SKY_DOWN] = SOIL_load_OGL_texture(
        "assets/images/nightsky_down.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB |
            SOIL_FLAG_COMPRESS_TO_DXT);

    glutMainLoop();
    return 0;
}
