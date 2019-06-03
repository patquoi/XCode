#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "IA.h"

#define DEBUT 0
#define FIN   1

#define NBPLQ 0
#define NBOPR 1
#define RESIM 2

#define NBMAXSOL 1000

const int NbValPlq=14, // Nb valeurs de plaques différentes 
          NbValPlqDbl=10, // Nb valeurs de plaques doubles possibles (de 1 à 10)
		  NbMaxPlqCpl=3, // On peut dobler maximum 3 plaques (ex: 1 1 2 2 3 3)
		  CompteMin=969,//101
		  CompteMax=999,
		  NbMaxTirages=13243,
		  NbRenvois=2, // Haut, Bas
		  ValPlq[NbValPlq]={1,2,3,4,5,6,7,8,9,10,25,50,75,100};
          
const char stEntete[]="lcpdbwww - Le Compte Pour De Bon version html - Patrice Fouquet, 2006.\n",
           stHTMLHautBas[NbRenvois][48]={"<A HREF=\"#fin\">[Fin]</A>&nbsp;", "<A HREF=\"#deb\">[D&eacute;but]</A>&nbsp;"},
		   stHTMLSuffixeHautBas[NbRenvois][5]={"#deb","#fin"},
		   stHTMLEnTetePageAvecCompte[]="\
<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\
<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n\
<HTML>\n\
 <HEAD>\n\
  <TITLE>Tirage : %d %d %d %d %d %d - %d</TITLE>\n\
 </HEAD>\n\
<BODY BGCOLOR=\"#FFFFFF\" TEXT=\"#00000\">\n", 
           stHTMLEnTetePageSansCompte[]="\
<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\
<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n\
<HTML>\n\
 <HEAD>\n\
  <TITLE>Tirage : %d %d %d %d %d %d</TITLE>\n\
 </HEAD>\n\
<BODY BGCOLOR=\"#FFFFFF\" TEXT=\"#00000\">\n\
<A NAME=\"deb\">\n\
</A><H2>Tirage : %d %d %d %d %d %d</H2>\n\
<TABLE BORDER=1 CELLSPACING=1 CELLPADDING=5>\n\
<TR><TH>Compte</TH><TH>Solution(s)</TH><TH>Solution la plus simple</TH><TH>Solution la plus complexe</TH></TR>\n",
           stHTMLLigneAvecCompte[]="<TR><TD ALIGN=CENTER><A NAME=\"%d\">%d</TD><TD ALIGN=RIGHT><A HREF=\"%d,%d,%d,%d,%d,%d-%d.html\">%d</A></TD>",
		   stHTMLPiedPage[]="%s\n</BODY>\n</HTML>\n",
           stHTMLPiedPageDebut[]="\
<H3>Statistiques pour ce tirage</H3>\n\
Nombre de comptes trouv&eacute;s (ayant au moins une solution) : %d/%d (%.2f%%)<BR>\n\
Nombre de solutions distinctes : %d (moyenne de %.2f solutions par compte)<BR>\n\
Nombre de solutions le plus &eacute;lev&eacute; : %d (compte <A HREF=\"#%d\">%d</A>)<BR>\n\
Le compte le plus difficile : <A HREF=\"#%d\">%d</A><BR>\n\
Dur&eacute;e des calculs : %d' %02d\"<BR>\n\
Nombre de solutions envisag&eacute;es pour chaque compte : %s (soit %s par seconde)<BR>\n\
R&eacute;sultat interm&eacute;diaire le plus &eacute;lev&eacute; : ",
		   stHTMLPiedPageFin[]=" (compte <A HREF=\"#%d\">%d</A>).\n",
		   stHTMLCopyright[]="<A NAME=\"fin\"><I>Copyright &copy; 2007 Patrice Fouquet</I>",
           stHTMLSolutionsTitre[]="<A NAME=\"deb\">\n<H2>%d %d %d %d %d %d - %d : %d solution(s)</H2>\n",
           stHTMLRepetition[]="<TD ROWSPAN=%d>",
		   stHTMLRepetitionAligneADroite[]="<TD ALIGN=RIGHT ROWSPAN=%d>",
		   stHTMLNbPlaques[]=" plaques</TD>",
		   stHTMLPluriel[2][2]={"","s"},
		   stHTMLNbOperations[]=" op&eacute;ration%s</TD>",
		   stHTMLAucunResultatIntermediaire[]="<I>&nbsp;</I>", 
		   stHTMLFinLigne[]="</TD></TR>\n",
		   stHTMLTableauDebut[]="\
<TABLE BORDER=1 CELLSPACING=1 CELLPADDING=5>\n\
<TR><TH COLSPAN=2>Nombre de plaques et<BR>d'op&eacute;rations n&eacute;cessaires</TH><TH>R&eacute;sultat<BR>interm&eacute;diaire<BR>maximal</TH><TH>N&deg;</TH><TH>Solution</TH></TR>\n",
		   stHTMLTableauFin[]="</TABLE><BR>\n\n",
           stHTMLSolutionsApprocheesDebut[]="<A NAME=\"deb\">\n<H2>%d %d %d %d %d %d - %d : Compte introuvable</H2>\n",
		   stHTMLSolutionsApprocheesFin[]="Voir solutions approch&eacute;es <A HREF=\"%d,%d,%d,%d,%d,%d-%d.html\">%d</A> ou <A HREF=\"%d,%d,%d,%d,%d,%d-%d.html\">%d</A>\n",
           stHTMLDebutCellule[]="<TD>",
		   stHTMLFinCellule[]="</TD>",
		   stHTMLDebutLigne[]="<TR>",
		   stHTMLLigneBlanche[]="<BR><BR>\n",
		   stHTMLFinLigneSansSolution[]="<TD>&nbsp;</TD><TD>&nbsp;</TD></TR>\n",
		   stHTMLComptePrecedent[]="[<A HREF=\"%d,%d,%d,%d,%d,%d-%d.html%s\">Compte pr&eacute;c&eacute;dent</A>]&nbsp;",
		   stHTMLCompteSuivant[]="[<A HREF=\"%d,%d,%d,%d,%d,%d-%d.html%s\">Compte suivant</A>]&nbsp;",
		   stHTMLTirage[]="[<A HREF=\"%d,%d,%d,%d,%d,%d.html\">Tirage</A>]&nbsp;",
		   stHTMLComptePrecedentInactif[]="[Compte pr&eacute;c&eacute;dent]&nbsp;",
		   stHTMLCompteSuivantInactif[]="[Compte suivant]&nbsp;",
		   stHTMLTiragePrecedent[]="[<A HREF=\"%d,%d,%d,%d,%d,%d.html%s\">Tirage pr&eacute;c&eacute;dent</A>]&nbsp;",
		   stHTMLTirageSuivant[]="[<A HREF=\"%d,%d,%d,%d,%d,%d.html%s\">Tirage suivant</A>]&nbsp;",
		   stHTMLTiragePrecedentInactif[]="[Tirage pr&eacute;c&eacute;dent]&nbsp;",
		   stHTMLTirageSuivantInactif[]="[Tirage suivant]&nbsp;"; 
		   
// Statistiques
string stResultatIntermediaireMax, stResultatIntermediaireMaxCLPD;
int iTirage,
    Tirage[NbMaxTirages][NbMaxPlaques],
    Plaque[NbMaxPlaques],
	Repet[NBMAXSOL][3],
    CompteNbMaxSolutions, CompteResultatIntermediaireMax, CompteLePlusDifficile, 
    NbMinSolutionsCLPD, NbMaxPlaquesCLPD, NbMaxOperationsCLPD, 
    NbMaxSolutions, NbComptesTrouves, NbComptesCherches,
	NbSolutionsTirage;

// Types
enum TOption {oFormeInfixee, oOrdreComplexite};
enum TRenvoi {rHaut, rBas};


// Convertisseur de grands nombres

char *stHTMLAjouteSeparateursMilliers(char *szBuffer, const int Nombre)
 {
  int Diviseur, i; 
  for(i=0,Diviseur=1; Nombre/Diviseur; i++,Diviseur*=10)
   {
    sprintf(szBuffer+i+i/3, "%d", (Nombre/Diviseur)%10);
	if (!(i%3)) szBuffer[i+i/3-1]=' ';
    szBuffer[i+i/3+1]=0;
   }
  int t=strlen(szBuffer); 
  for(int j=0; j<=t/2; j++)
   {
    char szBuffer_j_=szBuffer[j];
	szBuffer[j]=szBuffer[t-j-1];
	szBuffer[t-j-1]=szBuffer_j_;
   } 
  return szBuffer; 
 }
 
void EcritLiensPageCompte(const bool AvecRenvoiHautBas, const TRenvoi Renvoi, FILE *f)
 {
  if (Compte>CompteMin)
   fprintf(f, stHTMLComptePrecedent, 
	       Plaque[0], Plaque[1], Plaque[2], Plaque[3], Plaque[4], Plaque[5], Compte-1,
		   stHTMLSuffixeHautBas[Renvoi]);
  else
   fprintf(f, stHTMLComptePrecedentInactif);
  fprintf(f, stHTMLTirage, Plaque[0], Plaque[1], Plaque[2], Plaque[3], Plaque[4], Plaque[5]);
     if (AvecRenvoiHautBas) fprintf(f, "%s", stHTMLHautBas[Renvoi]);
  if (Compte<CompteMax)
   fprintf(f, stHTMLCompteSuivant, 
	       Plaque[0], Plaque[1], Plaque[2], Plaque[3], Plaque[4], Plaque[5], Compte+1,
		   stHTMLSuffixeHautBas[Renvoi]);
  else
   fprintf(f, stHTMLCompteSuivantInactif);
  fprintf(f, stHTMLLigneBlanche);		   
 }

void EcritLiensPageTirage(const bool AvecRenvoiHautBas, const TRenvoi Renvoi, const int iTirage, FILE *f)
 {
  if (iTirage>0)
   fprintf(f, stHTMLTiragePrecedent, 
	       Tirage[iTirage-1][0], Tirage[iTirage-1][1], Tirage[iTirage-1][2], Tirage[iTirage-1][3], Tirage[iTirage-1][4], Tirage[iTirage-1][5],
		   stHTMLSuffixeHautBas[Renvoi]);
  else
   fprintf(f, stHTMLTiragePrecedentInactif);
  if (AvecRenvoiHautBas) fprintf(f, stHTMLHautBas[Renvoi]);
  if (iTirage<NbMaxTirages)
   fprintf(f, stHTMLTirageSuivant, 
           Tirage[iTirage+1][0], Tirage[iTirage+1][1], Tirage[iTirage+1][2], Tirage[iTirage+1][3], Tirage[iTirage+1][4], Tirage[iTirage+1][5],
		   stHTMLSuffixeHautBas[Renvoi]);
  else
   fprintf(f, stHTMLTirageSuivantInactif);
  fprintf(f, stHTMLLigneBlanche);		   
 }

void EcritSolutionsCompteEnCours(FILE *f0)
 {
  char stNomFichier[30];
  sprintf(stNomFichier, "%d,%d,%d,%d,%d,%d-%d.html", 
		  Plaque[0], Plaque[1], Plaque[2], Plaque[3], Plaque[4], Plaque[5], Compte); 
  FILE *f=fopen(stNomFichier, "wt");
  fprintf(f, 
	      stHTMLEnTetePageAvecCompte, 
	      Plaque[0], Plaque[1], Plaque[2], Plaque[3], Plaque[4], Plaque[5], Compte); 
  NbComptesCherches++;
  if (DistanceRecherche) // Résultat approché
   {
    fprintf(f, stHTMLSolutionsApprocheesDebut, 
	        Plaque[0], Plaque[1], Plaque[2], Plaque[3], Plaque[4], Plaque[5], Compte); 
	EcritLiensPageCompte(false, rHaut, f);
	fprintf(f, stHTMLSolutionsApprocheesFin, 
	        Plaque[0], Plaque[1], Plaque[2], Plaque[3], Plaque[4], Plaque[5], Compte-DistanceRecherche, Compte-DistanceRecherche,
		    Plaque[0], Plaque[1], Plaque[2], Plaque[3], Plaque[4], Plaque[5], Compte+DistanceRecherche, Compte+DistanceRecherche); 
	fprintf(f, stHTMLLigneBlanche);
	EcritLiensPageCompte(false, rBas, f);
	fprintf(f, stHTMLLigneBlanche);
	fprintf(f, stHTMLPiedPage, stHTMLCopyright);		
	fprintf(f0, stHTMLFinLigneSansSolution);
   }
  else // Le Compte Est Bon : on affiche les solutions 
   {
    NbComptesTrouves++;
	
	fprintf(f, stHTMLSolutionsTitre, Plaque[0], Plaque[1], Plaque[2], Plaque[3], Plaque[4], Plaque[5], Compte, ComplexitesSolutions.size());
    EcritLiensPageCompte(true, rHaut, f);
	fprintf(f, stHTMLTableauDebut);
	
	// On compte les occurrences pour définir les ROWSPAN
	string stDrnNbPlq, stDrnNbOpr, stDrnResIM;
	int  iSol=0, iDrnNbPlq=0, iDrnNbOpr=0, iDrnResIM=0;
	string stNbPlq, stNbOpr, stResIM;
	for(set<string>::iterator i=ComplexitesSolutions.begin(); i!=ComplexitesSolutions.end(); i++)
     {
	  string s=*i;
	  stNbPlq=s.substr(0, 1);
	  if (stNbPlq==stDrnNbPlq)
	   {
	    Repet[iDrnNbPlq][NBPLQ]++;
		Repet[iSol][NBPLQ]=0;
	   }
	  else
	   {
	    iDrnNbPlq=iDrnNbOpr=iDrnResIM=iSol;
		stDrnNbPlq=stNbPlq; stDrnNbOpr=""; stDrnResIM=""; 
	    Repet[iSol][NBPLQ]=Repet[iSol][NBOPR]=Repet[iSol][RESIM]=1;
	   }
	  
	  stNbOpr=s.substr(2, 1);
	  if (stNbOpr==stDrnNbOpr)
	   {
	    Repet[iDrnNbOpr][NBOPR]++;
		Repet[iSol][NBOPR]=0;
	   }
	  else
	   {
	    iDrnNbOpr=iDrnResIM=iSol;
		stDrnNbOpr=stNbOpr; stDrnResIM="";
		Repet[iSol][NBOPR]=Repet[iSol][RESIM]=1; 
	   }
	   
	  stResIM=s.substr(4, 12);
	  if (stResIM==stDrnResIM)
	   {
	    Repet[iDrnResIM][RESIM]++;
		Repet[iSol][RESIM]=0;
	   }
	  else
	   {
	    iDrnResIM=iSol;
		stDrnResIM=stResIM;
		Repet[iSol][RESIM]=1; 
	   }

	  iSol++;
	 }
	
    // Statistiques
	int NbSol=ComplexitesSolutions.size(),
	    NbPlq=(int)(stNbPlq[0]-'0'),
	    NbOpr=(int)(stNbOpr[0]-'0');
	if ((NbSol<NbMinSolutionsCLPD)||
	    ((NbSol==NbMinSolutionsCLPD)&&
         (NbPlq>NbMaxPlaquesCLPD))||
		((NbSol==NbMinSolutionsCLPD)&&
         (NbPlq==NbMaxPlaquesCLPD)&&
		 (NbOpr>NbMaxOperationsCLPD))||
		((NbSol==NbMinSolutionsCLPD)&&
         (NbPlq==NbMaxPlaquesCLPD)&&
		 (NbOpr==NbMaxOperationsCLPD)&&
		 (stResIM>stResultatIntermediaireMaxCLPD)))
	   {
	    NbMinSolutionsCLPD=NbSol;
	    NbMaxPlaquesCLPD=NbPlq;
		NbMaxOperationsCLPD=NbOpr;
		stResultatIntermediaireMaxCLPD=stResIM;
		CompteLePlusDifficile=Compte;
	   }

	// On écrit le tableau
	iSol=0; 
	string stSol;
	for(set<string>::iterator i=ComplexitesSolutions.begin(); i!=ComplexitesSolutions.end(); i++)
     {
	  string s=*i;
	  fprintf(f, stHTMLDebutLigne);
	  stNbPlq=s.substr(0, 1);
	  if (Repet[iSol][NBPLQ]>0)
	   {
	    fprintf(f, stHTMLRepetition, Repet[iSol][NBPLQ]);
	    for(int j=0; j<stNbPlq.size(); j++) fprintf(f, "%c", stNbPlq[j]);
		fprintf(f, stHTMLNbPlaques);
	   }
	  stNbOpr=s.substr(2, 1);
	  if (Repet[iSol][NBOPR]>0)
	   {
	    fprintf(f, stHTMLRepetition, Repet[iSol][NBOPR]);
	    for(int j=0; j<stNbOpr.size(); j++) fprintf(f, "%c", stNbOpr[j]);
		fprintf(f, stHTMLNbOperations, stHTMLPluriel[stNbOpr[0]>'1']);
	   }
	  stResIM=s.substr(4, 12);
	  if (stResIM>stResultatIntermediaireMax) 
	   {
	    stResultatIntermediaireMax=stResIM;
		CompteResultatIntermediaireMax=Compte;
	   }
	  if (Repet[iSol][RESIM]>0)
	   {
	    fprintf(f, stHTMLRepetitionAligneADroite, Repet[iSol][RESIM]);
	    if (stResIM=="           0")
		 fprintf(f, stHTMLAucunResultatIntermediaire);
		else
		 { 
		  for(int j=0; j<stResIM.size(); j++) fprintf(f, "%c", stResIM[j]);
		 } 
		fprintf(f, stHTMLFinCellule);
	   }
	  fprintf(f, "<TD ALIGN=RIGHT>%d</TD>", iSol+1);
	  fprintf(f, stHTMLDebutCellule);
	  stSol=s.substr(17);
	  for(int j=0; j<stSol.size(); j++) fprintf(f, "%c", stSol[j]);
	  fprintf(f, stHTMLFinLigne);
	  
	  if (i==ComplexitesSolutions.begin())
	   { // Solution la plus simple en en-tête
	    fprintf(f0, stHTMLDebutCellule);
	    for(int j=0; j<stSol.size(); j++) fprintf(f0, "%c", stSol[j]);
		fprintf(f0, "%s%s", stHTMLFinCellule, stHTMLDebutCellule);
	   }
	    	
	  iSol++;
	 }
    // Solution la plus complexe
	for(int j=0; j<stSol.size(); j++) fprintf(f0, "%c", stSol[j]);
	fprintf(f0, stHTMLFinLigne);
	fprintf(f, stHTMLTableauFin);
	EcritLiensPageCompte(true, rBas, f); 
	fprintf(f, stHTMLPiedPage, stHTMLCopyright);
   }
  fclose(f); 
 }

void EcritSolutionsTirageEnCours()
 {
  char stNomFichier[30];
  // Initialisation
  CompteNbMaxSolutions=CompteResultatIntermediaireMax=CompteLePlusDifficile=0;
  NbMaxPlaquesCLPD=NbMaxOperationsCLPD=0;
  NbMaxSolutions=NbComptesTrouves=NbComptesCherches=0;
  NbMinSolutionsCLPD=0x7FFFFFFF; 
  NbSolutionsTirage=0;
  stResultatIntermediaireMax=" ";
  stResultatIntermediaireMaxCLPD=" ";

  sprintf(stNomFichier, "%d,%d,%d,%d,%d,%d.html",  
          Plaque[0], Plaque[1], Plaque[2], Plaque[3], Plaque[4], Plaque[5]);
  FILE *f=fopen(stNomFichier, "wt");
  fprintf(f, 
	      stHTMLEnTetePageSansCompte, 
		  Plaque[0], Plaque[1], Plaque[2], Plaque[3], Plaque[4], Plaque[5], 
		  Plaque[0], Plaque[1], Plaque[2], Plaque[3], Plaque[4], Plaque[5]); 
  EcritLiensPageTirage(true, rHaut, iTirage, f);
  
  TSituation s;
  for(int i=0; i<NbMaxPlaques; i++) 
   s.AjouteNombre(Plaque[i]); 
  printf("Tirage %d %d %d %d %d %d\n",  
          Plaque[0], Plaque[1], Plaque[2], Plaque[3], Plaque[4], Plaque[5]);
  time_t TopChrono=time(NULL);
  for(Compte=CompteMin; Compte<=CompteMax; Compte++)  
   {
    DistanceRecherche=0x7FFFFFFF;
    NbCalculsEnvisages=0;
    Solutions.clear(); ComplexitesSolutions.clear();
	s.Cherche();
	int NbSolutions=ComplexitesSolutions.size();
	if (NbSolutions>NbMaxSolutions) 
	 {
	  NbMaxSolutions=NbSolutions;
	  CompteNbMaxSolutions=Compte;
	 }
	NbSolutionsTirage+=NbSolutions; 
	fprintf(f, stHTMLLigneAvecCompte, Compte, Compte, Plaque[0], Plaque[1], Plaque[2], Plaque[3], Plaque[4], Plaque[5], Compte, NbSolutions);
    EcritSolutionsCompteEnCours(f);
    if (!((Compte-100)%31)) printf(" => Calcul des comptes de %d à %d terminé.\n", Compte-30, Compte);
   }
 fprintf(f, stHTMLTableauFin);
 int Duree=(int)(difftime(time(NULL), TopChrono)); 
 char stCalculsParSeconde[16],  stCalculsEnvisages[16];
 fprintf(f, stHTMLPiedPageDebut, 
			NbComptesTrouves, NbComptesCherches, (100.0*NbComptesTrouves)/NbComptesCherches,
			NbSolutionsTirage, 1.0*NbSolutionsTirage/(1+CompteMax-CompteMin),  
			NbMaxSolutions, CompteNbMaxSolutions, CompteNbMaxSolutions, 
			CompteLePlusDifficile, CompteLePlusDifficile, 
			Duree/60, Duree%60,
			stHTMLAjouteSeparateursMilliers(stCalculsEnvisages, NbCalculsEnvisages),
			stHTMLAjouteSeparateursMilliers(stCalculsParSeconde, NbCalculsEnvisages*(1+CompteMax-CompteMin)/Duree));
 for(int j=0; j<stResultatIntermediaireMax.size(); j++) fprintf(f, "%c", stResultatIntermediaireMax[j]);
 fprintf(f, stHTMLPiedPageFin, CompteResultatIntermediaireMax, CompteResultatIntermediaireMax);
 fprintf(f, stHTMLLigneBlanche);
 EcritLiensPageTirage(true, rBas, iTirage, f);
 fprintf(f, stHTMLPiedPage, stHTMLCopyright);
 fclose(f);
}

int main (int argc, char * const argv[])
 {
  printf(stEntete);
  set<TOption> Options;
  Options.clear();
  Options.insert(oFormeInfixee);
  Options.insert(oOrdreComplexite);

  iTirage=0;
  // On commence par déterminer le nombre de plaques à deux exemplaires (de 0 à 3)
  for(int NbCplPlq=0; NbCplPlq<4; NbCplPlq++)
   {
	//printf("\n---- %d Doublon(s) à partir d'ici -----", NbCplPlq);
	int iValPlq[NbMaxPlaques],
	    iMaxCplPlq; //Il y a au moins deux plaques dans un tirage qui peuvent être répétées

	for(iValPlq[0]=0; iValPlq[0]<NbValPlq+NbCplPlq-5; iValPlq[0]++)
	 {
	  if (iValPlq[0]<NbValPlqDbl) iMaxCplPlq=0;
	  for(iValPlq[1]=iValPlq[0]+1; iValPlq[1]<NbValPlq+NbCplPlq-4; iValPlq[1]++)
	   {
		for(int i=0; i<2; i++) 
		 if (iValPlq[i]<NbValPlqDbl) iMaxCplPlq=i;
	    for(iValPlq[2]=iValPlq[1]+1; iValPlq[2]<NbValPlq+NbCplPlq-3; iValPlq[2]++)
	     {
		  for(int i=0; i<3; i++) 
		   if (iValPlq[i]<NbValPlqDbl) iMaxCplPlq=i;
		  for(iValPlq[3]=iValPlq[2]+1; iValPlq[3]<(NbCplPlq==3?iValPlq[2]+2:NbValPlq+NbCplPlq-2); iValPlq[3]++)
	       {
			for(int i=0; i<4; i++) 
			 if ((NbCplPlq<6-i)&&(iValPlq[i]<NbValPlqDbl)) iMaxCplPlq=i;
			for(iValPlq[4]=iValPlq[3]+1; iValPlq[4]<(NbCplPlq>=2?iValPlq[3]+2:NbValPlq+NbCplPlq-1); iValPlq[4]++)
	         { 
			  for(int i=0; i<5; i++) 
			   if ((NbCplPlq<6-i)&&(iValPlq[i]<NbValPlqDbl)) iMaxCplPlq=i;
	          for(iValPlq[5]=iValPlq[4]+1; iValPlq[5]<(NbCplPlq>=1?iValPlq[4]+2:NbValPlq); iValPlq[5]++)
			   {
			    for(int i=0; i<NbMaxPlaques; i++) 
				 if ((NbCplPlq<6-i)&&(iValPlq[i]<NbValPlqDbl)) iMaxCplPlq=i;
			    // On a les NbMaxPlaques-NbCplPlq plaques, reste à définir les NbCplPlq couples de plaques...
				if (iMaxCplPlq+1>=NbCplPlq) // On vérifie que l'on a assez de plaques à doubler
				 {
				  // On définit les plaques à doubler
		          int iValPlqCpl[NbMaxPlqCpl];
				  for(iValPlqCpl[0]=0; iValPlqCpl[0]<=(NbCplPlq>0?iMaxCplPlq-(NbCplPlq-1):0); iValPlqCpl[0]++)
				   for(iValPlqCpl[1]=iValPlqCpl[0]+1; iValPlqCpl[1]<=(NbCplPlq>1?iMaxCplPlq-(NbCplPlq-2):iValPlqCpl[0]+1); iValPlqCpl[1]++) 
				    for(iValPlqCpl[2]=iValPlqCpl[1]+1; iValPlqCpl[2]<=(NbCplPlq>2?iMaxCplPlq:iValPlqCpl[1]+1); iValPlqCpl[2]++) 
			         {
					  for(int i=0; i<NbMaxPlaques-NbCplPlq; i++)
					   { 
					    int iPlq=i+((NbCplPlq>0)&&(iValPlqCpl[0]<=i))
						          +((NbCplPlq>1)&&(iValPlqCpl[1]<=i))
						          +((NbCplPlq>2)&&(iValPlqCpl[2]<=i));
					    Tirage[iTirage][iPlq]=ValPlq[iValPlq[i]];
						// Plaques doublées
						if ((NbCplPlq>0)&&(iValPlqCpl[0]==i)) Tirage[iTirage][iPlq-1]=ValPlq[iValPlq[i]];
						if ((NbCplPlq>1)&&(iValPlqCpl[1]==i)) Tirage[iTirage][iPlq-1]=ValPlq[iValPlq[i]];
						if ((NbCplPlq>2)&&(iValPlqCpl[2]==i)) Tirage[iTirage][iPlq-1]=ValPlq[iValPlq[i]];
					   }
					   
					  /* switch(NbCplPlq)
					   {
					    case 0: printf("\n%5d) %3d %3d %3d %3d %3d %3d : iValPlq=(%d,%d,%d,%d,%d,%d)",
					                   ++iTirage, Tirage[iTirage][0], Tirage[iTirage][1], Tirage[iTirage][2], Tirage[iTirage][3], Tirage[iTirage][4], Tirage[iTirage][5], 
							           iValPlq[0], iValPlq[1], iValPlq[2], iValPlq[3], iValPlq[4], iValPlq[5]);
							    break;
					    case 1: printf("\n%5d) %3d %3d %3d %3d %3d %3d : iValPlq=(%d,%d,%d,%d,%d) iMaxCplPlq=%d iValPlqCpl=(%d)",
					                   ++iTirage, Tirage[iTirage][0], Tirage[iTirage][1], Tirage[iTirage][2], Tirage[iTirage][3], Tirage[iTirage][4], Tirage[iTirage][5], 
							           iValPlq[0], iValPlq[1], iValPlq[2], iValPlq[3], iValPlq[4], iMaxCplPlq, iValPlqCpl[0]);
							    break;
					    case 2: printf("\n%5d) %3d %3d %3d %3d %3d %3d : iValPlq=(%d,%d,%d,%d) iMaxCplPlq=%d iValPlqCpl=(%d,%d)",
					                   ++iTirage, Tirage[iTirage][0], Tirage[iTirage][1], Tirage[iTirage][2], Tirage[iTirage][3], Tirage[iTirage][4], Tirage[iTirage][5], 
							           iValPlq[0], iValPlq[1], iValPlq[2], iValPlq[3], iMaxCplPlq, iValPlqCpl[0], iValPlqCpl[1]);
							  break;
					    case 3: printf("\n%5d) %3d %3d %3d %3d %3d %3d : iValPlq=(%d,%d,%d) iMaxCplPlq=%d iValPlqCpl=(%d,%d,%d)",
					                   ++iTirage, Tirage[iTirage][0], Tirage[iTirage][1], Tirage[iTirage][2], Tirage[iTirage][3], Tirage[iTirage][4], Tirage[iTirage][5], 
							           iValPlq[0], iValPlq[1], iValPlq[2], iMaxCplPlq, iValPlqCpl[0], iValPlqCpl[1], iValPlqCpl[2]);
							    break;
					   } */
					   
					  iTirage++; 
					 }
		         }
			   }
			 }
		   }
	     }
	   }
     }
   }
   
  // On crée les pages
  for(iTirage=0; iTirage<NbMaxTirages; iTirage++)
   {
    time_t TopChrono=time(NULL);
    for(int j=0; j<NbMaxPlaques; j++)
	 Plaque[j]=Tirage[iTirage][j];
	EcritSolutionsTirageEnCours();
	printf("Temps : %.0f secondes.\n\n", difftime(time(NULL), TopChrono));
   }
  return 0;
 }
