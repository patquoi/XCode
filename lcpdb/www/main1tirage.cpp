#include <stdio.h>

#include "IA.h"

#define DEBUT 0
#define FIN   1

#define NBPLQ 0
#define NBOPR 1
#define RESIM 2

#define NBMAXSOL 1000

const char stEntete[]="\
lcpdbwww - Le Compte Pour De Bon version html - Patrice Fouquet, 2006.\n",
           stSyntaxe[]="\
Syntaxe : lcpdbwww Plq1 Plq2 Plq3 Plq4 Plq5 Plq6\n",

           stHTMLEnTetePageAvecCompte[]="\
<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\
<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n\
<HTML>\n\
 <HEAD>\n\
  <TITLE>Tirage : %s %s %s %s %s %s - %d</TITLE>\n\
 </HEAD>\n\
<BODY BGCOLOR=\"#FFFFFF\" TEXT=\"#00000\">\n", // LINK=\"#00FFFF\" VLINK=\"#A0A0FF\">\n",
           stHTMLEnTetePageSansCompte[]="\
<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\
<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n\
<HTML>\n\
 <HEAD>\n\
  <TITLE>Tirage : %s %s %s %s %s %s</TITLE>\n\
 </HEAD>\n\
<BODY BGCOLOR=\"#FFFFFF\" TEXT=\"#00000\">\n\
<H2>Tirage : %s %s %s %s %s %s</H2>\n\
<TABLE BORDER=1 CELLSPACING=1 CELLPADDING=5>\n\
<TR><TH>Compte</TH><TH>Solution(s)</TH><TH>Solution la plus simple</TH><TH>Solution la plus complexe</TH></TR>\n",
stHTMLLigneAvecCompte[]="<TR><TD ALIGN=CENTER><A NAME=\"%d\">%d</TD><TD ALIGN=RIGHT><A HREF=\"%s,%s,%s,%s,%s,%s-%d.html\">%lu</A></TD>",
		   stHTMLPiedPage[]="%s\n</BODY>\n</HTML>\n",
           stHTMLPiedPageDebut[]="\
<H3>Statistiques pour ce tirage</H3>\n\
Nombre de comptes trouv&eacute;s (ayant au moins une solution) : %d/%d (%.2f%%)<BR>\n\
Nombre de solutions le plus &eacute;lev&eacute; : %d (compte <A HREF=\"#%d\">%d</A>)<BR>\n\
Le compte le plus difficile : <A HREF=\"#%d\">%d</A><BR>\n\
R&eacute;sultat interm&eacute;diaire le plus &eacute;lev&eacute; : ",
		   stHTMLPiedPageFin[]=" (compte <A HREF=\"#%d\">%d</A>).\n%s\n</BODY>\n</HTML>\n",
		   stHTMLCopyright[]="<BR><BR><I>Copyright &copy; 2007 Patrice Fouquet</I>",
stHTMLSolutionsTitre[]="<H2>%s %s %s %s %s %s - %d : %lu solution(s)</H2>\n",
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
		   stHTMLTableauFin[]="<TABLE>\n\n",
           stHTMLSolutionsApprochees[]="<H2>%s %s %s %s %s %s - %d : Compte introuvable</H2>\nVoir solutions approch&eacute;es <A HREF=\"%s,%s,%s,%s,%s,%s-%d.html\">%d</A> ou <A HREF=\"%s,%s,%s,%s,%s,%s-%d.html\">%d</A>\n",
           stHTMLDebutCellule[]="<TD>",
		   stHTMLFinCellule[]="</TD>",
		   stHTMLDebutLigne[]="<TR>",
		   stHTMLFinLigneSansSolution[]="<TD>&nbsp;</TD><TD>&nbsp;</TD></TR>\n";
int Repet[NBMAXSOL][3];

// Statistiques
string stResultatIntermediaireMax=" ", stResultatIntermediaireMaxCLPD=" ";
int CompteNbMaxSolutions, CompteResultatIntermediaireMax, CompteLePlusDifficile, 
    NbMinSolutionsCLPD=0x7FFFFFFF, NbMaxPlaquesCLPD=0, NbMaxOperationsCLPD=0, 
    NbMaxSolutions=0, NbComptesTrouves=0, NbComptesCherches=0;
 		   
enum TOption {oFormeInfixee, oOrdreComplexite};

void EcritSolutionsCompteEnCours(char * const argv[], FILE *f0)
 {
  char stNomFichier[30];
  sprintf(stNomFichier, "%s,%s,%s,%s,%s,%s-%d.html",  
          argv[1], 
	  	  argv[2], 
		  argv[3], 
		  argv[4], 
		  argv[5], 
		  argv[6],
		  Compte); 
  FILE *f=fopen(stNomFichier, "wt");
  fprintf(f, 
	      stHTMLEnTetePageAvecCompte, 
	      argv[1], 
		  argv[2], 
		  argv[3], 
		  argv[4], 
		  argv[5], 
		  argv[6],
		  Compte); 
  NbComptesCherches++;
  if (DistanceRecherche) // Résultat approché
   {
    fprintf(f, stHTMLSolutionsApprochees, 
	        argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], Compte,
            argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], Compte-DistanceRecherche, Compte-DistanceRecherche,
		    argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], Compte+DistanceRecherche, Compte+DistanceRecherche); 
	fprintf(f, stHTMLPiedPage, stHTMLCopyright);		
	fprintf(f0, stHTMLFinLigneSansSolution);
   }
  else // Le Compte Est Bon : on affiche les solutions 
   {
    NbComptesTrouves++;
	
	fprintf(f, stHTMLSolutionsTitre, argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], Compte, ComplexitesSolutions.size());
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
	fprintf(f, stHTMLPiedPage, stHTMLCopyright);
   }
  fclose(f); 
 }

int main (int argc, char * const argv[])
 {
  printf(stEntete);
  if (argc<NbMaxPlaques+1)
   {
    printf("Nombre d'arguments insuffisants\n\n%s", stSyntaxe); 
    return 1;
   }
   
  set<TOption> Options;
  Options.clear();
  Options.insert(oFormeInfixee);
  Options.insert(oOrdreComplexite);
  DistanceRecherche=0x7FFFFFFF;
  NbCalculsEnvisages=0;
   
  char stNomFichier[30];
  sprintf(stNomFichier, "%s,%s,%s,%s,%s,%s.html",  
          argv[1], 
	  	  argv[2], 
		  argv[3], 
		  argv[4], 
		  argv[5], 
		  argv[6]);
  FILE *f=fopen(stNomFichier, "wt");
  fprintf(f, 
	      stHTMLEnTetePageSansCompte, 
	      argv[1], 
		  argv[2], 
		  argv[3], 
		  argv[4], 
		  argv[5], 
		  argv[6],
		  argv[1], 
		  argv[2], 
		  argv[3], 
		  argv[4], 
		  argv[5], 
		  argv[6]); 
  
  TSituation s;
  for(int i=1; i<=NbMaxPlaques; i++) 
   s.AjouteNombre(atoi(argv[i])); 
  printf("Tirage %s %s %s %s %s %s",  
          argv[1], 
	  	  argv[2], 
		  argv[3], 
		  argv[4], 
		  argv[5], 
		  argv[6]);
  for(Compte=101; Compte<1000; Compte++)  
   {
    DistanceRecherche=0x7FFFFFFF;
    NbCalculsEnvisages=0;
    if (Compte%10==1) printf("\nCompte ");
    printf("%d ", Compte);
    Solutions.clear(); ComplexitesSolutions.clear();
	s.Cherche();
	if (ComplexitesSolutions.size()>NbMaxSolutions) 
	 {
	  NbMaxSolutions=ComplexitesSolutions.size();
	  CompteNbMaxSolutions=Compte;
	 }
	fprintf(f, stHTMLLigneAvecCompte, Compte, Compte, argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], Compte, ComplexitesSolutions.size());
    EcritSolutionsCompteEnCours(argv, f);
   }
 fprintf(f, stHTMLTableauFin); 
 fprintf(f, stHTMLPiedPageDebut, 
			NbComptesTrouves, NbComptesCherches, (100.0*NbComptesTrouves)/NbComptesCherches, 
			NbMaxSolutions, CompteNbMaxSolutions, CompteNbMaxSolutions, 
			CompteLePlusDifficile, CompteLePlusDifficile);
 for(int j=0; j<stResultatIntermediaireMax.size(); j++) fprintf(f, "%c", stResultatIntermediaireMax[j]);
 fprintf(f, stHTMLPiedPageFin, CompteResultatIntermediaireMax, CompteResultatIntermediaireMax, stHTMLCopyright);
 fclose(f);
 printf("\nTerminé.\n");
 return 0;
}
