#ifndef LIAISON_H_
#define LIAISON_H_

/*!
 * \file Liaison.h
 * \brief Gère les liaisons entre les réservoirs
 * \author Sophie Jacquin & Guillaume Pataut
 */	


#include "lien.h"
#include "Reservoir.h"


/*!
 * \class Liaison
 * \brief classe représentant une liaison entre deux réservoirs
 */
class Liaison
{
private:
	int reservoirIni;/*!< Réservoir en amont*/
	int reservoirFin;/*!< Réservoir en aval*/
	int nbLiens;/*!< Nombre de liens dans la liaison*/
	int longueur;/*!< Temps de parcours*/
	int minTpsConstant;/*!< Temps minimum de fonctionnement*/
	double deltaUp;/*!< Variation maximum du débit*/
	double deltaLo;/*!< Variation minimum du débit*/
	int numero;/*!< Numéro de la liaison*/
	vector<Lien> listeLiens;/*!< Vecteur des liens*/
	int nbPlant;
	static int compteur;

public:
	// Constructeurs :
	/*!
	 * \brief Constructeur
	 *
	 * Constructeur de la classe Liaison
	 *
	 * \param _reservoirIni : réservoir en Amont
	 * \param _reservoirFin : réservoir en Aval
	 * \param _nbLiens : nombre de liens de la liaison
	 * \param _longueur : temps de parcours
	 * \param _deltaUp : variation max du débit
	 * \param _deltaLo : variation min du débit
	 * \param _tempsMinConstant : Temps minimum de fonctionnement
	 */
	Liaison(int _reservoirIni, int _reservoirFin, int _nbLiens, vector<Lien> _listeLiens, int _longueur, double _deltaUp, double _deltaLo, int _tempsMinConstant)
	{
		reservoirIni=_reservoirIni;
		reservoirFin=_reservoirFin;
		nbLiens=_nbLiens;
		listeLiens=_listeLiens;
		longueur=_longueur;
		deltaUp=_deltaUp;
		deltaLo=_deltaLo;
		minTpsConstant=_tempsMinConstant;
		numero=compteur;
		nbPlant=0;
		for(int i=0;i<nbLiens;i++)
		{
			if(listeLiens[i].getNbTurbines()+listeLiens[i].getNbPompes()>0)
				nbPlant++;
		}
		++compteur;

	}
	
	//Accesseurs :
	/*!
	 * \brief Donne le numéro de la liaison
	 *
	 * \return un entier : numero
	 */
	int getNumero() const
	{
		return numero;
	}
	
	/*!
	 * \brief Donne le nombre de liens
	 *
	 * \return un entier : nbLiens
	 */
	int getNbLiens()
	{
		return nbLiens;
	}
	
	/*!
	 * \brief Donne un lien spécifique constant
	 *
	 * \param i : entier, numéro du lein à forunir
	 * \return le ième lien de la liaison
	 */	
	Lien getLien(int i) const
	{
		return listeLiens[i];
	}

	/*!
	 * \brief Donne par référence un lien spécifique
	 *
	 * \param i : entier, numéro du lein à forunir
	 * \return le ième lien de la liaison
	 */	
	Lien & getLien(int i)
	{
		return listeLiens[i];
	}

	/*!
	 * \brief Donne les liens de la liaison
	 *
	 * \return le vecteur des liens : listeLiens
	 */
	vector<Lien> & getLiens()
	{
		return listeLiens;
	}

	/*!
	 * \brief Ajoute un lien
	 *
	 * \param _L : lien à ajouter
	 */
	void adLien(const Lien & _L)
	{
		nbLiens++;
		listeLiens.push_back(_L);
		if(_L.getNbTurbines()+_L.getNbPompes()>0)
			nbPlant++;
	}

	/*!
	 * \brief Donne le débit minimum de la liaison
	 *
	 * \return un double : Qmin
	 */
	double getQmin()
	{
		int qmin=listeLiens[0].getQmin();
		int i;
		for(i=1;i<nbLiens;i++)
			qmin+=listeLiens[i].getQmin();
	
		return qmin;
	}
	
	/*!
	 * \brief Donne le débit maximum de la liaison
	 *
	 * \param V : double qui permet de définir les Qmax pour les turbines et pompes
	 * \return un double : Qmax
	 */
	double getQmax(double V)
	{
		int qmax=0;
		int i;
		for(i=0;i<nbLiens;i++)
			qmax+=listeLiens[i].getQmax(V);

		return qmax;
	}


	double getMaxQmax()
	{
		// cout <<" liaisons"<< endl;
		double rep = 0.0;
		for (int i=0; i<nbLiens; i++)
		{
			rep = max(rep,listeLiens[i].getMaxQmax());
			// cout<< rep <<endl;
		}
		return rep ;
	}
	/*!
	 * \brief Donne le temps minimum de fonctionnement
	 *
	 * \return un entier : minTpsConstant
	 */
	int & getMinTpsConstant() 
	{
		return minTpsConstant;
	}

	/*!
	 * \brief Donne la variation minimum du débit
	 *
	 * \return un double : deltaLo
	 */
	double getDeltaLo() const
	{
		return deltaLo;
	}

	/*!
	 * \brief Donne la variation maximum du débit
	 *
	 * \return un double : deltaUp
	 */
	double getDeltaUp() const
	{
		return deltaUp;
	}

	/*!
	 * \brief Donne le temps de parcours
	 *
	 * \return un entier : longueur
	 */
	int & getLongueur()
	{
		return longueur;
	}

	/*!
	 * \brief Donne le réservoir en amont
	 *
	 * \return un entier : reservoirIni
	 */
	int getReservoirIni() const
	{
		return reservoirIni;
	}

	/*!
	 * \brief Donne le réservoir en aval
	 *
	 * \return un entier : reservoirFin
	 */
	int getReservoirFin() const
	{
		return reservoirFin;
	}

	//autre
	/*!
	 * \brief Détermine si la liaison doit s'arrêter dans le temps
	 *
	 * \return true si un des liens possède un arret programmé, false sinon
	 */
	bool arretProgramme() const
	{
		bool ret = false;
		for (int i=0; i<nbLiens; i++)
		{
			if (listeLiens[i].arretProgramme())
				ret = true;
		}

		return ret;
	}

	/*!
	 * \brief Détermine si la liaison possède une turbine ou une pompe en conflit.
	 *
	 * \return true si un des liens possède une turbine ou une pompe en conflit; false sinon
	 */
	bool contrainteDincompatibilite() const
	//inutile car si incompatibilité=> 1 seul Lien
	{
		
		int cont=0;
		int rep=false;
		while(!rep && cont<nbLiens)
		{
			rep=listeLiens[cont].contrainteDincompatibilite();
			cont++;
		}
	
		return rep;
	}
	
	/*!
	 * \brief Détermine si la liaison possède une turbine ou une pompe en arrêt à un certain moment
	 *
	 * \param h : heure de test
	 * \return true si la liaison posséde une turbine ou une pompe en arrêt à l'heure h, false sinon
	 */
	bool enArret(int h) const 
	{
		int cont=0;
		bool rep=false;
		while(!rep && cont<nbLiens)
		{
			// cout << " lien " << cont << endl;
			rep=listeLiens[cont].enArret(h);
			cont++;
		}
	
		return rep;
	}
	void addPlant()
	{
		nbPlant=nbPlant+1;
	}
	int get_nbPlant()
	{
		return nbPlant;
	}
	
	/*!
	 * \brief Détermine si la liaison possède une turbine ou une pompe avec contrainte de palier
	 *
	 * La liaison ne possède alors qu'un seul lien
	 *
	 * \return true si le lien possède une contrainte de palier, false sinon
	 */
	bool palier() const
	{
		bool rep=false;
		if(nbLiens==1)
		{
			rep=listeLiens[0].palier();
		}
	
		return rep;
	}
};
int Liaison::compteur=0;

#endif
