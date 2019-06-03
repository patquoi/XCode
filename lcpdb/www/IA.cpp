/*
 *  IA.cpp
 *  lcpdb
 *
 *  Created by Patrice Fouquet on Feb-12-06.
 *  Converted from Delphi version program.
 *  The (C++) notes indicates propre C++ code
 *  Copyright 2007 Patrice Fouquet. All rights reserved.
 *
 */

#include "IA.h"
//-----------------------------------------------------------------------------
// Variables globales transverses
//-----------------------------------------------------------------------------
FILE *SortieEnonce=NULL;
char *stNomFichierEnonce=NULL, *stConclusion=NULL, *stNbSolutionsTempsEcoule=NULL;
TOptimisationsCalcul OptimisationsCalcul={true,true,true,true,true,true};
int OrdreChrono=0, 
    NbCalculsEnvisages=0, // v1.2
    Compte=0,
	DistanceRecherche=0;
TNiveauDifficulte NiveauRecherche=ndIndefini;
bool ParametreVide=false; // Utilisé par PositionOperateur(...)
char stStatistiques[128]; // Buffer de TStatSolution::stApercu()
set<string> Solutions; // équivalant à FormMain.ListBox.Items
set<string> ComplexitesSolutions; // Solutions précédées des codes donnant la complexité

//-----------------------------------------------------------------------------
// Constantes globales transverses
//-----------------------------------------------------------------------------
const char          stOperateur[NbOperateurs] = {'+', '*'};
const char stOperateurContraire[NbOperateurs] = {'-', '/'};
const char stOperateurEgal[2]="=";
const char stAvertissementOptCalc[NbOptimisationsCalcul][90]={"Multiplication/Division par 1. Opération ignorée.",
                                                              "Opérande résultat d'une opération de même  type trouvé : opérations regroupées.",
                                                              "Opérande égal au résultat trouvé : opération ignorée.",
                                                              "Deux opérandes inverses ou opposés trouvés. Opérandes ignorés.",
                                                              "Résultat intermédiaire non utilisé. Opération ignorée.",
                                                              "Résultat intermédiaire égal au compte. Suite ignorée."};
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

// [Nombre d'opérandes à choisir][Nombre d'opérandes sélectionnables][ n° de choix d'opérandes][ n°d'opérande]
//                                         1° Choix      2° Choix       3° Choix      4° Choix       5° Choix      6° Choix      7° Choix       8° Choix      9° Choix      10° Choix     11° Choix      12° Choix     13° Choix     14° Choix      15° Choix     16° Choix      17° Choix     18° Choix      19° Choix      20° Choix
const int NumeroOperandeChoix[1+NbMaxOperandes-NbMinOperandes]
                             [1+NbMaxOperandes-NbMinOperandes]
							 [NbMaxChoixOperandes]
							 [NbMaxPlaques]=
							               {{{{0,1,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}},                // 2 parmi 2 {1 possibilité}
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

// Version WWW (brute)
const char stFormatComplexiteCalcul[]="%d;%d;%12d;%s";
 	 
//-----------------------------------------------------------------------------
// Méthodes transverses
//-----------------------------------------------------------------------------
void VerifieNumeroOperandeChoix() // procédure de vérification à usage unique et donc plus utilisé mais laissé en guise d'exemple
 {
  int x, xx;
  const int Puiss6[NbMaxPlaques] ={46656,7776,1296,216,36,6};
  // Vérification de NumeroOperandeChoix
  for(int nmo=2; nmo<=NbMaxPlaques; nmo++)
   for(int no=2; no<=nmo; no++)
    {
     x=0;
     for(int nco=0; nco<NbChoixOperandes[no][nmo]; nco++)
      {
       xx=0;
       for(int ioc=0; ioc<no; ioc++)
        {
         if (NumeroOperandeChoix[no][nmo][nco][ioc]>=nmo)
          {
		   printf("\nIndex hors limite (%d/%d,%d) [%d!<%d].", no,nmo,nco, NumeroOperandeChoix[no][nmo][nco][ioc], nmo);
           xx+=Puiss6[ioc]*NumeroOperandeChoix[no][nmo][nco][ioc];
          }
         if (!xx)
          printf("\nIndex d'opérandes indéfinis pour ce choix (%d/%d,%d).", no, nmo, nco);
         if (xx<=x)
          printf("\nDistribution irréguliére (%d/%d,%d) [%d!>%d].", no, nmo, nco, xx, x);
         x=xx;
        }
      }
	}  
 };
//-----------------------------------------------------------------------------
void InitialiseOptimisations(TOptimisationsCalcul *Optimisations)
 {
  for(int i=0; i<NbOptimisationsCalcul; i++) 
   (*Optimisations)[i]=false;
 }; 
//-----------------------------------------------------------------------------
void AjouteOptimisation(const TOptimisationCalcul Optimisation,
						TOptimisationsCalcul *Optimisations,
                        const char *stFormatPrefixeAvertissement,
						const char *stArgumentPrefixeAvertissement)
 {
  if (!(*Optimisations)[Optimisation])
   {
    (*Optimisations)[Optimisation]=true;
	printf(stFormatPrefixeAvertissement, stArgumentPrefixeAvertissement);
	printf("%s\n", stAvertissementOptCalc[Optimisation]);
   }
 };
//-----------------------------------------------------------------------------
int Rang(const set<string> *Lignes, char *Ligne)
 {
  set<string>::iterator il;
  int i;
  string stLigne(Ligne);
  if (Lignes->count(stLigne))
   {
    for(i=1,il=Lignes->begin(); (*il)!=stLigne; il++, i++);
    return i;
   }
  else return 0; 
 };
//-----------------------------------------------------------------------------
// Renvoie les optimisations effectuées le cas échéant.
// Si modification il y a : IdxDrnOperande, Operande[], Contraire[] et
//                          stAvertissements sont modifiés
//-----------------------------------------------------------------------------
void TesteOptimisationOperandes( TOptimisationsCalcul *Optimisations,
                                 const TOperateur Operateur, // Opérateur de l'opération
								 const int Resultat,
								 const char *stPrefixeAvertissement,
								 const char *stArgumentPrefixeAvertissement,
								 TOperandesLigneCalcul Operande, // Opérandes[0..IdxDrnOperande]
								 TMarquagesLigneCalcul Contraire,
								 int &IdxDrnOperande)
 {
  TMarquagesLigneCalcul ASupprimer={false,false,false,false,false};
  InitialiseOptimisations(Optimisations);
  // 1. D'abord les optimisations d'opérations inutiles
  //  - ocPasDeResultatEgalOperande
  if (OptimisationsCalcul[ocPasDeResultatEgalOperande]) // Exception : Cas Compte par 1 = Compte
   for(int i=0; i<IdxDrnOperande; i++)
    if (Operande[i]==Resultat)
     {
      AjouteOptimisation(ocPasDeResultatEgalOperande, Optimisations, stPrefixeAvertissement, stArgumentPrefixeAvertissement);
      return; // éa ne sert à rien de continuer... La ligne va étre supprimée
     }
  // 2. Cas des opérandes inutiles :
  //  - ocPasDOperandesContraires
  //  - ocPasDeMultiplicationDivisionPar1
  // On initialise les marqueurs
  for(int i=0; i<IdxDrnOperande; i++)
   {
    if (OptimisationsCalcul[ocPasDeMultiplicationDivisionPar1]&&
        (!ASupprimer[i])&& // On ne traite pas un opérande déjà à supprimer
        (Operande[i]==1)&&
	    (Operateur==oMultiplie))
     {
      AjouteOptimisation(ocPasDeMultiplicationDivisionPar1, Optimisations, stPrefixeAvertissement, stArgumentPrefixeAvertissement);
      ASupprimer[i]=true;
     }
    if (OptimisationsCalcul[ocPasDOperandesContraires]&&
	    (IdxDrnOperande>1)) // Si 2 opérandes : autorisé (a-a=0 impossible à  saisir mais a/a=1 possible et autorisé !)
     for(int j=i+1;j<IdxDrnOperande;j++)
      if ((!ASupprimer[i])&&(!ASupprimer[j])&& // On ne traite pas deux fois le même  opérande
          (Operande[i]==Operande[j])&&
          (Contraire[i]==!Contraire[j]))
       { 
        AjouteOptimisation(ocPasDOperandesContraires, Optimisations, stPrefixeAvertissement, stArgumentPrefixeAvertissement);
        ASupprimer[i]=ASupprimer[j]=true;
       }
   }
  // 3. On supprime les opérandes marquées comme telles
  for(int i=IdxDrnOperande-1;i>=0;i--)
   if (ASupprimer[i])
    for(int j=IdxDrnOperande-1;j>=i; j--)
     {
      Operande[j]=Operande[j+1];
      Contraire[j]=Contraire[j+1];
      IdxDrnOperande--;
     }
 }; 
//-----------------------------------------------------------------------------
int PositionOperateur( const TOperateur Operateur, // Indique le type d'opération
					   const char *stOperation, // chaîne de l'opération
					   bool &Contraire, // Indique si l'opérateur trouvé est contraire
					   bool &FinOperation) // Indique si '=' est l'opérateur trouvé
 {
  TPositionsOperateurs PosOperateur;
  FinOperation=false; // 1 seul cas de fin donc False par défaut
  PosOperateur[0]=strchr(stOperation, stOperateur[Operateur])-stOperation;
  PosOperateur[1]=strchr(stOperation, stOperateurContraire[Operateur])-stOperation;
  if (!(PosOperateur[0]+PosOperateur[1]))
   { // On atteint le dernier opérande. PosOperateur = Pos(stOperateurEgal);
    FinOperation=true;
	return strchr(stOperation, stOperateurEgal[0])-stOperation;
   }
  else
   if (!PosOperateur[0])
    {
     Contraire=true;
	 return PosOperateur[1];
    }
   else
    if (!PosOperateur[1])
	 {
         Contraire=false; // 2019/04 : avant return
      return PosOperateur[0];
	 }
    else
     {
	  int Position=min(PosOperateur[0], PosOperateur[1]);
      Contraire=(Position==PosOperateur[1]);
      return Position;
	 }
 };
//-----------------------------------------------------------------------------
void stReduitEtOrdonneOperationSaisie( char *OpRedEtOrd, // Sortie (C++)
                                       const char *stOperation, // Entrée
                                       const char *stFormatPrefixeAvertissement,
                                       TOperateur &Operateur,
                                       int &Resultat,
                                       int &NbOperandes)
 {
     const char *stPosOperateur; // 2019/04 : ajout de const
     char *stResteATraiter=new char[1+strlen(stOperation)],
       stOperande[TailleMaxNombre];
  TOperandesLigneCalcul Operande;
  TMarquagesLigneCalcul Contraire;
  int IdxDrnOperande, PosOperateur, OperandeTemp;
  TPositionsOperateurs PosOperateurs;
  bool DrnOperandeTrv, ContraireTemp;
  TOptimisationsCalcul Optimisations;
  // 1. Initialisation
  PosOperateur=(strchr(stOperation, stOperateurEgal[0])-stOperation); // On cherche où se trouve l'opérateur =
  Resultat=atoi(strchr(stOperation, stOperateurEgal[0]))+1; // On extrait le résultat de l'opération
  IdxDrnOperande=0;
  DrnOperandeTrv=false;
  
  // 2. On cherche l'opérateur (entre + et *)
  if ((stPosOperateur=strchr(stOperation, stOperateur[oPlus])))
   PosOperateurs[oPlus]=(int)(stPosOperateur-stOperation);
  if ((stPosOperateur=strchr(stOperation, stOperateurContraire[oPlus])))
   PosOperateurs[oPlus]=(int)(stPosOperateur-stOperation);
   
  if ((stPosOperateur=strchr(stOperation, stOperateur[oMultiplie])))
   PosOperateurs[oMultiplie]=(int)(stPosOperateur-stOperation);
  if ((stPosOperateur=strchr(stOperation, stOperateurContraire[oMultiplie])))
   PosOperateurs[oMultiplie]=(int)(stPosOperateur-stOperation);

  if (PosOperateurs[oPlus]>0)
   Operateur=oPlus;
  else
   Operateur=oMultiplie;

    // 3. On localise le premier opérateur pour déterminer le premier opérande
  PosOperateur=PositionOperateur(Operateur, stOperation, Contraire[1], ParametreVide);

  // 4. On détermine l'opérande et sa "contrariété" positive pour le premier
  Operande[0]=atoi(strncpy(stOperande, stOperation, PosOperateur));
  strcpy(stResteATraiter, strchr(stOperation, PosOperateur)+1);
  Contraire[0]=false;

// 5. On détermine les autres opérandes (é présent on part de l'opérateur (le premier) jusqu'au dernier opérande)
do
 {
  IdxDrnOperande++;
  PosOperateur=PositionOperateur(Operateur, stResteATraiter, Contraire[IdxDrnOperande+1], DrnOperandeTrv);
     Operande[IdxDrnOperande]=atoi(strncpy(stOperande, stOperation, PosOperateur));
  strcpy(stResteATraiter, strchr(stOperation, PosOperateur)+1);
 }
while(!DrnOperandeTrv);
delete[] stResteATraiter; // OPTIMISATION n°15 - libération de mémoire
// 5c. On teste les optimisations de calculs et on modifie le cas échéant Operande, Contraire et IdxDrnOperande
TesteOptimisationOperandes( &Optimisations,
                            Operateur,
                            Resultat,
                            stFormatPrefixeAvertissement,
                            stOperation,
							Operande,
                            Contraire,
                            IdxDrnOperande);
if (Optimisations[ocPasDeResultatEgalOperande]|| // Si opération inutile OU
    (!IdxDrnOperande)) // Il ne reste plus qu'un opérande ALORS
 {
  OpRedEtOrd[0]='\x0';
  return;
 }
// 6. On trie les opérandes par ordre de contrariété (Non-contraires puis contraires) et valeur (ordre décroissant)
for(int i=0;i<IdxDrnOperande;i++)
 for(int j=i+1;j<IdxDrnOperande;j++)
  if ((Contraire[i]>Contraire[j])||
      ((Contraire[i]==Contraire[j])&&
       (((Operande[i]<Operande[j])&&(!Contraire[i]))||
        ((Operande[i]>Operande[j])&&(Contraire[i])))))
   {
    OperandeTemp=Operande[i]; ContraireTemp=Contraire[i];
	Operande[i]=Operande[j];
    Contraire[i]=Contraire[j];
    Operande[j]=OperandeTemp;
    Contraire[j]=ContraireTemp;
   }
 // 7. On reforme l'opération en chaîne pour la transmettre
 NbOperandes=IdxDrnOperande;
 sprintf(OpRedEtOrd, "%d", Operande[0]);
 for(int i=0;i<IdxDrnOperande; i++)
  {
   if (Contraire[i])
    strncpy(OpRedEtOrd+strlen(OpRedEtOrd), stOperateurContraire+Operateur, 1);
   else
    strncpy(OpRedEtOrd+strlen(OpRedEtOrd), stOperateur+Operateur, 1);
   sprintf(OpRedEtOrd+strlen(OpRedEtOrd), "%d", Operande[i]);
  }
 strcat(OpRedEtOrd, stOperateurEgal);
 sprintf(OpRedEtOrd+strlen(OpRedEtOrd), "%d", Resultat);
 return;
};
//-----------------------------------------------------------------------------
char *stSignesChanges(const char *stCalcul, const bool ChangerSigne, const TOperateur Operateur)
 {
  char *stNvCalcul=new char[1+strlen(stCalcul)];
  strcpy(stNvCalcul, stCalcul);
  if (ChangerSigne)
   {
    for(int i=0;i<strlen(stCalcul);i++)
     {
      if (stCalcul[i]==stOperateur[Operateur])
       stNvCalcul[i]=stOperateurContraire[Operateur];
      if (stCalcul[i]==stOperateurContraire[Operateur])
       stNvCalcul[i]=stOperateur[Operateur];
     }
   }
  return stNvCalcul; 
 };
//-----------------------------------------------------------------------------
// Méthodes classe TOptimisation
//-----------------------------------------------------------------------------
bool TOptimisation::LignesCalculMemeResultatOrdonnees(const int IndexLigneCalcul1, int IndexLigneCalcul2)
 {
  const int NbLignesComparees = 2;
  TPositionsOperateurs PosOperateur;
  bool DrnOperandeTrv;
  int Operande[NbLignesComparees], 
      Index[NbLignesComparees]={IndexLigneCalcul1, IndexLigneCalcul2};
  char *stResteATraiter[NbLignesComparees];
  // On suppose ici que les résultats ont même  valeur. On souhaite connaétre laquelle des deux lignes de calcul on va prendre.
  // 1. On compare d'abord l'ordre des résultats dans la ligne les utilisant (ordre décroissant)
  if (NbOperandesLigneCalcul[Index[0]]!=NbOperandesLigneCalcul[Index[1]])
   return (NbOperandesLigneCalcul[Index[0]]>NbOperandesLigneCalcul[Index[1]]);
  // à partir d'ici les deux lignes de calcul ont le même  nombre d'opérandes
  for(int i=0; i<NbLignesComparees; i++)
   stResteATraiter[i]=stLigneCalcul[Index[i]];
  do
   {
    PosOperateur[0]=PositionOperateur(OperateurLigneCalcul[Index[0]], stResteATraiter[0], ParametreVide, DrnOperandeTrv);
    PosOperateur[1]=PositionOperateur(OperateurLigneCalcul[Index[1]], stResteATraiter[1], ParametreVide, DrnOperandeTrv);
    for(int i=0; i<NbLignesComparees; i++)
     Operande[i]=atoi(strncpy(stOperande, stResteATraiter[i], PosOperateur[i]));
    if (Operande[0]!=Operande[1])
     return (Operande[0]>Operande[1]);
   } // à partir d'ici les iémes opérandes sont égaux, on continue...
  while(!DrnOperandeTrv);
  return true; // DrnOperandeTrv est vrai alors les lignes sont identiques : on ne change rien !
 };
//------------------------------------------------------------------------------
void TOptimisation::ConstruitCalculReduitEtOrdonne(char *stCalcul, const int IndexLigne)
 { // On modifie stCalcul
  int IndexLigneSvt,
      PosOperateur,
	  Operande;
  bool DrnOperandeTrv;
  char stResteATraiter[TailleMaxOperation],
       stCalculOperandes[TailleMaxOperation];
  // 1. cas trivial : Ligne de calcul vide : on renvoie la ligne transmise
  if (!stLigneCalcul[IndexLigne])
   return;
  // 2. On initialise les chaînes de traitement
  stCalculOperandes[0]='\x0';
  strcpy(stResteATraiter, stLigneCalcul[IndexLigne]);
  // 3. On extrait les opérandes et on va chercher les éventuelles lignes qui
  //    définissent les opérandes comme résultats intermédiaires
  do
   {
    PosOperateur=PositionOperateur(OperateurLigneCalcul[IndexLigne], stResteATraiter, ParametreVide, DrnOperandeTrv);
    Operande=atoi(strncpy(stOperande, stResteATraiter, PosOperateur));
    strcpy(stResteATraiter, stResteATraiter+PosOperateur+1);
    IndexLigneSvt=NbMaxLignesCalcul; // On calcul un minimum...
    for(int i=0; i<IndexLigne; i++)
     if ((ResultatLigneCalcul[i]==Operande)&& // On a trouvé une ligne dont le résultat est égal à l'opérande ET
         (!LigneCalculUtilisee[i])&& // ce résultat n'a pas encore été utilisé ET
         ((IndexLigneSvt==NbMaxLignesCalcul)|| // SOIT on a pas encore trouvé de ligne concurrente
          (RangLigneMemeResultat[i]<RangLigneMemeResultat[IndexLigneSvt]))) // SOIT on en a une mais pas meilleure ALORS
      IndexLigneSvt=i; // ON prend cette ligne
    if (IndexLigneSvt<NbMaxLignesCalcul)
     {
      LigneCalculUtilisee[IndexLigneSvt]=true; // On a trouvé une ligne
      ConstruitCalculReduitEtOrdonne(stCalculOperandes, IndexLigneSvt);
     }
   }
  while(!DrnOperandeTrv);
  // 4. On reconstruit la ligne de calcul en intercalant les lignes de calcul des
  //    opérandes entre la chaîne construite et la ligne des résultats en
  //    faisant attention aux chaînes vides
  if (stCalcul[0])
   if (!stCalculOperandes[0])
    sprintf(stCalcul, "%s %s %s", stCalcul, stCalculOperandes, stLigneCalcul[IndexLigne]);
   else
    sprintf(stCalcul, "%s %s", stCalcul, stLigneCalcul[IndexLigne]);
  else
   if (!stCalculOperandes[0])
    sprintf(stCalcul, "%s %s", stCalculOperandes, stLigneCalcul[IndexLigne]);
   else
    strcpy(stCalcul, stLigneCalcul[IndexLigne]);
 };
//------------------------------------------------------------------------------
void TOptimisation::stReduitEtOrdonneCalculSaisi(char *stCalcul)
 { // stCalcul est modifié
  int NbLignesCalcul, 
      Operande,
      PosOperateur,
	  PositionEspace;
  bool FusionOperande,
       FusionLigne,
       ChangerSigne;
  char stResteATraiter[TailleMaxCalcul],
       *stOperateurSuivant,
       stPartieLigneTraitee[TailleMaxOperation];
  TLignesCalcul stLigneCalcul;
  NbLignesCalcul=0;
  printf("Calcul saisi : %s\n", stCalcul);
  printf("Optimisation des lignes...\n");

  // Phase 1 : réduit & ordonne chaque ligne indépendemment l'une de l'autre
  //------------------------------------------------------------------------------
  strcpy(stResteATraiter, stCalcul);
  while (stResteATraiter[0])
   {
    PositionEspace=strchr(stResteATraiter, ' ')-stResteATraiter;
    if (!PositionEspace)
     PositionEspace=strlen(stResteATraiter);
	stResteATraiter[PositionEspace]='\x0'; // On cache les autres lignes à traiter 
    stReduitEtOrdonneOperationSaisie( stLigneCalcul[NbLignesCalcul],
	                                  stResteATraiter,
                                      " - Ligne %s : ",
                                      OperateurLigneCalcul[NbLignesCalcul],
                                      ResultatLigneCalcul[NbLignesCalcul],
                                      NbOperandesLigneCalcul[NbLignesCalcul]); // Initialisation Phase 2
    stResteATraiter[PositionEspace]=' '; // On restaure les autres lignes à traiter 
    LigneCalculUtilisee[NbLignesCalcul]=false; // Initialisation Phase 2
    NbLignesCalcul++;
    strcpy(stResteATraiter, stResteATraiter+PositionEspace+1);
   }
   
  // Phase 2 : réduit & ordonne les lignes en groupant les opérations de même type
  //------------------------------------------------------------------------------
  printf("Regroupement des opérations de même  type...\n");
  if (NbLignesCalcul<2)
   {
    printf("Calcul optimisé (réduit et ordonné): %s", stLigneCalcul[0]);
	strcpy(stCalcul, stLigneCalcul[0]);
    return;
   }
  // 2a. On teste chaque ligne pour vérifier les fusions possibles
  for(int i=1; i<NbLignesCalcul; i++) // Pour chaque ligne de calcul (sauf la première qui ne peut contenir de résultat intermédiaire)
   {
    strncpy(stResteATraiter, stLigneCalcul[i], 1+strchr(stLigneCalcul[i], stOperateurEgal[0])-stLigneCalcul[i]); // On vire le résultat de la ligne (on garde le signe '=')
    stPartieLigneTraitee[0]='\x0';
    FusionLigne=false;
    stOperateurSuivant[0]='\x0';
    while(stResteATraiter[0])
     {
      // On localise le prochain opérateur
      PosOperateur=PositionOperateur(OperateurLigneCalcul[i], stResteATraiter, ParametreVide, ParametreVide);
      Operande=atoi(strncpy(stOperande, stResteATraiter, PosOperateur));
      ChangerSigne=(stOperateurSuivant[0]==stOperateurContraire[OperateurLigneCalcul[i]]); // On teste si l'opérateur est contraire avant de récupérer le suivant
      stOperateurSuivant[0]=stResteATraiter[PosOperateur];
      strcpy(stResteATraiter, stResteATraiter+PosOperateur+1);
      // On regarde si l'opérande n'est pas le résultat d'une ligne précédente
      FusionOperande=false;
      for(int j=0; j<i; j++)
       {
	    if ((stLigneCalcul[j][0])&& // Ligne encore utile (pas fusionnée et pas vide)
            (Operande==ResultatLigneCalcul[j])&& // Opérande = Résultat
            (!LigneCalculUtilisee[j])) // Résultat de la ligne non encore utilisé
         {
          LigneCalculUtilisee[j]=true; // L'opérande a été trouvé comme résultat ALORS on le marque sa ligne comme utilisée
          if (OperateurLigneCalcul[i]==OperateurLigneCalcul[j]) // même type d'opération
           { // ALORS on fusionne : on remplace l'opérande par la ligne de calcul
            printf(" - \"%s\" et \"%s\"", stLigneCalcul[j], stLigneCalcul[i]);
			      stLigneCalcul[j][strchr(stLigneCalcul[j], stOperateurEgal[0])-stLigneCalcul[j]]='\x0'; // On coupe la ligne au signe '='
            char *stSignes; // OPTIMISATION n°15 : libération de mémoire (1/3)
			      sprintf( stPartieLigneTraitee, "%s%s%s", 
			               stPartieLigneTraitee,
			               stSignes=stSignesChanges( stLigneCalcul[j], // OPTIMISATION n°15 : libération de mémoire (2/3)
                                               ChangerSigne,
                                               OperateurLigneCalcul[i]),
			      stOperateurSuivant);
			      delete stSignes; // OPTIMISATION n°15 : libération de mémoire (3/3)
            stLigneCalcul[j][0]='\x0';
            FusionOperande=true;
            if (!FusionLigne)
			      FusionLigne=true; // Indique qu'il faudra réduire et ordonner ensuite
            break; // On a trouvé la ligne de calcul, on arréte la recherche aprés la fusion
           }
		 }
       }
      if (!FusionOperande) // L'opérande courant N'a PAS été remplacé par une ligne de calcul : on l'ajoute directement avec l'opérateur suivant
       sprintf(stPartieLigneTraitee+strlen(stPartieLigneTraitee), "%d%s", Operande, stOperateurSuivant);
     }
    // Si une fusion de ligne a été faite on réduit et ordonne la ligne de calcul à nouveau
    if (FusionLigne)
     {
      sprintf(stLigneCalcul[i], "%s%d", stPartieLigneTraitee, ResultatLigneCalcul[i]);
      stReduitEtOrdonneOperationSaisie( stLigneCalcul[i],
                                        stLigneCalcul[i],
                                        " - Optimisation nouvelle ligne %s : ",
                                        OperateurLigneCalcul[i],
                                        ResultatLigneCalcul[i],
                                        NbOperandesLigneCalcul[i]);
	   }
   }
  // 2b. On supprime les lignes inutilisées (sauf la derniére) c'est-é-dire les lignes dont le résultat n'est pas utilisé.
  printf("Suppression des lignes dont le résultat n'est pas utilisé...\n");
  for(int i=0; i<NbLignesCalcul-1; i++)
   if ((!LigneCalculUtilisee[i])&&
       stLigneCalcul[i][0])
    {
     printf(" - Ligne \"%s\"", stLigneCalcul[i]);
     stLigneCalcul[i][0]='\x0';
	  };
// Phase 3: On trie les lignes de même  résultat et on reconstitue le calcul
//------------------------------------------------------------------------------
// 3a. On initialise les marqueurs
  printf("Tri des lignes et reconstitution du calcul finalisé...\n");
  for(int i=0; i<NbLignesCalcul; i++)
   {
    LigneCalculUtilisee[i]=false; 
    RangLigneMemeResultat[i]=0;
   }
  // 3b. On classe les lignes de même  résultat (sauf la derniére ligne, bien sûr)
  for(int i=0; i<NbLignesCalcul-2; i++)
   for(int j=i+1; j<NbLignesCalcul-1; j++)
    if (ResultatLigneCalcul[i]==ResultatLigneCalcul[j])
     if (LignesCalculMemeResultatOrdonnees(i, j))
      RangLigneMemeResultat[j]++;
     else
      RangLigneMemeResultat[i]++;
  // Phase 3 : On ordonne les lignes par récurrence
  stCalcul[0]='\x0';
  ConstruitCalculReduitEtOrdonne(stCalcul, NbLignesCalcul-1); // Appel Récursif
  printf("Calcul optimisé (réduit et ordonné): %s\n", stCalcul);
};
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
TOperation::TOperation(TSituation *Situation, TOperation *Operation) // clôneur
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
  // Ordre des Critéres de comparaison :

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
   } // Note : On considére maintenant que l'on traite UNIQUEMENT des résultats

  // 5. Nombre d'opérandes : Ordre décroissant (de surcroùt des non résultats sont
  //                         à la fin si l'on considére qu'ils n'ont qu'un opérande)
  if (NombreSrc->Operation->NbOperandes()!=NombreDst->Operation->NbOperandes())
   return (NombreSrc->Operation->NbOperandes()>NombreDst->Operation->NbOperandes());
  
// Note : à présent, le nombre d'opérandes est identique.
// 6. ENFIN, on compare récursivement chaque opérande dans l'ordre de leurs index respectifs.
// Si nombres strictement égaux (comme deux non résultats de mÍme valeur et contrariété),
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
    for(int i=FNbOperandes-2;i>=0;i--)
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
     if (Contraire[i])
      sprintf(stOperation+strlen(stOperation), "%c", stOperateurContraire[Operateur]);
     else
      sprintf(stOperation+strlen(stOperation), "%c", stOperateur[Operateur]);
    strcat(stOperation, Situation->Nombre[IndexOperande[i]]->stApercuEnonce());
   }
  sprintf(stOperation+strlen(stOperation), "%s%d", stOperateurEgal, Resultat());
  return stOperation;
 };

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
void TSituation::AjouteResultat(TOperation *Operation) // Ici on transmet Operation, on ne le clône pas, il l'a déjà  été dans TSituation(Operation)
 {
  Nombre[FNbNombres++]=new TNombre(Operation);
 };
//-----------------------------------------------------------------------------
TSituation::TSituation(TOperation *Operation) // clôneur : fabrique une nouvelle situation à partir d'une opération (et de sa situation)
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
void TSituation::EcritApercuEnonceSiInexistant(TOperation *Operation)
// Ecrit uniquement si l'aperçu n'a pas déjà  été écrit
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
  if (MeilleurResultat) // On a trouvé mieux
   DistanceRecherche=abs(Resultat-Compte);
  if (Resultat==Compte) 
   {
    if (!Solutions.count(string(stApercu))) // on n'a pas déjà trouvé cette solution
     {
	  char stComplexiteCalcul[TailleMaxChaineComplexiteCalcul]; 
	  sprintf(stComplexiteCalcul, 
	          stFormatComplexiteCalcul, // %d plaques utilisées, %d opérations, résultat intermédiaire maximal=%12d : %s
	          NbPlaquesUtilisees(),
			  1+Operation->NbOperations(),
			  Operation->ResultatMax(),
			  stApercu); 
      ComplexitesSolutions.insert(string(stComplexiteCalcul));
      Solutions.insert(string(stApercu));
     }
   }  
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
                        if ((*Resultat)%(Operande->Valeur())) // OPTIMISATION n°2 : Pas de division non entiére
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
       OperandeResultatMemeType, // Indicateur d'opérande résultat de mÍme type d'opération (OPTIMISATION n°4)
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
      for(int NumeroChoixContrarietes=0; NumeroChoixContrarietes<NbChoixContrarietes[NbOperandes]-1; NumeroChoixContrarietes++) // OPTIMISATION n°5 : On ne peut avoir que des contraires donc il y a une possibilité en moins : la derniére !
       {
        // 2.a On détermine les index des opérandes choisis parmi la liste des nombres DISPONIBLES
        OperandeResultatMemeType=false; // Initialisation de l'indicateur d'opérande résultat de mÍme type d'opération (OPTIMISATION n°4)
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
              OperationImpossible) // Optimisations nécessitant que toutes les opérandes soient définies dans NumeroOperande et ContraireOperande
           break; // Opérande non utilisable trouvé (OPTIMISATION n°4) ou opération impossible (OPTIMISATIONS n°1 à n°3) alors on cherche d'autres possibilités...
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
        if ((Operateur==oPlus)||(NbOperandes>2)) // ATTENTION : on peut diviser un nombre par lui-même  car a/a=1 est autorisé (si 2 opérandes seulement) contrairement à a-a=0.
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
        // à présent les données de la nouvelle situation sont : 1. NbMaxOperandes,
        //                                                       2. NbOperandes,
        //                                                       3. Operateur,
        //                                                       4. NumeroChoix,
        //                                                       5. NumeroOperande[0..NbOperandes]
        // On génére la situation suivante et on poursuit la recherche...}
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
