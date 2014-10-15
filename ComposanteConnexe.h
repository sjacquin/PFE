#ifndef _ComposanteConnexe_h
#define _ComposanteConnexe_h

/*!
 * \file ComposanteConnexe.h
 * \author Sophie Jacquin & Guillaume Pataut
 */

#include <iostream>
#include <vector>


/*!
 * \class ComposanteConnexe
 *
 * Classe qui gère les composantes connexes entre les réservoirs
 */

class ComposanteConnexe
{
private:
	int nbElementaryCircuits;/*!< Nombre de circuits élémentaires */
	int nbReservoirs;/*!< Nombre de réservoirs*/
	vector<int> reservoirs;/*!< Vecteur des réservoirs */
	vector<vector<int> > circuits;/*!< Vecteur des circuits */

public:
	//constructeurs :
	/*!
	 * \brief Constructeur par un réservoir
	 *
	 * \param R : réservoir de la composante connexe
	 */
	ComposanteConnexe(int & R)
	{
		nbElementaryCircuits=0;
		nbReservoirs=1;
		reservoirs.push_back(R);
	}
	
	/*!
	 * \brief Constructeur pour	plusieurs réservoirs avec circuits
	 *
	 * \param _reservoirs : vecteur des réservoirs
	 * \pram _circuits : vecteur des circuits
	 */
	ComposanteConnexe(vector<int> &_reservoirs, vector<vector<int> > & _circuits)
	{
		nbReservoirs=_reservoirs.size();
		nbElementaryCircuits=_circuits.size();
		reservoirs=_reservoirs;
		circuits=_circuits;	
	}

	/*!
	 *
	 * \brief Constructeur pour plusieurs réservoirs sans circuit
	 *
	 * \param _reservoirs : vecteur des réservoirs
	 */
	ComposanteConnexe(vector<int> &_reservoirs)
	{
		nbReservoirs=_reservoirs.size();
		nbElementaryCircuits=0;
		reservoirs=_reservoirs;
	}
	
	//accesseurs:
	/*!
	 * \brief Donne le nombre de circuit élémentaire
	 *
	 * \return un entier : nbElementaryCircuits
	 */
	int getNbElementaryCircuits() const
	{
		return nbElementaryCircuits;
	}
	
	/*!
	 * \brief Donne le nombre de réservoirs
	 *
	 * \return un entier : nbReservoirs
	 */
	int getNbReservoirs() const
	{
		return nbReservoirs;
	}
	
	/*!
	 * \brief Donne un vecteur constant des réservoirs
	 *
	 * \return un vecteur des réservoirs constant : reservoirs
	 */
	const vector<int> & getReservoirs() const
	{
		return reservoirs;
	}
	
	/*!
	 * \brief Donne un réservoir précis
	 *
	 * \param i : entier
	 *
	 * \return le ième réservoir de la composante connexe
	 */
	int & getReservoir(int i)
	{
		return reservoirs[i];
	}
	
	/*!
	 * \brief Donne un circuit constant précis
	 *
	 * \param i : entier
	 *
	 * \return le ième circuit élémentaire de la composante connexe
	 */
	const vector<int> & getCircuit(int i) const
	{
		return circuits[i];
	}
	
	//modificateurs 

	/*!
	 * \brief Modifie un réservoir donné
	 *
	 * Modifie le jème réservoir de la composante connexe en le remplaçant par R
	 *
	 * \param j : entier
	 * \param R : réservoir
	 */
	void setReservoir(int j, const int & R)
	{
		reservoirs[j] = R;
	}

	/*!
	 * \brief Modifie un circuit donné
	 *
	 * \param i : entier
	 * \param list : vecteur de réservoir composant le circuit
	 */
	void setCircuit(int i, vector<int> list)
	{
		circuits[i] = list;
	}

	/*!
	 * \brief Ajoute un circuit à la composante connexe
	 *
	 * \param C : circuit
	 */
	void adCircuit(vector<int> & C)
	{
		nbElementaryCircuits++;
		circuits.push_back(C);
	} 
	
	//autre :
	/*!
	 * \brief Détermine si un réservoir appartient à la composante connexe
	 *
	 * \param R : entier, numéro du réservoir dans le système
	 * 
	 * \return true si le réservoir numéro R appartient à la composante connexe, false sinon
	 */
	bool in(int R)
	{
		bool rep=false;
		for(int i=0;i<nbReservoirs;i++)
		{
			if(reservoirs[i]==R)
			{
				rep=true;
				break;
			}
		}
		return rep;
	}

	bool inCircuit(int R)
	{
		bool rep=false;
		for (int i=0; rep == false && i<nbElementaryCircuits; i++)
		{
			for (int j=0; rep == false && j<circuits[i].size(); j++)
				rep = (circuits[i][j]==R);
		}
		return rep;
	}
	/*!
	 * \brief Surcharge d'affectation
	 *
	 * \param C : ComposanteConnexe
	 *
	 * \return la composante connexe remplacée par C
	 */
	ComposanteConnexe & operator=(const ComposanteConnexe & C)
	{
		nbReservoirs = C.nbReservoirs;
		nbElementaryCircuits = C.nbElementaryCircuits;
		reservoirs = C.reservoirs;
		circuits = C.circuits;

		return *this;
	}
		
};
#endif
