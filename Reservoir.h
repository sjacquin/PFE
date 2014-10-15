#ifndef _Reservoir_h
#define _Reservoir_h

/*!
 * \file Reservoir.h
 * \author Sophie Jacquin & Guillaume Pataut
 */

#include <iostream>
#include <stdlib.h> 

using namespace std;

/*!
 * \class Reservoir
 *
 * \brief Classe qui gère les réservoirs
 */

class Reservoir
{
private:
	//Atributs
	string name; /*!< Nom du réservoir */
    double Vmax;/*!< Volume maximum */
	int nbIntVmin;/*!< Nombre d'intervalles pour des volumes minima */
	int* intVmin;/*!< Tableau des intervalles */ 
	double* Vmin;/*!< Tableau des volumes minima */
	double Vinit;/*!< Volume à l'état initial */
	double* apports;/*!< Tableau des apports */
	double apportAnnuel;/*!< Quantité des apports annuels */
	static int compteur;
	int numero;/*!< Identifiant du réservoir */
	int cardEntrees;/*!< Nombre de liaisons entrant dans le réservoir */
	int cardSorties;/*!< Nombre de liaisons sortant du réservoir */
	vector<int> adjacents;/*!< Vecteur des réservoirs adjacents */
	vector<int> entrees;/*!< Vecteur des liaisons entrantes */
	vector<int> sorties;/*!< Vecteur des liaisons sortantes */

public:
	// Méthodes
	
	// Constructeurs :
	/*!
	 * \brief Constructeur
	 *
	 * \param _Vinit : double, volume initial
	 * \param _Vmax : double, volume maximal
	 * \param _nbIntVmin : entier, nombre de volumes minima
	 * \param _intVmin : tableau d'entier, tableau des intervalles
	 * \param _Vmin : tableau de double, tableau des volumes minima
	 * \param _adjacent : vecteur d'entier, vecteur des identifiants des réservoirs adjacents
	 */
	Reservoir(string _name, double _Vinit, double _Vmax, int _nbIntVmin, int* _intVmin, double* _Vmin, vector<int> _adjacent)
	{
		name = _name;
		Vinit=_Vinit;
		Vmax=_Vmax;
		nbIntVmin=_nbIntVmin;
		Vmin=(double*)malloc(nbIntVmin*sizeof(double));
		for(int i=0;i<nbIntVmin;i++)
			Vmin[i]=_Vmin[i];
		
		intVmin=(int*)malloc(nbIntVmin*sizeof(int));
		for(int i=0;i<nbIntVmin;i++)
			intVmin[i]=_intVmin[i];
	
		apports=new double[8760];
		apportAnnuel = 0;

       	adjacents=_adjacent;
		cardEntrees=0;
		cardSorties=0;
		numero = compteur;
       	++compteur;	
	}

	//Accesseurs
	/*!
	 * \brief Donne le volume initial
	 *
	 * \return un double : Vinit
	 */
	double getVinit() 
	{
       	return Vinit;
	}
	
	/*!
	 * \brief Donne le nom du réservoir
	 *
	 * \return un string : name
	 */
	string getName() 
	{
       	return name;
	}

	/*!
	 * \brief Donne le nombre de volumes minima
	 *
	 * \return un entier : nbIntVmin
	 */
	int getNbIntVmin() const
	{
		return nbIntVmin;
	}

	int getIntVmin(int i)
	{
		return intVmin[i];
	}

	double getVmin(int i)
	{
		
		return Vmin[i];
	}
	double getMinVmin() const
	{
		double r=Vmin[0];
		for(int i=1;i<nbIntVmin;i++)
		{
			if(Vmin[i]<r) r=Vmin[i];
		}
		return r;
	}
	/*!
	 * \brief Donne le volume minimum pour un instant donné
	 *
	 * \param i : entier, temps
	 *
	 * \return un double : le volume minimum pour l'instant i
	 */
	double getRealVmin(int i)
	//faux
    {
    	/*
		 int j;
		 bool b=false;
		 j=0;
		 while (b==false)
		 {
			 if (i<intVmin[j])
			 	b=true;
			 else
			 	j++; 
		 }
		*/
		double ret;
		
		int j=0;
		while (j<nbIntVmin && intVmin[j]<=i)
			j++;
		if (j == 0)
			ret = Vmin[j];
		else
			ret = Vmin[j-1];

        return ret;
    }
	double getNumIntVmin(int i)
		
	    {
	    	
			double ret;
			
			int j=0;
			while (j<nbIntVmin && intVmin[j]<=i)
				j++;
			if (j == 0)
				ret = j;
			else
				ret = j-1;

	        return ret;
	    }
    /*!
	 * \brief Donne le volume maximum
	 *
	 * \return un double : Vmax
	 */
    double getVmax()
    {
        return Vmax;
    }
	
	/*!
	 * \brief Donne l'identifiant du réservoir
	 *
	 * \return par référence un entier : numero
	 */
    int & getNumero()
    {
        return numero;
    }
    
    /*!
	 * \brief Donne l'apport annuel
	 *
	 * \return un double : apportAnnuel
	 */
    double getApportAnnuel()
    {
        return apportAnnuel;
    }
	
	/*!
	 * \brief Donne l'apport à une heure donnée
	 *
	 * \param i : entier, heure
	 *
	 * \return un double : l'apport à l'heure i
	 */
	double getApport(int i) const
	{
		return apports[i];
	}
	
	/*!
	 * \brief Donne le nombre de liaisons entrantes
	 *
	 * \return un entier : cardEntrees
	 */
	int getCardEntrees() const
	{
		return cardEntrees;
	}
	
	/*!
	 * \brief Donne le nombre de liaisons sortantes
	 *
	 * \return un entier : cardSorties
	 */
	int getCardSorties() const
	{
		return cardSorties;
	}
	
	/*!
	 * \brief Donne le vecteur des liaisons entrantes
	 *
	 * \return un vecteur d'entier : entrees
	 */
	vector<int> getEntrees() const
	{
		return entrees;
	}
	
	/*!
	 * \brief Donne le vecteur des liaisons sortantes
	 *
	 * \return un vecteur d'entier : sorties
	 */
	vector<int> & getSorties()
	{
		return sorties;
	}
	
	/*!
	 * \brief Donne le vecteur des réservoirs adjacents
	 *
	 * \return un vecteur d'entier : adjacents
	 */
	vector<int> & getAdjacents() 
	{
		return adjacents;
	}
	
	//modificateurs :

	/*!
	 * \brief Modifie le vecteur des réservoirs adjacents
	 *
	 * \param _adjacent : vecteur d'entier, vecteur des réservoirs adjacents
	 */
	void setAdjacents(vector<int> _adjacent)
	{
		adjacents = _adjacent;
	}

	/*!
	 * \brief Modifie l'identifiant du réservoir
	 *
	 * \param _id : entier, identifiant
	 */
	void setNumero(int & _id)
	{
		numero = _id;
	}


	/*!
	 * \brief Modifie le tableau des apports ainsi que l'apport annuel
	 *
	 * \param _apports : tableau de double, tableau des apports par heure
	 */
	void adApports(double* _apports)
	{
		for(int i=0;i<8760;i++)
        	apports[i]=3600*_apports[i];

        apportAnnuel=0;
		for(int j=0; j< 8760;j++)
		   	apportAnnuel= apportAnnuel+apports[j];
	}

	/*!
	 * \brief Ajoute une liaison entrante et incrémente le nombre de liaison entrante
	 *
	 * \param i : entier, identifiant de la liaison entrante
	 */
	void adEntree(int i)
	{
		entrees.push_back(i);
		cardEntrees++;
	}
	
	/*!
	 * \brief Ajoute une liaison sortante et incrémente le nombre de liaison sortante
	 *
	 * \param i : entier, identifiant de la liaison sortante
	 */
	void adSortie(int i)
	{
		sorties.push_back(i);
		cardSorties++;
	}
	
};
int Reservoir:: compteur=0;

#endif
