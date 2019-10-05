#ifndef FONCTION_H_INCLUDED
#define FONCTION_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Les includes et les constante pour l'affichage
#include <windows.h>
#include <conio.h>
#include <time.h>
#define NB_FONCTIONALITES 9
//define les tableaux 1 lignage
#define _1 192
#define _2 193
#define _3 217
#define _4 195
#define _5 197
#define _6 180
#define _7 218
#define _8 194
#define _9 191
#define _0 179
#define _ 196
//define un etablea 2 soulignage
#define __1 200
#define __2 202
#define __3 188
#define __4 204
#define __5 206
#define __6 185
#define __7 201
#define __8 203
#define __9 187
#define __0 186
#define __ 205
int cou;
typedef enum booleen
{
    faux,vrai
}booleen;

typedef struct entete
{
    int nb_bloc; //nombre de blocs
    int nb_art; //nombre d'articles
    int nb_car_sup; //nombre de caracteres supprimees
    int nb_car_ins; //nombre de caractere inserer
    char nom[20];
}entete;

typedef struct bloc
{
    int n_bloc; //numero de bloc
    int t_libre; //nombre de caracteres libres
    int cle_max; //la cle maximal dans le bloc
    int nb_c; //le nombre de caractere chauvauché du bloc precedent
    char tab[10000]; //tableau contenant les enregistrements
}bloc;

typedef struct enreg
{
    char taille[4];
    char vide[2];
    char cle[5];
    char c[2];
    char info[1000];
}enreg;
// La machine abstraite

void Ouvrir(FILE **f,char nomfichier[20],char mode); ///Ouvrir le fichier

void Fermer(FILE **f);///fermer un fichier

void LireDir(FILE *f,int i,bloc *buf); ///lire dans le buffer le contenu de i'eme bloc

void EcrireDir(FILE *f,int i,bloc *buf); ///Ecrire le contenu de buf dans le i'eme bloc dans fichier f

int Entete(FILE *f,int i); ///retourne la valeur de la i'eme valeur d'entete

void nom(FILE *f,char nomfichier[20]); ///retourne le nom de fichier

void Aff_entete(FILE *f,int i,int *val ); ///affecte la valeur val a la i'eme valeur dans l'entete

void Aff_entete_nom(FILE *f,char nomfichier[20]); ///affecter un nom a un fichier

void recherche (FILE *f ,int cle ,int *i ,int *j ,booleen *trouv); ///recherche la cle dans f

void rech(FILE *f,bloc *buf,int cle,int *j,booleen *trouv);   ///recherche la position j du cle dans le bloc (dans buf)

void  insertion(enreg e,FILE *f);  ///L'insertion d'une cle

void decalage(bloc *buf,int j, char s1[],char s2[],booleen *b); ///on fait un decalage a partir de la position j i.e un inser s1 a part

void affiche(FILE *f);///afficher le contenu de tout le fichier article par article

void afficheBloc(FILE *f,int i);///afficher le contenu de bloc article par article

void Suppression(FILE *f,int cle);///supprimer l'article de cle

void afficheEntete(FILE *f);///afficher le contenu de l'entete

void reorganisation(FILE **f,int aut);///reorganiser le fichier

void chargementinit(FILE *f,int n); ///charger le fichier f par n article aleatoires

/***************************** Les Modules de l'Affichage  ****************************************************/
void gotoxy(int xpos, int ypos);
void color(int t,int f);
int delay(int x);
void clr_pro(int t,int lig );
void chargement();
int ecrire(char ss[],int tm);
void esi (int x, int y,int fond,int couleur,int t);
void textbackground(int bcolor);
int wherey();
int afficher(int i);
void menu(FILE *f);
void entete_menu();
void accueil();

#endif // FONCTION_H_INCLUDED
