#ifndef _Pompe_h
#define _Pompe_h

/*! 
 * \file Pompe.h
 * \author Sophie Jacquin & Guillaume Pataut
 */

#include "Turbine.h"

/*!
 * \class Pompe
 *
 * \brief Classe qui gère les pompes
 */

class Pompe : public Turbine
{
private:
	vector<int> achat;/*!< Vecteur des numéros des turbines auxquelles la pompe achète sa production */
	int nbAchat;/*!< Nombre de turbines auxquelles la pompe achète sa production */
	bool turp;

public:
	//constructeurs
	/*!
	 * \brief Constructeur avec production
	 *
	 * \param _nbInt : entier, nombre d'intervalles
     * \param _nbPieces: entier, 
     * \param _prodMin : double, production minimum
     * \param _qmax : tableau de double, débits maxima
     * \param _production : matrice de double, productions
     * \param _distance : entier, distance avec le réservoir en amont
     * \param _intReservoirs : tableau de double
     * \param _piece : tableau de double : 
     * \param reservoirP : entier, numéro du réservoir en amont
     * \param _catPrix : entier, catégorie de prix
     * \param minT : double, minimum technique de production
     * \param _incompatibilités : entier, numéro de la pompe incompatible
     * \param deb : entier, début arrêt programmé
     * \param fin : entier, fin arrêt programmé
     * \param _nbAchat : entier, nombre de turbines auxquelles on achète la production
     * \param _achat : vecteur d'entier, vecteur des numéros des turbines auxquelles on achète la production
     * \param _minTpsConstant : entier, temps minimum de production constante
     */
	Pompe(string _name, int _nbInt, int _nbPieces, double _prodMin, double* _qmax, double** _production, int _distance, double* _intReservoirs, double* _piece, int reservoirP, int _catPrix, int minT, int _incompatibilites, int deb, int fin, int _nbAchat, vector<int> _achat, int _minTpsConstant, double _palier, bool _turp) :Turbine(_name, _nbInt, _nbPieces, _prodMin, _qmax,  _production, _distance, _intReservoirs, _piece, reservoirP, _catPrix, minT, _incompatibilites, deb, fin, _minTpsConstant, _palier)
	{	
	//(int _nbInt, int _nbPieces, double _prodMin, double* _qmax, double** _production, int _distance, vector<double> _prix, double* _intReservoirs, double* _piece, int reservoirP, int minT, int _incompatibilites, int deb, int fin)
		achat=_achat;
		nbAchat=_nbAchat;
		turp = _turp;		
	}

	/*!
	 * \brief Constructeur sans production
	 *
	 * \param _prodMin : double, production minimum
	 * \param _distance : entier, distance avec le réservoir en amont
	 * \param reservoirP : entier, numéro du réservoir en amont
     * \param _catPrix : entier, catégorie de prix
     * \param minT : double, minimum technique de production
     * \param _incompatibilités : entier, numéro de la pompe incompatible
     * \param deb : entier, début arrêt programmé
     * \param fin : entier, fin arrêt programmé
     * \param _nbAchat : entier, nombre de turbines auxquelles on achète la production
     * \param _achat : vecteur d'entier, vecteur des numéros des turbines auxquelles on achète la production
     * \param _minTpsConstant : entier, temps minimum de production constante
     */
	Pompe(string _name, double _prodMin, int _distance, int reservoirP, int _catPrix, int minT, int _incompatibilites, int deb, int fin, int _nbAchat, vector<int> _achat, int _minTpsConstant, double _palier, bool _turp):Turbine(_name, _prodMin,  _distance, reservoirP, _catPrix, minT, _incompatibilites, deb, fin, _minTpsConstant, _palier)
	{
		achat=_achat;
		nbAchat=_nbAchat;
		turp = _turp;			
	}

	//Accesseurs :
	/*!
	 * \brief Donne le nombre de turbines auxquelles on achète la production
	 *
	 * \return un entier : nbAchat
	 */
	int getNbAchats() const
	{
		return nbAchat;
	}

	bool getTurp()
	{
		return turp;
	}

	void setTurb(bool b)
	{
		turp = b;
	}

	/*!
	 * \brief Donne le vecteur des identifiants des turbines auxquelles on achète la production
	 *
	 * \return par référence un vecteur d'entier constant : achat
	 */
	const vector<int> & getAchat() const
	{
		return achat;
	}
};

#endif
