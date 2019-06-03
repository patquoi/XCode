/*
 *  IA.h
 *  lcpdb
 *
 *  Created by Patrice Fouquet on 11/11/06.
 *  Copyright 2006 Patrice Fouquet. All rights reserved.
 *
 */

#include <set>
#include <stdio.h> 
#include <iostream>
#include <string>
using namespace std; // pour l'utilisation des ensembles set<...>

enum TNiveauDifficulte {ndIndefini=-1, ndtresFacile=0, ndTreDifficile=8, ndIntrouvable=9};
enum TOperateur {oPlus, oMultiplie};

const int NbOptimisationsCalcul = 6;
enum TOptimisationCalcul {ocPasDeMultiplicationDivisionPar1, // Paramétrable (Activé par défaut)
                          ocPasDeResultatMemeTypeOperande, // Paramétrable (Activé par défaut)
                          ocPasDeResultatEgalOperande, // Paramétrable (Activé par défaut)
                          ocPasDOperandesContraires, // Paramétrable (Activé par défaut)
                          ocResultatInutilise, // Toujours activé
                          ocPasDeResultatEgalCompte}; // Paramétrable (Activé par défaut). v1.2

typedef bool TOptimisationsCalcul[NbOptimisationsCalcul];

const int TailleMaxNombre            = 12; // cha�nes de nombres gérés jusqu'à 10 milliards exclus. Si modifié alors changer aussi stFormatComplexiteCalcul dans IA.cpp
const int NbOperateurs               =  2; // Card(TOperateur)         
const int NbMaxPlaques               =  6; // Les 6 nombres donnés au départ
const int NbContrarietes             =  2; // true et false (contraires [-;/] et non contraires [=;x]) 
const int NbMinOperandes             =  2; // Une opération a toujour au moins deux opérandes 
const int NbMaxOperandes             =  NbMaxPlaques; // On ne peut dépasser le nombre de plaques dans une seule opération
const int NbMaxNombresGeneres        = 11; // Nombres donnés (6 max) + Résultats intermédiaires (5 max)
const int NbMaxChoixOperandes        = 20; // Le plus de possibilités de choix = 4 opérandes parmi 6 = 20
const int NbMaxChoixContrarietes     = 64; // Le plus de possibilités de choix = 6 indicateurs contraires = 2^6 = 64 possibilités
const int NbChiffresMaxResultatMax   =  9; // Le Résultat max peut avoir au maximum 9 chiffres
const int NbMaxLignesCalcul          =  5; // Nombre maximum de lignes de calcul
const int TailleMaxLigneCalcul       = 99; // Taille d'une ligne du tableau de type TLigneCalcul
const char stPluriel[2][2]           = {"", "s"}; // Gestion du pluriel pour enlever les parenthèses. v1.2
const int TailleMaxOperation         = TailleMaxNombre*(NbMaxPlaques+1); // Les zéros terminaux des nombres sont remplacés par les opérateurs 
const int TailleMaxCalcul            = TailleMaxOperation*(NbMaxPlaques-1); // Idem mais avec les lignes et les retours à la ligne
const int TailleMaxAvertissements    = 1024;

extern int Compte, DistanceRecherche, NbCalculsEnvisages;
extern TOptimisationsCalcul OptimisationsCalcul;
extern set<string> Solutions;
extern set<string> ComplexitesSolutions;

class TSituation;
class TNombre;
 
class TOperation
 {
  private:
  TSituation *Situation;
  int FResultat;
  int FNbOperandes; // Rq : Théoriquement c'est supérieur ou égal à deux mais pas en pure création où on part de zéro !
  char stOperation[TailleMaxCalcul]; // En profondeur une opération peut être tout le calcul ! Utilisé par stApercu???()
    
  public:
  TOperateur Operateur;
  int IndexOperande[NbMaxPlaques]; // Index de l'opérande dans Situation (de TIndexNombreUtilisable dans TIndexNombreGenere)
  bool Contraire[NbMaxPlaques];
  void AffecteSituation(TSituation **Situation);
  int Resultat() 
   {
    if (!FResultat) CalculeResultat(); // Si FResultat nul alors le Résultat n'a jamais été calculé (par sécurité)
    return FResultat;
   };
  int NbOperandes() { return FNbOperandes; }; 
  TOperation(TSituation *Situation, const TOperateur Operateur); // Constructeur basique
  TOperation(TSituation *Situation, TOperation *Operation); // Clôneur
  ~TOperation() {};
  void CalculeResultat(); // Calcule le Résultat et le stocke dans FResultat.
  bool OperandeAClasserAvant( const TOperation *OperationSrc, 
                              const int IndexOperandeSrc,
							  const bool ContraireSrc,
							  const int IndexOperationDst,
							  bool &ContenusIdentiques);
  void AjouteOperande(const int IndexOperande, const bool Contraire, const bool AvecTri);
  void CopieNombresDans(TSituation *Situation); // Copie (clône) les nombres de this dans Situation
  void MarqueOperandesUtilises(); // Marque les propriétés EstUtilise des opérandes de Self
  int NbOperations(); // Retourne le nombre de niveaux d'opérations sous-jacentes (0=opération avec uniquement des nombres donnés)
  int ResultatMax(); // Retourne le Résultat le plus élevé trouvé dans les opérandes à tous les niveaux (nombres calculés uniquement)
  char *stApercuProfondeur();
  char *stApercuEnonce();
  void AfficheInfos(const char *stIndentation);
 };
  
class TNombre
 {
  private:
  TSituation *Situation;
  int FValeur;
  char stValeur[TailleMaxNombre]; // Buffer pour calculer la chaîne de la valeur par stApercu???()
  bool OperationCree; // Optimisation n°15 : libération de mémoire
  protected:
  bool FEstUtilise;
  
  public:
  TOperation *Operation;
  int Valeur() { return FValeur; };
  bool EstUtilise() { return FEstUtilise; };
  bool EstResultat() { return (Operation!=NULL); };
  TNombre(TSituation *Situation, const int Valeur) // Constructeur basique
   {
    this->Situation   = Situation;
    this->FValeur     = Valeur;
    this->Operation   = NULL;
    this->FEstUtilise = false;
	this->OperationCree = false; // Optimisation n°15 : libération de mémoire
   };
  TNombre(TSituation *Situation, TNombre *Nombre) // Constructeur de copie/déréférencement suivant Nombre.EstUtilise
   {
    this->Situation   = Situation;
    this->FValeur     = Nombre->Valeur();
    this->FEstUtilise = Nombre->FEstUtilise;
    if (Nombre->EstResultat())
     this->Operation  = Nombre->Operation; // Déréférence au lieu de copier (OPTIMISATION n°12)
    else
     this->Operation  = NULL;
	this->OperationCree = false; // Optimisation n°15 : libération de mémoire
   };
  TNombre(TOperation *Operation) // Crée le Résultat d'Operation comme nombre
   {
    Operation->AffecteSituation(&(this->Situation));
    this->FValeur      = Operation->Resultat();
    this->Operation    = Operation;
    this->FEstUtilise  = false;
	this->OperationCree = true; // Optimisation n°15 : libération de mémoire
   };
  ~TNombre()
   {
    if (OperationCree&&Operation) // Optimisation n°15 : libération de mémoire 
	 delete Operation;
   };
  TNombre *Copie(TSituation *Situation) // Copie/D�r�f�rence selon qu'il est utilisé ou non (OPTIMISATION n°11)
   {
    if (FEstUtilise)
     return this;
    else
     return new TNombre(Situation, this);
   };
  void Utilise() { FEstUtilise = true; };
  bool EstSituation(const TSituation *Situation) { return (Situation==this->Situation); };
  char *stApercuProfondeur() { return Operation->stApercuProfondeur(); };
  char *stApercuEnonce() { sprintf(stValeur, "%d", FValeur); return stValeur; };
  void AfficheInfos(const char *stIndentation);
 };

class TSituation
 {
  private:
  int FNbNombres; // calculé automatiquement. Renvoyé à l'extérieur de TSituation par NbNombres() (C++)
    
  public:
  TNombre *Nombre[NbMaxNombresGeneres]; // ATTENTION : Etait un tableau dynamique (C++)
  int NbNombres() { return FNbNombres; }; // Renvoie le nombre d'éléments définis dans Nombre[] (C++)
  void AjouteResultat(TOperation *Operation); // Ici on transmet OperationSrc, on ne le clône pas, il l'a déjà été dans new TSituation(OperationSrc)
  TSituation(); // Constructeur à vide
  TSituation(TOperation *Operation); // Générateur : fabrique une nouvelle situation ‡ partir de l'opération
  ~TSituation(); 
  int CalculeNbMaxOperandes();
  void AjouteNombre(const int Valeur);
  void CopieNombre(TNombre *Nombre);
  void EcritApercuProfondeur();
  void EcritApercuEnonceSiInexistant(TOperation *Operation); 
  int NbResultatsInutilises();
  int NbPlaquesUtilisees();
  bool EstOperationPossible(int *Resultat,
                            const TOperateur Operateur,
                            const int NumeroOperande,
                            const bool ContraireOperande);
  void Cherche();
  void AfficheInfos(const char *stIndentation);
 };
  
// Types de IA.cpp
typedef        int TPositionsOperateurs[NbOperateurs];
typedef        int TOperandesLigneCalcul[NbMaxPlaques];
typedef       bool TMarquagesLigneCalcul[NbMaxPlaques]; 
typedef TOperateur TOperateursLignesCalcul[NbMaxLignesCalcul]; 
typedef        int TResultatsLignesCalcul[NbMaxLignesCalcul];
typedef       bool TMarquagesLignesCalcul[NbMaxLignesCalcul];
typedef        int TRangsLignesCalcul[NbMaxLignesCalcul];
typedef        int TNbOperandesLignesCalcul[NbMaxLignesCalcul];
typedef       char TLignesCalcul[TailleMaxLigneCalcul][NbMaxLignesCalcul];  

class TOptimisation // Permet l'implémentation de procédures & fonctions imbriquées de Delphi (C++)
 { // Utilisée en instance statique car instance unique
  private:
  char stOperande[TailleMaxNombre]; // Buffer d'extraction d'opérandes par strncpy(...)
  TNbOperandesLignesCalcul NbOperandesLigneCalcul;
  TLignesCalcul stLigneCalcul;
  TOperateursLignesCalcul OperateurLigneCalcul;
  TResultatsLignesCalcul ResultatLigneCalcul;
  TMarquagesLignesCalcul LigneCalculUtilisee;
  TRangsLignesCalcul RangLigneMemeResultat;
  public:
  TOptimisation() {};
  bool LignesCalculMemeResultatOrdonnees(const int IndexLigneCalcul1, const int IndexLigneCalcul2); 
  void ConstruitCalculReduitEtOrdonne(char *stCalcul, const int IndexLigne);
  void stReduitEtOrdonneCalculSaisi(char *stCalcul); // envoie les avertissements sur la sortie standard  
 };



  
