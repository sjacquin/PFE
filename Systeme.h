#ifndef _Systeme_h
#define _Systeme_h

/*!
 * \file Systeme.h
 * \author SophieJacquin & Guillaume Pataut
 */

#include <iostream>
#include <vector>
#include <fstream>

//#include "utils.h"
#include "Turbine.h"
#include "Liaison.h"
#include "ComposanteConnexe.h"

/*!
 * \class Systeme
 *
 * \brief Classe qui gère tous le système hydroélectrique
 */

class Systeme
{
private:
	int nbPrix;/*!< Nombre de catégories de prix */
	vector<int> nbParamPrix;/*!< Vecteur des différents test de prix pour chaque catégorie */
	vector<double *> prix;/*!< Vecteur des prix horiaires */
	int nbReservoirs;/*!< Nombre de réservoirs */
	int nbTurbines;/*!< Nombre de turbines */
	int nbPompes;/*!< Nombre de pompes */
	int nbLiaisons;/*!< Nombre de liaison */
	int nbComposantesConnexes;/*!< Nombre de composantes connexes */
	vector<Liaison> liaisons;/*!< Vecteur des liaisons */
	vector<Pompe> pompes;/*!< Vecteur des pompes */
	vector<Reservoir> reservoirs;/*!< Vecteur des réservoirs */
	vector<Turbine> turbines;/*!< Vecteur des turbines */
	vector<ComposanteConnexe> composantesConnexes;/*!< Vecteur des composantes connexes */
	
public:
	//Constructeur:
	/*!
	 * \brief Constructeur par défaut
	 */
	Systeme()
	{	
		nbPrix = 0;
		nbReservoirs=0;
		nbTurbines=0;
		nbPompes= 0;
		nbLiaisons=0;	
		nbComposantesConnexes=0;
	}

	//Modificateur :
	/*!
	 * \brief Ajoute un nombre de test pour un nouveau prix
	 *
	 * \param _param : entier, nombre de tests
	 */
	void adNbParamPrix(int _param)
	{
		nbParamPrix.push_back(_param);
	}

	/*!
	 * \brief Ajoute par référence un réservoir donné dans le système
	 *
	 * \param R : Reservoir
	 */
	void adReservoir(Reservoir & R)
	{
		reservoirs.push_back(R);
		nbReservoirs++;
	}

	/*!
	 * \brief Ajoute par référence une turbine dans le système
	 *
	 * \param T : Turbine 
	 */
	void adTurbine(Turbine & T)
	{
		turbines.push_back(T);
		nbTurbines++;
	}

	/*!
	 * \brief Ajoute par référence une pompe dans le système
	 *
	 * \param P : Pompe
	 */
	void adPompe(Pompe & P)
	{
		pompes.push_back(P);
		nbPompes++;
	}

	/*!
	 * \brief Ajoute par référence une liaison dans le système
	 *
	 * \param L : Liaison
	 */
	void adLiaison(Liaison & L)
	{
		liaisons.push_back(L);
		nbLiaisons++;
	}
	
	/*!
	 * \brief Ajoute par référence une composante connexe dans le système
	 *
	 * \param comp : ComposanteConnexe
	 */
	void adComposanteConnexe(ComposanteConnexe & comp)
	{
		composantesConnexes.push_back(comp);
		nbComposantesConnexes++;
	}
	
	/*!
	 * \brief Ajoute un nouvelle catégorie de prix horaire
	 *
	 * \param _prix : tableau de double, tableau des prix horaires
	 */
	void adPrix(double* _prix)
	{
		prix.push_back(_prix);
		nbPrix++;
	}

	//Acsesseurs:
	/*!
	 * \brief Donne le nombre de tests possibles pour une catégorie de prix donnée
	 *
	 * \param i : entier, catégorie de prix
	 *
	 * \return un entier, le nombre de tests possibles pour la \a i ème catégorie de prix
	 */ 
	int getNbParamPrix(int i) const
	{
		return nbParamPrix[i];
	}

	/*!
	 * \brief Donne le vecteur contenant tous les prix horaires de toutes les catégories de prix
	 *
	 * \return un vecteur de tableau de double, \a prix
	 */
	vector<double*> getAllPrices() const
	{
		return prix;
	}

	/*!
	 * \brief Donne le tableau des prix horaires pour une catégorie de prix donnée
	 *
	 * \param i : entier, catégorie de prix
	 *
	 * \return un tableau de double, le tableau des prix horaires de la \a i ème catégorie de prix
	 */
	double* getPrix(int i) const
	{
		return prix[i];
	}

	/*!
	 * \brief Donne le nombre de composantes connexes
	 *
	 * \return un entier, \a nbComposanteConnexe
	 */
	int getNbComposantesConnexes() const
	{
		return nbComposantesConnexes;
	}
	
	/*!
	 * \brief Donne le nombre de réservoirs
	 *
	 * \return un entier, \a nbReservoirs
	 */
	int getNbReservoirs() const
	{
		return nbReservoirs;
	}

	/*!
	 * \brief Donne le nombre de turbines
	 *
	 * \return un entier, \a nbTurbines
	 */
	int getNbTurbines() const
	{
		return nbTurbines;
	}
	
	/*!
	 * \brief Donne le nombre de pompes
	 *
	 * \return un entier, \a nbPompes
	 */
	int getNbPompes() const
	{
		return nbPompes;
	}
	
	/*!
	 * \brief Donne le nombre de liaisons
	 *
	 * \return un entier, \a nbLiaisons
	 */
	int getNbLiaisons() const
	{
		return nbLiaisons;
	}

	/*!
	 * \brief Donne le nombre de catégories de prix
	 *
	 * \return un entier, \a nbPrix
	 */
	int getNbPrix() const
	{
		return nbPrix;
	}
	
	/*!
	 * \brief Donne le vecteur de toutes les liaisons du système
	 *
	 * \return par référence un vecteur constant de Liaison, \a liaisons
	 */
	const vector<Liaison>&  getLiaisons() const
	{
		return liaisons;
	} 
	
	/*!
	 * \brief Donne le vecteur de toutes les pompes du système
	 *
	 * \return par référence un vecteur constant de Pompe, \a pompes
	 */
	const vector<Pompe>& getPompes() const
	{
		return pompes;
	}
	
	/*!
	 * \brief Donne une liaison précise
	 *
	 * \param i : entier, identifiant de la liaison
	 *
	 * \return par copie une Liaison, la \a i ème liaison du système
	 */
	Liaison getLiaison(int i) const
	{
		return liaisons[i];
	}

	/*!
	 * \brief Donne une liaison précise
	 *
	 * \param i : entier, identifiant de la liaison
	 *
	 * \return par référence une Liaison, la \a i ème liaison du système
	 */
	Liaison & getLiaison(int i)
	{
		return liaisons[i];
	}
	
	/*!
	 * \brief Donne un réservoir précis
	 *
	 * \param i : entier, identifiant du réservoir
	 *
	 * \return par référence un Reservoir, le \a i ème réservoir du système
	 */
	Reservoir & getReservoir(int i)
	{
		return (reservoirs[i]);
	}
	
	/*!
	 * \brief Donnepar référence constante un réservoir précis
	 *
	 * \param i : entier, identifiant du réservoir
	 *
	 * \return un Reservoir constant, le \a i ème réservoir du système
	 */
	const Reservoir getReservoir(int i) const
	{
		return (reservoirs[i]);
	}

	/*!
	 * \brief Donne une turbine précis
	 *
	 * \param i : entier, identifiant de la turbine
	 *
	 * \return par référence un Turbine, la \a i ème turbine du système
	 */
	Turbine & getTurbine(int i)
	{
		return turbines[i];
	}

	/*!
	 * \brief Donne une pompe précis
	 *
	 * \param i : entier, identifiant de la pompe
	 *
	 * \return par référence un Pompe, la \a i ème pompe du système
	 */
	Pompe & getPompe(int i)
	{
		return pompes.at(i);
	}
	
	/*!
	 * \brief Donne une composante connexe précis
	 *
	 * \param i : entier, identifiant de la composante connexe
	 *
	 * \return par référence un ComposanteConnexe, la \a i ème composante connexe du système
	 */
	ComposanteConnexe & getComposanteConnexe(int i)
	{
		return composantesConnexes[i];
	}
	
	//Autres : 
	/*!
	 * \brief Modifie une composante connexe du système par une composante connexe donnée
	 *
	 * \param j : entier, identifiant de la composante connexe du système
	 * \param C : ComposanteConnexe
	 */
	void setComposanteConnexe(int j, ComposanteConnexe & C)
	{
		composantesConnexes[j] = C;
	}

	/*!
	 * \brief Donne le vecteur de toutes les liaisons entre deux réservoirs donnés
	 *
	 * \param A : entier, identifiant du réservoir initial
	 * \param B : entier, identifiant du réservoir final
	 *
	 * \return un vecteur de pointeurs sur Liaison
	 */
	vector<Liaison*> getLiaisonsAToB(int A, int B)
	{
		vector<Liaison*> result;
		int i;
		for(i=0;i<reservoirs[A].getCardSorties();i++)
		{
			int numLiaison=reservoirs[A].getSorties()[i];
			//cout<<"voyons donc :"<<liaisons[numLiaison].getReservoirFin()<<endl;
			if(liaisons[numLiaison].getReservoirFin()==B)
				result.push_back(&liaisons[numLiaison]);
		}
	
		return result;
	}

	
	/*!
	 * \brief Donne le temps de parcours minimum entre deux réservoirs donnés
	 *
	 * \param r1 : entier, réservoir initial
	 * \param r2 : entier, réservoir final
	 *
	 * \return un entier. S'il vaut \a INT_MAX, cela signifie qu'il n'existe aucune liaison permettant d'aller de \a r1 à \a r2.
	 */
	int distanceMinimale(int r1,int r2)
	{
		vector<Liaison*> li= getLiaisonsAToB(r1,r2);
		int i;
		int dist=10000;
		
		for(i=0;i<li.size();i++)
		{
			if(dist>li[i]->getLongueur())
				dist=li[i]->getLongueur();
		}
		//cout<<" taille li :"<<li.size()<<" r1 "<<r1<<" r2 "<<r2<<" dist "<<dist<<endl;
		return dist;
	}

	int getMaxDeltaT()
	{
		int ret = 0;
		for (int i=0; i<nbLiaisons; i++)
		{
			ret = max(ret,liaisons[i].getLongueur());
		}

		return ret;
	}

	
	int plusLongChemin(int res, vector<bool> & P, vector<int> & maxDist, vector<bool> & dejaCalc)
    {
        Reservoir & R = reservoirs[res];
        vector<int> entrees = R.getEntrees();
        for (int card=0; card<R.getCardEntrees(); card++)
        {
            Liaison & L =liaisons[entrees[card]];
            int resIni = L.getReservoirIni();
            if (!P[resIni])
            {
                vector<bool> P_copie = P;
                P_copie[resIni] = true;
                int res_tmp = L.getLongueur();
                if (dejaCalc[resIni])
                    res_tmp += maxDist[resIni];
                else
                    res_tmp += plusLongChemin(resIni,P_copie,maxDist,dejaCalc);
                if (res_tmp>maxDist[res])
                {
                    maxDist[res] = res_tmp;
                    P = P_copie;
                }
            }
        }
        dejaCalc[res] = true;
        // cout << "Réservoir " << res << " a pour longueur max : " << maxDist[res] << endl;
        return maxDist[res];
    }
};

#endif
