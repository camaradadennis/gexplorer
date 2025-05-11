# gexplorer

Graph Explorer application.

Esse é um protótipo. Se você está lendo isso, provavelmente está no meu grupo
do trabalho final para a disciplina de algoritmos e estruturas de dados 2,
no curso de bacharelado em sistemas de informação na Universidade Federal de
Goiás.

Esse protótipo foi construído em C++, utilizando as bibliotecas
[GTKMM](https://gtkmm.gnome.org/) e
[Boost Graph Library](https://www.boost.org/doc/libs/1_88_0/libs/graph/doc/).


## Instruções de compilação

A compilação é facilitada pelo [Meson](https://mesonbuild.com). Você vai
precisar dos seguintes pacotes (comandos de instalação válidos para alguma
distribuição derivada do Debian):

- gtkmm (`apt install gtkmm-4.0-dev`)
- boost (`apt install libboost-dev`)
- meson (`apt install meson`)

Após isso, navegue para a pasta onde está o código fonte e digite os comandos
de compilação:
```
$ meson setup build
$ cd build
$ meson compile
$ ./gexplorer
```
