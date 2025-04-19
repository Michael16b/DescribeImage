# Describe Image Jeu multijoueur en réseau (inspiré de "Scribble It!")

## 🎯 Présentation du projet
Ce projet, réalisé dans le cadre du cours d'ingénierie des réseaux, consiste en un jeu multijoueur en réseau dans lequel les joueurs doivent deviner le mot correspondant à une image affichée à l'écran. Le joueur le plus rapide à répondre correctement marque le plus de points. Le jeu met l'accent sur la rapidité, la précision et l’interaction en temps réel.

## 🧩 Fonctionnalités principales
- Affichage aléatoire d’images associées à des mots-clés
- Synchronisation réseau entre les joueurs
- Système de score et de chronomètre

## 🛠️ Choix techniques

### Bibliothèques graphiques
- `SDL2` : rendu graphique multiplateforme
- `SDL2_image` : prise en charge des formats d’image (JPG, PNG, etc.)
- `SDL2_ttf` : affichage de texte avec polices personnalisées

### Protocole réseau
- **UDP** : faible latence pour des échanges rapides
- **RUDP (Reliable UDP)** : surcouche maison ajoutée pour fiabiliser les échanges, sans les contraintes de TCP

## ⚙️ Installation des dépendances
```bash
sudo apt-get update
sudo apt-get install libsdl2-2.0-0
sudo apt-get install libsdl2-dev
sudo apt-get install libsdl2-image-dev
sudo apt install libsdl2-ttf-dev
```

## ⚙️ Compilation
### Côté Client
```bash
gcc -o editor editor.c -lSDL2 -lSDL2_image -lSDL2_ttf
./editor
```
### Côté Serveur

```bash
gcc -o server server.c -pthread
./server
```

## Compilation lorsque l'executable existe déjà
make rebuild

## Nettoyage
make clean

