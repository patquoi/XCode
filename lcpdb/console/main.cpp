#include "IA.h"

char stEntete[]="\
lcpdb - Le Compte Pour De Bon - Patrice Fouquet, 2006.\n";

char stSyntaxe[]="\
Syntaxe : lcpdb [-Options] Plq1 Plq2 Plq3 Plq4 Plq5 Plq6 CompteÀTrouver\n\
Options :\n\
 I Affichage des solutions sous forme infixée (normale) par ordre alpha\n\
 C Affichage des solutions par ordre de complexité (simple > complexe)\n\
 P Affichage des solutions sous forme de liste (en profondeur) par ordre alpha\n\n\
Toutes les options sont cumulables.\n\
Si aucune option spécifiée, affiche uniquement le nombre de solutions trouvées.\n\
Exemple : lcpdb -ICP 2 5 10 25 50 100 444\n";

enum TOption {oFormeInfixee, oOrdreComplexite, oEnProfondeur};
  
int main(int argc, char * const argv[])
 {
  cout << stEntete;
  if (argc<NbMaxPlaques+2)
   {
    cout << "Nombre d'arguments insuffisants\n\n" << stSyntaxe;
    return 1;
   }
   
  bool AvecOptions=(argv[1][0]=='-'); 
  Compte=atoi(argv[7+!!AvecOptions]);
  
  set<TOption> Options;
  Options.clear();
  
  if (AvecOptions)
   for(int i=1; i<strlen(argv[1]); i++)
    switch(argv[1][i])
     {
      case 'I': cout << "Option I demandée (forme normale infixée).\n";
                Options.insert(oFormeInfixee);
                break;
      case 'C': cout << "Option C demandée (ordre de complexité).\n";
                Options.insert(oOrdreComplexite);
                break;
      case 'P': cout << "Option P demandée (forme de liste en profondeur).\n";
                Options.insert(oEnProfondeur);
                break;
      default:  cout << "/!\\ Option " << argv[1][i] << " inconnue - ignorée.\n";
                break;
     }
  DistanceRecherche=0x7FFFFFFF;
  NbCalculsEnvisages=0;
  
  TSituation s;
  
  for(int i=1+AvecOptions; i<=NbMaxPlaques+AvecOptions; i++) 
   s.AjouteNombre(atoi(argv[i])); 
   
  cout << "Tirage : ";
  for(int i=0; i<s.NbNombres(); i++)
   cout << s.Nombre[i]->stApercuEnonce() << " ";
  cout << "- " <<  Compte << "\n";
  
  s.Cherche();
  
  if (DistanceRecherche)
   cout << "Le compte est approché à± " << DistanceRecherche << " : " << Compte-DistanceRecherche << " ou " << Compte+DistanceRecherche << "...\n";
  else
   cout << "Le compte est bon...\n";
  
  cout << Solutions.size() << " solution(s) trouvée(s).\n";
  if (Options.count(oFormeInfixee))
   for(set<string>::iterator i=Solutions.begin(); i!=Solutions.end(); i++)
    cout << *i << "\n";
  if (Options.count(oEnProfondeur))
   for(set<string>::iterator i=SolutionsProfondeur.begin(); i!=SolutionsProfondeur.end(); i++)
    cout << *i << "\n";
  if (Options.count(oOrdreComplexite))
   for(set<string>::iterator i=ComplexitesSolutions.begin(); i!=ComplexitesSolutions.end(); i++)
    cout << *i << "\n";
  return 0;
}
