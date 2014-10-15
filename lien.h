#ifndef LIEN_H_
#define LIEN_H_

/*!
 * \file lien.h
 * \author Sophie Jacquin & Guillaume Pataut
 */

#include <float.h>

#include "Pompe.h"

/*!
 * \class Lien
 * \brief Classe qui gère les liens "physiques" entre les réservoirs
 */

class Lien{
private:
	double qmin;/*!< Débit minimum */
	double qmax;/*!< Débit maximum */
	vector<Turbine*> listeTurbines;/*!< Vecteur de turbines */
	vector<Pompe*> listePompes;/*!< Vecteur de pompes */
	int nbPompes;/*!< Nombre de pompes */
	int nbTurbines;/*!< Nombre de trubines */

public:
	// Constructeurs
	/*!
	 * \brief Constructeur avec pompes et turbines
	 *
	 * \param _nbPompes : entier, nombre de pompes
	 * \param _nbTurbines : entier, nombre de turbines
	 * \param _pompes : vecteur des pompes
	 * \param _turbines : vecteur des turbines
	 * \param _qmin : double, débit minimum
	 * \param _qmax : double, débit maximum
	 */
	Lien(int _nbPompes, int _nbTurbines, vector<Pompe*> _pompes, vector<Turbine*> _turbines, double _qmin, double _qmax)
	{
		nbPompes=_nbPompes;
		nbTurbines=_nbTurbines;
		listePompes=_pompes;
		listeTurbines=_turbines;
		qmin=_qmin;
		qmax=_qmax;
	}

	/*!
	 * \brief Constructeur par défaut
	 */
	Lien()
	{
		qmin=0;
		qmax=FLT_MAX;
		nbPompes=0;
		nbTurbines=0;
	}

	/*!
	 * \brief Constructeur sans turbine ni pompe
	 *
	 * \param _qmin : double, débit minimum
	 * \param _qmax : double, débit maximum
	 */
	Lien(double _qmin, double _qmax)
	{
		qmin=_qmin;
		qmax=_qmax;
		nbPompes=0;
		nbTurbines=0;
	}

	//accessseurs:
	/*!
	 * \brief Donne le nombre de pompes du lien
	 *
	 * \return un entier : nbPompes
	 */
	int getNbPompes() const
	{
		return nbPompes;
	}

	/*!
	 * \brief Donne le nombre de turbines du lien
	 *
	 * \return un entier : nbTurbines
	 */
	int getNbTurbines() const
	{
		return nbTurbines;
	}

	/*!
	 * \brief Donne le vecteur constant des turbines
	 *
	 * \return un vecteur : listeTurbines
	 */
	const vector<Turbine*> & getListeTurbines() const
	{
		return listeTurbines;
	}
	/*!
	 * \brief Donne le vecteur constant des turbines
	 *
	 * \return un vecteur : listeTurbines
	 */
	const vector<Pompe*> & getListePompes() const
	{
		return listePompes;
	}
	
	/*!
	 * \brief Donne un pointeur sur une turbine précise
	 *
	 * \param i : entier
	 *
	 * \return un pointeur sur la ième turbine du lien
	 */
	Turbine* & getTurbine(int i)
	{
		return listeTurbines[i];
	}
	
	/*!
	 * \brief Donne un pointeur sur une pompe précise
	 *
	 * \param i : entier
	 *
	 * \return un pointeur sur la ième pompe du lien
	 */
	Pompe* & getPompe(int i)
	{
		return listePompes[i];
	}

	/*!
	 * \brief Donne le débit minimum du lien
	 *
	 * \return un double : qmin
	 */
	double getQmin() const
	{
		return qmin;
	}
	double getQminInt(int V)const
	{
		double rep=qmin;
		int i;
		for(i=0;i<nbPompes;i++)
		{
			if(listePompes[i]->getQminInt(V)>rep)
				rep=listePompes[i]->getQminInt(V);
		}

		for(i=0;i<nbTurbines;i++)
		{
			if(listeTurbines[i]->getQminInt(V)>rep)
				rep=listeTurbines[i]->getQminInt(V);
		}

		return rep;
	}

	double getQminVol(double V)const
	{
		double rep=qmin;
		int i;
		for(i=0;i<nbPompes;i++)
		{
			if(listePompes[i]->getQmin(V)>rep)
				rep=listePompes[i]->getQmin(V);
		}

		for(i=0;i<nbTurbines;i++)
		{
			if(listeTurbines[i]->getQmin(V)>rep)
				rep=listeTurbines[i]->getQmin(V);
		}

		return rep;
	}

	/*!
	 * \brief Donne le débit maximum du lien en fonction d'un certain volume
	 *
	 * \param V : double, volume présent
	 *
	 * \return un double : débit maximum
	 */
	double getQmaxBasique()const
	{
		return qmax;
	}
	double getQmax(double V) const
	{
		double rep=qmax;
		int i;
		for(i=0;i<nbPompes;i++)
		{
			if(listePompes[i]->getQmax(V)<rep)
				rep=listePompes[i]->getQmax(V);
		}
	
		for(i=0;i<nbTurbines;i++)
		{
			if(listeTurbines[i]->getQmax(V)<rep)
				rep=listeTurbines[i]->getQmax(V);
		}
	
		return rep;
	}

	double getMaxQmax()
	{
		double rep = 0.0;
		for(int i=0;i<nbPompes;i++)
			rep = max(rep,listePompes[i]->getMaxQmax());
		// cout << rep << endl;

		for(int i=0;i<nbTurbines;i++)
			rep = max(rep,listeTurbines[i]->getMaxQmax());


		// cout << rep << endl;
		if (rep == 0.0)
			rep = qmax;
		
		// cout << rep << endl;
		return rep;

	
	}
	
	double getQmaxInt(int V) const
		{
			double rep=qmax;
			int i;
			for(i=0;i<nbPompes;i++)
			{
				if(listePompes[i]->getQmaxInt(V)<rep)
					rep=listePompes[i]->getQmaxInt(V);
			}

			for(i=0;i<nbTurbines;i++)
			{
				if(listeTurbines[i]->getQmaxInt(V)<rep)
					rep=listeTurbines[i]->getQmaxInt(V);
			}

			return rep;
		}
	
	/*!
	 * \brief Donne le temps minimum de fonctionnement constant
	 *
	 * \return un entier
	 */
	int getMinTpsConstant()
	{
		int minTpsConstant = 0;
		for (int i=0; i<nbTurbines; i++)
		{
			if (minTpsConstant < listeTurbines[i]->getMinTpsConstant())
				minTpsConstant = listeTurbines[i]->getMinTpsConstant();
		}

		for (int i=0; i<nbPompes; i++)
		{
			if (minTpsConstant < listePompes[i]->getMinTpsConstant())
				minTpsConstant = listePompes[i]->getMinTpsConstant();
		}

		return minTpsConstant;
	}

	/*!
	 * \brief Détermine si une turbine ou une pompe du lien est en arrêt à une heure donnée
	 *
	 * \param h : entier, heure
	 *
	 * \return true si une pompe ou une turbine du lien est en arrêt à l'heure h, false sinon
	 */
	bool enArret(int h) const
	{
		bool rep=false;
		
		int cont=0;
		while(!rep && cont<nbPompes)
		{
			// cout << "pompe " << cont << endl;
			rep=listePompes[cont]->estEnArret(h);
			cont++;
		}

		cont=0;
		while(!rep &&cont<nbTurbines)
		{
			// cout << "turbine " << cont << endl;
			rep=listeTurbines[cont]->estEnArret(h);
			cont++;
		}

		return rep;
	}

	/*!
	 * \brief Détermine si une turbine du lien possède une contrainte d'incompatibilité
	 *
	 * \return true si une turbine du lien possède une contrainte d'incompatibilité, false sinon
	 */
	bool contrainteDincompatibilite() const
	{
		bool rep=false;
		
		for(int i=0;!rep && i<nbTurbines;i++)
		{
			if(listeTurbines[i]->getIncompatibilites()!=-1)
			 	rep=true;	
		}

		for(int i=0;!rep && i<nbPompes;i++)
		{
			if(listePompes[i]->getIncompatibilites()!=-1)
			 	rep=true;	
		}
		
		return rep;
		
	}
	int NumIncompatibilite() const
		{
			int rep=-1;

			for(int i=0;i<nbTurbines;i++)
			{
				if(listeTurbines[i]->getIncompatibilites()!=-1)
				 	rep=listeTurbines[i]->getIncompatibilites();
			}

			for(int i=0;i<nbPompes;i++)
			{
				if(listePompes[i]->getIncompatibilites()!=-1)
				 	rep=listePompes[i]->getIncompatibilites();
			}

			return rep;

		}
	int distIncompatibilite() const
			{
				int rep=-1;

				for(int i=0;i<nbTurbines;i++)
				{
					if(listeTurbines[i]->getIncompatibilites()!=-1)
					 	rep=listeTurbines[i]->getDistance();
				}

				for(int i=0;i<nbPompes;i++)
				{
					if(listePompes[i]->getIncompatibilites()!=-1)
					 	rep=listePompes[i]->getDistance();
				}

				return rep;

			}

	/*!
	 * \brief Détermine si une turbine ou une pompe du lien possède un arrêt programmé
	 *
	 * \return true si une pompe ou une turbine du lien possède un arrêt programmé, false sinon
	 */
	bool arretProgramme() const
	{
		bool ret = false;
		for (int i=0; i<nbTurbines; i++)
		{
			if ((listeTurbines[i])->arretProgramme())
				ret = true;
		}
		for (int i=0; i<nbPompes; i++)
		{
			if ((listePompes[i])->arretProgramme())
				ret = true;
		}
		return ret;
	}

	/*!
	 * \brief Détermine si une turbine du lien possède une contrainte de palier
	 *
	 * \return true si une turbine du lien possède une contrainte de palier, false sinon
	 */
	bool palier() const
	{
		bool rep=false;
		int i=0;
		if(nbTurbines>0)
		{
			while(rep==false && i<nbTurbines)
			{
				if( listeTurbines[i]->getPalier()>0)
					rep=true;
				i++;
			}
		}
		
		return rep;
	}
	double valPalier() const
	{
		double rep=0;
		int i=0;
		if(nbTurbines>0)
		{
			while(rep==0 && i<nbTurbines)
			{
				if( listeTurbines[i]->getPalier()>0)
					rep=listeTurbines[i]->getPalier();
				i++;
			}
		}

		return rep;
	}
};
	
	
#endif 
/* LIEN_H_ */
