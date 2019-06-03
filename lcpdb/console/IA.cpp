/*
 *  IA.cpp
 *  lcpdb
 *
 *  Created by Patrice Fouquet on 11/11/06.
 *  Converted from Delphi version program.
 *  The (C++) notes indicates propre C++ code
 *  Copyright 2006 Patrice Fouquet. All rights reserved.
 *
 */

#include "IA.h"
//-----------------------------------------------------------------------------
// Variables globales transverses
//-----------------------------------------------------------------------------
char *stNomFichierProfondeur=NULL, *stNomFichierEnonce=NULL, *stConclusion=NULL, *stNbSolutionsTempsEcoule=NULL;
TOptimisationsCalcul OptimisationsCalcul={true,true,true,true,true,true};
TSortieEcran SortieEcran=seTout;
int OrdreChrono=0, 
    NbCalculsEnvisages=0, // v1.2
    Compte=0,
    DistanceRecherche=0;
TNiveauDifficulte NiveauRecherche=ndIndefini;
bool ParametreVide=false; // Utilisé par PositionOperateur(...)
char stStatistiques[128]; // Buffer de TStatSolution::stApercu()
set<string> Solutions; // équivalant à FormMain.ListBox.Items
set<string> ComplexitesSolutions; // Solutions précédées des codes donnant la complexité  
set<string> SolutionsProfondeur; // Solutions écrites sous forme de listes

//-----------------------------------------------------------------------------
// Constantes globales transverses
//-----------------------------------------------------------------------------
const char stOperateur[NbOperateurs] = {'+', '*'};
const char stOperateurContraire[NbOperateurs] = {'-', '/'};
const char stOperateurEgal[2]="=";
/*
const char stAvertissementOptCalc[NbOptimisationsCalcul][90]={"Multiplication/Division par 1. Opération ignorée.",
                                                              "Opérande résultat d'une opération de même type trouvé : opérations regroupées.",
                                                              "Opérande égal au résultat trouvé : opération ignorée.",
                                                              "Deux opérandes inverses ou opposés trouvés. Opérandes ignorés.",
                                                              "Résultat intermédiaire non utilisé. Opération ignorée.",
                                                              "Résultat intermédiaire égal au compte. Suite ignorée."};
*/
const bool AjoutSansTri=false;
const bool AjoutAvecTri=true;

// [NbOperandes][NbMaxOperandes] Sur un total de  0  1  2  3  4  5  6 nombres... On en prend :
const int NbChoixOperandes[NbMaxOperandes+1]
                          [NbMaxOperandes+1]
                                              ={{ 0, 0, 0, 0, 0, 0, 0}, // 0
                                                { 0, 0, 0, 0, 0, 0, 0}, // 1
                                                { 0, 0, 1, 3, 6,10,15}, // 2
                                                { 0, 0, 0, 1, 4,10,20}, // 3
                                                { 0, 0, 0, 0, 1, 5,15}, // 4
                                                { 0, 0, 0, 0, 0, 1, 6}, // 5
                                                { 0, 0, 0, 0, 0, 0, 1}};// 6

const int NbChoixContrarietes[7]={1,2,4,8,16,32,64};

// [Nombre d'opérandes à choisir][Nombre d'opérandes sélectionnables][N° de choix d'opérandes][N° d'opérande]
//                                         1° Choix      2° Choix       3° Choix      4° Choix       5° Choix      6° Choix      7° Choix       8° Choix      9° Choix      10° Choix     11° Choix      12° Choix     13° Choix     14° Choix      15° Choix     16° Choix      17° Choix     18° Choix      19°Choix      20° Choix
const int NumeroOperandeChoix[1+NbMaxOperandes-NbMinOperandes]
                             [1+NbMaxOperandes-NbMinOperandes]
                             [NbMaxChoixOperandes]
                             [NbMaxPlaques]=
                                           {{{{0,1,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}},  // 2 parmi 2 {1 possibilité}
                                             {{0,1,0,0,0,0},{0,2,0,0,0,0},{1,2,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}},  // 2 parmi 3 {3 possibilités}
                                             {{0,1,0,0,0,0},{0,2,0,0,0,0},{0,3,0,0,0,0},{1,2,0,0,0,0},{1,3,0,0,0,0},{2,3,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}},  // 2 parmi 4 {6 possibilités}
                                             {{0,1,0,0,0,0},{0,2,0,0,0,0},{0,3,0,0,0,0},{0,4,0,0,0,0},{1,2,0,0,0,0},{1,3,0,0,0,0},{1,4,0,0,0,0},{2,3,0,0,0,0},{2,4,0,0,0,0},{3,4,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}},  // 2 parmi 5 {10 possibilités}
                                             {{0,1,0,0,0,0},{0,2,0,0,0,0},{0,3,0,0,0,0},{0,4,0,0,0,0},{0,5,0,0,0,0},{1,2,0,0,0,0},{1,3,0,0,0,0},{1,4,0,0,0,0},{1,5,0,0,0,0},{2,3,0,0,0,0},{2,4,0,0,0,0},{2,5,0,0,0,0},{3,4,0,0,0,0},{3,5,0,0,0,0},{4,5,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}}, // 2 parmi 6 {15 possibilités}
                                            {{{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}},
                                             {{0,1,2,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}},  // 3 parmi 3 {1 possibilité}
                                             {{0,1,2,0,0,0},{0,1,3,0,0,0},{0,2,3,0,0,0},{1,2,3,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}},  // 3 parmi 4 {4 possibilités}
                                             {{0,1,2,0,0,0},{0,1,3,0,0,0},{0,1,4,0,0,0},{0,2,3,0,0,0},{0,2,4,0,0,0},{0,3,4,0,0,0},{1,2,3,0,0,0},{1,2,4,0,0,0},{1,3,4,0,0,0},{2,3,4,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}},  // 3 parmi 5 {10 possibilités}
                                             {{0,1,2,0,0,0},{0,1,3,0,0,0},{0,1,4,0,0,0},{0,1,5,0,0,0},{0,2,3,0,0,0},{0,2,4,0,0,0},{0,2,5,0,0,0},{0,3,4,0,0,0},{0,3,5,0,0,0},{0,4,5,0,0,0},{1,2,3,0,0,0},{1,2,4,0,0,0},{1,2,5,0,0,0},{1,3,4,0,0,0},{1,3,5,0,0,0},{1,4,5,0,0,0},{2,3,4,0,0,0},{2,3,5,0,0,0},{2,4,5,0,0,0},{3,4,5,0,0,0}}}, // 3 parmi 6 {20 possibilités}
                                            {{{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}},
                                             {{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}},
                                             {{0,1,2,3,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}},  // 4 parmi 4 {1 possibilité}
                                             {{0,1,2,3,0,0},{0,1,2,4,0,0},{0,1,3,4,0,0},{0,2,3,4,0,0},{1,2,3,4,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}},  // 4 parmi 5 {5 possibilités}
                                             {{0,1,2,3,0,0},{0,1,2,4,0,0},{0,1,2,5,0,0},{0,1,3,4,0,0},{0,1,3,5,0,0},{0,1,4,5,0,0},{0,2,3,4,0,0},{0,2,3,5,0,0},{0,2,4,5,0,0},{0,3,4,5,0,0},{1,2,3,4,0,0},{1,2,3,5,0,0},{1,2,4,5,0,0},{1,3,4,5,0,0},{2,3,4,5,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}}, // 4 parmi 6 {15 possibilités}
                                            {{{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}},
                                             {{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}},
                                             {{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}},
                                             {{0,1,2,3,4,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}},  // 5 parmi 5 {1 possibilité}
                                             {{0,1,2,3,4,0},{0,1,2,3,5,0},{0,1,2,4,5,0},{0,1,3,4,5,0},{0,2,3,4,5,0},{1,2,3,4,5,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}}, // 5 parmi 6 {6 possibilités}
                                            {{{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}},
                                             {{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}},
                                             {{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}},
                                             {{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}},
                                             {{0,1,2,3,4,5},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}}}};// 6 parmi 6 {1 possibilité}

// Constantes pour les chaînes de complexité des calculs
const int TailleMaxChaineComplexiteCalcul=128+TailleMaxCalcul;                                   
const char stFormatComplexiteCalcul[]="%d plaques utilisées, %d opération(s), résultat intermédiaire maximal=%12d : %s.";
 
//------------------------------------------------------------------------------
// METHODES DE CLASSES
//-----------------------------------------------------------------------------
// Méthodes de la classe TOperation
//-----------------------------------------------------------------------------
void TOperation::AffecteSituation(TSituation **Situation)
 {
  (*Situation)=this->Situation;
 };
//-----------------------------------------------------------------------------
TOperation::TOperation(TSituation *Situation, const TOperateur Operateur) // Constructeur basique
 {
  this->Situation = Situation;
  this->Operateur = Operateur;
  this->FResultat=0; // Résultat pas encore défini (0 par convention car résultat nul impossible)
  this->FNbOperandes=0;
  for(int i=0; i<NbMaxPlaques; i++)
   {
    IndexOperande[i]=0;
    Contraire[i]=false;
   }
 }
TOperation::TOperation(TSituation *Situation, TOperation *Operation) // Clôneur
 {
  this->Situation = Situation;
  this->Operateur = Operation->Operateur;
  this->FResultat = Operation->Resultat(); // On fait confiance à la propriété de l'opération fournie !
  FNbOperandes=0;
  for(int i=0; i<Operation->NbOperandes(); i++)
   AjouteOperande(Operation->IndexOperande[i], Operation->Contraire[i], AjoutSansTri);
  for(int i=Operation->NbOperandes(); i<NbMaxPlaques; i++)
   {
    IndexOperande[i]=0;
    Contraire[i]=false;
   } 
 };
//-----------------------------------------------------------------------------
// Calcule le résultat et le stocke dans FResultat. Renvoie Vrai si l'opération
// est possible (>0 et division entière)
//-----------------------------------------------------------------------------
void TOperation::CalculeResultat()
 {
  TNombre *Operande;
  FResultat=(Operateur==oMultiplie);// élément neutre
  for(int i=0; i<FNbOperandes; i++)
   {
    Operande=Situation->Nombre[IndexOperande[i]];
    if (Operateur==oPlus)
     if (Contraire[i])
      FResultat-=Operande->Valeur();
     else
      FResultat+=Operande->Valeur();
    else
     if (Contraire[i])
      FResultat/=Operande->Valeur();
     else
      FResultat*=Operande->Valeur();
   }
};
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// On veut comparer les nombres
//  - OperationSrc.Situation.Nombre[IndexOperandeSrc] de contrariété ContraireSrc à
//  - self.Situation.Nombre[self.IndexOperande[IndexOperationDst]] de
//    contrariété self.Contraire[IndexOperationDst]
// Renvoie True si le premier est à ajouter avant le deuxième. False sinon.
// ContenusIdentiques est modifié : True si les contenus sont identiques
// (indépendamment des ordres de création). False sinon.
//-----------------------------------------------------------------------------

bool TOperation::OperandeAClasserAvant( const TOperation *OperationSrc,
                                        const int IndexOperandeSrc,
                                        const bool ContraireSrc,
                                        const int IndexOperationDst,
                                        bool &ContenusIdentiques)
 {
  bool    OperandesIdentiques,
          OperandeAnterieur; // Pessimiste : Valeur par défaut dans le cas où les nombres sont strictement égaux en contenu (mais le premier arrivé est le mieux placé et n'est pas déplacé).
  TNombre *NombreDst,
          *NombreSrc;
  ContenusIdentiques=false; // Cas le plus fréquent (différent bien sûr)
  // Ordre des Critères de comparaison :

  // 1. Contrariété : Non Contraires d'abord ; Contraires ensuite
  if (ContraireSrc!=this->Contraire[IndexOperationDst])
   return (ContraireSrc<this->Contraire[IndexOperationDst]);
  // Note : On peut maintenant utiliser UNIQUEMENT ContraireSrc car this->Contraire[IndexOperationDst]==ContraireSrc !
  NombreDst=this->Situation->Nombre[this->IndexOperande[IndexOperationDst]];
  NombreSrc=OperationSrc->Situation->Nombre[IndexOperandeSrc];

  // 2. Valeur : Ordre décroissant si non contraire ; Ordre croissant si contraire
  if (NombreSrc->Valeur()!=NombreDst->Valeur())
   return ((NombreSrc->Valeur()>NombreDst->Valeur())&&(!ContraireSrc))||
       ((NombreSrc->Valeur()<NombreDst->Valeur())&&( ContraireSrc));

  // 3. Type : Résultats d'abord ; Non Résultats ensuite
  if (NombreSrc->EstResultat()!=NombreDst->EstResultat())
   return (NombreSrc->EstResultat()>NombreDst->EstResultat());

  // 4. Cas des nombres non résultats égaux : le premier arrivé est le mieux placé !
  if (!NombreSrc->EstResultat())
   {
    ContenusIdentiques=true; // On indique néanmoins qu'en contenu, ils sont égaux
    return false;
   } // Note : On considère maintenant que l'on traite UNIQUEMENT des résultats

  // 5. Nombre d'opérandes : Ordre décroissant (de surcroît des non résultats sont
  //                         à la fin si l'on considère qu'ils n'ont qu'un opérande)
  if (NombreSrc->Operation->NbOperandes()!=NombreDst->Operation->NbOperandes())
   return (NombreSrc->Operation->NbOperandes()>NombreDst->Operation->NbOperandes());
  
// Note : À présent, le nombre d'opérandes est identique.
// 6. ENFIN, on compare récursivement chaque opérande dans l'ordre de leurs index respectifs.
// Si nombres strictement égaux (comme deux non résultats de même valeur et contrariété),
// On passe aux opérandes suivantes de même index et ainsi de suite
  for(int i=0; i<NombreSrc->Operation->NbOperandes(); i++)
   {
    OperandeAnterieur=NombreDst->Operation->OperandeAClasserAvant( NombreSrc->Operation,
                                                                   NombreSrc->Operation->IndexOperande[i],
                                                                   NombreSrc->Operation->Contraire[i],
                                                                   i,
                                                                   OperandesIdentiques);
    if (!OperandesIdentiques)
     return OperandeAnterieur;
   }
  return false; // Valeur par défaut dans le cas où les nombres sont strictement égaux en contenu (mais le premier arrivé est le mieux placé et n'est pas déplacé).
 };
//-----------------------------------------------------------------------------
void TOperation::AjouteOperande(const int IndexOperande, const bool Contraire, const bool AvecTri)
// Ajoute IndexOperande dans Self.IndexOperande
 {
  bool NombresEgaux; // Résultat de fonction inutilisé pour cet appel de fonction
  FNbOperandes++;
  if (AvecTri)
   {
    if (FNbOperandes>1) // On ne trie que s
     {
      for(int i=FNbOperandes-2;i>=0;i--)
       {
        if (OperandeAClasserAvant(this, IndexOperande, Contraire, i, NombresEgaux))
         { // On décale les opérandes actuelles et on continue à chercher antérieurement
          this->IndexOperande[i+1] = this->IndexOperande[i];
          this->Contraire[i+1]     = this->Contraire[i];
         }
        else // On a trouvé l'endroit du nouvel opérande
         {
          this->IndexOperande[i+1] = IndexOperande;
          this->Contraire[i+1]     = Contraire;
          return;
         }
       }
     }
    // On a pas encore affecté : il s'agit alors de la première place !
    this->IndexOperande[0] = IndexOperande;
    this->Contraire[0]     = Contraire;
   }
  else // Sans tri : on ajoute à la fin
   {
    this->IndexOperande[FNbOperandes-1] = IndexOperande;
    this->Contraire[FNbOperandes-1]     = Contraire;
   }
 };
//-----------------------------------------------------------------------------
void TOperation::CopieNombresDans(TSituation *Situation)
 {
  for(int i=0; i<this->Situation->NbNombres(); i++)
   Situation->CopieNombre(this->Situation->Nombre[i]);
 };
//-----------------------------------------------------------------------------
void TOperation::MarqueOperandesUtilises()
// Marque les propriétés EstUtilise des opérandes de Self
 {
  for(int i=0; i<FNbOperandes; i++) 
   Situation->Nombre[IndexOperande[i]]->Utilise();
 };
//-----------------------------------------------------------------------------
int TOperation::NbOperations()
 {
  int NbOp=0;
  for(int i=0;i<NbMaxPlaques; i++)
   if (Situation->Nombre[IndexOperande[i]]->EstResultat())
    NbOp+=(1+Situation->Nombre[IndexOperande[i]]->Operation->NbOperations());
  return NbOp;
 };
//-----------------------------------------------------------------------------
int TOperation::ResultatMax()
 {
  int ResMax=0;
  for(int i=0; i<NbMaxPlaques; i++)
   {
    if ((Situation->Nombre[IndexOperande[i]]->EstResultat())&&
        (ResMax<Situation->Nombre[IndexOperande[i]]->Valeur()))
     ResMax=max( Situation->Nombre[IndexOperande[i]]->Valeur(),
                 Situation->Nombre[IndexOperande[i]]->Operation->ResultatMax());
   }
  return ResMax; 
 };
//-----------------------------------------------------------------------------
char *TOperation::stApercuProfondeur()
 {
  char *stValeur;
  sprintf(stOperation, "(%c", stOperateur[Operateur]);
  for(int i=0; i<FNbOperandes; i++)
   {
    if (Situation->Nombre[IndexOperande[i]]->EstResultat())
     stValeur=Situation->Nombre[IndexOperande[i]]->Operation->stApercuProfondeur();
    else
     stValeur=Situation->Nombre[IndexOperande[i]]->stApercuEnonce();
    if (Contraire[i])
     if (Operateur==oPlus)
      sprintf(stOperation+strlen(stOperation), " -%s", stValeur);
     else
      sprintf(stOperation+strlen(stOperation), " 1/%s", stValeur); 
    else
     sprintf(stOperation+strlen(stOperation), " %s", stValeur);
   }
  return strcat(stOperation, ")");
 };
//-----------------------------------------------------------------------------
char *TOperation::stApercuEnonce()
 {
  stOperation[0]='\x0'; 
  // D'abord on écrit les opérations antérieures
  for(int i=0; i<FNbOperandes; i++)
   if (Situation->Nombre[IndexOperande[i]]->EstResultat())
    strcat(stOperation, Situation->Nombre[IndexOperande[i]]->Operation->stApercuEnonce());
  // Ensuite, on peut écrire l'opération courante.
  strcat(stOperation, " ");
  for(int i=0; i<FNbOperandes; i++)
   {
    if (i)
     {
      if (Contraire[i])
       sprintf(stOperation+strlen(stOperation), "%c", stOperateurContraire[Operateur]);
      else
       sprintf(stOperation+strlen(stOperation), "%c", stOperateur[Operateur]);
     }
    strcat(stOperation, Situation->Nombre[IndexOperande[i]]->stApercuEnonce());
   }
  sprintf(stOperation+strlen(stOperation), "%s%d", stOperateurEgal, Resultat());
  return stOperation;
 };
//-----------------------------------------------------------------------------
// Méthodes de la classe TNombre
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Méthodes de la classe TSituation
//-----------------------------------------------------------------------------
TSituation::TSituation()
 {
  FNbNombres=0;
  for(int i=0; i<NbMaxNombresGeneres; i++)
   Nombre[i]=NULL; 
 };
//-----------------------------------------------------------------------------
void TSituation::AjouteResultat(TOperation *Operation) // Ici on transmet Operation, on ne le clône pas, il l'a déjà été dans TSituation(Operation)
 {
  Nombre[FNbNombres++]=new TNombre(Operation);
 };
//-----------------------------------------------------------------------------
TSituation::TSituation(TOperation *Operation) // Clôneur : fabrique une nouvelle situation à partir d'une opération (et de sa situation)
 {
  // 0. On initialise le compte de nombres.
  FNbNombres=0; 
  // 1. On clône d'abord Operation pour qu'elle soit égale à this.
  TOperation *OperationDst=new TOperation(this, Operation);
  // 2. On copie les nombres de Operation->Situation dans Situation (self)
  Operation->CopieNombresDans(this);
  // 3. On marque les opérandes d'OperationDst comme utilisé
  OperationDst->MarqueOperandesUtilises();
  // 4. On ajoute le résultat comme nombre à Self
  AjouteResultat(OperationDst);
 };
//-----------------------------------------------------------------------------
TSituation::~TSituation()
 {
  for(int i=0; i<FNbNombres; i++)
   if (Nombre[i]->EstSituation(this))
    delete Nombre[i];
 };
//-----------------------------------------------------------------------------
int TSituation::CalculeNbMaxOperandes()
 {
  int NbMaxOp=0;
  for(int i=0; i<FNbNombres; i++)
   NbMaxOp+=(!Nombre[i]->EstUtilise());
  return NbMaxOp;  
 };
//-----------------------------------------------------------------------------
void TSituation::AjouteNombre(const int Valeur) 
 {
  Nombre[FNbNombres++]=new TNombre(this, Valeur);
 };
//-----------------------------------------------------------------------------
void TSituation::CopieNombre(TNombre *Nombre) // Copie d'un nombre 
 {
  this->Nombre[FNbNombres++]=Nombre->Copie(this); // OPTIMISATION n°11
 }; 
//-----------------------------------------------------------------------------
bool TSituation::EcritApercuEnonceSiInexistant(TOperation *Operation)
// Comme Précédent mais écrit uniquement si l'aperçu n'a pas déjà été écrit
// et renvoie alors true ; sinon, n'écrit rien et renvoie false.
 {
  int Resultat=Operation->Resultat(); 
  char *stApercu;
  bool MeilleurResultat=(abs(Resultat-Compte)<DistanceRecherche);
  for(int i=0; i<FNbNombres; i++)
   if (Nombre[i]->EstResultat()&&(!Nombre[i]->EstUtilise()))
    {
     stApercu=Nombre[i]->Operation->stApercuEnonce();
     break;
    }
  if (MeilleurResultat|| // Soit on a trouvé mieux...
      (!Solutions.count(string(stApercu)))) // Soit on a pas trouvé mieux mais on a pas déjà trouvé cette solution
   {
    if (MeilleurResultat) // Si meilleur Résultat, on stocke la nouvelle distance et on vide les solutions
     {
      DistanceRecherche=abs(Resultat-Compte);
      Solutions.clear();
      SolutionsProfondeur.clear(); 
      if ((SortieEcran)&(seTypeArbre)) 
       ComplexitesSolutions.clear();
     }
    if ((SortieEcran)&(seTypeArbre))
     {
      char stComplexiteCalcul[TailleMaxChaineComplexiteCalcul]; 
      sprintf(stComplexiteCalcul, 
              stFormatComplexiteCalcul, // %d plaques utilisées, %d opérations, résultat intermédiaire maximal=%12d : %s
              NbPlaquesUtilisees(),
              1+Operation->NbOperations(),
              Operation->ResultatMax(),
              stApercu); 
      ComplexitesSolutions.insert(string(stComplexiteCalcul));
     } 
    Solutions.insert(string(stApercu));
    SolutionsProfondeur.insert(string(Operation->stApercuProfondeur()));
    return true;
   }
  else 
   return true;
 };
//-----------------------------------------------------------------------------
int TSituation::NbResultatsInutilises()
 {
  int NbRes=0;
  for(int i=0; i<FNbNombres; i++)
   NbRes+=(Nombre[i]->EstResultat()&&(!Nombre[i]->EstUtilise()));
  return NbRes;
 };
//-----------------------------------------------------------------------------
int TSituation::NbPlaquesUtilisees()
 {
  int NbPlq=0;
  for(int i=0; i<NbMaxPlaques; i++)
   NbPlq+=(Nombre[i]->EstUtilise());
  return NbPlq;
 };
//-----------------------------------------------------------------------------
bool TSituation::EstOperationPossible( int *Resultat,
                                       const TOperateur Operateur,
                                       const int NumeroOperande,
                                       const bool ContraireOperande)
 {
  bool Possible=true;
  TNombre *Operande=Nombre[NumeroOperande];
  switch(Operateur)
   {
    case oPlus:      if (ContraireOperande)
                      {
                       if ((*Resultat)<=Operande->Valeur()) // OPTIMISATION n°1 : Pas de différence négative ou nulle
                        Possible=false;
                       else 
                        (*Resultat)-=Operande->Valeur();
                      }
                     else
                      (*Resultat)+=Operande->Valeur();
                     break; 
    case oMultiplie: if (OptimisationsCalcul[ocPasDeMultiplicationDivisionPar1]&&
                         (Operande->Valeur()==1)) // OPTIMISATION n°3 : Pas de multiplication/division par 1
                      Possible=false;
                     else
                      if (ContraireOperande)
                       {
                        if ((*Resultat)%(Operande->Valeur())) // OPTIMISATION n°2 : Pas de division non entière
                         Possible=false;
                        else 
                         (*Resultat)/=Operande->Valeur();
                       }
                     else
                      (*Resultat)*=Operande->Valeur();
                     break;
   }
  return Possible; 
 }
//-----------------------------------------------------------------------------
void TSituation::Cherche()
 {
  int NbMaxOperandes=CalculeNbMaxOperandes(),
      Resultat,
      NbNombresDisponibles=0,
      NumeroOperande[NbMaxPlaques],
      IndexNombreDisponible[NbMaxPlaques];
  bool ContraireOperande[NbMaxPlaques],
       OperandeResultatMemeType, // Indicateur d'opérande résultat de même type d'opération (OPTIMISATION n°4)
       OperationImpossible,      // Indicateur d'opération impossible ou multiplication/division par 1 (OPTIMISATIONS n°1 à 3)
       OperandeEgalResultat,     // Indicateur d'existance d'un opérande égal au résultat (OPTIMISATION n°7)
       OperandesContraires,      // Indicateur d'existence d'opérandes contraires : opposés ou inverses (OPTIMISATION n°8)
       Contrariete;
  TOperation *Operation;
  TSituation *Situation;

  // 1. On détermine les index des nombres disponibles dans le tableau Self.Nombre
  for(int IndexNombre=0; IndexNombre<FNbNombres; IndexNombre++)
   if (!Nombre[IndexNombre]->EstUtilise())
    {
     IndexNombreDisponible[NbNombresDisponibles]=IndexNombre;
     NbNombresDisponibles++;
    }

  // 2. On choisit une opération à effectuer parmi les cas possibles
  for(int NbOperandes=2; NbOperandes<=NbMaxOperandes; NbOperandes++)
   for(int i=0; i<NbOperateurs; i++)
    {
     TOperateur Operateur=(TOperateur)i;
     for(int NumeroChoixOperandes=0; NumeroChoixOperandes<NbChoixOperandes[NbOperandes][NbMaxOperandes]; NumeroChoixOperandes++)
      for(int NumeroChoixContrarietes=0; NumeroChoixContrarietes<NbChoixContrarietes[NbOperandes]-1; NumeroChoixContrarietes++) // OPTIMISATION n°5 : On ne peut avoir que des contraires donc il y a une possibilité en moins : la dernière !
       {
        // 2.a On détermine les index des opérandes choisis parmi la liste des nombres DISPONIBLES
        OperandeResultatMemeType=false; // Initialisation de l'indicateur d'opérande résultat de même type d'opération (OPTIMISATION n°4)
        OperationImpossible=false; // Initialisation de l'indicateur d'opération impossible (OPTIMISATIONS n°1~3)
        OperandeEgalResultat=false; // Initialisation de l'indicateur d'opérande = Résultat (OPTIMISATION n°7)
        OperandesContraires=false; // Initialisation de l'indicateur d'opérandes contraires (OPTIMISATION n°8)
        Resultat=(Operateur==oMultiplie); // Initialisation du résultat de l'opération pour savoir si elle est possible
        for(int j=0; j<NbContrarietes; j++) // On teste d'abord les non contraires avant les contraires pour cumuler le résultat
         {
          Contrariete=((j%2)>0); 
          for(int IndexOperande=0; IndexOperande<NbOperandes; IndexOperande++)
           if (Contrariete==((NumeroChoixContrarietes&NbChoixContrarietes[IndexOperande])>0))
            { 
             NumeroOperande[IndexOperande]=IndexNombreDisponible[NumeroOperandeChoix[NbOperandes-2][NbMaxOperandes-2][NumeroChoixOperandes][IndexOperande]];
             ContraireOperande[IndexOperande]=Contrariete;
             if (!EstOperationPossible( &Resultat,
                                        Operateur,
                                        NumeroOperande[IndexOperande],
                                        ContraireOperande[IndexOperande]))
              {
               OperationImpossible=true;
               break;
              }
             if (OptimisationsCalcul[ocPasDeResultatMemeTypeOperande]&&
                 Nombre[NumeroOperande[IndexOperande]]->EstResultat()&&
                 (Nombre[NumeroOperande[IndexOperande]]->Operation->Operateur==Operateur))
              {
               OperandeResultatMemeType=true;
               break;
              }
            }
          if (OperandeResultatMemeType||
              OperationImpossible) // Optimisations ne nécessitant que toutes les opérandes soient définies dans NumeroOperande et ContraireOperande
           break; // Opérande non utilisable trouvé (OPTIMISATION n°4) ou opération impossible (OPTIMISATIONS n°1 à 3) alors on cherche d'autres possibilités...
         }
        if (OperandeResultatMemeType||
            OperationImpossible) 
         {
          NbCalculsEnvisages++; // v1.2
          continue;
         }
        // On teste chaque opérande pour savoir s'il est égal au résultat (OPTIMISATION n°7) : cela nécessite que NumeroOperande et ContraireOperande soient définis entièrement
        for(int IndexOperande=0; IndexOperande<NbOperandes; IndexOperande++)
         if (OptimisationsCalcul[ocPasDeResultatEgalOperande]&&
             (Nombre[NumeroOperande[IndexOperande]]->Valeur()==Resultat))
          {
           OperandeEgalResultat=true;
           break;
          }
        if (OperandeEgalResultat)
         {
          NbCalculsEnvisages++; // v1.2
          continue;
         }
        // On teste si deux opérandes contraires existent (OPTIMISATION n°8)
        if ((Operateur==oPlus)||(NbOperandes>2)) // ATTENTION : on peut diviser un nombre par lui-même car a/a=1 est autorisé (si 2 opérandes seulement) contrairement à a-a=0.
         for(int IndexOperande=0; IndexOperande<NbOperandes-1; IndexOperande++) // OPTIMISATION n°14 : IndexOperande < IndexOperandeContraire
          for(int IndexOperandeContraire=IndexOperande+1; IndexOperandeContraire<NbOperandes; IndexOperandeContraire++)
           if (OptimisationsCalcul[ocPasDOperandesContraires]&&
               (Nombre[NumeroOperande[IndexOperande]]->Valeur()==Nombre[NumeroOperande[IndexOperandeContraire]]->Valeur())&&
               (ContraireOperande[IndexOperande]==!ContraireOperande[IndexOperandeContraire]))
            {
             OperandesContraires=true;
             break;
            }
        if (OperandesContraires)
         {
          NbCalculsEnvisages++; // v1.2
          continue;
         }
        // 2.b On crée une instance de la nouvelle opération
        Operation=new TOperation(this, Operateur);
        // 2.c On ajoute les opérandes à la nouvelle opération (avec tri dans AjouteOperande)
        for(int IndexOperande=0; IndexOperande<NbOperandes; IndexOperande++)
         Operation->AjouteOperande(NumeroOperande[IndexOperande], ContraireOperande[IndexOperande], AjoutAvecTri);
        // 2.d On fait calculer le résultat en interne
        Operation->CalculeResultat();
        // À présent les données de la nouvelle situation sont : 1. NbMaxOperandes,
        //                                                       2. NbOperandes,
        //                                                       3. Operateur,
        //                                                       4. NumeroChoix,
        //                                                       5. NumeroOperande[0..NbOperandes]
        // On génère la situation suivante et on poursuit la recherche...}
        Situation=new TSituation(Operation);
        if ((abs(Operation->Resultat()-Compte)<=DistanceRecherche)&&
            (Situation->NbResultatsInutilises()==1)) // OPTIMISATION n°6 : On ne garde la solution que s'il ne reste aucun résultat (intermédiaire) inutilisé
          Situation->EcritApercuEnonceSiInexistant(Operation);
        // v1.2
        if ((!OptimisationsCalcul[ocPasDeResultatEgalCompte])|| // Si optimisation non activée OU
            (Operation->Resultat()!=Compte)) // activé mais compte pas trouvé ALORS (OPTIMISATION n°9. v1.2)
         Situation->Cherche(); // on continue le calcul en profondeur
        delete Situation;
        delete Operation;
        NbCalculsEnvisages++; // v1.2
       }
    }
 };
//-----------------------------------------------------------------------------

