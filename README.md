# Ray Tracer Híbrido (C e Lua)

Este repositório contém a implementação de um gerador de imagens estáticas (Ray Tracing simples) desenvolvido para a disciplina de Conceitos de Linguagens de Programação, utilizando duas linguagens de programação com vocações distintas operando em conjunto.

## Estrutura de Arquivos
O repositório é composto pelos seguintes diretórios e arquivos:
* `main.c`: Código-fonte em C que é o motor de cálculo.
* `interface.lua`: Script de interface do usuário escrito em Lua, responsável por definir parâmetros e acionar a geração da imagem.
* `Makefile`: Arquivo de automação contendo as regras de compilação do executável e a execução do caso de estudo.
* `src/`: Diretório contendo o código-fonte original do interpretador Lua (versão 5.5.0). 

## Como Compilar
Não é necessário instalar pacotes extras ou o interpretador Lua nativamente no sistema, pois o projeto compila sua própria máquina virtual embutida a partir da pasta `src`.

Para gerar o executável, abra o terminal na raiz do projeto e execute:
```bash
make
# (ou mingw32-make caso esteja no Windows)
```
Após a compilação, para rodar o caso de estudo e gerar a imagem `resultado_esfera.ppm`, utilize o comando:
```bash
make run
# (ou mingw32-make run)
```
A imagem gerada estará no formato Portable Pixmap Format (.ppm) e pode ser visualizada usando a extensão [PBM/PPM/PGM Viewer for Visual Studio Code](https://marketplace.visualstudio.com/items?itemName=ngtystr.ppm-pgm-viewer-for-vscode) ou visualizadores de imagem como GIMP.