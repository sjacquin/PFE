#ifndef _Turbine_h
#define _Turbine_h

/*!
 * \file Turbine.h
 * \author Sophie Jacquin & Guillaume Pataut
 */


/*!
 * \class Turbine
 * \brief Classe qui gère les turbines
 */
class Turbine 
{
private:
	string name;
    int distance;/*!< Distance au réservoir en amont */
	double** production;/*!< Matrice des coefficients de production */
	double* qmax;/*!< Débits maxima */
	double prodMin;/*!< Production minimum */
	int minTmpsConstant;/*!< Temps minimum de fonctionnement constant */
	int nbInt;/*!< Nombre de discrétisation des volumes */
	int nbPieces;/*!< Nombre de discrétisation des débits */ 
	double* pieces;/*!< Tableau des valeurs de discrétisation des débits */
	double* intReservoirs;/*!< Tableau des valeurs de discrétisation des volumes */
	int reservoirParent;/*!< Réservoir en amont */
	int catPrix;/*!< Catégorie de prix */
	double* prix;/*!< Prix horaire */
    int numero;/*!< Identifiant */
    double minTechnique;/*!< Minimum technique de production */
    int incompatibilites;/*!< Numéro de la pompe ou turbine incompatible */
    int debutArretProgramme;/*< Début de l'arrêt programmé */
    int finArretProgramme;/*!< Fin de l'arrêt programmé */
    double palier;/*!< Palier de production */
    int numLien;/*!< Numéro de la liaison associée */
    static int compteur;
    
public:
    //Constructeurs 
    /*!
     * \brief Constructeur général
     * 
     * \param _nbInt : entier, nombre de discrétisation des volumes
     * \param _nbPieces: entier, nombre de discrétisation des débits
     * \param _prodMin : double, production minimum
     * \param _qmax : tableau de double, débits maxima
     * \param _production : matrice de double, productions
     * \param _distance : entier, distance avec le réservoir en amont
     * \param _intReservoirs : tableau de double, tableau des valeurs de discrétisation des volumes
     * \param _piece : tableau de double, tableau des valeurs de discrétisation des débits
     * \param reservoirP : entier, numéro du réservoir en amont
     * \param _catPrix : entier, catégorie de prix
     * \param minT : double, minimum technique de production
     * \param _incompatibilités : entier, numéro de la pompe incompatible
     * \param deb : entier, début arrêt programmé
     * \param fin : entier, fin arrêt programmé
     * \param _minTpsConstant : entier, temps minimum de production constante
     */
	Turbine(string _name, int _nbInt, int _nbPieces, double _prodMin, double* _qmax, double** _production, int _distance, double* _intReservoirs, double* _piece, int reservoirP, int _catPrix, double minT, int _incompatibilites, int deb, int fin, int _minTpsConstant, double _palier)
	{

		int i,j;
		palier = _palier;
		name = _name;
		minTmpsConstant = _minTpsConstant;
		reservoirParent=reservoirP;
		catPrix = _catPrix;
		nbInt=_nbInt;
		nbPieces=_nbPieces;
		prodMin=_prodMin;
		qmax=new double[nbInt];
		
		production=new double*[nbPieces];
		for(i=0;i<nbPieces;i++)
		{			
			production[i]=new double[nbInt];
			for(j=0;j<nbInt;j++)
				production[i][j]=_production[i][j];
		}
		
		intReservoirs=(double*) malloc(nbInt*sizeof(double));
		for(i=0;i<nbInt;i++)
			intReservoirs[i]=_intReservoirs[i];
		
		pieces=(double*) malloc(nbPieces*sizeof(double));
		for(i=0;i<nbPieces;i++)
			pieces[i]=_piece[i];
		
		qmax=(double*) malloc(nbInt*sizeof(double));
		for(j=0;j<nbInt;j++)
				qmax[j]=_qmax[j]*3600;
		
		distance=_distance;
		
		numero=compteur;
		minTechnique=minT;
		prodMin=minT; //en attendant eclairsissement
		incompatibilites=_incompatibilites;
		debutArretProgramme=deb;
		finArretProgramme=fin;
		compteur++;
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
     * \param _minTpsConstant : entier, temps minimum de production constante
     */
	Turbine(string _name, double _prodMin, int _distance, int reservoirP, int _catPrix, double minT, int _incompatibilites, int deb, int fin, int _minTpsConstant, double _palier)
	{
		int i,j;
		palier = _palier;
		name = _name;
		minTmpsConstant = _minTpsConstant;
		reservoirParent=reservoirP;
		catPrix = _catPrix;
		prodMin=minT;//_prodMin;
		distance=_distance;
		numero=compteur;
		minTechnique=minT;
		incompatibilites=_incompatibilites;
		debutArretProgramme=deb;
		finArretProgramme=fin;
		// qmax=(double*) malloc(nbInt*sizeof(double));
		// intReservoirs=(double*) malloc(nbInt*sizeof(double));
		// pieces=(double*) malloc(nbPieces*sizeof(double));
		// production=new double*[nbPieces];
		// for(i=0;i<nbPieces;i++)
		// {			
		// 	production[i]=new double[nbInt];
		// }
		compteur++;
	}

	/*!
	 * \brief Donne la distance au réservoir en amont
	 *
	 * \return un entier : distance
	 */	
	int getDistance()const
	{
		return distance;
	}

	/*!
	 * \brief Donne le nom de la turbine
	 *
	 * \return une string name
	 */	
	string getName() const
	{
		return name;
	}

	/*!
	 * \brief Donne la production minimum de la turbine
	 *
	 * \return un double : prodMin
	 */
	double getProdMin()const
	{
		return prodMin;
	}

	/*!
	 * \brief Donne le nombre de discrétisation des volumes
	 *
	 * \return un entier : nbInt
	 */
	int getNbInt() const
	{
		return nbInt;
	}

	/*!
	 * \brief Donne le nombre de discrétisation des débits
	 *
	 * \return un entier : nbPieces
	 */
	int getNbPieces()const
	{
		return nbPieces;
	}

	/*!
	 * \brief Donne l'identifiant de la turbine
	 *
	 * \return un entier : numero
	 */
	int getNumero()const
	{
		return numero;
	}

	/*!
	 * \brief Donne le temps minimum constant de fonctionnement
	 *
	 * \return un entier : minTpsConstant
	 */
	int getMinTpsConstant() const
	{
		return minTmpsConstant;
	}

	/*!
	 * \brief Donne le volume pour une certaine discrétisation
	 *
	 * \return un double : la \a i ème valeur de discrétisation des volumes
	 */
	double getBinfIntReservoir(int i)const
	{
		return intReservoirs[i];
	}
	double* getListeIntReservoirs()const
	{
		return intReservoirs;
	}

	/*!
	 * \brief Donne le débit pour une certaine discrétisation
	 *
	 * \return un double : la \a i ème valeur de discrétisation des débits
	 */
	double getBSupPiece(int i)const
	{
		return pieces[i+1];
	}

	//TODO
	/*!
	 * \brief Donne ...
	 *
	 * \return un double : ...
	 */
	double getBmaxMorceau(int m)const
	{
		//if (m==0) return pieces[0];
		 return (pieces[m+1]-pieces[m]);
	}

	/*!
	 * \brief Donne le débit par borne inférieure pour une certaine discrétisation
	 *
	 * \return un double : la \a i valeur de bornes inférieures pour la discrétisation des débits
	 */
	double getBInfPiece(int i)const
	{
		//if(i==0) return 0;
		//else
		return pieces[i];
	}
	double* getListePieces() const
	{
		return pieces;
	}

	/*!
	 * \brief Donne la production du palier
	 *
	 * \return un double : palier
	 */
	double getPalier()const
	{
		return palier;
	}

	/*!
	 * \brief Donne le numéro de discrétisation dans lequel se situe un volume donné
	 *
	 * \param V : double, volume 
	 *
	 * \return un entier : le numéro de discrétisation dans lequel V est inclus
	 */
	int getIntervalle(double V)
	{
		int Int=0;
		for(int i=1;i<nbInt;i++)
		{
			if(V>=intReservoirs[i])
				Int=i;
		}

		return Int;
	}

	/*!
	 * \brief Donne le prix d'une unité à une heure donnée par référence
	 *
	 * \param t : entier, heure
	 *
	 * \return un double : le prix de production d'une unité à l'heure t
	 */
	double & getPrix(int t)
	{
		return prix[t];
	}

	/*!
	 * \brief Donne le prix d'une unité à une heure donnée sans modification possible
	 *
	 * \param t : entier, heure
	 *
	 * \return un double : le prix de production d'une unité à l'heure t
	 */
	double getPrix(int t) const
	{
		return prix[t];
	}

	/*!
	 * \brief Donne le bénéfice réalisé à une heure donné, pour un volume donné et un débit donné
	 *
	 * \param V : double, volume
	 * \param q : double, débit
	 * \param t : entier, heure
	 *
	 * \return un double : le bénéfice à l'heure t pour un volume V et pour un débit q
	 */
	double getBenefice(double V, double q, int t) const
	{
		//cout<<"dans getBenefice"<<endl;
		int i;
		double result=0;
		int Int=0;
		for(i=0;i<nbInt;i++)
		{
			if(V>=intReservoirs[i])
				Int=i;
		}	
		if(q>qmax[Int])
			result=-100000;
		//cout<<"bpour1"<<endl;
		int pi=0;
		for(i=0;i<nbPieces-1;i++)
		{
			if(q>=pieces[i])
				pi=i;
		}
		//cout<<"bpour2"<<endl;
		double prod=0;
		for(i=0;i<pi;i++)
		{

				prod=prod+ production[i][Int]*(pieces[i+1]-pieces[i]);
		}
		
		//cout<<"bpour3"<<endl;
		
			prod=prod+ production[pi][Int]*(q-pieces[pi]);


		
		//cout<<"finget"<<endl;
		result+= prod*prix[t];
		return result;
	}

	double getProdV(double V, double q) const
	{
		
		int i;
	
		int Int=0;
		for(i=0;i<nbInt;i++)
		{
			if(V>=intReservoirs[i])
				Int=i;
		}	
			if(q>qmax[Int])q=qmax[Int];
		int pi=0;
		for(i=0;i<nbPieces-1;i++)
		{
			if(q>=pieces[i])
				pi=i;
		}
	
		double prod=0;
		for(i=0;i<pi;i++)
		{
			/*if (i==0)
				prod=prod+ production[i][Int]*pieces[0];
			else */
				prod=prod+ production[i][Int]*(pieces[i+1]-pieces[i]);
		}
		

		if(pi>0)
			prod=prod+ production[pi][Int]*(q-pieces[pi]);
		else
			prod=prod+ production[pi][Int]*q;
		
		
		
		return prod;
	}

	double getMaxQmax()
	{
		double res = 0.0;
		for (int i=0; i<nbInt; i++)
			res = max(res, qmax[i]);

		return res;
	}

	/*!
	 * \brief Donne le bénéfice réalisé à une heure donné, pour une discrétisation des volumes donnée et un débit donné
	 *
	 * \param Int : entier, numéro de discrétisation des volumes
	 * \param q : double, débit
	 * \param t : entier, heure
	 *
	 * \return un double : le bénéfice à l'heure t pour la discrétisation numéro Int des volumes et pour un débit q
	 */
	double getBeneficeInt(int Int, double q, int t) const
	{
		int i;
		
		int pi=0;
		if(q<getQmin(Int))return -10000;
		if(q>qmax[Int]) return -10000;
		for(i=0;i<nbPieces-1;i++)
		{
			if(q>=pieces[i])
				pi=i+1;
		}
		
		double prod=0;
		for(i=0;i<pi;i++)
		{
			if (i==0)
				prod=prod+ production[i][Int]*pieces[0];
			else
				prod=prod+ production[i][Int]*(pieces[i]-pieces[i-1]);
		}
		
		if(pi>0)
			prod=prod+ production[pi][Int]*(q-pieces[pi-1]);
		else
			prod=prod+ production[pi][Int]*q;

		return prod*prix[t];
	}
	
	
	/*!
	 * \brief Donne le coefficient de production pour des discrétisations de volume et de débit donnés
	 *
	 * \param pieces : entier, discrétisation du débit
	 * \param interval : entier, discrétisation du volume
	 *
	 * \return un double : le coefficient de poduction pour la discrétisation numéro \a interval des volumes et pour la discrétisation numéro \a pieces des débits
	 */
	double getProd(int pieces, int interval)
	{
		return production[pieces][interval];
	}

	/*!
	 * \brief Donne la catégorie de prix
	 *
	 * \return un entier : catPrix
	 */
	int getCatPrix() const
	{
		return catPrix;
	}

	/*!
	 * \brief Donne le débit minimum en fonction d'un volume donné
	 *
	 * \param V : double, volume
	 *
	 * \return un double : débit minimum pour le volume V
	 */
	double getQmin(double V)const
	{
		int i;
		double prod=0;
		double q=0;
		bool trouve =false;
		int rep=0;
		//cout<<"dans qmin"<<endl;
		for(i=0;i<nbInt;i++)
		{
			if(V>=intReservoirs[i])rep=i;
		}

		int pi=0;
		//cout<<"deb while nbPieces="<<nbPieces<<endl;
		while( trouve==false && pi<nbPieces-1)
		{
			//cout<<prodMin<<" "<<pi<<" rep="<<rep<<" "<<production[pi][rep]<<endl;
			double p=(prodMin-prod)/production[pi][rep];
			//cout<<p<<endl;
			if(p< pieces[pi+1]-pieces[pi])
			{
				trouve=true;
				q=q+p;
			}
			else
			{
				q=pieces[pi+1];
				prod= prod+ production[pi][rep]*(pieces[pi+1]-pieces[pi]);
				pi++;
			}

		}
		//cout<<"q="<<q<<endl;
		return q;
	}

	/*!
	 * \brief Donne le débit associé au palier de production pour un certain volume
	 * 
	 * \param V : double, volume
	 * 
	 * \return un double : le débit du palier de production pour le volume V
	 */
	double getQPalier(double V)
	{
		int i;
		double prod=0;
		double q=0;
		bool trouve =false;
		int rep=0;
		for(i=0;i<nbInt;i++)
		{
			if(V>=intReservoirs[i])
				rep=i;
		}
		
		int pi=0;
		while(trouve==false && pi<nbPieces-1)
		{
			//cout<<"je devrais hurler"<<endl;
			double p=(palier-prod)/production[pi][rep];
			if(p< pieces[pi+1]-pieces[pi] )
			{
				trouve=true;
				q=q+p;
			}
			else
			{
				q=pieces[pi+1];
				prod= prod+ production[pi][rep]*(pieces[pi+1]-pieces[pi]);

			}
			pi++;
		}
		
		return q;
	}
	double getQPalierInt(int rep)
	{
		int i;
		double prod=0;
		double q=0;
		bool trouve =false;


		int pi=0;
		while(trouve==false && pi<nbPieces-1)
		{
			//cout<<"je devrais hurler"<<endl;
			double p=(palier-prod)/production[pi][rep];
			if( p< pieces[pi+1]-pieces[pi])
			{
				trouve=true;
				q=q+p;
			}
			else
			{
				q=pieces[pi+1];
				prod= prod+ production[pi][rep]*(pieces[pi+1]-pieces[pi]);

			}
			pi++;
		}

		return q;
	}

	/*!
	 * \brief Donne le débit associé à la production minimum pour une certaine discrétisation des volumes
	 *
	 * \param rep : entier, numéro de discrétisation des volumes
	 *
	 * \return un double : le débit associé à la production minimum pour la discrétisation numéro \a rep des volumes
	 */
	double getQminInt(int rep) const
	{
		int i;
		double prod=0;
		double q=0;
		bool trouve =false;
		
		int pi=0;
		while( trouve==false && pi<nbPieces-1)
		{
			double p=(prodMin-prod)/production[pi][rep];
			if(p< pieces[pi+1]-pieces[pi])
			{
				trouve=true;
				q=q+p;
			}
			else
			{
				q=pieces[pi+1];
				prod= prod+ production[pi][rep]*(pieces[pi+1]-pieces[pi]);
			}
			pi++;
		}

		return q;
	}

	/*!
	 * \brief Donne le réservoir en amont
	 * 
	 * \return un enetier : numéro du réservoir en amont
	 */
	int getReservoirParent()
	{
		return reservoirParent;
	}
	
	/*!
	 * \brief Donne lme débit maximum en fonction d'un volume donné
	 *
	 * \param V : double, volume
	 *
	 * \return un double : débit maximum pour le volume V
	 */
	double getQmax(double V)
	{
		int t=getIntervalle(V);
	
		return qmax[t];
	}
	double getQmaxInt( int i)
	{
		return qmax[i];
	}
	/*!
	 * \brief Donne le minimum technique de production
	 *
	 * \return un double : minTechnique
	 */
	double getMinimumTechnique() const
	{
		return minTechnique;
	}
	
	/*!
	 * \brief Donne le numéro de la pompe incompatible
	 *
	 * \return un entier : incompatibilites
	 */
	int  getIncompatibilites()
	{
		return incompatibilites;
	}
	
	/*!
	 * \brief Donne l'heure du début de l'arrêt programmé
	 *
	 * \return un entier : debutArretProgramme
	 */
	int getDebutArretProgramme() const
	{
		return debutArretProgramme;
	}
	
	/*!
	 * \brief Donne l'heure de fin de l'arrêt programmé
	 *
	 * \return un entier : finArretProgramme
	 */
	int getFinArretProgramme() const
	{
		return finArretProgramme;
	}

	/*!
	 * \brief Détermine si la turbine possède un arrêt programmé ou non
	 *
	 * \return true si la turbine possède un arrêt programmé, false sinon
	 */
	bool arretProgramme()
	{
		bool ret = true;
		if (debutArretProgramme ==0)
		{
			if (finArretProgramme==0)
				ret=false;
		}
		return ret;
	}
	
	/*!
	 * \brief Détermine si la turbine est en arrêt à une heure donnée
	 *
	 * \param h : entier, heure
	 *
	 * \return true si la turbine est en arrêt à l'heure h, false sinon
	 */
	bool estEnArret(int h)
	{
		bool ret = false;
		if (arretProgramme()==true)
			ret = (h+distance>=debutArretProgramme && h+distance<finArretProgramme);

		return ret;
	}

	/*!
	 * \brief Donne le numéro de la liaison associée
	 *
	 * \return un entier : numLien
	 */
	int getNumLiaison() const
	{
		return numLien;
	}
	
	//modificateurs :

	/*!
	 * \brief Modifie le tableau des prix
	 * 
	 * \param _prix : tableau de double, tableau de prix
	 */
	void setPrix(double* _prix)
	{
		prix = _prix;
	}
	
	/*!
	 * \brief Modifie le numéro de la liaison associée
	 * 
	 * \param _num : entier, numéro de la liaison
	 */
	void setNumLiaison(int _num)
	{
		numLien=_num;
	}
	
	/*!
	 * \brief Modifie la catégorie de prix
	 * 
	 * \param _catPrix : entier, catégorie de prix 
	 */
	void setCatPrix(int _catPrix)
	{
		catPrix=_catPrix;
	}

	/*!
	 * \brief Modifie le nombre de discrétisation des volumes
	 * 
	 * \param _nbInt : entier, nombre de discrétisation des volumes
	 */
	void setNbInt(int _nbInt)
	{
		nbInt = _nbInt;
	}	
	
	/*!
	 * \brief Modifie le nombre de discrétisation des débits
	 * 
	 * \param _nbPieces : nombre de discrétisation des débits
	 */
	void setNbPieces(int _nbPieces)
	{
		nbPieces = _nbPieces;
	}
	
	/*!
	 * \brief Modifie le numéro du réservoir en amont
	 * 
	 * \param i : entier, numéro du réservoir en amont
	 */
	void setReservoirParent(int i)
	{
		reservoirParent = i;
	}

	/*!
	 * \brief Modifie la production de la turbine
	 * 
	 * \param _nbInt : entier, nombre de discrétisation des volumes
	 * \param _nbPieces : entier, nombre de discrétisation des débits
	 * \param _intReservoirs : tableau de double, tableau des valeurs de discrétisation des volumes
	 * \param _piece : tableau de double, tableau des valeurs de discrétisation des débits
	 * \param _qmax : tableau de double, tableau des débits maxima
	 * \param _production : matrice de double, matrice des coefficients de production
	 */
	void setProd(int _nbInt, int _nbPieces, double* _intReservoirs, double* _piece, double* _qmax, double** _production)
	{
		nbInt = _nbInt;
		nbPieces = _nbPieces;
		intReservoirs = _intReservoirs;
		pieces = _piece;
		//cout<<"dans setProd piece[1="<<pieces[1]<<endl;
		qmax =_qmax;
		for(int i=0;i<nbInt;i++)
			qmax[i]*=3600;
        //double** production = (double**) malloc((nbPieces-1)*sizeof(double*));
       // for (int i=0; i<nbPieces-1; i++)
        //{   // production[i] = (double*) malloc(nbInt*sizeof(double));
		// for(int j=0;j<nbInt;j++)
		production = _production;
        //}
		//cout<<" dans setProd prod[0]="<<production[0][0]<<endl;
	}
	
	/*!
	 * \brief Modifie la valeur de production du palier
	 *
	 * \param _p : double, production
	 */
	void setPalier(double _p)
	{
		palier=_p;
	}
};
int Turbine::compteur=0;
#endif
