# OpenGL Farm Scene

## Dependências

- g++
- GLUT
- SOIL

## Build

```sh
# Instala as dependências em distros debian-based
$ sudo apt-get install freeglut3-dev libsoil-dev

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
