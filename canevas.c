// canevas.c   pour projet Genealogie SDA2 2024-25�
//

// ANDRIANIAINA   NY HAJA NIRINA
///////////////////////////////////

#include "base.h"

// Types predefinis
#define LG_MAX  64

typedef struct s_date { Nat jour, mois, annee; } Date;

typedef Nat Ident;
#define omega  0

typedef struct s_individu {
	Car nom[LG_MAX];
	// a completer ici
	Date naiss, deces;
	Ident id, ipere, imere, icadet, ifaine;
} *Individu;

typedef struct s_genealogie {
	Individu* tab;				// tableau des individus triés par nom
	Nat nb_individus, id_cur, tab_max_tab,taille_max_rang;
	Nat *rang;
} *Genealogie;

// DEFINIR ICI VOS CONSTANTES


// PARTIE 1: PROTOTYPES des operations imposees
Ent compDate(Date d1, Date d2);
void genealogieInit(Genealogie* g);
void genealogieFree(Genealogie* g);
Individu nouvIndividu(Ident i, Chaine s, Ident p, Ident m, Date n, Date d);
void freeIndividu(Individu id);
// selecteurs
Chaine nomIndividu(Individu ind);
Date naissIndividu(Individu ind);
Nat cardinal(Genealogie g);
Individu kieme(Genealogie g, Nat k);
Individu getByIdent(Genealogie g, Ident i);
Nat getPos(Genealogie g, Chaine name);
Individu getByName(Genealogie g, Chaine name, Date naissance);
// modificateurs
Ident adj(Genealogie g, Chaine s, Ident p, Ident m, Date n, Date d);
void insert(Genealogie g, Nat pos, Chaine s, Ident p, Ident m, Date n, Date d);
void adjFils(Genealogie g, Ident idx, Ident fils, Ident pp, Ident mm);

//PARTIE 2: PROTOTYPES des operations imposees
void affiche_freres_soeurs(Genealogie g, Ident x, Chaine buff);
void affiche_enfants(Genealogie g, Ident x, Chaine buff);
void affiche_cousins(Genealogie g, Ident x, Chaine buff);
void affiche_oncles(Genealogie g, Ident x, Chaine buff);

//PARTIE 2: PROTOTYPES des operations imposees
void deviennent_freres_soeurs(Genealogie g, Ident x, Ident y);
void devient_pere(Genealogie g, Ident x, Ident y);
void devient_mere(Genealogie g, Ident x, Ident y);

//PARTIE 4: PROTOTYPES des operations imposees
Bool estAncetre(Genealogie g, Ident x, Ident y);
Bool ontAncetreCommun(Genealogie g, Ident x, Ident y);
Ident plus_ancien(Genealogie g, Ident x);
void affiche_parente(Genealogie g, Ident x, Chaine buf);
void affiche_descendance(Genealogie g, Ident x, Chaine buf);

// PROTOTYPES DE VOS FONCTIONS INTERMEDIAIRES


/// PARTIE 1: Construction de l�arbre g�n�alogique et acc�s 
/// ///////////////////////////////////////////////////////
/// 

//PRE: None
Ent compDate(Date d1, Date d2)
{
	if(d1.annee < d2.annee) return -1;
	if(d1.annee > d2.annee) return 1;
	if(d1.mois < d2.mois) return -1;
	if(d1.mois > d2.mois) return 1;
	if(d1.jour < d2.jour) return -1;
	if(d1.jour > d2.jour) return 1;
	return 0;
}

//PRE: None
void genealogieInit(Genealogie *g)
{
	*g = MALLOC(struct s_genealogie);
	if(*g == NULL)	{return "Erreur d'allocation mémoire pour Généalogie. \n"; abort();}
	(*g)->nb_individus = 0;
	(*g)->id_cur = 1;
	
	// Allocation mémoire pour le tableau de pointeur des individus
	(*g)->tab_max_tab = 12;
	(*g)->tab = (Individu*)malloc((*g)->tab_max_tab * sizeof(Individu));
	if((*g)->tab == NULL) {return "Erreur d'allocation mémoire pour les Individus.\n"; abort();}
	
	// Allocation mémoire pour les rangs
	(*g)->taille_max_rang = (*g)->tab_max_tab;
	(*g)->rang = (Nat*)malloc((*g)->taille_max_rang * sizeof(Nat));
	if((*g)->rang == NULL) {return "Erreur d'allocation mémoire pour les rangs.\n" ;abort();}
}

//PRE: None
void genealogieFree(Genealogie *g)
{
	free(*g);
	// *g = NULL;
}

//PRE: None
Individu nouvIndividu(Ident i, Chaine s, Ident p, Ident m, Date n, Date d)
{
	Individu nouveau = MALLOC(struct s_individu);
	nouveau->id = i;
	// Affecter le nom
	strncpy(nouveau->nom, s, LG_MAX - 1);
	nouveau->nom[LG_MAX - 1] = '\0';
	nouveau->ipere = p;
	nouveau->imere = m;
	nouveau->naiss = n;
	nouveau->deces = d;
	nouveau->ifaine = omega;
	nouveau->icadet = omega;
	return nouveau;
}

//PRE: None
// Supprimer un individu id
void freeIndividu(Individu id)
{
	if(id == NULL){ return ;}
	free(id);
}

// Selecteur
// ////////////////////
Chaine nomIndividu(Individu ind) { return ind->nom; }
Date naissIndividu(Individu ind) { Date d = { ind->naiss.jour,ind->naiss.mois,ind->naiss.annee }; return d; }
Nat cardinal(Genealogie g) { return g->nb_individus; }
Individu kieme(Genealogie g, Nat k) {
	if(k >=g->nb_individus)  return NULL;
	return g->tab[k];
}

//PRE: None
Individu getByIdent(Genealogie g, Ident i)
{
	if(g==NULL || g->tab == NULL) return NULL;

	for(Nat k = 0; k < g->nb_individus; k++){
		if(g->tab[k]!=NULL && g->tab[k]->id == i) return g->tab[k];
	}
	return NULL;
}

//PRE: None
Nat getPos(Genealogie g, Chaine name)
{
	if(g == NULL || g->tab == NULL || name == NULL) return (Nat)-1;

	Nat debut = 0;
	Nat fin = g->nb_individus;
	
	while(debut < fin){
		Nat milieu = (debut + fin)/2;
		if(g->tab[milieu] == NULL || g->tab[milieu]->nom == NULL) return 0;

		if(chaineCompare(g->tab[milieu]->nom, name) == 0) {
			while (milieu > 0 && g->tab[milieu -1 ] != NULL && chaineCompare(g->tab[milieu -1]->nom, name) == 0)
			{
				milieu--;
			}
			return milieu;
		}
		else if(chaineCompare(g->tab[milieu]->nom, name) > 0){
			// recherche dans la moitié gauche du tableau
			fin = milieu;
		}else {
			// recherche dans la moitié droite
			debut = milieu + 1;
		}
	}
	// Renvoie la position de name si il n'est pas dans le tableau
	return debut;
}

//PRE: None ##################### ----- Recherche dichotomique ----- #######################################
Individu getByName(Genealogie g, Chaine name, Date naissance)
{
	if(g == NULL || g->tab == NULL) return NULL;
	
	Nat pos = getPos(g, name);			// position du name

	// Individu non trouvé
	if(pos == g->nb_individus) return NULL;		
	if(chaineCompare(g->tab[pos]->nom, name) != 0) return NULL;

	// Recherche du plus jeune avec nom = name
	if(naissance.jour == 0 && naissance.mois == 0 && naissance.annee == 0 )
	{
		Individu jeune = g->tab[pos];
		Nat i = pos + 1;

		// Parcourir les individus ayant le même nom
		while(i < g->nb_individus && chaineCompare(g->tab[i]->nom, name)==0){
			Date currentDate = g->tab[i]->naiss;
			Date jeuneDate = jeune->naiss;

			if(compDate(currentDate, jeuneDate)>0){
				jeune = g->tab[i];
			}
			i++;
		}
		return jeune;
	}

	// Si la date de naissance est spécifiée
	Nat i = pos;
	while(i < g->nb_individus && chaineCompare(g->tab[i]->nom, name) == 0){
		if( g->tab[i]->naiss.jour == naissance.jour &&
			g->tab[i]->naiss.mois == naissance.mois &&
			g->tab[i]->naiss.annee == naissance.annee ) {
				return g->tab[i];
			}
		i++;
	}

	return NULL;

	// DONE
}

//PRE: (pos>=1 => chaineCompare(g[pos-1]->nom,s)<=0) 
//     && (pos<g->nb_individus-1 => chaineCompare(g[pos+1]->nom,s)>=0)
void insert(Genealogie g, Nat pos, Chaine s, Ident p, Ident m, Date n, Date d)
{
	if(g == NULL || g->tab == NULL) return;		// problème d'allocation

	// Si l'individu existe déjà
	if(getByName(g, s, n) != NULL) return;

	//Agrandir le tableau induvidu et rang Dans le cas où la position est supérieur au nombre d'individu
	if(g->nb_individus == g->tab_max_tab){
		g->tab = realloc(g->tab, (g->tab_max_tab*2) * sizeof(Individu));
		if(g->tab == NULL) return "Erreur de réallocation mémoire lors de l'insertion.\n";
		g->tab_max_tab *= 2;

		g->rang = realloc(g->rang, g->tab_max_tab * sizeof(Nat));
		if (g->rang == NULL) return "Erreur de réaloction mémoire pour le tableau des rangs.\n";
		g->taille_max_rang = g->tab_max_tab;
	}

	// Création de l'individu
	Individu nouveau = nouvIndividu(g->id_cur, s, p, m, n, d);
	g->id_cur++;
	g->nb_individus++;

	// Décalage des éléments 
	Nat i = g->nb_individus-1; // Commence à la dernière position valide
	while (i > pos) {
		g->tab[i] = g->tab[i - 1];
		g->rang[i] = g->rang[i - 1];
		i--;
	}

	// Insertion de l'individu
	g->tab[pos] = nouveau;
	g->rang[pos] = pos;

	// DONE
}

// PRE: getByIdent((g,x)!=NULL) && getByIdent(g,filsa)!=NULL &&  (pp!=omega || mm!=omega)
/**
 * @brief ajouter fils qui est le fils ainé de p || m
 * Fils ainé est la tête de la liste
 * 
 */
void adjFils(Genealogie g, Ident idx, Ident fils, Ident pp, Ident mm)
{
	if(g == NULL || g->tab == NULL) return;		// problème d'allocation

	Individu enfant = getByIdent(g, idx);	// Récupère l'individu correspondant à l'identifiant idx
	if(enfant == NULL ) return ;

	// Si le père est spécifié
	if(pp != omega){
		Individu pere = getByIdent(g, pp);	// accès au père

		// Si le père n'a pas encore de fils
		if(pere->ifaine == omega){
			pere->ifaine = fils;
			enfant->icadet = omega;	// Pas de cadet
		}else{
			// Insérer idx dans la liste triée des enfants
			Ident courant= pere->ifaine;
			Individu filsCourant = getByIdent(g, courant);	// Accès au fils ainé
			Ident precedent = omega;

			/**
			 * On parcourt la liste des enfants du père
			 * jusqu'à trouver la position d'insertion
			 * ou jusqu'à atteindre la fin de la liste
			 * 
			 * Si le fils ainé est plus jeune que l'enfant => compDate(aine, enfant) < 0
			 * on le place en tête de liste
			 * 
			 */
			while(courant!= omega && compDate(filsCourant->naiss, enfant->naiss) < 0 && chaineCompare(filsCourant->nom, enfant->nom) != 0){
				precedent = courant;	// Parcours de la liste des enfants
				courant= filsCourant->icadet;	// Accès au fils ainé
				filsCourant = getByIdent(g, courant);	// Accès au fils ainé
			}

			if(precedent == omega){
				// Si le fils ainé est plus jeune que l'enfant
				pere->ifaine = idx;	// idx devient le fils ainé
				enfant->icadet = courant;	// Le fils ainé devient le cadet
			}else{
				// Réajustement de la liste après l'insertion du nouvel enfant
				filsCourant = getByIdent(g, precedent);	// Accès au fils cadet
				if(filsCourant->icadet == idx) return;	// Si l'enfant est déjà dans la liste	
				enfant->icadet = filsCourant->icadet;	// Le fils ainé devient le cadet
				filsCourant->icadet = idx;	// Le fils cadet devient le fils ainé
			}
		}
	}

	// Si la mère est spécifiée
	if(mm != omega){
		Individu mere = getByIdent(g, mm);	// accès au mère
		if(mere == NULL) return;			// Si la mère n'existe pas

		// Si la mère n'a pas encore de fils
		if(mere->ifaine == omega){
			mere->ifaine = fils;	// idx devient le fils ainé
			enfant->icadet = omega;	// Pas de cadet
		}else{
			// Insérer idx dans la liste triée des enfants
			Ident courant = mere->ifaine;
			Individu filsCourant = getByIdent(g, courant);	// Accès au fils ainé
			Ident precedent = omega;

			while(courant!= omega && compDate(filsCourant->naiss, enfant->naiss) < 0 && chaineCompare(filsCourant->nom, enfant->nom) != 0){
				precedent = courant;	// Parcours de la liste des enfants
				courant = filsCourant->icadet;	// Accès au fils ainé
				filsCourant = getByIdent(g, courant);	// Récupére le prochain enfant
			}

			if(precedent == omega){
				// Si le fils ainé est plus jeune que l'enfant
				mere->ifaine = idx;	// idx devient le fils ainé
				enfant->icadet = courant;	// Le fils ainé devient le cadet de idx
			}else{
				// Réajusement de la liste après l'insertion d'un nouvel enfant
				filsCourant = getByIdent(g, precedent);	// Accès au fils cadet	
				if(filsCourant->icadet == idx) return;	// Si l'enfant est déjà dans la liste)
				enfant->icadet = filsCourant->icadet;	// Le fils ainé devient le cadet
				filsCourant->icadet = idx;	// Le fils cadet devient le fils ainé
			}
		}
	}
}

//PRE:  (p==omega || getByIdent(g,p)!=NULL) && (m==omega || getByIdent(g,m)!=NULL) &&
//      !freres_soeurs(g,p,m) && getByIdent(g,p)->ifaine == getByIdent(g,m)->ifaine &&
//      compDate(getByIdent(g,p)->naiss,n)<0 && compDate(getByIdent(g,m)->naiss,n)<0
Ident adj(Genealogie g, Chaine s, Ident p, Ident m, Date n, Date d)
{
	// valeurs interdites
	if (s == NULL || s[0] == 0 || n.jour == 0 || n.mois == 0 || n.annee == 0) return omega;

	if(g==NULL)	genealogieInit(&g);
	
	// Si l'individu existe déjà
	Individu ind = getByName(g, s, n);
	if(ind != NULL) return omega;

	Nat pos = getPos(g, s);	// position du nom dans le tableau

	insert(g, pos, s, p, m, n, d);	// insertion de l'individu

	Ident nouvelId = g->id_cur - 1;	// Récupère l'id du nouvel individu

	// Si le père et la mère sont spécifiés
	if(p != omega && m != omega){
		adjFils(g, nouvelId, nouvelId, p, m);
		return nouvelId;
	}

	// Si le père seulement est spécifié
	if(p != omega){
		adjFils(g, nouvelId, nouvelId, p, omega);
	}
	// Si la mère seulement est spécifiée
	if(m != omega){
		adjFils(g, nouvelId, nouvelId, omega, m);
	}

	return nouvelId;
}


// 
/// PARTIE 2: Affichages 
/// ///////////////////////////////////////////////////////
/// 

//PRE: None
void affiche_freres_soeurs(Genealogie g, Ident x, Chaine buff)
{
	if( g == NULL || buff == NULL) return;
	buff[0] = '\0';
	Individu ind = getByIdent(g, x);
	if(ind == NULL) return;
	Individu pere = getByIdent(g, ind->ipere);
	/**
	 * On parcours la liste des enfants du père
	 */
	if(pere != NULL){
		Individu filsAine = getByIdent(g, pere->ifaine);	// Récupere le fils ainé de la fratrie
		while(filsAine != NULL){
			if(filsAine->id != x){
				chaineCopie(buff, nomIndividu(filsAine));
				buff = buff + strlen(buff);	// On avance le buffer
				buff[0] = ' ';	// On ajoute un espace entre les noms
				buff++;
				buff[0] = '\0';	// On termine la chaine
			}
			filsAine = getByIdent(g, filsAine->icadet);
		}
	}
}

//PRE: None
void affiche_enfants(Genealogie g, Ident x, Chaine buff)
{
	if(g == NULL || buff == NULL) return;
	buff[0] = '\0';
	Individu parent = getByIdent(g, x);
	if(parent == NULL) return;

	Individu filsAine = getByIdent(g, parent->ifaine);	// Récupere le fils ainé de la fratrie
	while(filsAine != NULL){
		chaineCopie(buff, nomIndividu(filsAine));
		buff = buff + strlen(buff);	// On avance le buffer
		buff[0] = ' ';	// On ajoute un espace entre les noms
		buff++;
		buff[0] = '\0';	// On termine la chaine
		filsAine = getByIdent(g, filsAine->icadet);	// Accès au fils ainé
	}
}

//PRE: None
void affiche_cousins(Genealogie g, Ident x, Chaine buff)
{
	if(g == NULL || buff == NULL) return;

	Individu ind = getByIdent(g, x);
	if(ind == NULL) return;

	// Si on connait le père et la mère
	if(ind->ipere != omega && ind->imere != omega) {
		Individu pere = getByIdent(g, ind->ipere);
		Individu grand_perePtrnl = getByIdent(g, pere->ipere);
		Individu filsAine = getByIdent(g, grand_perePtrnl->ifaine);
				// Récupere le fils ainé de la fratrie
		while(filsAine != NULL){
			if(filsAine->id != ind->ipere){
				affiche_enfants(g, filsAine->id, buff);
			}
			filsAine = getByIdent(g, filsAine->icadet);	// Accès au fils ainé
		}
	}

	// Si on connaît que le père
	Individu pere = getByIdent(g, ind->ipere);
	Individu grand_perePtrnl = getByIdent(g, pere->ipere);
	
	if(pere != NULL && grand_perePtrnl != NULL){
		Individu filsAine = getByIdent(g, grand_perePtrnl->ifaine);
			// Récupere le fils ainé de la fratrie
		while(filsAine != NULL){
			if(filsAine->id != ind->ipere){
				affiche_enfants(g, filsAine->id, buff);
			}
			filsAine = getByIdent(g, filsAine->icadet);	// Accès au fils ainé
		}
	}

	// Si on connaît que la mère
	Individu mere = getByIdent(g, ind->imere);
	Individu grand_pereMtrnl = getByIdent(g, mere->imere);
	if(mere != NULL && grand_pereMtrnl != NULL){
		Individu filsAine = getByIdent(g, grand_pereMtrnl->ifaine);
			// Récupere le fils ainé de la fratrie
		while(filsAine != NULL){
			if(filsAine->id != ind->imere){
				affiche_enfants(g, filsAine->id, buff);
			}
			filsAine = getByIdent(g, filsAine->icadet);	// Accès au fils ainé
		}
	}
}

//PRE: None
void affiche_oncles(Genealogie g, Ident x, Chaine buff)
{
	if(g == NULL || buff == NULL) return;

	Individu ind = getByIdent(g, x);
	if(ind == NULL) return;

	// Si on connait le père et la mère
	if(ind->ipere != omega || ind->imere != omega) {
		Individu pere = getByIdent(g, ind->ipere);
		Individu mere = getByIdent(g, ind->imere);

		// affiche la fratrie du père
		if(pere->ipere != omega || pere->imere != omega){
			affiche_freres_soeurs(g, pere->id, buff);
		}

		// affiche la fratrie de la mère
		if(mere->ipere != omega || mere->imere != omega){
			affiche_freres_soeurs(g, mere->id, buff);
		}
	}
}

// 
/// PARTIE 3: Créer Liens de parente 
/// ///////////////////////////////////////////////////////
/// 
void deviennent_freres_soeurs(Genealogie g, Ident x, Ident y)
{
	if(g == NULL) return;
	Individu ind1 = getByIdent(g, x);
	Individu ind2 = getByIdent(g, y);
	
	if(ind1 == NULL || ind2 == NULL) return;

	// Si x->ipere != y->ipere && x->imere != y->imere ou les deux n'ont pas encore de parents attribués
	if( (ind1->ipere != omega && ind1->imere != omega && ind2->ipere != omega && ind2->imere != omega) &&
		(ind1->ipere != ind2->ipere && ind1->imere != ind2->imere)) return;	

	// Si x->ipere == omega && x->imere == omega
	if(ind1->ipere == omega && ind1->imere == omega){
		// Ajout de x a la fratrie
		adjFils(g, ind1->id, ind1->id, ind2->ipere, ind2->imere);
	}
	
	// Si y->ipere == omega && y->imere == omega
	if(ind2->ipere == omega && ind2->imere == omega){
		// Ajout de y a la fratrie
		adjFils(g, ind2->id, ind2->id, ind1->ipere, ind1->imere);
	}

}

/**
 * x devient le père de y
 * Prec : x != NULL && y != NULL
 */
void devient_pere(Genealogie g, Ident x, Ident y)
{
	if(g == NULL) return;
	Individu pere = getByIdent(g, x);
	Individu enfant = getByIdent(g, y);

	if( pere == NULL && enfant == NULL ) return ;

	Individu filsAine = getByIdent(g, pere->ifaine);
	
	if(enfant->ipere == omega){
		adjFils(g, enfant->id, enfant->id, x, filsAine->imere);
	}
}


void devient_mere(Genealogie g, Ident x, Ident y)
{
	if(g == NULL) return;
	Individu mere = getByIdent(g, x);
	Individu enfant = getByIdent(g, y);

	if( mere == NULL && enfant == NULL ) return ;

	Individu filsAine = getByIdent(g, mere->ifaine);
	
	if(enfant->imere == omega){
		adjFils(g, enfant->id, filsAine->id, filsAine->ipere, x);
	}
}


// 
/// PARTIE 4: Parcours de la genealogie 
/// ///////////////////////////////////////////////////////
/// 

//PRE: None
Bool estAncetre(Genealogie g, Ident x, Ident y)
{
	if(g == NULL) return false;
	Individu ancetre = getByIdent(g, x);
	Individu descendant = getByIdent(g, y);

	if(ancetre == NULL || descendant == NULL) return false;
	// Si le descendant n'a pas de père ou de mère
	if(descendant->ipere == omega && descendant->imere == omega) return false;

	while(descendant->ipere != omega || descendant->imere != omega){
		if(descendant->ipere == ancetre->id || descendant->imere == ancetre->id) return true;
		// On remonte dans l'arbre
		descendant = getByIdent(g, descendant->ipere);
		if(descendant == NULL) return false;
	}
	return false;
}

//PRE: None
Bool ontAncetreCommun(Genealogie g, Ident x, Ident y)
{
	if(g == NULL) return false;
	Individu ind1 = getByIdent(g, x);
	Individu ind2 = getByIdent(g, y);
	
	if(ind1 == NULL || ind2 == NULL) return false;

	if(x == y) return true;
	return ontAncetreCommun(g, ind1->ipere, y) || ontAncetreCommun(g, ind1->imere, y) ||
		ontAncetreCommun(g, x, ind2->ipere) || ontAncetreCommun(g, x, ind2->imere);
}

//PRE: None
Ident plus_ancien(Genealogie g, Ident x)
{
	Individu ind = getByIdent(g, x);
	Ident ancetre_pere = ind->id;
	Ident ancetre_mere = ind->id;

	if(ind->ipere!=omega){
		ancetre_pere = plus_ancien(g, ind->ipere);
	}
	
	if(ind->imere!=omega){
		ancetre_mere = plus_ancien(g, ind->imere);
	}
	
	// Comparer les dates de naissainces des 2 ancetres pour savoir qui le plus ancien
	if(ancetre_pere != omega && ancetre_mere != omega){
		Individu ancetre1 = getByIdent(g, ancetre_pere);
		Individu ancetre2 = getByIdent(g, ancetre_mere);
		if(compDate(ancetre1->naiss, ancetre2->naiss) < 0){
			return ancetre_pere;
		}else{
			return ancetre_mere;
		}
	}

	if(ancetre_pere != omega) return ancetre_pere;
	if(ancetre_mere != omega) return ancetre_mere;

	return omega;
}

//PRE: None
//PRE: None
void affiche_parente(Genealogie g, Ident x, Chaine buff)
{
	buff[0] = '\0';

    if (g == NULL || buff == NULL) return;

    Individu ind = getByIdent(g, x);
    if (ind == NULL) return;

    // Initialisation des listes pour parcourir les générations
    Ident parents[100]; // Liste des individus de la génération actuelle
    Nat parent_count = 0; // Nombre d'individus dans la génération actuelle

    // Ajouter les parents initiaux (père et mère)
    if (ind->ipere != omega) parents[parent_count++] = ind->ipere;
    if (ind->imere != omega) parents[parent_count++] = ind->imere;

    Ident next_generation[100]; // Liste temporaire pour la génération suivante
    Nat next_gen_count = 0; // Compteur pour la génération suivante

    Nat generation = 1; // Compteur de génération

    while (parent_count > 0) {
        // Afficher la génération actuelle
        chaineCopie(buff, "\nGénération ");
        buff += strlen(buff);
        sprintf(buff, "%u :\n", generation);
        buff += strlen(buff);

        for (Nat i = 0; i < parent_count; i++) {
            Individu parent = getByIdent(g, parents[i]);
            if (parent != NULL) {
                // Afficher le parent
                chaineCopie(buff, nomIndividu(parent));
                buff += strlen(buff);
                buff[0] = ' ';
                buff++;
                buff[0] = '\0';


                affiche_freres_soeurs(g, parent->id, buff);
                buff += strlen(buff);
                buff[0] = ' ';
                buff++;
                buff[0] = '\0';

                // Ajouter les parents du parent à la génération suivante
                if (parent->ipere != omega) {
                    next_generation[next_gen_count++] = parent->ipere;
                }
                if (parent->imere != omega) {
                    next_generation[next_gen_count++] = parent->imere;
                }
            }
        }

        // Préparer la génération suivante
        for (Nat i = 0; i < next_gen_count; i++) {
            parents[i] = next_generation[i];
        }
        parent_count = next_gen_count;
        next_gen_count = 0;
        generation++;
    }
}

//PRE: None
void affiche_descendance(Genealogie g, Ident x, Chaine buff)
{
	buff[0]='\0';

	if(g == NULL || buff == NULL) return;
	Individu ind = getByIdent(g, x);
	if(ind == NULL) return;

	Ident descendance[100]; // Liste de la descendance
	Nat descendance_count = 0; // Compteur de la descendance
	
	Individu filsAine = getByIdent(g, ind->ifaine);
	while(filsAine != NULL){
		descendance[descendance_count++] = filsAine->id;	// Ajout du fils ainé à la liste de descendance
		filsAine = getByIdent(g, filsAine->icadet);
	}
	
	Ident next_generation[100]; // Liste temporaire pour la génération suivante
	Nat next_gen_count = 0; // Compteur pour la génération suivante
	
	Nat generation = 1; // Compteur de génération

	while(descendance_count > 0){
		// Afficher la génération actuelle
		chaineCopie(buff, "\nGénération ");
		buff += strlen(buff);
		sprintf(buff, "%u :\n", generation);
		buff += strlen(buff);

		// Afficher la descendance de chaque enfant
		for(Nat i = 0; i < descendance_count; i++){
			Individu enfant = getByIdent(g, descendance[i]);
			if(enfant != NULL){
				// Afficher l'enfant
				chaineCopie(buff, nomIndividu(enfant));
				buff += strlen(buff);
				buff[0] = ' ';
				buff++;
				buff[0] = '\0';

				Individu petit_filsAine = getByIdent(g, enfant->ifaine);	// Récupere le fils ainé de la fratrie
				while(petit_filsAine != NULL){
					next_generation[next_gen_count++] = petit_filsAine->id;	// Ajout du fils ainé à la liste de descendance
					petit_filsAine = getByIdent(g, petit_filsAine->icadet);
				}
			}
		}

		// Préparer la génération suivante
		for(Nat i = 0; i < next_gen_count; i++){
			descendance[i] = next_generation[i];
		}
		descendance_count = next_gen_count;
		next_gen_count = 0;
		generation++;
	}

}

// 
/// VOS FONCTIONS AUXILIAIRES 
/// ///////////////////////////////////////////////////////
/// 


// 
/// MAIN
/// ///////////////////////////////////////////////////////
/// 
int main()
{
	Car buf[500];
	Genealogie g;

	printf("******* Init:\n\n");
	genealogieInit(&g);
	printf("done.\n");

	printf("\n******* adj+getByIndent:\n");
	Date dnull = { 0,0,0 };

	// Famille Potter
	Date jhen = { 16,2,1867 }; Date jhed = { 21,11,1932 };
	Ident ihep = adj(g,"Henri", 0, 0, jhen, jhed);

	Date jfn = { 30,7,1905 }; Date jfd = { 2,3,1979 };
	Ident ijfl = adj(g,"Fleamont", ihep, 0, jfn, jfd);

	Date jeu = { 12,6,1907 }; Date jed = { 14, 1,1979 };
	Ident ijm = adj(g,"Euphemia", 0, 0, jeu, jed);

	Date jpn = { 27, 3, 1960 }; Date jpd = { 29, 7, 1981 };
	Ident ijp = adj(g, "James", ijfl, ijm, jpn, jpd);

	Date lpn = { 30, 1, 1960 }; Date lpd = { 29, 7, 1981 };
	Ident ilp = adj(g, "Lily", 0, 0, lpn, lpd);

	Date hn = { 31, 7, 1980 };
	Ident ih = adj(g, "Harry", ijp, ilp, hn, dnull);

	// Famille Weasley
	Date an = { 6, 2, 1950 };
	Ident iaw = adj(g, "Arthur", 0, 0, an, dnull);

	Date dpre = { 8,4, 1910 }; Date ddpre = { 23, 10, 1968 };
	Ident ipre = adj(g, "Prewett", 0, 0, dpre, ddpre);

	Date dfab = { 12, 5, 1946 }; Date ddfab = { 21,12, 1982 };
	Ident ifab = adj(g, "Fabian", ipre, 0, dfab, ddfab);

	Date mn = { 30, 10, 1949 };
	Ident imw = adj(g, "Molly", ipre, 0, mn, dnull);

	// ajouter ici les autres individus
	Date gn = { 11, 8, 1981 };
	Ident ig = adj(g, "Ginny", iaw, imw, gn, dnull );  // Ginny

	Date rn = { 1, 3, 1980 };
	Ident ir = adj(g, "Ron", iaw, imw, rn, dnull); // Ron

	Date frdn = { 1, 4, 1978 }; Date dfrd = { 5, 6, 1998 };
	Ident ifred = adj(g, "Fred", iaw, imw, frdn, dfrd); // Fred

	Date grgn = { 1, 4, 1978 }; 
	Ident igrg = adj(g, "George", iaw, imw, grgn, dnull); // George

	Date hrmn = { 19, 9, 1980 };
	Ident ihg = adj(g, "Hermione", 0, 0, hrmn, dnull); // Hermione

	Date blln = { 29, 11, 1970};
	Ident ibill = adj(g, "Bill", iaw, imw, blln, dnull); // Bill
	
	Date lln = { 2, 5, 2008 };
	Ident il = adj(g, "Lily", ih, ig, lln, dnull); // Lily (daughter of Harry)

	Date albn = { 15, 8, 2006 };
	Ident ia2 = adj(g, "Albus", ih, ig, albn, dnull); // Albus
	
	Date jmsn = {1, 5, 2005};
	Ident ij = adj(g, "James", ih, ig, jmsn, dnull); // James (son of Harry)

	Date rsn = { 6, 8, 2006 };
	Ident irose = adj(g, "Rose", ir, ihg, rn, dnull); // Rose


	for (Nat i = 0; i < cardinal(g); i++) {
		printf("%s\n", nomIndividu(kieme(g,i)));
	}
	printf("nb_individus: %d\n", cardinal(g));
	printf("Identifiant de Fabian: %u (must be 9)\n", ifab);
	printf("Identifiant de Arthur: %u (must be 7)\n", iaw);

	printf("\nAdding more people:\n");
	Date dgid = { 7, 2, 1945 }; Date ddgid = { 21,12, 1982 };
	Ident igid = adj(g, "Gideon", 0, 0, dgid, ddgid);
	printf("Linking Gideon as brother of Molly\n");
	deviennent_freres_soeurs(g, igid, imw);
	Date dhugo = { 19,4,2008 };
	Ident ihugo = adj(g, "Hugo", 0, 0, dhugo, dnull);
	printf("Linking Hugo as son of Hermione\n");
	devient_mere(g, ihg, ihugo);
	// nb_individus: 12

	printf("\nTry to add a double Harry:\n");
	Date hu2n = { 31, 7, 1980 };
	Ident ihu2 = adj(g, "Harry", 0, 0, hu2n, dnull);
	if (ihu2 != omega) printf("oh, no! Added double Harry!\n"); else printf("No double Harry: OK!\n");
	printf("Adding more Albus...\n");
	Date ho2n = { 1,8, 1909 };
	Ident itho2 = adj(g, "Albus", 0, 0, ho2n, dnull);
	if (itho2 == omega) printf("what! Could not add 2nd Albus!\n"); else printf("2nd Albus added: OK!\n");
	Date ho3n = { 2,2, 1832 };
	Ident itho3 = adj(g, "Albus", 0, 0, ho3n, dnull);
	if (itho3 == omega) printf("what! Could not add 3rd Albus!\n"); else printf("3rd Albus added: OK!\n");
	Individu idch = getByName(g, "Albus", dnull);
	if (idch != NULL) printf("Youngest Albus, date birth %d:%d:%d\n", naissIndividu(idch).jour, naissIndividu(idch).mois, naissIndividu(idch).annee);
	else printf("what? no Albus! There is a serious problem here...\n");

	printf("nb_individus: %d\n", cardinal(g));

	printf("\n******* fratrie:\n");
	printf("Freres/Soeurs de %s:\n", nomIndividu(getByIdent(g, ig)));
	buf[0] = 0;  affiche_freres_soeurs(g, ig, buf);
	printf("%s\n", buf);
	printf("Freres/Soeurs de %s:\n", nomIndividu(getByIdent(g, imw)));
	buf[0] = 0;  affiche_freres_soeurs(g, imw, buf);
	printf("%s\n", buf);

	printf("\n******* les enfants:\n");
	printf("Enfants de %s:\n", nomIndividu(getByIdent(g, ipre)));
	buf[0] = 0;  affiche_enfants(g, ipre, buf);
	printf("%s\n", buf);
	printf("Enfants de %s:\n", nomIndividu(getByIdent(g, imw)));
	buf[0] = 0;  affiche_enfants(g, imw, buf);
	printf("%s\n", buf);
	printf("Enfants de %s:\n", nomIndividu(getByIdent(g, ir)));
	buf[0] = 0;  affiche_enfants(g, ir, buf);
	printf("%s\n", buf);

	printf("\n******* les cousins:\n");
	printf("Cousins de %s:\n", nomIndividu(getByIdent(g, irose)));
	buf[0] = 0;   affiche_cousins(g, irose, buf);
	printf("%s\n", buf);

	printf("\n******* les oncles et tantes:\n");
	printf("Oncles/tantes de %s:\n", nomIndividu(getByIdent(g, ij)));
	buf[0] = 0;  affiche_oncles(g, ij, buf);
	printf("%s\n", buf);
	printf("Oncles/tantes de %s:\n", nomIndividu(getByIdent(g, ir)));
	buf[0] = 0;  affiche_oncles(g, ir, buf);
	printf("%s\n", buf);


	printf("\n******* les ancetres:\n");
	printf("%s ancetre de %s: %s\n", nomIndividu(getByIdent(g, ijfl)), nomIndividu(getByIdent(g, ia2)), estAncetre(g, ijfl, ia2) ? "oui" : "non");
	printf("%s ancetre de %s: %s\n", nomIndividu(getByIdent(g, ijfl)), nomIndividu(getByIdent(g, irose)), estAncetre(g, ijfl, irose) ? "oui" : "non");

	printf("\n******* les ancetres communs:\n");
	printf("%s et %s ont ancetre commun: %s\n", nomIndividu(getByIdent(g, irose)), nomIndividu(getByIdent(g, ij)), ontAncetreCommun(g, irose, ij) ? "oui" : "non");
	printf("%s et %s ont ancetre commun: %s\n", nomIndividu(getByIdent(g, ih)), nomIndividu(getByIdent(g, ibill)), ontAncetreCommun(g, ih, ibill) ? "oui" : "non");

	printf("\n******* ancetre plus ancien:\n");
	printf("L'ancetre le plus ancien de %s est %s\n", nomIndividu(getByIdent(g, ia2)), nomIndividu(getByIdent(g, plus_ancien(g, ia2))));

	printf("\n******* parente:\n");
	printf("parents de %s\n", nomIndividu(getByIdent(g, ia2)));
	buf[0] = 0; affiche_parente(g, ia2, buf);
	printf("%s\n", buf);

	printf("\n******* descendence:\n");
	printf("descendence de %s\n", nomIndividu(getByIdent(g, ijm)));
	buf[0] = 0;  affiche_descendance(g, ijm, buf);
	printf("%s\n", buf);

	printf("\n******* free:\n");
	genealogieFree(&g);
	printf("fin.(press key)\n");
	fgets(buf, 2, stdin);
	return 0;
}



