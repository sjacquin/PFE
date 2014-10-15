

#ifndef _eocasGeneral_h
#define _eocasGeneral_h
#include "Liaison.h"
#include"Systeme.h"

/**
 *  Always write a comment in this format before class definition
 *  if you want the class to be documented by Doxygen

 * Note that you MUST derive your structure from EO<fitT>
 * but you MAY use some other already prepared class in the hierarchy
 * like eoVector for instance, if you handle a vector of something....

 * If you create a structure from scratch,
 * the only thing you need to provide are
 *        a default constructor
 *        IO routines printOn and readFrom
 *
 * Note that operator<< and operator>> are defined at EO level
 * using these routines
 */
template< class FitT>
class eocasGeneral: public EO<FitT> {
public:
  /** Ctor: you MUST provide a default ctor.
   * though such individuals will generally be processed
   * by some eoInit object
   */
  eocasGeneral()
  {
    // START Code of default Ctor of an eocasGeneral object
	  nbHeures=0;
	  nbLiaisons=0;
	  nbReservoirs=0;
	
    // END   Code of default Ctor of an eocasGeneral object
  }

  /** Copy Ctor: you MUST provide a copy ctor if the default
   * one is not what you want
   * If this is the case, uncomment and fill the following
   */
  /*
  eocasGeneral(const eocasGeneral &)Turbine
  {
    // START Code of copy Ctor of an eocasGeneral object
    // END   Code of copy Ctor of an eocasGeneral object
  }
  */


  virtual ~eocasGeneral()
  {
    // START Code of Destructor of an eoEASEAGenome object
    // END   Code of Destructor of an eoEASEAGenome object
  }

  virtual string className() const { return "eocasGeneral"; }

  /** printing... */
    void printOn(ostream& os,Systeme* sys) const
    {
    	int i,t,j,k;
    	os<<"heure ; production ;";
    	for(i=0;i<sys->getNbTurbines()+sys->getNbPompes()-1;i++)
    		os<<";";
    	os<<"volumes ;";
    	for(i=0;i<nbReservoirs-1;i++)
    			os<<";";
    	os<<"flux; \n";
    	os<<";";
    	for(i=0;i<sys->getNbPompes();i++)
    	{
    		os<<sys->getPompe(i).getName()<<" ; ";
    	}
    	for(i=0;i<sys->getNbTurbines();i++)
    	{
    		os<<sys->getTurbine(i).getName()<<" ; ";
    	}
    	for(i=0;i<nbReservoirs;i++)
    	{
    		os<<sys->getReservoir(i).getName()<<" ; ";
    	}
    	for(i=0;i<nbLiaisons;i++)
    	{
    		Liaison liaison=sys->getLiaison(i);
    		int nbLiens=liaison.getNbLiens();
    		for(j=0;j<nbLiens;j++)
    		{
    			Lien lien=liaison.getLien(j);
    			if(lien.getQmaxBasique()==0) os<<"liaison fictive : ";//si le lien n'est pas fictif
    			//{
    				string precedent=sys->getReservoir(liaison.getReservoirIni()).getName();
    				for(k=0;k<lien.getNbTurbines();k++)
    				{
    					os<<precedent<<"->"<<lien.getTurbine(k)->getName()<<" ; ";
    					precedent=lien.getTurbine(k)->getName();
    				}
    				for(k=0;k<lien.getNbPompes();k++)
    				{
    					os<<precedent<<"->"<<lien.getPompe(k)->getName()<<" ; ";
    					precedent=lien.getPompe(k)->getName();
    				}
    				os<<precedent<<"->"<<sys->getReservoir(liaison.getReservoirFin()).getName()<<" ; ";
    			//}
    		}
    	}
    	os<<"\n"; //
    	//cout<<"fin lignes de titres"<<endl;
    	for(t=0;t<nbHeures;t++)
    	{
    		os<<t+1<<" ; ";

    		for(i=0;i<sys->getNbPompes();i++)
    		{
    			Pompe p=sys->getPompe(i);
    			int numL=p.getNumLiaison();
    			Liaison liaison=sys->getLiaison(numL);
    			int numLien;
    			for(j=0;j<liaison.getNbLiens();j++)
    			{
    				Lien lien=liaison.getLien(j);
    				for(k=0;k<lien.getNbPompes();k++)
    				{
    					if(lien.getPompe(k)->getNumero()==i)
    					{
    						numLien=j;
    					}
    				}
    			}
    			int dist=p.getDistance();
    			if(t-dist>-1)
    			{
    				std::vector<double> vQP=distribution(t-dist,numL,sys);
    				double qP=vQP[numLien];
    				os<<p.getProdV(contenus[t-dist][liaison.getReservoirIni()],qP)<<" ;";
    			}
    			else
    			{
    				os<<0<<" ; ";
    			}
    		}
    		for(i=0;i<sys->getNbTurbines();i++)
    		{
    			Turbine p=sys->getTurbine(i);
    			int numL=p.getNumLiaison();
    			Liaison liaison=sys->getLiaison(numL);
    			int numLien;
    			for(j=0;j<liaison.getNbLiens();j++)
    			{
    				Lien lien=liaison.getLien(j);
    				for(k=0;k<lien.getNbTurbines();k++)
    				{
    					if(lien.getTurbine(k)->getNumero()==i)
    					{
    						numLien=j;
    					}
    				}
    			}
    			int dist=p.getDistance();
    			if(t-dist>-1)
    			{
    				//cout<<"numLien= "<<numLien<<" nbLiens="<<liaison.getNbLiens()<<endl;
    				os<<p.getProdV(contenus[t-dist][liaison.getReservoirIni()],distribution(t-dist,numL,sys).at(numLien))<<" ;";
    			}
    			else
    			{
    				os<<0<<" ; ";
    			}
    		}
    		//cout<<"colone production"<<endl;
    		for(i=0;i<nbReservoirs;i++)
    		{
    			os<<contenus[t][i]<<" ; ";
    		}
    		//flux :
    		for(i=0;i<nbLiaisons; i++)
    		{
    			vector<double> distri=distribution(t,i,sys);
    			Liaison liaison=sys->getLiaison(i);
    			for(j=0;j<liaison.getNbLiens();j++)
    			{
    				Lien lien=liaison.getLien(j);
    				//if(lien.getQmaxBasique()>0)
    				//{
    				os<<distri[j]/3600<<" ; ";
    				for(k=0;k<lien.getNbTurbines();k++)
    				{
    					int dist=lien.getTurbine(k)->getDistance();
    					if(t-dist>=0)
    						os<<distribution(t-dist,i,sys)[j]/3600<<" ; ";
    					else os<<0<<" ; ";
    				}
    				for(k=0;k<lien.getNbPompes();k++)
    				{
    					int dist=lien.getPompe(k)->getDistance();
    					if(t-dist>0)
    						os<<distribution(t-dist,i,sys)[j]/3600<<" ; ";
    					else os<<0<<" ; ";
    				}
    				//}
    			}

    		}

    		os<<";\n";
    	}

      EO<FitT>::printOn(os);
      os << ' ';


    // END   Code of default output
    }

    void readFrom(istream& is)
         {
   	// of course you should read the fitness first!
   	EO<FitT>::readFrom(is);
       // START Code of input

   	/** HINTS
   	 * remember the eocasGeneral object will come from the default ctor
   	 * this is why having the sizes written out is useful
   	 */

       // END   Code of input
         }
    void readFromFile(string fileName,Systeme* sys)
      {
	// of course you should read the fitness first!
          int conteur=2;
          int i,j,k,t;
          for(i=0;i<sys->getNbTurbines()+sys->getNbPompes()-1;i++)
              conteur++;
          conteur++;
          for(i=0;i<nbReservoirs-1;i++)
              conteur++;
              conteur++;
         //premiere ligne sautée 
          conteur++;
          for(i=0;i<sys->getNbPompes();i++)
          {
             conteur++;
          }
          for(i=0;i<sys->getNbTurbines();i++)
          {
             conteur++;
          }
          for(i=0;i<nbReservoirs;i++)
          {
              conteur++;
          }
          for(i=0;i<nbLiaisons;i++)
          {
              Liaison liaison=sys->getLiaison(i);
              int nbLiens=liaison.getNbLiens();
              for(j=0;j<nbLiens;j++)
              {
                  Lien lien=liaison.getLien(j);
                                  
                  string precedent=sys->getReservoir(liaison.getReservoirIni()).getName();
                  for(k=0;k<lien.getNbTurbines();k++)
                  {
                      conteur++;
                    
                  }
                  for(k=0;k<lien.getNbPompes();k++)
                  {
                      conteur++;
                  }
                  conteur++;
               
              }
          }
          //deuxieme ligne sautée, maintenant on passe aux choses sérieuses :
          for(t=0;t<nbHeures;t++)
          {
              conteur++;
              
              for(i=0;i<sys->getNbPompes();i++)
              {
                  conteur++;
              }
              for(i=0;i<sys->getNbTurbines();i++)
              {
                  conteur++;
              }
              //cout<<"colone production"<<endl;
              for(i=0;i<nbReservoirs;i++)
              {
                  contenus[t][i]=atof(data[conteur].c_str());
                  conteur++;
              }
              //flux :
              for(i=0;i<nbLiaisons; i++)
              {
                  
                
                  Liaison liaison=sys->getLiaison(i);
                  int longueur=liaison.getLongueur();
                  debitEnCours[t][i][longueur]=0;
                  for(j=0;j<liaison.getNbLiens();j++)
                  {
                      Lien lien=liaison.getLien(j);
                      //if(lien.getQmaxBasique()>0)
                      //{
                      debitEnCours[t][i][longueur]+=atof(data[conteur].c_str());
                      longueur++;
                      for(k=0;k<lien.getNbTurbines();k++)
                      {
                          conteur++;
                      }
                      for(k=0;k<lien.getNbPompes();k++)
                      {
                          conteur++;
                      }
                      //}
                  }
                  for(j=1;j<longueur+1;j++)
                  {
                      debitEnCours[t+j][i][longueur-j]=debitEnCours[t][i][longueur];
                      
                  }
                  
              }
              

          }
 
          

      }
    //accesseurs:
    const vector< vector<int> >  getTempsMmPuissance()const
 		{
     	return tempsMmPuissance;
 		}
 
    const vector<int> &  getTempsMmPuissance(int h)const
		{
    	return tempsMmPuissance[h];
		}
    int getTempsMmPuissance(int h,int i)const
    {
    	return tempsMmPuissance[h][i];
    }
   vector< vector<double> > & getVolumes()
	{
		return contenus;
	}



  vector<vector< vector<double> > >& getDebitEnCours() 
    {
       return debitEnCours;
    }

    const vector< vector<double> > & getDebitEnCours(int i) const
		{
    	 return debitEnCours[i];
		}
    const vector<double> & getDebitEnCours(int h,int i) const
		{
    		return debitEnCours[h][i];
		}
double getDebitEnCours(int h,int i,int j) const
    		{
          if(h<0 || h>nbHeures||i>=nbLiaisons ||i<0 || j<0) 
              cerr <<"error casGeneral.h getDebitEnCours  "<<h<<" "<< i << " " << j <<endl;
      		return debitEnCours[h][i][j];
    		}
   
    int getNbLiaisons() const
    {
    	return nbLiaisons;
    }
    
    int getNbHeures() const
    {
    	return nbHeures;
    }
    int getNbReservoirs() const
	{
		return nbReservoirs;
	}

  vector<vector<double> > & getContenu()
  {
    return contenus;
  }
 vector<double>  & getVolumes(int i)
   {
     return contenus[i];
   }
    double getContenu(int h, int i) const
	{
		if(h<0 || h>=nbHeures||i>nbReservoirs ||i<0) 
      cerr <<"error casGeneral.h getContenu  "<<nbReservoirs<<" "<<h<<" "<<i<<endl;
		return contenus[h][i];
	}
	double getBenefice(int h)
	{
		return benefices[h];
	}
	bool getModif(int h)
	{
		return modif[h];
	}
    //modificateurs :
  
    void setDimensions(int _nbHeures,int _nbLiaisons,int _nbReservoirs, vector<Liaison> & liaisons)
    {
    	int i,j,h;
    	nbHeures=_nbHeures;
    	nbLiaisons=_nbLiaisons;
    	nbReservoirs=_nbReservoirs;
    	

    	for(int h=0;h<nbHeures;h++)
    	{
		benefices.push_back(0);
		modif.push_back(true);
    		vector< vector <double> > l;
    		vector<int> tps;
    		for(int i=0;i<nbLiaisons;i++)
    		{
    			vector<double> enCours;
    			int taille= liaisons[i].getLongueur();
    			for(j=0;j<taille+1;j++)
    			{
    				enCours.push_back(0);
    			}
    			l.push_back(enCours);
    			tps.push_back(1);
    		}
    		tempsMmPuissance.push_back(tps);
    		debitEnCours.push_back(l);
    		
    	}
    }
    void setContenu(vector<vector<double> > _qt)
    {
    	contenus=_qt;
    }
  void setDebit(vector<vector<vector<double> > > _deb)
  {
	  debitEnCours=_deb;
  }
    void adDebitEnCours(vector<double> deb, int h ,int i)
    {
    	debitEnCours[h][i]=deb;
    }
    void setContenu(int h,int r,double qte)
    {
	contenus[h][r]=qte;
    }
    void setTempsMemePuissance(int h,int l,int t)
    {
	tempsMmPuissance[h][l]=t;
    }
    void setDebitEnCours(int h,int l,int p,double q)
	{
    	if(h<0 || h>nbHeures || l>nbLiaisons ) cout<<"h :"<<h<< " num liaison "<<l <<" num piece "<<p<<endl;
		debitEnCours[h][l][p]=q;
	}
    void setBenefice(int h, double val)
    {
	benefices[h]=val;
    }
    void setModif(int h, bool b)
    {
	modif[h]=b;
     }



    vector<double>  distribution (int h,int i,Systeme* systeme) const
    {

    	//cout<<"debut distribution"<<endl;
    	int j,k;

    		Liaison liaison=systeme->getLiaison(i);
    		int nbLiens=liaison.getNbLiens();
    		vector<double> distribution(nbLiens,0);
    		int longL=liaison.getLongueur();
    		double contenu;
    		if(h>0)contenu=contenus[h-1][liaison.getReservoirIni()];
    		else contenu=systeme->getReservoir(liaison.getReservoirIni()).getVinit();
    		//cout<<"qtot?"<<endl;
    		double qTot=debitEnCours[h][i][longL];


    			int numCond=-1;
    			bool cond2=true;
    			for(j=0;j<nbLiens;j++)
    			{
    				double qminL=liaison.getLien(j).getQmin();
    				if(qminL<=qTot+0.00001 || qminL==0)
    				{
    					qTot-=qminL;
    					distribution[j]+=qminL;

    				}
    				else
    				{
    					cond2=false;

    					distribution[j]+=qTot;
    					qTot=0;
    					//if(h<1000)cout<<" qmin ="<<qminL<<" qTot="<<qTot<<" h="<<h<<endl;
    				}
    				if(liaison.getLien(j).getNbPompes()+liaison.getLien(j).getNbTurbines()>0)
    				{
    					numCond=j;
    				}

    			}
    			//cout<<"si cond2"<<endl;
    			if(cond2&&numCond>-1)
    			{
    				int nbTurbines=liaison.getLien(numCond).getNbTurbines();
    				int nbPompes=liaison.getLien(numCond).getNbPompes();

    				double qPmin=0;
    				double qPmax=900000000000;
    				for(j=0;j<nbTurbines;j++)
    				{
    					//cout<<"ds pour1"<<endl;
    					double qminj=liaison.getLien(numCond).getTurbine(j)->getQmin(contenu);

    					if(qminj>qPmin)qPmin=qminj;

    					double qmaxj=liaison.getLien(numCond).getTurbine(j)->getQmax(contenu);
    					if(qmaxj<qPmax)qPmax=qmaxj;
    				}
    				//cout<<"apres pour1"<<endl;
    				for(j=0;j<nbPompes;j++)
    				{
    					//cout<<"j="<<j<<" numCond="<<numCond<<" contenu="<<contenu<<" Vmax=" <<systeme->getReservoir(liaison.getReservoirIni()).getVmax()<<" "<<
    							//systeme->getReservoir(liaison.getReservoirIni()).getName() <<endl;
    					double qminj=liaison.getLien(numCond).getPompe(j)->getQmin(contenu);
    					//cout<<"qminj="<<qminj<<endl;
    					if(qminj>qPmin)qPmin=qminj;
    					double qmaxj=liaison.getLien(numCond).getPompe(j)->getQmax(contenu);
    					//cout<<"qmaxj="<<qmaxj<<endl;
    					if(qmaxj<qPmax)qPmax=qmaxj;
    				}

    				if(qTot>qPmin)
    				{
    					double qte;
    					if(qTot>qPmax)
    					{
    						qte=qPmax;

    						qTot-=qPmax;
    					}
    					else
    					{
    						qte=qTot;
    						qTot=0;
    					}
    					distribution[numCond]+=qte;

    				}



    			}
    			//cout<<"distribuer qte d'eau restante:"<<endl;
    			//distribuer qte d'eau restante:
    			if(cond2)
    			{
    				for(j=0;j<nbLiens;j++)
    				{
    					if(j!=numCond)
    					{

    					double qmaxL=liaison.getLien(j).getQmax(contenu);
    					double qminL=liaison.getLien(j).getQmin();

    					if(qmaxL-qminL<qTot)
    					{
    						//cout<<qmaxL<<" ";
    						distribution[j]+=qmaxL-qminL;
    						qTot-=(qmaxL-qminL);
    					}
    					else
    					{
    						distribution[j]+=qTot;
    						qTot=0;
    					}}
    				}

    			}
    		//	cout<<"fin distribution"<<endl;
    			return distribution;
    }
    
    
    bool readDataFile(string fileName) {
        // open the file
        bool ret = true;
        // cout << "readDataFile" << endl;
        // cout << fileName << endl;
        std::ifstream file(fileName.c_str(), std::ios::in);
        // cout << "ok" << endl; 
        std::string buff;
        data.clear();
        if (file) 
        {
            getline(file, buff);
            while (getline(file, buff, ';'))
            {
                if (buff[0] != '\n')
                    data.push_back(buff);
                else if(buff != "\n")
                    data.push_back(buff.substr(1,buff.size()));
            }
            // cout << "pas ok " << endl;
        }
        else
            ret = false;
        file.close();
        return ret;
    }


private:			   // put all data here
    // START Private data of an eocasGeneral object
    vector<vector<double> > contenus;//temps*reservoir
    vector<vector<int> > tempsMmPuissance;//temps*liaisons
   vector< vector< vector<double> > >  debitEnCours; //temps*liaisons*longueur 
	vector<double> benefices;
	vector<bool> modif;
    int nbLiaisons;
    int nbHeures;
    int nbReservoirs;
    vector<string> data;/*!< Vecteur des données récupérées lors de la lecture des fichiers */

    // END   Private data of an eocasGeneral object
};

#endif
