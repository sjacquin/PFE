/** -*- mode: c++; c-indent-level: 4; c++-member-init-indent: 8; comment-column: 35; -*-

The above line is usefulin Emacs-like editors
 */

/*
Template for evaluator in EO, a functor that computes the fitness of an EO
==========================================================================
*/

#ifndef _eocasGeneralEvalFunc_h
#define _eocasGeneralEvalFunc_h

// include whatever general include you need
#include <stdexcept>
#include <fstream>
#include <math.h>  
// include the base definition of eoEvalFunc
#include "eoEvalFunc.h"

/**
  Always write a comment in this format before class definition
  if you want the class to be documented by Doxygen
*/
template <class EOT>
class eocasGeneralEvalFunc : public eoEvalFunc<EOT>
{
public:
	/// Ctor - no requirement
// START eventually add or modify the anyVariable argument
  eocasGeneralEvalFunc(Systeme* _systeme,int _nbHeures,vector<int> _pompeTurpe,  vector<vector<double> > _coefsTurpe,double  penalite=1000000.0)
  //  eocasGeneralEvalFunc( varType  _anyVariable) : anyVariable(_anyVariable)
// END eventually add or modify the anyVariable argument
  {
    // START Code of Ctor of an eocasGeneralEvalFunc object
	  nbHeures=_nbHeures;
	  systeme=_systeme;
	  pompeTurpe=_pompeTurpe;
	  coefsTurpe=_coefsTurpe;
	  coefPenalite=penalite;
    // END   Code of Ctor of an eocasGeneralEvalFunc object
  }
  eocasGeneralEvalFunc()
    //  eocasGeneralEvalFunc( varType  _anyVariable) : anyVariable(_anyVariable)
  // END eventually add or modify the anyVariable argument
    {
      // START Code of Ctor of an eocasGeneralEvalFunc object
  	  nbHeures=0;
  	  
    }

  /** Actually compute the fitness
   *
   * @param EOT & _eo the EO object to evaluate
   *                  it should stay templatized to be usable
   *                  with any fitness type
   */
  void operator()(EOT & _eo)
  {
	 // cout<<"debut eval"<<endl;
    // test for invalid to avoid recomputing fitness of unmodified individuals
    if (_eo.invalid())
      {
	double fit=0;		   // to hold fitness value
    // START Code of computation of fitness of the eocasGeneral object
	//Calcul //cout prod et pénalites
	
	for(int h=0;h<nbHeures;h++)
	{


		//if(_eo.getModif(h))

			calculBenefice(h,_eo);
			//_eo.setBenefice(h,b);

		fit+=_eo.getBenefice(h);
	}
	//calcul turpe :
	//fit-=turpe(_eo);
		
    // E   Code of computation of fitness of the eocasGeneral object
	_eo.fitness(fit);
      }
   // cout<<"fin eval"<<endl;
  }
/*double calculBenefice2Units(int h,EOT & _eo,int numL)
{
	Liaison liaison=systeme->getLiaison(numL);
	int lien1=-1, lien2,i;
	for(i=0;i<liaison.getNbLiens();i++)
	{
		if(liaison.getLien(i).getNbTurbines()+liaison.getLien(i).getNbPompes()>0)
		{
			if(lien1==-1)
				lien1=i;
			else lien2=i;
		}
	}
	double coef1,coef2;
	coef1=
}*/
void calculBenefice(int h,EOT & _eo)
{
	//cout<<"debut calcul benefice"<<endl;
	//double penalite=10000;
	int i,j,k;
	double benefice=0;
	int nbLiaison=systeme->getNbLiaisons();
	for(i=0;i<nbLiaison;i++)
	{
		Liaison liaison=systeme->getLiaison(i);
		int nbLiens=liaison.getNbLiens();
		int longL=liaison.getLongueur();
		double contenu;
		if(h>0)contenu=_eo.getContenu(h-1,liaison.getReservoirIni());
		else contenu=systeme->getReservoir(liaison.getReservoirIni()).getVinit();
		//cout<<"qtot?"<<endl;
		double qTot=_eo.getDebitEnCours(h,i,longL);

		//verification delta lo,up :
		bool cond1=true;
		double dlo=liaison.getDeltaLo();
		double dup=liaison.getDeltaUp();
		
		if(h>0)
		{
			double qTotp=_eo.getDebitEnCours(h-1,i,longL);
			if(dlo>0 && dlo<FLT_MAX && qTot<qTotp-dlo)
			{
				//cout<<"dlo="<<dlo<<endl;
				benefice-=coefPenalite*(qTotp-dlo-qTot);
				//cout<<" dlo violée à lh: "<<h<<" dlo="<<dlo;
				cond1=false;
			}
			 if(dup>0 &&qTot>qTotp+dup)
			 {
				 benefice-=coefPenalite*(qTot-qTotp+dup);
				// cout<<" dup violée à lh: "<<h<<" dup="<<dup;
				 cond1=false;
			 }
		}
		//cout<<"deltalo up"<<endl;
		//if(cond1)
		//{
			int numCond=-1;
			bool cond2=true;
			for(j=0;j<nbLiens;j++)
			{
				double qminL=liaison.getLien(j).getQmin();
				if(qminL<=qTot+0.00001 || qminL==0)
				{
					qTot-=qminL;
				}
				else
				{
					cond2=false;

					benefice -=coefPenalite*(qminL-qTot);
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
				//cout<<"regarder si possible de respecter et utiliser au max :"<<endl;
				//regarder si possible de respecter et utiliser au max :
				//cout <<" qPmin et qPmax "<<qPmin<<" "<<qPmax;
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
					//cout<<"là"<<endl;
					for(j=0;j<nbTurbines;j++)
					{
						int dist=liaison.getLien(numCond).getTurbine(j)->getDistance();
						//cout<<"dist="<<dist<<endl;
						if(h+dist<nbHeures)benefice+=liaison.getLien(numCond).getTurbine(j)->getBenefice(contenu,qte,h+dist);
					}
					//cout<<"apres premier for"<<endl;
					for(j=0;j<nbPompes;j++)
					{
						int dist=liaison.getLien(numCond).getPompe(j)->getDistance();
						if(h+dist<nbHeures)benefice+=liaison.getLien(numCond).getPompe(j)->getBenefice(contenu,qte,h+dist);
					}
					//cout<<"apres 2eme for"<<endl;
					
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
						qTot-=qmaxL-qminL;
					}
					else
					{
						qTot=0;
					}}
				}
				if(qTot>0)
				{
					benefice-=coefPenalite*qTot;
					//cout<<" qmax non respectée reste qTot= "<<qTot<<" a h="<<h;
				}
			}
		//} //fin si cond1
	}
	benefice-=calculPenalitesAchatPompes(_eo,h);
	_eo.setModif(h,false);
	_eo.setBenefice(h,benefice);
	//cout<<"fin caolcul benefice"<<endl;
	
}
double turpe(EOT & _eo)
{
	//cout<<"debut turpe"<<endl;
	int i;
	double turpe=0;
	for (i=0;i<pompeTurpe.size();i++)
	{

		Pompe* p=&(systeme->getPompe(pompeTurpe[i]));
		int numL=p->getNumLiaison();

		Liaison liaison=systeme->getLiaison(numL);
		int longL=liaison.getLongueur();
		int dist= p->getDistance();
		int rA= liaison.getReservoirIni();

		//Calcul Esoutire:
		double Esoutire=0;
		for(int h=0;h<nbHeures;h++)
		{
			Esoutire+=p->getProdV(_eo.getContenu(h,rA),_eo.getDebitEnCours(h,numL,longL-dist));
		}

		//calcul To:
		double to=Esoutire/coefsTurpe[i][3]*nbHeures;
		//calcul CS :
		double CS=coefsTurpe[i][0]*coefsTurpe[i][3]+coefsTurpe[i][1]*coefsTurpe[i][3]*pow(to,coefsTurpe[i][2]);
		//Calcul Turpe :

		if (Esoutire != 0)
			turpe+=1000*CS/Esoutire;
		else 
			turpe = 0;

		
	}
	//cout<<"fin turpe"<<endl;
	return turpe;
}
double calculPenalitesAchatPompes(EOT & _eo,int t)
{
	double penalite=0;
	int nbPompes=systeme->getNbPompes();
	int i, j,k;
	for(i=0;i<nbPompes;i++)
	{
		Pompe p=systeme->getPompe(i);
		vector<int>  achats =p.getAchat();

		double prodA=0;
		//cout<<"ici"<<endl;
		for(j=0;j<p.getNbAchats();j++)
		{
			Turbine T=systeme->getTurbine(achats[j]);
			Liaison l=systeme->getLiaison(T.getNumLiaison());
			if(t-T.getDistance()>=0){
			double vol=_eo.getContenu(t-T.getDistance(),T.getReservoirParent());
			int lg=l.getLongueur() -T.getDistance();
			vector<Lien> liens =l.getLiens();
			double deb=_eo.getDebitEnCours(t,T.getNumLiaison(),lg);
			for(k=0;k<liens.size();k++)
			{
				if(liens[k].getNbPompes()+liens[k].getNbTurbines()==0)
				{
					deb-=liens[k].getQmin();
				}
			}


			if(deb>T.getQmin(vol))prodA+=T.getProdV(vol,deb);
		}
		}
		//cout<<"fin calcul prodA"<<endl;
		Liaison l=systeme->getLiaison(p.getNumLiaison());
		double prodP=0;
		if(t-p.getDistance()>=0){
		double vol=_eo.getContenu(t-p.getDistance(),p.getReservoirParent());
		int lg=l.getLongueur() -p.getDistance();
		vector<Lien> liens =l.getLiens();
		double deb=_eo.getDebitEnCours(t,p.getNumLiaison(),lg);
		for(k=0;k<liens.size();k++)
		{
			if(liens[k].getNbPompes()+liens[k].getNbTurbines()==0)
			{
				deb-=liens[k].getQmin();
			}
		}
		//cout<<"fin"<<endl;


		prodP+=p.getProdV(vol,deb);


	}
		if(prodP<prodA)
			penalite+=penalite*(prodA-prodP);


	}
	return penalite;

}
private:
// START Private data of an eocasGeneralEvalFunc object
  //  varType anyVariable;		   // for example ...
double coefPenalite;
  Systeme* systeme;
  int nbHeures;
  vector<int> pompeTurpe;
  vector<vector<double> >coefsTurpe;
  
// END   Private data of an eocasGeneralEvalFunc object
};


#endif
