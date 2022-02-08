# OpenGL Farm Scene

Cenário 3D de uma fazenda utilizando OpenGL. O usuário é capaz de andar livremente pelo cenário por meio de interações no teclado, assim como direcionar o olhar ao mexer o mouse.

![Fazenda](/assets/images/farm.png?raw=true "Fazenda")

## Dependências

-   **g++**
-   **OpenGL** - *API para Computação Gráfica*
-   **GLUT** - *Criação e controle de janelas*
-   **SOIL** - *Carregar as texturas*

## Build

```sh
# Instala as dependências em distros debian-based
$ sudo apt-get install freeglut3-dev libsoil-dev

# Clona o repositório
$ git clone https://github.com/neumanf/opengl-farm

# Muda para o diretório do projeto
$ cd opengl-farm

# Compila
$ g++ src/main.cpp -lm -lGL -lGLU -lglut -lSOIL -o farm

# Roda
$ ./farm
```

## Controles

| Atalho | Ação                             |
| ------ | -------------------------------- |
| W      | Movimenta a câmera para frente   |
| A      | Movimenta a câmera para esquerda |
| S      | Movimenta a câmera para trás     |
| D      | Movimenta a câmera para direita  |
| Q      | Fecha a aplicação                |
