## Installation
`
sudo apt-get update
sudo apt-get install libsdl2-2.0-0
sudo apt-get install libsdl2-dev
sudo apt-get install libsdl2-image-dev
sudo apt install libsdl2-ttf-dev
`

## Compilation
On lance un premier terminal et on tape la commande suivante :
`
gcc -o server server.c -pthread
./server
`
Puis on lance un deuxième terminal et on tape la commande suivante :
`gcc -o editor editor.c -lSDL2 -lSDL2_image -lSDL2_ttf
./editor
`


## Compilation lorsque l'executable existe déjà
make rebuild

## Nettoyage
make clean

