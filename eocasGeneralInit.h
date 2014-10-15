/** -*- mode: c++; c-indent-level: 4; c++-member-init-indent: 8; comment-column: 35; -*-

The above line is usefulin Emacs-like editors
 */

/*
Template for EO objects initialization in EO
============================================
 */

#ifndef _eocasGeneralInit_h
#define _eocasGeneralInit_h

// include the base definition of eoInit
#include <eoInit.h>

#include "utils.h"
#include "Systeme.h"
/**
 *  Always write a comment in this format before class definition
 *  if you want the class to be documented by Doxygen
 *
 * There is NO ASSUMPTION on the class GenoypeT.
 * In particular, it does not need to derive from EO (e.g. to initialize
 *    atoms of an eoVector you will need an eoInit<AtomType>)
 */
template <class GenotypeT>
class eocasGeneralInit: public eoInit<GenotypeT> {
public:
	/// Ctor - no requirement
	// START eventually add or modify the anyVariable argument
	eocasGeneralInit(int _nbHeures, Systeme* _systeme, double _biais=0.25)
//  eocasGeneralInit( varType  _anyVariable) : anyVariable(_anyVariable)
// END eventually add or modify the anyVariable argument
{
		// START Code of Ctor of an eocasGeneralInit object
		nbHeures=_nbHeures;
		systeme=_systeme;
		nbLiaisons=systeme->getNbLiaisons();
		nbReservoirs=systeme->getNbReservoirs()-1;
		nbComposantesConnexes= systeme->getNbComposantesConnexes();
		biais = _biais;


		// END   Code of Ctor of an eocasGeneralInit object
}


	/** initialize a genotype
	 *
	 * @param _genotype  generally a genotype that has been default-constructed
	 *                   whatever it contains will be lost
	 */
	void operator()(GenotypeT & _genotype)
	{
		// START Code of random initialization of an eocasGeneral object
		cout<<"debut ini"<<endl;
		int i,j,k,l;
		int h;
		vector<Liaison> vecL =systeme->getLiaisons();
		
		_genotype.setDimensions(nbHeures,nbLiaisons,nbReservoirs,vecL);

		
		//initialisation qte d'eau dans les reservoirs:
		vector<double> qteReservoir;
		vector <vector<double> > debitEnCours;
		for(i=0;i<nbReservoirs;i++)
		{
			Reservoir* R=&(systeme->getReservoir(i));
			qteReservoir.push_back(R->getVinit()+R->getApport(0));
		}
		
		_genotype.setModif(0,true);https://www.facebook.com/
		//ajouter à contenu
		// calcul du contenu des reservoirs pour les autres heures :
		vector< vector<double> > contenus;
		contenus.push_back(qteReservoir);
		for(h=1;h<nbHeures;h++)
		{
			_genotype.setModif(h,true);
			qteReservoir.clear();
			for(i=0;i<nbReservoirs;i++)
			{
				//Reservoir* R=&(systeme->getReservoir(i));
				qteReservoir.push_back(0);
			}
			contenus.push_back(qteReservoir);		
		}
		_genotype.setContenu(contenus);
		//cout<<"contenus ini"<<endl;
		for(i=0;i<nbComposantesConnexes-1;i++)
		{
			//cout<<"num composante :"<<i<<endl;
			//calcul tot à faire sortir = somme de tout ce qui rentre: (pas possible d'avoir reservoir final)
			ComposanteConnexe C= systeme->getComposanteConnexe(i);
			//cout<<"comp recupérée"<<endl;
			int tailleComposante=C.getNbReservoirs();
			double qTot=0;
			for(h=0;h<nbHeures;h++)
			{
				for(j=0;j<tailleComposante;j++)
				{
					//ajouter les apports

					Reservoir R=systeme->getReservoir(C.getReservoir(j));
					qTot=qTot+ R.getApport(h);
					//ajouter les entrées calculées précédement :
					vector<int> entrees=R.getEntrees();
					//if(h==0)cout<<i<<" Rnum "<<R.getNumero()<<" card entrees "<<R.getCardEntrees()<<" cardSorties "<<R.getCardSorties()<<endl;
					for(k=0;k<R.getCardEntrees();k++)
					{
						int entree= entrees[k];
						if(h==0 ) cout<<entrees[k]<<" ";
						Liaison liaisonE=systeme->getLiaison(entree);
						int pred=liaisonE.getReservoirIni();
						bool entreeComposante=true;
						for(l=0;l<tailleComposante;l++)
						{
							if(liaisonE.getReservoirIni()==C.getReservoir(l))
								entreeComposante=false;
						}
						if(entreeComposante)qTot=qTot+ _genotype.getDebitEnCours(h,entree,0);
						//if(_genotype.getDebitEnCours(h,entree,0)<0)cout<<"Inferieur 0"<<endl;
					}
					if(h==0)cout<<endl;
				}
			}
			//cout<<"qTot calculé"<<endl;
			if(tailleComposante==1)// si unique réservoir
			{

				Reservoir R=systeme->getReservoir(C.getReservoir(0));
				int numR=R.getNumero();
				double qteRT=0;
				vector<double> qteMaxInt;
				//si nombre intervalles de definitions de Vmin plus grand que 1 alors calcul de qte max intermediaires :
				int nbIntVmin=R.getNbIntVmin();
				if(nbIntVmin>1)
				{
					//calcul des qteMaxInt pour chaque intervalle : 
					for(j=1;j<nbIntVmin;j++)
					{
						double Vp= R.getVinit();
						for(k=0;k<R.getIntVmin(j);k++)
						{
							Vp+=R.getApport(k);
							//cout<<R.getApport(k)<<" ";
							for(int z=0;z<R.getCardEntrees();z++)
							{
								Vp+=_genotype.getDebitEnCours(k,R.getEntrees()[z],0);
							}
						}
						qteMaxInt.push_back(Vp-R.getVmin(j));
					}
				}
				//Pour chaque heure :
				double qteR=0;
				double qteRP;
				for(h=0;h<nbHeures;h++)
				{
					//cout<<"l 158"<<endl;
					//calcul contenu ini:
					if(h>0) _genotype.setContenu(h,numR,_genotype.getContenu(h-1,numR)+R.getApport(h));
					else _genotype.setContenu(h,numR,R.getVinit()+R.getApport(h));
					//ajout des entrees : 
					vector<int> entrees=R.getEntrees();
					int nbEntrees=R.getCardEntrees();
					for(int li=0;li<nbEntrees;li++)
					{
						_genotype.setContenu(h,numR,_genotype.getContenu(h,numR)+_genotype.getDebitEnCours(h,entrees[li],0));
					}
					
					//calcul qte min et max qu'il faut débiter pour respecter  contraintes sur le contenu des reservoirs :
					double Vmax=R.getVmax();
					double Vmin=R.getRealVmin(h);
					if(Vmin<0.000001)Vmin=0;
					double vmin=0;
					
					if(vmin < _genotype.getContenu(h,numR)-Vmax) vmin= _genotype.getContenu(h,numR)-Vmax;
				
					double vmax= _genotype.getContenu(h,numR) -Vmin;
					
					if(qteRT+vmax>qTot) vmax=qTot-qteRT;
					if(nbIntVmin>1)
					{
						int numInt=R.getNumIntVmin(h);
						if(numInt<nbIntVmin-1 && qteRT+vmax>qteMaxInt[numInt]) vmax=qteMaxInt[numInt]-qteRT;
						//cout<<"qteMaxInt[numInt] "<<qteMaxInt[numInt]<<" qteRT "<<qteRT<<" nbIntVmin "<<nbIntVmin<<" numInt "<<numInt<<endl;
					}
					// Pour chaque liaison :
					int cardSorties= R.getCardSorties();
					vector<int> sorties=R.getSorties();
					qteRP=qteR;
					 qteR=0;

					//if(vmin>vmax)cout <<"vmin>vmax h"<<h<<" vmin "<<vmin<< " vmax "<<vmax<<" qTot-qteRT " <<qTot-qteRT<<" "<<qTot<< " qteRT "<<qteRT<<endl;
					//cout <<"Vmax "<<Vmax<<" Vmin "<<R.getRealVmin(h)<<endl;
					//cout<<"calcul qmin qmax "<<cardSorties-1 <<" "<<sorties.size()<<" "<<numR<<endl;
				
					for(j=0;j<cardSorties-1;j++)
					{
						//liaison : 
						int numL= sorties[j];
						Liaison liaison =systeme->getLiaison(numL);
						int longL=liaison.getLongueur();
						//calcul qte_min et qte_max pour la liaison :
						double qte_min=0;
						double qte_max=vmax-qteR;
						//cout<<"au deb "<<qte_min<<" "<<qte_max<<endl;
						//vérifier si arret programmé :
						bool ArretProgramme=liaison.enArret(h);
						if(ArretProgramme)
						{
							qte_max=0;
							
						}
						//cout<<"liasons"<<endl;
						//Incompatibilités : 
						//cout<<"av incompatibilite qte_min="<<qte_min<<" qte_max="<<qte_max<<endl;
						if(liaison.contrainteDincompatibilite())
						{
							//cout<<"dans si"<<endl;
							int nbLiens=liaison.getNbLiens();
							bool constraint=true;

							Lien  li1=liaison.getLien(0);
							bool b=false;
							vector<Turbine*> vT=li1.getListeTurbines();
							vector<Pompe*> vP=li1.getListePompes();
							for(l=0;l<li1.getNbTurbines()+li1.getNbPompes();l++)
							{
								//cout<<"dans pour"<<endl;
								
								//cout<<"numI"<<numI<<endl;
								if(li1.getNbPompes()>0)//à vérifier si ça colle
								{
									int numI=vP[l]->getIncompatibilites();
									   if(numI>-1)
								{
									//cout<<"cas Pompe "<<numI<<endl;
									//regarder si allumée à h+distT : qte d'eau qui arrive :
									int disT=vP[l]->getDistance();
									Turbine tI=systeme->getTurbine(numI);
									int distTI=tI.getDistance();
									int numLI=tI.getNumLiaison();
									int longLI=systeme->getLiaison(numLI).getLongueur();
									if(h+disT<nbHeures &&_genotype.getDebitEnCours(h+disT,numLI,longLI-distTI)>0)
									{
										qte_max=0; 
									}
								}

								}
								else {
									int numI=vT[l]->getIncompatibilites();
									if(numI>-1)//à vérifier si ça colle
								
								{
									//cout<<"cas Turb "<<numI<<endl;
									//regarder si allumée à h+distT : qte d'eau qui arrive :
									int disT=vT[l]->getDistance();
									Pompe tI=systeme->getPompe(numI);
									int distTI=tI.getDistance();
									int numLI=tI.getNumLiaison();
									int longLI=systeme->getLiaison(numLI).getLongueur();
									if(h+disT<nbHeures &&_genotype.getDebitEnCours(h+disT,numLI,longLI-distTI)>0)
									{
										qte_max=0; 
									}

								}
								}
							}

						}
						int tpsMinConstant=liaison.getMinTpsConstant();
					//	cout<<"ap incompatibilite qte_min="<<qte_min<<" qte_max="<<qte_max<<endl;
							//cout<<"incompatibilité"<<endl;
						//palier de production
						//cout<<"av palier qte_min="<<qte_min<<" qte_max="<<qte_max<<endl;
						int nbLiens=liaison.getNbLiens();
						Lien  li1=liaison.getLien(0);
						if(qte_max<=0.000001 && h>0){
							if(nbLiens==1 && li1.getNbTurbines()+li1.getNbPompes()>0)//la contrainte ne peut exister sinon
							{
								vector<Turbine*> vT=li1.getListeTurbines();
								int nbTurb_li1=li1.getNbTurbines();
								for(k=0;k<nbTurb_li1;k++)
								{
									double palier=vT[k]->getPalier();
									if(palier>0  )
									{
										//if(h<5500 &&h>500)cout<<"ICI0 h="<<h<<" qpred="<<_genotype.getDebitEnCours(h-1,numL,longL)<<" numL="<<numL<<endl;
										//calcul prodPredMin  :
										double QPalier;
										if(h>1)QPalier=vT[k]->getQPalier(_genotype.getContenu(h-2,numR));
										else QPalier=vT[k]->getQPalier(R.getVinit());
										if((_genotype.getDebitEnCours(h-1,numL,longL)!=QPalier||_genotype.getTempsMmPuissance(h-1,numL)<tpsMinConstant ) && _genotype.getDebitEnCours(h-1,numL,longL)>0)
										{
											//recherche derniere heure ou il est possible de se ramener à QPalier :
											int hQ=h-1;
											if(hQ>0)QPalier=vT[k]->getQPalier(_genotype.getContenu(hQ-1,numR));
											else QPalier=vT[k]->getQPalier(R.getVinit());
											//cout<<"lav while"<<endl;
											while(hQ>-1 && (_genotype.getDebitEnCours(hQ,numL,longL)<QPalier ||_genotype.getTempsMmPuissance(hQ,numL)<tpsMinConstant))
											{
												hQ--;
												if(hQ>0)QPalier=vT[k]->getQPalier(_genotype.getContenu(hQ-1,numR));
												else QPalier=vT[k]->getQPalier(R.getVinit());
											}
											int tps=0;
											if(hQ>-1)tps=_genotype.getTempsMmPuissance(hQ,numL);
											//cout<<"LAAAAAAA hQ "<<hQ<<endl;
											//recherche liaison de redirection :
											double dif=0;
											if(hQ>-1) dif=_genotype.getDebitEnCours(hQ,numL,longL)-QPalier;
											int RFin=liaison.getReservoirFin();
											vector<Liaison*> lcandidats=systeme->getLiaisonsAToB(numR,RFin);
											int nbCandidats=lcandidats.size();
											int kand=0;
											int numRedirection=-1;
											while(kand<nbCandidats&&numRedirection==-1)//attention k !
											{
												if(lcandidats[kand]->getNumero()!=numL && lcandidats[kand]->getLongueur()==longL  )
												{
													bool accepte=true;
													
													for(l=hQ-tps;l<h;l++)
													{
														if(l>-1 && lcandidats[kand]->enArret(l)) accepte=false;
													}
													if(lcandidats[kand]->getMinTpsConstant()>1)accepte=false;
													if(lcandidats[kand]->contrainteDincompatibilite()) accepte=false;
													if(lcandidats[kand]->palier())accepte=false;
													if(accepte) numRedirection=lcandidats[kand]->getNumero();
												}
												kand++;
											}
											//cout<<"num redec "<<endl;
											if(numRedirection==-1)cerr <<"Ini: ajout conduite necessaire pour contraintes temps Min de fonctionement l369 "<<numR<<" "<<RFin<<" nbCand "<<nbCandidats<<" "<<R.getCardSorties()<<endl;
											//modif debits en cours :
											
											for(l=hQ+1-tps;l<hQ+1;l++)
											{
												//modif liason :
												for(int m=longL;m>-1;m--)
												{
													// cout << "est-ce là l 348" << endl;
													_genotype.setDebitEnCours(l+(longL-m),numL,m,QPalier);
												}
												//modif redirection
												for(int m=longL;m>-1;m--)
												{
													// cout << "est-ce là l 354" << endl;
													_genotype.setDebitEnCours(l+(longL-m),numRedirection,m,_genotype.getDebitEnCours(l+(longL-m),numRedirection,m)+dif);
												}

											}
											//autres heures :
											for(l=hQ+1;l<h;l++)
											{
												//modif redirection
												for(int m=longL;m>-1;m--)
												{
													// cout << "est-ce là l 365" << endl;
													_genotype.setDebitEnCours(l+(longL-m),numRedirection,m,_genotype.getDebitEnCours(l+(longL-m),numRedirection,m)+_genotype.getDebitEnCours(l+(longL-m),numL,m));
													// cout << "est-ce là l 367" << endl;
													_genotype.setDebitEnCours(l+(longL-m),numL,m,0);
												}
											}
											
										

										}
									}

								}
							}
						}
						//cout<<"ap palier de production qtemin="<<qte_min<<" qte_max="<<qte_max<<endl;
						//tempsMinimal de fonctionement 
						//cout<<"av temps Min qtemin="<<qte_min<<" qmax="<<qte_max<<endl;
						
						if(tpsMinConstant>1 && h>0)
						{
						//	cout <<"ds tpsMin"<<endl;
							//recuperertpsMinconstant heure pred  :
							int tps= _genotype.getTempsMmPuissance(h-1,numL);
							//if(h==1) cout<<"tps="<<tps<<" tpsMin="<<tpsMinConstant<<endl;
							if(tps<tpsMinConstant)
							{
								//reccuperer qtePred : 
								double qtePred = _genotype.getDebitEnCours(h-1,numL,longL);
								//regarder s'il est possible de debiter qtePred  
								if(qtePred>=qte_min && qtePred<=qte_max)
								{
									//if(h>5000 && h<6000)cout<<"là 1 à h ="<<h<<endl;
									qte_min=qtePred;
									qte_max=qtePred;
									//cout<<"par là h="<<h<<endl;
								}
							
								else
								{
									if(qtePred>qte_max)
									{
										//if(h>5000 && h<6000)cout<<"là à h ="<<h<<endl;
										qte_min=qte_max;
										//correction sur les heures precedentes :
										//chercher autre liaison avec meme autre reservoir de meme taille et non contrainte :
										int RFin=liaison.getReservoirFin();
										vector<Liaison*> lcandidats=systeme->getLiaisonsAToB(numR,RFin);
										int nbCandidats=lcandidats.size();
										k=0;
										int numRedirection=-1;
										while(k<nbCandidats&&numRedirection==-1)
										{
											if(lcandidats[k]->getNumero()!=numL && lcandidats[k]->getLongueur()==longL  )
											{
												//verifier si contrainte 6 ne s'applique pas :
												bool accepte=true;
												for(l=h-tps;l<h;l++)
												{
													if(lcandidats[k]->enArret(l)) accepte=false;
												}
												//verifier si contrainte 9 n'existe pas :
												if(lcandidats[k]->getMinTpsConstant()>1)accepte=false;
												//verifier si contrainte 10 n'existe pas :
												if(lcandidats[k]->contrainteDincompatibilite()) accepte=false;
												//vérifier si contrainte 11 n'existe pas :
												if(lcandidats[k]->palier())accepte=false;
												if(accepte) numRedirection=lcandidats[k]->getNumero();
											}
											k++;
										}
										if(numRedirection==-1)cerr <<"Ini: ajout conduite necessaire pour contraintes temps Min de fonctionement l369 "<<numR<<" "<<RFin<<" nbCand "<<nbCandidats<<" "<<R.getCardSorties()<<endl;
										//redistribuer pour toute les conduites precedentes et effectuer les changements necessaires:
										double dif=qtePred-qte_max;
										for(l=h-tps;l<h;l++)
										{
											//modif liaison :

											for(k=l;k<l+longL+1;k++)
											{	
												// cout << k << " " << numL << " " << longL-(k-l) << endl;
												// cout << "est-ce là l 446" << endl;
												_genotype.setDebitEnCours(k,numL,longL-(k-l),qte_max);
											}
											//modif redirection :
											for(k=l;k<l+longL+1;k++)
											{
												// cout << k << " " << numRedirection << " " << longL-(k-l) << endl;
												// cout << "est-ce là l 453" << endl;
												_genotype.setDebitEnCours(k,numRedirection,longL-(k-l),_genotype.getDebitEnCours(k,numRedirection,longL-(k-l))+dif);
											}
										}
										
									}
									
									
								}
							}
						//	cout<<"fin minCons"<<endl;
						}
			//	cout<<"temps minimale de fonctionnement qmin="<<qte_min<<" qmax="<<qte_max<<endl;
						//Autres contraintes si possible :
						eoUniformGenerator<double> prob(0,1);
						double p=prob();
						double qminL=liaison.getQmin();
						if(qminL>qte_min && qminL<qte_max)
							qte_min=qminL;
						else if (qminL<qte_max) p=1;
						double deltaLo=liaison.getDeltaLo();
						if(h>0 && _genotype.getDebitEnCours(h-1,numL,longL)-deltaLo>qte_min && _genotype.getDebitEnCours(h-1,numL,longL)-deltaLo<=qte_max)
							qte_min=_genotype.getDebitEnCours(h-1,numL,longL)-deltaLo;
						else if(h>0 && _genotype.getDebitEnCours(h-1,numL,longL)-deltaLo>qte_max)
							p=1;
						double qmaxL;
						if(h-1>-1)qmaxL=liaison.getQmax(_genotype.getContenu(h-1,numR));
						else qmaxL=liaison.getQmax(R.getVinit());
						//
					   if(qmaxL<qte_max && qte_min< qmaxL)
							qte_max=qmaxL;
					   else p=0;
					   double deltaU= liaison.getDeltaUp();
					   if(h>0 && _genotype.getDebitEnCours(h-1,numL,longL)+deltaU<qte_max && _genotype.getDebitEnCours(h-1,numL,longL)+deltaU>=qte_min)
						   qte_max=_genotype.getDebitEnCours(h-1,numL,longL)+deltaU;
					   else if (h>0 &&  _genotype.getDebitEnCours(h-1,numL,longL)+deltaU<qte_min)
						   p=0;
					 //  cout<<"là"<<endl;
					   if(R.getAdjacents().size()==1)
					   {
						  // cout<<R.getAdjacents()[0]<<endl;
						   Reservoir Rsuc=systeme->getReservoir(R.getAdjacents()[0]);
						 //  cout<<"av if"<<endl;
						   if(Rsuc.getVmax()==0 && Rsuc.getCardSorties()==1 )
						   {
							   double deltaUp=systeme->getLiaison(Rsuc.getSorties()[0]).getDeltaUp();
							   if(deltaUp>0 && deltaUp<FLT_MAX && qteR+qte_max>qteRP+deltaUp &&qteRP+deltaUp-qteR>=qte_min)
								   qte_max=qteRP+deltaUp-qteR;
							   else if (qteRP+deltaUp-qteR<qte_min)
								   p=0;
						   }
						  // cout<<"fin if"<<endl;
					   }
						//cout<<"autres qtemin="<<qte_min<<" qte_max="<<qte_max<<endl;

						//if(qte_min>qte_max) cerr<<"l 365 numR "<<numR<<" vmin "<<vmin<<" vmax "<<vmax<<" Vmin "<<Vmin<<" Vmax "<<Vmax<<" h "<<h<< " numComp "<<i<<" qte_min="<<qte_min<<" qte_max="<<qte_max<<endl;
						double minRestantADeverser=0;
						for(int lF=0;lF<cardSorties;lF++)
						{
							int numLF= sorties[lF];
							Liaison liaisonF =systeme->getLiaison(numLF);
							if(lF<j)
								minRestantADeverser+=liaisonF.getQmin()*(nbHeures-1-h);
							else
								minRestantADeverser+=liaisonF.getQmin()*(nbHeures-h);
						}
						if(qte_max>(qTot-qteRT)-minRestantADeverser && (qTot-qteRT)-minRestantADeverser>qte_min)
							qte_max=(qTot-qteRT)-minRestantADeverser;

						//choix quantite :
					   if(qte_min<0)qte_min=0;
						if(qte_max<qte_min && qte_min<qte_max+0.00001)
							qte_max=qte_min;
						eoUniformGenerator<double> choixQtePart1(qte_min,(qte_max+qte_min)/2);
						eoUniformGenerator<double> choixQtePart2((qte_max+qte_min)/2,qte_max);
						
						//cas normaux :
							double q;
						//if(p==0)q=qte_min;
						//else if(p==1) q=qte_max;
						 if(p<0.5) q=choixQtePart1();
						else q=choixQtePart2();

						//cas palier de production:
						if(nbLiens==1 && li1.getNbTurbines()+li1.getNbPompes()>0)
						{
							vector<Turbine*> vT=li1.getListeTurbines();
							int nbTurb_li1=li1.getNbTurbines();
							for(k=0;k<nbTurb_li1;k++)
							{
								double palier=vT[k]->getPalier();
								if(palier>0 &&h>0)
								{
									double QPalier;
									if(h>1)QPalier=vT[k]->getQPalier(_genotype.getContenu(h-2,numR));
									else QPalier=vT[k]->getQPalier(R.getVinit());
									double QNPalier=vT[k]->getQPalier(_genotype.getContenu(h-1,numR));
									double qPred=_genotype.getDebitEnCours(h-1,numL,longL);
									if(qte_min==0 && (qPred==QPalier||qPred==0))
									{
										if (p<0.8){
											q=0;
										//	if(h<5500)cout<<"ICI1 h="<<h<<"  qpred "<<qPred<<" qpalier="<<QPalier<<" numL="<<numL<<endl;
										}
									}
									else if(qte_min==0)
									{
										if(q==0){
											q=qte_max;
										//	if(h<5500)cout<<"ICI2 h="<<h<<"  qpred "<<qPred<<" qpalier="<<QPalier<<" qte_max="<<qte_max<<endl;
										}
										if(QNPalier<=qte_max && p<0.8) q=QNPalier;
										//else if(p<0.9)q=qte_max;
									}
									else if(qte_min<=QNPalier && QNPalier<=qte_max)
									{
										if(p<0.8)q=QNPalier;
										else if(p<0.85)q=qte_min;
										else if(p<0.95)q=qte_max;
									}

								}
							}
						}
						//cout<<"cas Pazlier"<<endl;
						//modification de l'individu :

						
						//modification débit en cours :
						//cout<<"ici"<<endl;
						for(k=h;k<h+longL+1;k++)
						{
							// cout<<"k "<<k<<" numL "<<numL<<" longL-(k-h) "<<longL-(k-h)<<endl;
							// cout << "est-ce là l 572" << endl;
							// cout << k << " " << numL << " " << longL-(k-h) << endl;
							if(k<nbHeures)_genotype.setDebitEnCours(k,numL,longL-(k-h),q);
							//if(numL==3 && q==0 && h<5500)cout<<"q nul à "<<h<<endl;
						}
						//Modif temps même puissance :
						if(h>0 && _genotype.getDebitEnCours(h-1,numL,longL)==q &&liaison.getMinTpsConstant()>1)
						{
							//if(numL==18) cout<<"modif ici"<<endl;
							/*if(_genotype.getTempsMmPuissance(h-1,numL)==liaison.getMinTpsConstant())
							{
								_genotype.setTempsMemePuissance(h,numL,liaison.getMinTpsConstant());
							}
							else
							{*/
								_genotype.setTempsMemePuissance(h,numL,_genotype.getTempsMmPuissance(h-1,numL)+1);
							//}
						}
						//modification reservoir:
						_genotype.setContenu(h,numR,_genotype.getContenu(h,numR)-q);
						//modification réservoir d'arrivé :
						//int numRF=liaison.getReservoirFin();
						//if(numRF<nbReservoirs &&l+longL<nbHeures )_genotype.setContenu(h+longL,numRF,_genotype.getContenu(h,numRF)+q);
						//modif qteR
						qteR+=q;
						qteRT+=q;
						//cout<<"modification débit en cours"<<endl;

					}
					//derniere liaison :
					//liaison :
					
					int numL= sorties[cardSorties-1];
					// if (numL==3)cout<<"at derniere!!!"<<endl;
					Liaison liaison =systeme->getLiaison(numL);
					int longL=liaison.getLongueur();
					int tpsMinConstant=liaison.getMinTpsConstant();
					//calcul qte_min et qte_max pour la liaison :
					double qte_min=0;//vmin;
					if(qteR<vmin)qte_min=vmin-qteR;
					double qte_max=vmax-qteR;
					//cout<<" derniere liaison qtemax et min initiales :"<<qte_max<<" "<<qte_min<<" vmin "<<vmin<<" vmax "<<vmax<<" numR "<<numR<<" Vmin" <<Vmin <<endl;
					if(h==nbHeures -1 && qte_min> qte_max +0.0001)cout<<"pb heure fin "<<qte_min<<" qte_max="<<qte_max<<endl;
					if(h==nbHeures-1)qte_min=qte_max;
					
					//contrainte incompatibilité :
					if(liaison.contrainteDincompatibilite())
					{
						int nbLiens=liaison.getNbLiens();
						bool constraint=true;
						Lien  li1=liaison.getLien(0);
						bool b=false;
						vector<Turbine*> vT=li1.getListeTurbines();
						for(l=0;l<li1.getNbTurbines();l++)
						{
							int numI=vT[l]->getIncompatibilites();
							if(numI>-1)
							{
								//regarder si allumée à h+distT : qte d'eau qui arrive :
								int disT=vT[l]->getDistance();
								Pompe tI=systeme->getPompe(numI);
								int distTI=tI.getDistance();
								int numLI=tI.getNumLiaison();
								int longLI=systeme->getLiaison(numLI).getLongueur();
								if(h+disT<nbHeures &&_genotype.getDebitEnCours(h+disT,numLI,longLI-distTI)>0)
								{
									//alors l'éteindre
									//chercher liaison non contrainte :
									int numRI=systeme->getLiaison(numLI).getReservoirIni();
									int longLI=systeme->getLiaison(numLI).getLongueur();
									int numRIF=systeme->getLiaison(numLI).getReservoirFin();
									vector<Liaison*> lcandidats=systeme->getLiaisonsAToB(numRI,numRIF);
									int nbCandidats=lcandidats.size();
									k=0;
									int numRedirection=-1;
									while(k<nbCandidats&&numRedirection==-1)
									{
										if(lcandidats[k]->getNumero()!=numLI && lcandidats[k]->getLongueur()==longLI  )
										{
											//verifier si contrainte 6 ne s'applique pas :
											bool accepte=true;

											if(h+disT<nbHeures && h+disT-distTI>0 && lcandidats[k]->enArret(h+disT-distTI)) accepte=false;

											//verifier si contrainte 9 n'existe pas :
											if(lcandidats[k]->getMinTpsConstant()>1)accepte=false;
											//verifier si contrainte 10 n'existe pas :
											if(lcandidats[k]->contrainteDincompatibilite()) accepte=false;

											//vérifier si contrainte 11 n'existe pas :
											if(lcandidats[k]->palier())accepte=false;
											if(accepte) numRedirection=lcandidats[k]->getNumero();
										}
										k++;
									}
									if(numRedirection==-1)cerr <<"Ini: ajout conduite necessaire pour contraintes temps Min de fonctionement"<<endl;
									//modifier redirection : 
									for(int t=h+disT-distTI;t<h+disT-distTI+longLI+1;t++)
									{
										if(t<nbHeures && t>=0 &&h+disT-distTI>-1)
										{
											// cout << t << " " << numRedirection << " " << h+disT-distTI+longLI-t << endl;
											// cout << "est-ce là l 673" << endl;
											_genotype.setDebitEnCours(t,numRedirection,h+disT-distTI+longLI-t,_genotype.getDebitEnCours(h+disT-distTI,numLI,longLI)+_genotype.getDebitEnCours(h+disT-distTI,numRedirection,longLI));
										}
									}
									//modifier LI
									for(int t=h+disT-distTI;t<h+disT-distTI+longLI+1;t++)
									{
										// cout << t << " " << numLI << " " << h+disT-distTI+longLI-t << endl;
										// cout << "est-ce là l 681" << endl;
										_genotype.setDebitEnCours(t,numLI,h+disT-distTI+longLI-t,0);
									}


								}
							}
						}
					}
					int nbLiens=liaison.getNbLiens();
					Lien  li1=liaison.getLien(0);
					if(qte_max<0.000001 && h>0){
						if(nbLiens==1 && li1.getNbTurbines()+li1.getNbPompes()>0)
						{
							vector<Turbine*> vT=li1.getListeTurbines();
							int nbTurb_li1=li1.getNbTurbines();
							for(k=0;k<nbTurb_li1;k++)
							{
								double palier=vT[k]->getPalier();
								if(palier>0  )
								{
									double QPalier;
									if(h>1)QPalier=vT[k]->getQPalier(_genotype.getContenu(h-2,numR));
									else QPalier=vT[k]->getQPalier(R.getVinit());
									if(_genotype.getDebitEnCours(h-1,numL,longL)!=QPalier && _genotype.getDebitEnCours(h-1,numL,longL)>0 )
									{
										int hQ=h-1;
										if(hQ>0)QPalier=vT[k]->getQPalier(_genotype.getContenu(hQ-1,numR));
										else QPalier=vT[k]->getQPalier(R.getVinit());
										while(hQ>-1 && (_genotype.getDebitEnCours(hQ,numL,longL)<QPalier ||_genotype.getTempsMmPuissance(hQ,numL)<tpsMinConstant))
										{
											hQ--;
											if(hQ>0)QPalier=vT[k]->getQPalier(_genotype.getContenu(hQ-1,numR));
											else QPalier=vT[k]->getQPalier(R.getVinit());
										}
										int tps=0;
															if(hQ>-1)tps=_genotype.getTempsMmPuissance(hQ,numL);
															//cout<<" hQ "<<hQ<<endl;
															//recherche liaison de redirection :
															double dif=0;
															if(hQ>-1) dif=_genotype.getDebitEnCours(hQ,numL,longL)-QPalier;
															int RFin=liaison.getReservoirFin();
															vector<Liaison*> lcandidats=systeme->getLiaisonsAToB(numR,RFin);
															int nbCandidats=lcandidats.size();
															int kand=0;
															int numRedirection=-1;
															while(kand<nbCandidats&&numRedirection==-1)//attention k !
															{
																if(lcandidats[kand]->getNumero()!=numL && lcandidats[kand]->getLongueur()==longL  )
																{
																	bool accepte=true;
																	
																	for(l=hQ-tps;l<h;l++)
																	{
																		if(l>-1 && lcandidats[kand]->enArret(l)) accepte=false;
																	}
																	if(lcandidats[kand]->getMinTpsConstant()>1)accepte=false;
																	if(lcandidats[kand]->contrainteDincompatibilite()) accepte=false;
																	if(lcandidats[kand]->palier())accepte=false;
																	if(accepte) numRedirection=lcandidats[kand]->getNumero();
																}
																kand++;
															}
															cout<<"num redec "<<endl;
															if(numRedirection==-1)cerr <<"Ini: ajout conduite necessaire pour contraintes temps Min de fonctionement l369 "<<numR<<" "<<RFin<<" nbCand "<<nbCandidats<<" "<<R.getCardSorties()<<endl;
															//modif debits en cours :
															
															for(l=hQ+1-tps;l<hQ+1;l++)
															{
																//modif liason :
																for(int m=longL;m>-1;m--)
																{
																	// cout << l+(longL-m) << " " << numL << " " << m << endl;
																	// cout << "est-ce là l 754" << endl;
																	_genotype.setDebitEnCours(l+(longL-m),numL,m,QPalier);
																}
																//modif redirection
																for(int m=longL;m>-1;m--)
																{
																	// cout << l+(longL-m) << " " << numRedirection << " " << m << endl;
																	// cout << "est-ce là l 761" << endl;
																	_genotype.setDebitEnCours(l+(longL-m),numRedirection,m,_genotype.getDebitEnCours(l+(longL-m),numRedirection,m)+dif);
																}

															}
															//autres heures :
															for(l=hQ+1;l<h;l++)
															{
																//modif redirection
																for(int m=longL;m>-1;m--)
																{
																	// cout << "est-ce là l 772" << endl;
																	_genotype.setDebitEnCours(l+(longL-m),numRedirection,m,_genotype.getDebitEnCours(l+(longL-m),numRedirection,m)+_genotype.getDebitEnCours(l+(longL-m),numL,m));
																	// cout << "est-ce là l 774" << endl;
																	_genotype.setDebitEnCours(l+(longL-m),numL,m,0);
																}
															}
															
														

														}
													}

												}
											}
										}
					//cout<<"contrainte incompatibilite l 559"<<endl;
					//containte palier de production: à reprendre
					/*int nbLiens=liaison.getNbLiens();
					Lien  li1=liaison.getLien(0);
					if(qte_max==0 && h>0){
						if(nbLiens==1 && li1.getNbTurbines()+li1.getNbPompes()>0)
						{
							vector<Turbine*> vT=li1.getListeTurbines();
							for(k=0;k<li1.getNbTurbines();k++)
							{
								double palier=vT[k]->getPalier();
								if(palier>0 )
								{
									//calcul prodPredMin  :
									double QPalier;
									if(h>1)QPalier=vT[k]->getQPalier(_genotype.getContenu(h-2,numR));
									else QPalier=vT[k]->getQPalier(R.getVinit());
									if(_genotype.getDebitEnCours(h-1,numL,longL)!=QPalier && _genotype.getDebitEnCours(h-1,numL,longL)>0)
									{
										//corriger :
										int tps=_genotype.getTempsMmPuissance(h-1,numL);
										//chercher qty_max que l'on peut retirer aux horraires precedentes sans violer la contrainte sur le contenu :
										double qty_max=_genotype.getDebitEnCours(h-1,numL,longL);
										for(l=0;l<tps;l++)
										{
											if(h-1-l>-1){
											double vol=_genotype.getContenu(h-1-l,numR);
											if(Vmax-vol<qty_max) qty_max= Vmax-vol;
											}
										}
										//correction : à revoir réservoir d'arrivé
										double qty=qty_max/2;
										for(l=h-tps;l<h;l++)
										{
											for(int m=l;m<=l+longL;m++)
											{
												//cout << "est-ce là l 823" << endl;
												_genotype.setDebitEnCours(m,numL,longL-(m-l),_genotype.getDebitEnCours(m,numL,longL-(m-l))-qty);//a verifier
											}
											_genotype.setContenu(l,numR,_genotype.getContenu(l,numR)+qty);
											//int numSuc=liaison.getReservoirFin();
											//if(numSuc<nbReservoirs &&l+longL<nbHeures )_genotype.setContenu(l+longL,numSuc,_genotype.getContenu(l,numSuc)-qty);
											qteRT-=qty;
										}
										qte_max=qty*tps;
										qte_min=qty*tps;
								//	cout<<"contrainte palier de prod qtemax et min :"<< qte_max <<" "<<qte_min<<endl;
									}
								}
							}
						}
					}*/
					//cout<<"contrainte palier de production"<<endl;
					//essaie satisfaction autres contraintes:
					eoUniformGenerator<double> prob(0,1);

					double p=prob();
					double qminL=liaison.getQmin();
					if(qminL>qte_min && qminL<qte_max)
						qte_min=qminL;
					else p=1;
					double qmaxL;
					if(h>0) qmaxL=liaison.getQmax(_genotype.getContenu(h-1,numR));
					else qmaxL=liaison.getQmax(R.getVinit());
					if(qmaxL<qte_max && qte_min< qmaxL)
						qte_max=qmaxL;
					else p=0;
					 if(R.getAdjacents().size()==1)
					 {
						 Reservoir Rsuc=systeme->getReservoir(R.getAdjacents()[0]);
						 if(Rsuc.getVmax()==0 && Rsuc.getCardSorties()==1 )
						 {
							 double deltaUp=systeme->getLiaison(Rsuc.getSorties()[0]).getDeltaUp();
							 if(deltaUp>0 && deltaUp<FLT_MAX && qteR+qte_max>qteRP+deltaUp &&qteRP+deltaUp-qteR>=qte_min)
								 qte_max=qteRP+deltaUp-qteR;
							 else if (qteRP+deltaUp-qteR<qte_min)
								 p=0;
							 double deltaLo=systeme->getLiaison(Rsuc.getSorties()[0]).getDeltaLo();
							 if(deltaLo>0 && deltaLo<FLT_MAX && qteR+qte_min<qteRP-deltaLo &&qteRP-deltaLo-qteR<=qte_max)
								 qte_min=qteRP-deltaLo-qteR;
							 else if(h>0 && qteRP-deltaLo-qteR>qte_max)
								 p=1;
						 }
					 }

					//if(h==nbHeures-1) qte_max=qte_min;
					//cout<<"autres"<<endl;
					//choix debit :
					//if(qte_max<qte_min)cout<<"choix debit l558 qte_min"<<qte_min<<" qte_max "<<qte_max<<" qminL "<<qminL<<" qmaxL "<<qmaxL<<endl;
						double minRestantADeverser=0;
						for(int lF=0;lF<cardSorties;lF++)
						{
							int numLF= sorties[lF];
							Liaison liaisonF =systeme->getLiaison(numLF);

								minRestantADeverser+=liaisonF.getQmin()*(nbHeures-1-h);

						}
						if(qte_max>(qTot-qteRT)-minRestantADeverser && (qTot-qteRT)-minRestantADeverser>qte_min)
							qte_max=(qTot-qteRT)-minRestantADeverser;
					if(qte_min<0)qte_min=0;
					if(qte_max<qte_min && qte_max+0.001>qte_min)qte_max=qte_min;
					eoUniformGenerator<double> choixQteP1(qte_min,(qte_max+qte_min)/2);
					eoUniformGenerator<double> choixQteP2((qte_max+qte_min)/2,qte_max);
					//cout <<"passé"<<endl;
					//cout<<"debit choisi"<<endl;
					double q;
					//cas normaux :

					//if(p==0)q=qte_min;
					//else if(p==1) q=qte_max;
					 if(p<0.55)q=choixQteP1();
					else q=choixQteP2();
			//	cout<<"cas normal l629"<<endl;
					//cas palier de production:
					if(nbLiens==1 && li1.getNbTurbines()+li1.getNbPompes()>0)
					{
						vector<Turbine*> vT=li1.getListeTurbines();
						for(k=0;k<li1.getNbTurbines();k++)
						{
							double palier=vT[k]->getPalier();
							if(palier>0 &&h>0 )
							{
								double QPalier;
								if(h>1)QPalier=vT[k]->getQPalier(_genotype.getContenu(h-2,numR));
								else QPalier=vT[k]->getQPalier(R.getVinit());
								double QNPalier=vT[k]->getQPalier(_genotype.getContenu(h-1,numR));
								double qPred=_genotype.getDebitEnCours(h-1,numL,longL);
								if(qte_min==0 && (qPred==QPalier||qPred==0))
								{
									if (p<0.5) {
										q=0;
										if(h<5500)cout<<"ICI1 h="<<h<<"  qpred "<<qPred<<" qpalier="<<QPalier<<endl;
									}
								}
								else if(qte_min==0)
								{
									if(q==0)q=qte_max;
									if(QNPalier<qte_max && p<0.3) q=QNPalier;
									else if(p<0.35)q=qte_max;
								}
								else if(qte_min<QNPalier && QNPalier<qte_max )
								{
									if(p<0.3)q=QNPalier;
									else if(p<0.5 )q=qte_min;
									else if(p<0.55)q=qte_max;
								}
							}
						}
					}
				//	cout<<"cas pallier"<<endl;
					//modification de l'individu :
					//int nbLiens=liaison.getNbLiens();
					//lien  li1=liaison.getLien(0);

					//modification débit en cours :
					for(k=h;k<h+longL+1;k++)
					{
						// cout << "est-ce là l 934" << endl;
						// cout << k << " " << numL << " " << longL-(k-h) << endl;
						if (k<nbHeures)
							_genotype.setDebitEnCours(k,numL,longL-(k-h),q);
					}
					//modification reservoir:
					_genotype.setContenu(h,numR,_genotype.getContenu(h,numR)-q);
					if (_genotype.getContenu(h,numR)>Vmax+0.00001)cout <<"ATTENTION "<<_genotype.getContenu(h,numR)<<" q "<<q<<" Vmax "<<Vmax<<" numR "<<numR<<endl;
					//Modif temps même puissance :
					if(h>0 && _genotype.getDebitEnCours(h-1,numL,longL)==q &&liaison.getMinTpsConstant()>1)
					{
						if(numL==18) cout<<"modif ici q="<<q<< " "<<_genotype.getDebitEnCours(h-1,numL,longL)<<endl;
						/*if(_genotype.getTempsMmPuissance(h-1,numL)==liaison.getMinTpsConstant())
						{
							_genotype.setTempsMemePuissance(h,numL,liaison.getMinTpsConstant());
						}*/
						//else
						//{
							_genotype.setTempsMemePuissance(h,numL,_genotype.getTempsMmPuissance(h-1,numL)+1);
						//}
					}
					//modification réservoir d'arrivé :
					//int numRF=liaison.getReservoirFin();
					//_genotype.setContenu(h+longL,numRF,_genotype.getContenu(h,numRF)+q);
					//modif qteR
					qteR+=q;
					qteRT+=q;
					//cout<<"modif debits en cours"<<endl;
				}
				//cout<<"ici??"<<endl;
			}
			//cas des composantes connexes avec circuits :
			else{
				//cout<<"debut sinon"<<endl;
				
				vector<Liaison*> liaisonsSortie =getListeLiaisonsSortantes(C,*systeme);
				int nbSorties=liaisonsSortie.size();
				vector<vector<double> > aDebiter;
				vector<vector<int> > avantLe;
				//vector<int>  Rnum;//(C.getReservoirs());
				
			

				//initialisation de aDebiter: 
				for(int z=0;z<nbReservoirs;z++)
				{
					
					vector<double> vec;
					vector<int> vecI;
					for(j=0;j<nbReservoirs+1;j++)
					{
						vec.push_back(0);
						if (z!=j&&j<nbReservoirs) vecI.push_back(nbHeures-systeme->distanceMinimale(z,j));
						else vecI.push_back(nbHeures);
					}
					aDebiter.push_back(vec);
					avantLe.push_back(vecI);
				}
				//cout<<"ini de aDebiter"<<endl;
				//flux initiale, aDebiter :

				for(int y=0;y<tailleComposante;y++)//pour chaque reservoir
				{
					
					//cout <<"y = "<<y<<endl;
					Reservoir& R=(systeme->getReservoir(C.getReservoir(y)));

					//calcul Qtot:
					double Qtot=0;
					for(h=0;h<nbHeures;h++)
					{
						Qtot+=R.getApport(h);
						//ajout provenance liaisons : 
						vector<int> entrees=R.getEntrees();
						for(k=0;k<R.getCardEntrees();k++)//remplacement y
						{
							int entree= entrees[k];
							int pred=systeme->getLiaison(entree).getReservoirIni();
							bool entreeComposante=!(C.in(pred));
							if(entreeComposante)Qtot+= _genotype.getDebitEnCours(h,entree,0);
						}

					}
					//cout<<"calcul Qtot "<<Qtot<<endl;
					
					//recherche de chemin vers les liaisons de sorties : 
					//cout<<"nbSorties "<<nbSorties<<endl;
					for(j=0;j<nbSorties;j++)
					{
						//cout<<"j "<<j<<endl;
						//choix qte:
						eoUniformGenerator<double> choix(0,Qtot);
						double q=choix();
						if(j==nbSorties-1)q=Qtot;
						Qtot-=q;
						//vector coloration;
						vector<int> coloration;
						coloration.push_back(R.getNumero());
						bool trouve=false;
						//cout<<"là"<<endl;
						int longJ=liaisonsSortie[j]->getLongueur();
						int numIni=liaisonsSortie[j]->getReservoirIni();
						if(R.getNumero()!=numIni)
						{
						//cout<<"avant while"<<endl;
						int a=-1;
						while(!trouve)
						{
							
							bool colorie=false;
							a++;
							while(!colorie)
							{
								//cout<<"col "<<coloration[a]<<" a="<<a<<endl;
								vector<int> adjacents(systeme->getReservoir(coloration[a]).getAdjacents());
								int taille=adjacents.size();
								int b=0;
							//	cout<<"deb while interne"<<endl;
								while(!colorie && b<taille)
								{
									//regarder si adjacent est dans les sommet deja coloriés:
									bool dej=false;
									dej=!C.in(adjacents[b]);
									//cout<<"dej apres C.in : "<<dej<<" adjb :" <<adjacents[b]<<endl;
									//cout<<"av pour1"<<endl;
									for(k=0;k<coloration.size();k++)
									{
										if(coloration[k]==adjacents[b])
											dej=true;
									}
									//cout<<"ap"<<endl;
									if(!dej)
									{
										colorie=true;
										coloration.insert(coloration.begin()+a+1,adjacents[b]);
										//si correspond à la liaison..
										
										if(adjacents[b]==numIni)
										{
											trouve=true;
											//mise à jours chemin :attention aux temps de transfert !! 
											int tpsMin=nbHeures-longJ;
											//entre a ième élèment et liaison:
											int numA=numIni;
											aDebiter[numIni][nbReservoirs]+=q;
											if(avantLe[numIni][nbReservoirs]>tpsMin)avantLe[numIni][nbReservoirs+j]=nbHeures-longJ;
											int numP;	
											//cout<<"deb pour"<<endl;
											//pour les autres:
											for(k=a;k>=0;k--)
											{
												numP=coloration[k];
												aDebiter[numP][numA]+=q;
												if(avantLe[numP][numA]>tpsMin) avantLe[numP][numA]=tpsMin;
												else if(avantLe[numP][numA]<tpsMin)tpsMin=avantLe[numP][numA];
												numA=numP;
											}

										}
									}
									b++;
								}
								//cout<<"fin whilz"<<endl;
								if(!colorie)a--;
							}//cout<<"fin while"<<endl;
						}//cout<<"fin trouve"<<endl;
						}
						else
						{
							aDebiter[numIni][nbReservoirs]+=q;
							if(avantLe[numIni][nbReservoirs]>nbHeures-longJ)avantLe[numIni][nbReservoirs]=nbHeures-longJ;
						}

					}//cout<<"fin put"<<endl;
				}
				//cout<<"chemins initialisés"<<endl;
				//verif àDebiter :
				double Qtot=0;
				for(int y=0;y<tailleComposante;y++)
				{
					Reservoir& R=(systeme->getReservoir(C.getReservoir(y)));
					
					for(h=0;h<nbHeures;h++)
					{
						Qtot+=R.getApport(h);
						vector<int> entrees=R.getEntrees();
						for(k=0;k<R.getCardEntrees();k++)//remplacement y
							{
							int entree= entrees[k];
							int pred=systeme->getLiaison(entree).getReservoirIni();
							Qtot+= _genotype.getDebitEnCours(h,entree,0);
							}
					}
				}
				//cout<<" Qtot="<<Qtot<<" aDebiter[5][8]="<<aDebiter[5][8]<<endl;
				//planification au sein des différents circuits :
				
				vector< vector<vector<double> > > aFaireCirculer;
				vector<int> tpsMax;
				int nbCircuits=C.getNbElementaryCircuits();
				for(j=0;j<nbCircuits;j++)
				{
					vector<int> circuit= C.getCircuit(j);
					vector<vector<double> > circule;
					tpsMax.push_back(nbHeures-1);
					//initialisation de aFaireCirculer et tpsMax:
					for(k=0;k<circuit.size();k++)
					{
						if(k<circuit.size()-1)
						{
							int dist=systeme->distanceMinimale(circuit[k],circuit[k+1]);
							//cout<<"distance minimale "<<dist<<" circuit[k] "<<circuit[k] <<" circuit k+1 "<<circuit[k+1]<<endl;
							if(nbHeures-dist<tpsMax[j])tpsMax[j]=nbHeures-dist;

						}
						else{int dist=systeme->distanceMinimale(circuit[circuit.size()-1],circuit[0]);
							//cout<<"distance minimale "<<dist<<" circuit[k] "<<circuit[k] <<" circuit k+1 "<<circuit[k+1]<<endl;
						if(nbHeures-dist<tpsMax[j])tpsMax[j]=nbHeures-dist;
						}
						vector<double> ini;
						for(l=0;l<circuit.size();l++)
						{
							ini.push_back(0);
						}
						circule.push_back(ini);
					}
					aFaireCirculer.push_back(circule);
				}
				//cout<<"ini a faire circuler tpsMax="<<tpsMax[0]<<endl;
				for(j=0;j<nbCircuits;j++)
				{
					
					vector<int> circuit= C.getCircuit(j);
					eoUniformGenerator<int> choixReservoirDepart(0,circuit.size());
					for(h=0;h<nbHeures;h++)
					{
						//cout<<"ici l 843"<<endl;
						
						//choix reservoir depart :
						int rDep=choixReservoirDepart();
						for(k=rDep;k<rDep+circuit.size();k++)//attention dernier circuit...
						{
							
							int numC=k%circuit.size();
							int numR=circuit[numC];
							Reservoir* R=&(systeme->getReservoir(numR));
							//calcul contenu ini:
							
							if(h>0) _genotype.setContenu(h,numR,_genotype.getContenu(h-1,numR)+R->getApport(h));
							else _genotype.setContenu(h,numR,R->getVinit()+R->getApport(h));
							//ajout des entrees 
							vector<int> entrees=R->getEntrees();
							for(int li=0;li<R->getCardEntrees();li++)
							{
								_genotype.setContenu(h,numR,_genotype.getContenu(h,numR)+_genotype.getDebitEnCours(h,entrees[li],0));
							}
							double Vmax=R->getVmax();
							double Vmin=R->getRealVmin(h);
							double vmin=0;
							double qteR=0;
							if(vmin < _genotype.getContenu(h,numR)-Vmax) vmin= _genotype.getContenu(h,numR)-Vmax;
							double vmax= _genotype.getContenu(h,numR) -Vmin;
							//if(h>0)cout<<"l 895pouette vmin "<<vmin <<" vmax "<<vmax <<" "<<_genotype.getContenu(h,numR)<<" contenu pred="<<_genotype.getContenu(h-1,numR)<<" Vmin "<<Vmin<<" h="<<h<<endl;
							Reservoir* Rs=&systeme->getReservoir(circuit[(k+1)%circuit.size()]);
							int numSuc=Rs->getNumero();
							double resteADeverser=0;
							if(k==rDep+circuit.size()-1)
							{
								//respect de Vmax du successeur :
								double VmaxS=Rs->getVmax();
								if(vmax>VmaxS-_genotype.getContenu(h,numSuc)) vmax=VmaxS-_genotype.getContenu(h,numSuc);
								resteADeverser=vmin-vmax;
								if(vmin>vmax)
								{
									vmax=vmin;
								}
							}
							if(vmin>vmax) cout<<"ATTENTION vmin="<<vmin<<">vmax="<<vmax<<endl;
							//cout<<"calcul qmin, qmax, non là!"<<endl;
							//if(qteRT+vmax>qTot) vmax=qTot-qteRT;
							int cardSorties= R->getCardSorties();
							vector<int> sorties=R->getSorties();
							//pour chaque liaison :
							//cout<<cardSorties-1<<endl;
							for(l=0;l<cardSorties;l++)//attention derniere liaison !
							{
								//liaison : 
								int numL= sorties[l];
								Liaison liaison =systeme->getLiaison(numL);
								int longL=liaison.getLongueur();
								int rF=liaison.getReservoirFin();
								int lFut=l+1;
								bool derniereDeSonEspece=true;
								int numRF=liaison.getReservoirFin();
								while(lFut<cardSorties && derniereDeSonEspece)
								{
									if(systeme->getLiaison(sorties[lFut]).getReservoirFin()==numRF)
										derniereDeSonEspece=false;
									lFut++;
								}
								//cout<<"l851"<<endl;
								//regarder si il s'agit d'une liaison à traiter :
								bool aTraiter=true;
								if(true){
									//calcul qte_min et qte_max pour la liaison :
									double qte_min=0;
								
									double qte_max=vmax-qteR;
									if(derniereDeSonEspece &&systeme->getReservoir(circuit[(k+1)%circuit.size()]).getNumero()==numRF && qteR<vmin)qte_min=vmin-qteR;
									//vérifier si arret programmé :
									bool ArretProgramme=liaison.enArret(h);
									if(ArretProgramme)
									{
										qte_max=0;
									}
								
									if(liaison.contrainteDincompatibilite() && !derniereDeSonEspece)
									{
										//cout<<"incomp"<<endl;
										int nbLiens=liaison.getNbLiens();
										bool constraint=true;
										//cout<<"ca"<<endl;
										Lien  li1=liaison.getLien(0);
										//bool b=false;
										//cout<<"là"<<endl;
										vector<Turbine*> vT=li1.getListeTurbines();
										vector<Pompe*> vP=li1.getListePompes();
										//cout<<"ici"<<endl;
										for(int m=0;m<li1.getNbTurbines()+li1.getNbPompes();m++)
										{
											
											if(li1.getNbPompes()>0)
											{
												int numI=vP[m]->getIncompatibilites();
												if(numI>-1){
												//regarder si allumée à h+distT : qte d'eau qui arrive :
												int disT=vP[m]->getDistance();
												Turbine tI=systeme->getTurbine(numI);
												int distTI=tI.getDistance();
												int numLI=tI.getNumLiaison();
												int longLI=systeme->getLiaison(numLI).getLongueur();
												if(_genotype.getDebitEnCours(h+disT,numLI,longLI-distTI)>0)
												{
													qte_max=0; 
												}
												}
											}
											else 
											{
												int numI=vT[m]->getIncompatibilites();
												if(numI>-1 )
												{
												//regarder si allumée à h+distT : qte d'eau qui arrive :
													int disT=vT[m]->getDistance();
													Pompe tI=systeme->getPompe(numI);
													int distTI=tI.getDistance();
													int numLI=tI.getNumLiaison();
													int longLI=systeme->getLiaison(numLI).getLongueur();
													if(_genotype.getDebitEnCours(h+disT,numLI,longLI-distTI)>0)
													{
														qte_max=0; 
													}
												}
											}
										}

									}
									else {
										if(liaison.contrainteDincompatibilite() )
										{
										//	cout<<"incomp cas der"<<endl;
											int nbLiens=liaison.getNbLiens();
											bool constraint=true;
											Lien  li1=liaison.getLien(0);
										bool b=false;
										vector<Turbine*> vT=li1.getListeTurbines();
										int nb_Turb_li1=li1.getNbTurbines();
										for(int lue=0;lue<nb_Turb_li1;lue++)//attenntio l
											{
											int numI=vT[lue]->getIncompatibilites();
											if(numI>-1)
											{
												int disT=vT[lue]->getDistance();
												Pompe tI=systeme->getPompe(numI);
												int distTI=tI.getDistance();
												int numLI=tI.getNumLiaison();
												int longLI=systeme->getLiaison(numLI).getLongueur();
												if(_genotype.getDebitEnCours(h+disT,numLI,longLI-distTI)>0)
												{
													int numRI=systeme->getLiaison(numLI).getReservoirIni();
													int longLI=systeme->getLiaison(numLI).getLongueur();
													int numRIF=systeme->getLiaison(numLI).getReservoirFin();
													vector<Liaison*> lcandidats=systeme->getLiaisonsAToB(numRI,numRIF);
													int nbCandidats=lcandidats.size();
													int cont=0;
													int numRedirection=-1;
													while(cont<nbCandidats&&numRedirection==-1)
													{
														if(lcandidats[cont]->getNumero()!=numLI && lcandidats[cont]->getLongueur()==longLI  )
														{
															bool accepte=true;
															if(lcandidats[cont]->enArret(h+disT-distTI)) accepte=false;
															if(lcandidats[cont]->getMinTpsConstant()>1)accepte=false;
															if(lcandidats[cont]->contrainteDincompatibilite()) accepte=false;
															if(lcandidats[cont]->palier())accepte=false;
															if(accepte) numRedirection=lcandidats[cont]->getNumero();
														}
														cont++;
													}
													if(numRedirection==-1)cerr <<"Ini: ajout conduite necessaire pour contraintes temps Min de fonctionement"<<endl;
													int longR=systeme->getLiaison(numRedirection).getLongueur();
													for(int t=h+disT-distTI;t<h+disT-distTI+longR+1;t++)
													{
														// cout << t << " " << numRedirection << " " << h+disT-distTI+longR-t << endl;
														// cout << "est-ce là l 1346" << endl;
														_genotype.setDebitEnCours(t,numRedirection,h+disT-distTI+longR-t,_genotype.getDebitEnCours(h+disT-distTI,numLI,longLI)+_genotype.getDebitEnCours(h+disT-distTI,numRedirection,longR));
													}
													for(int t=h+disT-distTI;t<h+disT-distTI+longLI+1;t++)
													{
														// cout << t << " " << numLI << " " << h+disT-distTI+longLI-t << endl;
														// cout << "est-ce là l 1352" << endl;
														_genotype.setDebitEnCours(t,numLI,h+disT-distTI+longLI-t,0);
													}
												}
											}
											}
										//cout<<"fin incomp der"<<endl;
									}
									}
									//if(numR==4 && h<1000)cout<<"l1061 qtemin="<<qte_min<<" qtemax="<<qte_max<<endl;
									//cout<<"Incompatibilité"<<endl;

									//palier de production : ça danger
									int nbLiens=liaison.getNbLiens();
									Lien  li1=liaison.getLien(0);
									int nb_Turb_li1=li1.getNbTurbines();
									int nb_Pompes_li1=li1.getNbPompes();
									if(qte_max==0 && h>0){
										if(nbLiens==1 && nb_Turb_li1+nb_Pompes_li1>0)
										{
											
											vector<Turbine*> vT=li1.getListeTurbines();
											for(int m=0;m<nb_Turb_li1;m++) //et pour pompes?
											{
												double palier=vT[m]->getPalier();//à reprendre
												if(palier>0 )
												{
													cout<<"palier"<<endl;
													//calcul prodPredMin  :
													double QPalier;
													if(h>1)QPalier=vT[k]->getQPalier(_genotype.getContenu(h-2,numR));
													else QPalier=vT[k]->getQPalier(R->getVinit());
													if(_genotype.getDebitEnCours(h-1,numL,longL)!=QPalier && _genotype.getDebitEnCours(h-1,numL,longL)>0)
													{
														//corriger :
														int tps=_genotype.getTempsMmPuissance(h-1,numL);
														//chercher qty_max que l'on peut retirer aux horraires precedentes sans violer la contrainte sur le contenu :
														double qty_max=_genotype.getDebitEnCours(h-1,numL,longL);
														for(int n=0; n<tps;n++)
														{
															double vol=_genotype.getContenu(h-1-n,numR);
															if(Vmax-vol<qty_max) qty_max= Vmax-vol;
														}
														//correction : vérifier contenu rf
														double qty=qty_max/2;
														for(int n=h-tps;n<h;n++)
														{
															for(int o=n;o<=n+longL;o++)
															{
																// cout << "est-ce là l 1401" << endl;
																_genotype.setDebitEnCours(k,numL,longL-(o-n),_genotype.getDebitEnCours(o,numR,longL-(o-n))-qty);//a verifier
															}
															_genotype.setContenu(l,numR,_genotype.getContenu(l,numR)+qty);
															int numSuc=liaison.getReservoirFin();
															if(C.in(numSuc)==false)
															{
																numSuc=nbReservoirs;
																
															}
															if(numSuc<nbReservoirs &&l+longL<nbHeures )_genotype.setContenu(l+longL,numSuc,_genotype.getContenu(l,numSuc)-qty);
															if(circuit[(k+1)%circuit.size()]==numSuc)aDebiter[numR][numSuc]+=qty;
															//ca pourrait aussi etre une liaison sortante !
															//si liaison sortante :
															else if(numSuc==nbReservoirs)
															{
																//cherche liaison :
																int cand=0;
																while(liaisonsSortie[cand]->getNumero()!=numL)
																{
																	cand++;
																	
																}
																cand+=nbReservoirs+1+cand;
																aDebiter[numR][cand]+=qty;
															}
															else aFaireCirculer[j][(k)%circuit.size()][(k+1)%circuit.size()]+=qty;//il faudrait modifier tout le circuit
														}
														qte_max=qty*tps;
														qte_min=qty*tps;
													}
												}
											}
										}
									}
									//cout<<"palier de production"<<endl;
									
									//tempsMinimal de fonctionement : ça danger
									int tpsMinConstant=liaison.getMinTpsConstant();
									if(tpsMinConstant>1 && h>0)
									{
										//cout<<"ds tps min"<<endl;
										//recuperer tpsMinconstant heure pred  :
										int tps= _genotype.getTempsMmPuissance(h,numL);
										if(tps<tpsMinConstant)
										{
											//reccuperer qtePred : 
											double qtePred = _genotype.getDebitEnCours(h-1,numL,longL);
											//regarder s'il est possible de debiter qtePred :
											if(qtePred>=qte_min && qtePred<=qte_max)
											{
												qte_min=qtePred;
												qte_max=qtePred;
											}
											else
											{
												if(qtePred>qte_max)
												{
													qte_min=qte_max;
													//correction sur les heures precedentes :
													//chercher autre liaison avec meme autre reservoir de meme taille et non contrainte :
													int RFin=liaison.getReservoirFin();
													vector<Liaison*> lcandidats=systeme->getLiaisonsAToB(numR,RFin);
													int nbCandidats=lcandidats.size();
													int cont =0;
													int numRedirection=-1;
													while(cont<nbCandidats&&numRedirection==-1)
													{
														if(lcandidats[cont]->getNumero()!=numL && lcandidats[cont]->getLongueur()==longL  )
														{
															//verifier si contrainte 6 ne s'applique pas :
															bool accepte=true;
															for(int m=h-tps;m<h;m++)
															{
																if(lcandidats[cont]->enArret(m)) accepte=false;
															}
															//verifier si contrainte 9 n'existe pas :
															if(lcandidats[cont]->getMinTpsConstant()>1)accepte=false;
															//verifier si contrainte 10 n'existe pas :
															if(lcandidats[cont]->contrainteDincompatibilite()) accepte=false;
															//vérifier si contrainte 11 n'existe pas :
															if(lcandidats[cont]->palier())accepte=false;
															if(accepte) numRedirection=lcandidats[cont]->getNumero();
														}
														cont++;
													}
													if(numRedirection==-1)cerr <<"Ini: ajout conduite necessaire pour contraintes temps Min de fonctionement"<<endl;
													//redistribuer pour toute les conduites precedentes et effectuer les changements necessaires:
													double dif=qtePred-qte_max;
													for(int m=h-tps;m<h;m++)
													{
														//modif liaison :
														for(int a=m;a<m+longL+1;a++)
														{
															// cout << "est-ce là l 1495" << endl;
															_genotype.setDebitEnCours(a,numL,longL-(a-m),qte_max);// a verifier...
														}
														//modif redirection :
														for(int a=m;a<m+longL+1;a++)
														{
															// cout << "est-ce là l 1501" << endl;
															_genotype.setDebitEnCours(a,numRedirection,longL-(a-m),_genotype.getDebitEnCours(a,numRedirection,longL-(a-m))+dif);
														}
													}
												}
											}
										}
									}//minfuncConst
									//if(numR==4 && h<1000)cout<<"l1198 qtemin="<<qte_min<<" qtemax="<<qte_max<<endl;
								//	cout<<"temps Min"<<endl;
									//Autres contraintes si possible :
									eoUniformGenerator<double> prob(0,1);
									double p=prob();
									double qminL=liaison.getQmin();
									if(qminL>qte_min && qminL<qte_max)
										qte_min=qminL;
									else if(qminL<qte_max)
										p=1;
									double qmaxL;
									if(h>0)qmaxL=liaison.getQmax(_genotype.getContenu(h-1,numR));
									else qmaxL=liaison.getQmax(R->getVinit());
									if(qmaxL<qte_max && qte_min< qmaxL)
										qte_max=qmaxL;
									else if(qte_min< qmaxL)
										p=0;
									//if(numR==4 && h<1000)cout<<"l1209 qtemin="<<qte_min<<" qtemax="<<qte_max<<endl;
									//cout<<"autres"<<endl;
									//choix qte :
									//calcul suc pour aDebiter
									int numSuc=liaison.getReservoirFin();
									//si lisaison sortante :
									//cout<<"qte_max av si l 1113 "<<qte_max<<endl;
									if(C.in(numSuc)==false)
									{
										numSuc=nbReservoirs;
										
										if(qte_max>aDebiter[numR][numSuc])
											qte_max=aDebiter[numR][numSuc];
										//cout<<"liaison sortante "<<numSuc<<endl;
									}
								/*	if(qte_max>aDebiter[numR][numSuc])//testons
										qte_max=aDebiter[numR][numSuc];*/
									//cout<<"qte_max l1128  "<<qte_max<<endl;
									//cout<<"liaison sortante"<<endl;
									//choix quantite :
									//cout<<"qte_max av "<<qte_max<<endl;
									//if(numR==4 && h<1000)cout<<"l1236 qtemin="<<qte_min<<" qtemax="<<qte_max<<endl;
									if(h>=tpsMax[j])
									{
										double qte_int =aDebiter[numR][numSuc];
										//regarder si successeur est dans le circuit (c à d si c'est le j+1)
										if(circuit[(k+1)%circuit.size()]==numSuc)
											qte_int+=aFaireCirculer[j][(k)%circuit.size()][(k+1)%circuit.size()];
										if(qte_max>qte_int) qte_max=qte_int;
										//qte_min=qte_max;
									}
									//cout<<"qte min l1266 "<<qte_min<<endl;
									//regarder si derniere liaison ayant ce successeur :
									
									if(h>=tpsMax[j] &&derniereDeSonEspece)
									{
										qte_min=aDebiter[numR][numSuc];
										if(circuit[(k+1)%circuit.size()]==numSuc) qte_min+=aFaireCirculer[j][(k)%circuit.size()][(k+1)%circuit.size()];
										 qte_max=qte_min;
									}
									//if(numR==4 && h<1000)cout<<"l1250 qtemin="<<qte_min<<" qtemax="<<qte_max<<endl;
									if(qte_min<0)qte_min=0;
									if(qte_min>qte_max && qte_min<qte_max+0.001)qte_max=qte_min;
									if(qte_min>qte_max) cout<<"numR l 1139 "<<numR<<" vmin "<<vmin<<" vmax "<<vmax<<" Vmin "<<Vmin<<" Vmax "<<Vmax<<" h "<<h<< " numComp "<<i<<" qte_min "<<qte_min<<" qte_max "<<qte_max<<endl;
									//if(numR==4 && h<1000) cout<<"pour sorti R4 qte_min="<<qte_min<<" qte_max="<<qte_max<<endl;
									//cout<<"ici qte_min="<<qte_min<<" qtemax="<<qte_max<<" dif "<<qte_max-qte_min<<endl;
									double pondere=(qte_max+10*qte_min)/11;
									if(qte_max-qte_min==0)pondere=qte_max;
									eoUniformGenerator<double> choixQtePart1(qte_min,pondere);
									eoUniformGenerator<double> choixQtePart2(pondere,qte_max);
									//cout<<"là"<<endl;
									double q;
									
									//cout<<"choix qte"<<endl;
									//cas normaux :

								     if(p<0.99)q=choixQtePart1();
								     else q=choixQtePart2();
								//	else if(p<0.2) q=qte_max;
								//	cout<<"cas normaux"<<endl;
									//cas palier de production:
									if(nbLiens==1 && li1.getNbTurbines()+li1.getNbPompes()>0)
									{
										vector<Turbine*> vT=li1.getListeTurbines();
										for(int m=0;m<li1.getNbTurbines();m++)
										{
											double palier=vT[m]->getPalier();
										//	cout<<"ici? "<<palier<<endl;
											if(palier>0 &&h>0)
											{
												//cout<<"dans palier"<<endl;
												double QPalier;
												if(h>1)QPalier=vT[k]->getQPalier(_genotype.getContenu(h-2,numR));
												else QPalier=vT[k]->getQPalier(R->getVinit());
												if(h>=2)QPalier=vT[m]->getQPalier(_genotype.getContenu(h-2,numR));
												else QPalier=vT[m]->getQPalier(R->getVinit());
												double QNPalier=vT[m]->getQPalier(_genotype.getContenu(h-1,numR));
												double qPred=_genotype.getDebitEnCours(h-1,numL,longL);
												if(qte_min==0 && (qPred==QPalier||qPred==0))
												{
													if (p<0.5) q=0;
												}
												else if(qte_min==0)
												{
													if(q==0)q=qte_max;
													if(QNPalier<qte_max && p<0.3) q=QNPalier;
													else if(p<0.5)q=qte_max;
												}
												else if(qte_min<QNPalier && QNPalier<qte_max)
												{
													if(p<0.3)q=QNPalier;
													else if(p<0.5)q=qte_min;
													else if(p<0.65)q=qte_max;
												}
											}
										}
									}
								//	cout<<"palier"<<endl;
									//si dernier reservior du circuit et liaison sortante ou que la longueur et plus grande que 0
									if(k==rDep+circuit.size()-1 &&(circuit[(k+1)%circuit.size()]!=numSuc||longL>0))q=qte_max;
									//modification du génome et des matrices : 
									//modification de l'individu :


									//modification débit en cours :
									for(int m=h;m<h+longL+1;m++)
									{
										//cout<<"piece "<<longL-(m-h)<<" long L "<< longL<<" m "<<m<<" numL "<<numL<<endl;
										// cout << "est-ce là l 1634" << endl;
										if(m<nbHeures)_genotype.setDebitEnCours(m,numL,longL-(m-h),q);
									}
									//cout<<"modif debit"<<endl;
									//modification reservoir:
									//cout<<"av modif "<<"contenu "<<_genotype.getContenu(h,numR)<<" h "<<h<<endl;
									_genotype.setContenu(h,numR,_genotype.getContenu(h,numR)-q);
									
									//modification réservoir d'arrivé : faire gaffe reservoir final
									//int numRF=liaison.getReservoirFin();
									if(numSuc<nbReservoirs && h+longL<nbHeures)_genotype.setContenu(h+longL,numRF,_genotype.getContenu(h,numRF)+q);
									//modif qteR
									qteR+=q;
									if(q<0)cout<<"attention q<0 "<<q<<endl;
									//cout<<"modif R et qR"<<endl;
									//modif àDebiter et aFaireCirculer
									//cout<<"/modif àDebiter et aFaireCirculer l1232"<<endl;
									if(aDebiter[numR][numSuc]>=q)
										{
											aDebiter[numR][numSuc]-=q;
											//cout<<"l1330 numR="<<numR<<" numSuc="<<numSuc<<endl;
											//si vers l'exterieur ou longueur sup à 0 alors modif reste à deverser
											if(k==rDep+circuit.size()-1&&(numSuc==nbReservoirs||longL>0))resteADeverser-=q;
										}
									else
									{
										//cout<<"l 1218"<<endl;
										double reste=q-aDebiter[numR][numSuc];
										aDebiter[numR][numSuc]=0;
										//si vers l'interieur :
										if(circuit[(k+1)%circuit.size()]==numSuc)
										{
										//	cout<<"si vers l'int"<<endl;
											//cout<<"vers l'interieur"<<endl;
											//if(numSuc==4)cout<<"5 bien dans si l 1329"<<endl;
											//cout<<k<<" reste "<<reste<<endl;
											if(reste<=aFaireCirculer[j][k%circuit.size()][(k+1)%circuit.size()])
											{
												aFaireCirculer[j][k%circuit.size()][(k+1)%circuit.size()]-=reste;
											}
											else
											{
												
												for(int m=0;m<circuit.size()-1;m++)
												{
													//cout<<"dans pour "<<k<<endl;
													if(m!=k%circuit.size())
													{
														//cout<<"modif aFaireC 0/1"<<endl;
														aFaireCirculer[j][m][m+1]+=reste-aFaireCirculer[j][(k)%circuit.size()][(k+1)%circuit.size()];
													}
												}
												if(circuit.size()-1!=k%circuit.size())
												{
													//cout<<"modif aFaireC 1/0"<<endl;
													aFaireCirculer[j][circuit.size()-1][0]+=reste-aFaireCirculer[j][(k)%circuit.size()][(k+1)%circuit.size()];
												}
												aFaireCirculer[j][(k)%circuit.size()][(k+1)%circuit.size()]=0;
											}
											if(longL>0 &&k==rDep+circuit.size()-1)resteADeverser-=q;
											//cout<<"fin si"<<endl;
										}
										
										//si vers l'exterieur :
										else{
											//cout<<"vers l'exterieur"<<endl;
											//si vers un autre circuit :
										
											if(k==rDep+circuit.size()-1)resteADeverser-=q;
										}
										//cout<<"si vers ext"<<endl;

										//sinon 
									}


								}
							}
						//	if(h==8759)cout<<"aFaire Circuler av tq :"<<aFaireCirculer[0][1][0]<<" "<<aFaireCirculer[0][0][1]<<" contenu R5="<<_genotype.getContenu(h,5)<<" contenu R4="<<_genotype.getContenu(h,4)<<endl;
						
							//si dernier reservoir deversement de reste à deverser :
							//cout<<"apres"<<endl;
							if(k==rDep+circuit.size()-1)
							{
								int kont=k+1;

								while(resteADeverser>0.00001)
								{
									//cout<<"ds tq l 1657 h:"<<h<<" "<<resteADeverser<<" "<< _genotype.getContenu(h,4)<<" "<< _genotype.getContenu(h,5)<<endl;
									//deb correction
									int numC=kont%circuit.size();
									int numR=circuit[numC];
									Reservoir* R=&systeme->getReservoir(circuit[numC]);
									//Calcul Qmin/Qmax pour respecter vmin et vmax :
									double Vmax=R->getVmax();
									double Vmin=R->getRealVmin(h);
									double vmin=0;
									double qteR=0;
									//mise à jour contenu :
									
									//cout<< "deb redistribution, contenu = "<<_genotype.getContenu(h,numR)<<" "<<numR<<endl;
									if(vmin < _genotype.getContenu(h,numR)-Vmax) vmin= _genotype.getContenu(h,numR)-Vmax;
									double vmax= _genotype.getContenu(h,numR) -Vmin;
									//cout<<"vmax deb "<<vmax<<" "<<numR<<endl;
									Reservoir* Rs=&systeme->getReservoir(circuit[(kont+1)%circuit.size()]);
									int numSuc=Rs->getNumero();
									//cout<<"l 1631"<<endl;
									//respect de Vmax du successeur :
									
									double VmaxS=Rs->getVmax();
									if(vmax>VmaxS-_genotype.getContenu(h,numSuc)) vmax=VmaxS-_genotype.getContenu(h,numSuc);
									//cout<<" vmax apres si 1 "<<vmax<<endl;
									resteADeverser=vmin-vmax;
									if(vmin>vmax)
									{
										
										vmax=vmin;
										//cout<<"on est ici "<<numR<<endl;
									}
									//cout<<" vmax apres si 2 "<<vmax<<endl;
									//if(qteRT+vmax>qTot) vmax=qTot-qteRT;
									int cardSorties= R->getCardSorties();
									vector<int> sorties=R->getSorties();
									//pour chaque liaison :
									//cout<<"pour chaque liaison :"<<endl;
									for(l=0;l<cardSorties;l++)//attention derniere liaison !
									{
										//liaison : 
										int numL= sorties[l];
									Liaison liaison =systeme->getLiaison(numL);
									numSuc=liaison.getReservoirFin();
									int longL=liaison.getLongueur();
									//regarder si il s'agit d'une liaison à traiter :
									bool aTraiter=true;
									//temporellement utilisable ? :
									if(nbHeures-longL<=h)aTraiter=false;
									int lFut=l+1;
									bool derniereDeSonEspece=true;
									int numRF=liaison.getReservoirFin();
									while(lFut<cardSorties && derniereDeSonEspece)
									{
										if(systeme->getLiaison(sorties[lFut]).getReservoirFin()==numRF)
											derniereDeSonEspece=false;
										lFut++;
									}
									if(true){
										//cout<<"aTraiter"<<endl;
										//calcul qte_min et qte_max pour la liaison :
										double qte_min=0;
										double qte_max=vmax-qteR;
										if(l==cardSorties-1&& qteR<vmin)qte_min=vmin-qteR;
										//if (kont<k+15)cout<<"qte_max l2062="<<qte_max<<" vmax "<<vmax<<" qteR "<<qteR<<endl;
										//cout<<"qte_max deb ?"<<qte_max<<" "<<numR<<" vmax "<<vmax<<" qteR "<<qteR<<endl;
										//vérifier si arret programmé :
										bool ArretProgramme=liaison.enArret(h);
										if(ArretProgramme)
										{
											qte_max=0;
										}
										//if (kont<k+15)cout<<"qte_max l2069 ="<<qte_max<<endl;
										if(C.in(numSuc)==false)
										{
											numSuc=nbReservoirs;
											if(qte_max>aDebiter[numR][numSuc])
												qte_max=aDebiter[numR][numSuc];
											//if(kont<k+15)cout <<" aDebext="<<aDebiter[numR][numSuc]<<endl;
										}
										//if (kont==k+1)cout<<"qte_max l2076 ="<<qte_max<<endl;
										//Incompatibilités :
										if(liaison.contrainteDincompatibilite() && !derniereDeSonEspece)
										{
											int nbLiens=liaison.getNbLiens();
											bool constraint=true;
											Lien  li1=liaison.getLien(0);
											bool b=false;
											vector<Turbine*> vT=li1.getListeTurbines();
											vector<Pompe*> vP=li1.getListePompes();
											for(int m=0;m<li1.getNbTurbines()+li1.getNbPompes();m++)
											{
												
												if( li1.getNbPompes()>0)
												{
													int numI=vP[m]->getIncompatibilites();
													if(numI>-1){
													//regarder si allumée à h+distT : qte d'eau qui arrive :
													int disT=vP[m]->getDistance();
													Turbine tI=systeme->getTurbine(numI);
													int distTI=tI.getDistance();
													int numLI=tI.getNumLiaison();
													int longLI=systeme->getLiaison(numLI).getLongueur();
													if(h+disT<nbHeures && _genotype.getDebitEnCours(h+disT,numLI,longLI-distTI)>0)
													{
														qte_max=0; 
													}}
												}
												else 
												{
													int numI=vT[m]->getIncompatibilites();
													if(numI>-1){
													//regarder si allumée à h+distT : qte d'eau qui arrive :
													int disT=vT[m]->getDistance();
													Pompe tI=systeme->getPompe(numI);
													int distTI=tI.getDistance();
													int numLI=tI.getNumLiaison();
													int longLI=systeme->getLiaison(numLI).getLongueur();
													if(h+disT<nbHeures &&_genotype.getDebitEnCours(h+disT,numLI,longLI-distTI)>0)
													{
														qte_max=0; 
													}}
												}
											}
										//	if (kont<k+15)cout<<"fin incomp qte_max="<<qte_max<<endl;
										}
										
										else if(liaison.contrainteDincompatibilite())
										{
											//cout<<"ici h="<<h<<endl;
											int nbLiens=liaison.getNbLiens();
											bool constraint=true;
											Lien  li1=liaison.getLien(0);
											bool b=false;
											vector<Turbine*> vT=li1.getListeTurbines();
											for(int lue=0;lue<li1.getNbTurbines();lue++)
											{
												int numI=vT[lue]->getIncompatibilites();
												if(numI>-1)
												{
													int disT=vT[lue]->getDistance();
													Pompe tI=systeme->getPompe(numI);
													int distTI=tI.getDistance();
													int numLI=tI.getNumLiaison();
													int longLI=systeme->getLiaison(numLI).getLongueur();
													if(_genotype.getDebitEnCours(h+disT,numLI,longLI-distTI)>0)
													{
														int numRI=systeme->getLiaison(numLI).getReservoirIni();
														int numRIF=systeme->getLiaison(numLI).getReservoirFin();
														vector<Liaison*> lcandidats=systeme->getLiaisonsAToB(numRI,numRIF);
														int nbCandidats=lcandidats.size();
														int cont =0;
														int numRedirection=-1;
														while(cont<nbCandidats&&numRedirection==-1)
														{
															if(lcandidats[cont]->getNumero()!=numLI && lcandidats[cont]->getLongueur()==longLI  )
															{
																bool accepte=true;
																if(lcandidats[cont]->enArret(h+disT-distTI)) accepte=false;
																if(lcandidats[cont]->getMinTpsConstant()>1)accepte=false;
																if(lcandidats[cont]->contrainteDincompatibilite()) accepte=false;
																if(lcandidats[cont]->palier())accepte=false;
																if(accepte) numRedirection=lcandidats[cont]->getNumero();
															}
															cont++;
														}
														if(numRedirection==-1)cerr <<"Ini: ajout conduite necessaire pour contraintes temps Min de fonctionement"<<endl;
														int longR=systeme->getLiaison(numRedirection).getLongueur();
														for(int t=h+disT-distTI;t<h+disT-distTI+longR+1;t++)
														{
															// cout << "est-ce là l 1891" << endl;
															_genotype.setDebitEnCours(t,numRedirection,h+disT-distTI+longR-t,_genotype.getDebitEnCours(h+disT-distTI,numLI,longLI)+_genotype.getDebitEnCours(h+disT-distTI,numRedirection,longR));
														}
														for(int t=h+disT-distTI;t<h+disT-distTI+longLI+1;t++)
														{
															// cout << "est-ce là l 1895" << endl;
															if(t<nbHeures)_genotype.setDebitEnCours(t,numLI,h+disT-distTI+longLI-t,0);
														}
													}
												}
											}
										}

											//cout<< "Incompatibilité l 1716"<<endl;
										//palier de production
										int nbLiens=liaison.getNbLiens();
										Lien  li1=liaison.getLien(0);
										if(qte_max==0 && h>0){
											if(nbLiens==1 && li1.getNbTurbines()+li1.getNbPompes()>0)
											{
												vector<Turbine*> vT=li1.getListeTurbines();
												for(int n=0;n<li1.getNbTurbines();n++)//attention k!
												{
													double palier=vT[n]->getPalier();
													if(palier>0 )
													{
														//calcul prodPredMin  :
														double QPalier;
														if(h>1)QPalier=vT[k]->getQPalier(_genotype.getContenu(h-2,numR));
														else QPalier=vT[k]->getQPalier(R->getVinit());
														if(_genotype.getDebitEnCours(h-1,numL,longL)!=QPalier && _genotype.getDebitEnCours(h-1,numL,longL)>0)
														{
															//corriger :
															int tps=_genotype.getTempsMmPuissance(h-1,numL);
															//chercher qty_max que l'on peut retirer aux horraires precedentes sans violer la contrainte sur le contenu :
															double qty_max=_genotype.getDebitEnCours(h-1,numL,longL);
															for(int m=0;m<tps;m++)
															{
																double vol=_genotype.getContenu(h-1-m,numR);
																if(Vmax-vol<qty_max) qty_max= Vmax-vol;
															}
															//correction : revoir contenu resrevoir arivé plus modif der
															double qty=qty_max/2;
															for(int m =h-tps;m<h;m++)
															{
																for(int a=m;a<=m+longL;a++)
																{
																	if(a>-1&& a<nbHeures)
																	{
																		// cout << "est-ce là l 1939" << endl;
																		_genotype.setDebitEnCours(a,numL,longL-(a-m),_genotype.getDebitEnCours(a,numR,longL-(a-m))-qty);
																	}
																}
																_genotype.setContenu(h,numR,_genotype.getContenu(h,numR)+qty);
																int numSuc=liaison.getReservoirFin();
																if(C.in(numSuc)==false)
																{
																	numSuc=nbReservoirs;
																	
																}
															//	cout<<"diminution suc"<<endl;
																if(numSuc<nbReservoirs &&h+longL<nbHeures )_genotype.setContenu(h+longL,numSuc,_genotype.getContenu(h,numSuc)+qty);
																if(circuit[(kont+1)%circuit.size()]==numSuc)aDebiter[numR][numSuc]+=qty;
																else if(numSuc==nbReservoirs){
																	//cherche liaison :
																	int cand=0;
																	while(liaisonsSortie[cand]->getNumero()!=numL)
																	{
																		cand++;
																	}
																	cand+=nbReservoirs+1;
																	aDebiter[numR][cand]+=qty;
																}
																else aFaireCirculer[j][k%circuit.size()][(kont+1)%circuit.size()]+=qty;
															}
															qte_max=qty*tps;
															qte_min=qty*tps;
														}
													}
												}
											}
										}
										//cout <<"pprod l 1772"<<endl;
										//tempsMinimal de fonctionement :
										int tpsMinConstant=liaison.getMinTpsConstant();
										if(tpsMinConstant>1 && h>0)
										{
											//cout<<"dans si l 1826"<<endl;
											//recuperertpsMinconstant heure pred  :
											int tps= _genotype.getTempsMmPuissance(h,numL);
											if(tps<tpsMinConstant)
											{
												//reccuperer qtePred : 
												double qtePred = _genotype.getDebitEnCours(h-1,numL,longL);
												//regarder s'il est possible de debiter qtePred :
												if(qtePred>=qte_min && qtePred<=qte_max)
												{
													qte_min=qtePred;
													qte_max=qtePred;
												}
												else
												{
													if(qtePred>qte_max)
													{
														qte_min=qte_max;
														//correction sur les heures precedentes :
														//chercher autre liaison avec meme autre reservoir de meme taille et non contrainte :
														int RFin=liaison.getReservoirFin();
														vector<Liaison*> lcandidats=systeme->getLiaisonsAToB(numR,RFin);
														int nbCandidats=lcandidats.size();
														int cont=0;
														int numRedirection=-1;
														while(cont<nbCandidats&&numRedirection==-1)
														{
															if(lcandidats[cont]->getNumero()!=numL && lcandidats[cont]->getLongueur()==longL  )
															{
																//verifier si contrainte 6 ne s'applique pas :
																bool accepte=true;
																for(int m=h-tps;m<h;m++)
																{
																	if(lcandidats[cont]->enArret(m)) accepte=false;
																}
																//verifier si contrainte 9 n'existe pas :
																if(lcandidats[cont]->getMinTpsConstant()>1)accepte=false;
																//verifier si contrainte 10 n'existe pas :
																if(lcandidats[cont]->contrainteDincompatibilite()) accepte=false;
																//vérifier si contrainte 11 n'existe pas :
																if(lcandidats[cont]->palier())accepte=false;
																if(accepte) numRedirection=lcandidats[cont]->getNumero();
															}
															cont++;
														}
														if(numRedirection==-1)cerr <<"Ini: ajout conduite necessaire pour contraintes temps Min de fonctionement"<<endl;
														//redistribuer pour toute les conduites precedentes et effectuer les changements necessaires:
														double dif=qtePred-qte_max;
														for(int m=h-tps;m<h;m++)
														{
															//modif liaison :
															for(int c=m;c<m+longL+1;c++)
															{
																// cout << "est-ce là l 2028" << endl;
																_genotype.setDebitEnCours(c,numL,longL-(c-m),qte_max);// a verifier...
															}
															//modif redirection :
															for(int c=m;c<m+longL+1;c++)
															{
																// cout << "est-ce là l 2034 " << endl;
																_genotype.setDebitEnCours(c,numRedirection,longL-(c-m),_genotype.getDebitEnCours(c,numRedirection,longL-(c-m))+dif);
															}
														}
													}
												}
											}
										}//minfuncConst
										//cout<<"min func "<<endl;
										//Autres contraintes si possible :
										eoUniformGenerator<double> choixP(0,1);
										double p=choixP();
										double qminL=liaison.getQmin();
										if(qminL>qte_min && qminL<qte_max)
											qte_min=qminL;
										else if(qminL<qte_max)
											p=1;
										double qmaxL;
										if(h>0) qmaxL=liaison.getQmax(_genotype.getContenu(h-1,numR));

										else qmaxL=liaison.getQmax(R->getVinit());
										if(qmaxL<qte_max && qte_min< qmaxL)
											qte_max=qmaxL;
										else if(qte_min<qmaxL)
											p=0;
										//cout<<"autre"<<endl;
										//choix qte :
										//calcul suc pour aDebiter
										int numSuc=liaison.getReservoirFin();
										if(C.in(numSuc)==false)
										{
											numSuc=nbReservoirs;
										}

										//cout<<"//choix quantite :"<<endl;
										//cout<<"contenu "<<_genotype.getContenu(h,numR)<<" "<<numR<<endl;
										if(h>=tpsMax[j])
										{
											
												double qte_int=aDebiter[numR][numSuc];
											//regarder si successeur est dans le circuit (c à d si c'est le j+1)
											if(circuit[(kont+1)%circuit.size()]==numSuc)
												qte_int+=aFaireCirculer[j][numC][(kont+1)%circuit.size()];
											if(qte_max>qte_int) qte_max=qte_int;

										}
										//cout<<"là"<<endl;
										if(qte_min<0)qte_min=0;
										if(qte_min>qte_max && qte_min<qte_max+0.00001)qte_max=qte_min;
										if(qte_min>qte_max) cout<<"l 1866 numR "<<numR<<" vmin "<<vmin<<" vmax "<<vmax<<" Vmin "<<Vmin<<" Vmax "<<Vmax<<" h "<<h<< " numComp "<<i<< " qmin="<<qte_min<<" qmax="<<qte_max<<endl;
										eoUniformGenerator<double> choixQtePart1(qte_min,(qte_max+qte_min)/2);
										eoUniformGenerator<double> choixQtePart2((qte_max+qte_min)/2,qte_max);
										//cout<<"qte_min "<<qte_min<<" qte_max "<<qte_max<<endl;
										double q;


										//cas normaux :

										if(p<0.7)q=choixQtePart1();
										else  q=choixQtePart2();
										//cout<<"cas normal"<<endl;
										//cas palier de production:
										if(nbLiens==1 && li1.getNbTurbines()+li1.getNbPompes()>0)
										{
											vector<Turbine*> vT=li1.getListeTurbines();
											for(int m=0;m<li1.getNbTurbines();m++)//attention k..?
											{
												double palier=vT[m]->getPalier();
												if(palier>0 &&h>0)
												{
													double QPalier;
													if(h>1)QPalier=vT[k]->getQPalier(_genotype.getContenu(h-2,numR));
													else QPalier=vT[k]->getQPalier(R->getVinit());
													double QNPalier=vT[m]->getQPalier(_genotype.getContenu(h-1,numR));
													double qPred=_genotype.getDebitEnCours(h-1,numL,longL);
													if(qte_min==0 && (qPred==QPalier||qPred==0))
													{
														if (p<0.5) q=0;
													}
													else if(qte_min<=0)
													{
														if(q<=0)q=qte_max;
														if(QNPalier<qte_max && p<0.3) q=QNPalier;
														else if(p<0.5)q=qte_max;
													}
													else if(qte_min<QNPalier && QNPalier<qte_max)
													{
														if(p<0.3)q=QNPalier;
														else if(p<0.5)q=qte_min;
														else if(p<0.65)q=qte_max;
													}
												}
											}
										}
										//cout<<"palier de prod"<<endl;
										if(circuit[(kont+1)%circuit.size()]!=numSuc||longL>0){
											q=qte_max;
											//cout<<"dans ce cas "<<numR<<longL<<endl;
										}
										//modification du génome et des matrices : 
										//modification de l'individu :


										//modification débit en cours :
										for(int t=h;t<h+longL+1;t++)
										{
											// cout << "est-ce là l 2140" << endl;
											if(t<nbHeures)_genotype.setDebitEnCours(t,numL,longL-(t-h),_genotype.getDebitEnCours(t,numL,longL-(t-h))+q);
										}
										//modification reservoir:
										//cout<<"l1985 q ="<<q<<endl;
										_genotype.setContenu(h,numR,_genotype.getContenu(h,numR)-q);
										if(_genotype.getContenu(h,numR)<0)
										{
											//cout<<"cont inf 0 l 2151 "<<_genotype.getContenu(h,numR);
											return;
										}
										//modification réservoir d'arrivé :regvoir pour der
										int numRF=liaison.getReservoirFin();
										if(numSuc<nbReservoirs && h+longL<nbHeures) _genotype.setContenu(h+longL,numRF,_genotype.getContenu(h+longL,numRF)+q);
										//modif qteR
										qteR+=q;
										//cout<<"qteR l 1937"<<endl;
										//modif àDebiter et aFaireCirculer
										if(aDebiter[numR][numSuc]>=q)
											{
												aDebiter[numR][numSuc]-=q;
												//if (kont==k+1||kont==k+2)		cout<<"l2145 numR="<<numR<<" numSuc="<<numSuc<<" q="<<q<<" vmin="<<vmin<<" vmax="<<vmax<<" h="<<h<<" contenu ="<<_genotype.getContenu(h,numR)<<endl;
												if(circuit[(kont+1)%(circuit.size())]!=numSuc)resteADeverser-=q;
											}
										else
										{
										//	if (kont==k+1)		cout<<"l2145 numR="<<numR<<" numSuc="<<numSuc<<" q="<<q<<" vmin="<<vmin<<" vmax="<<vmax<<" h="<<h<<" contenu ="<<_genotype.getContenu(h,numR)<<endl;
											
											double reste=q-aDebiter[numR][numSuc];
											aDebiter[numR][numSuc]=0;
											//si vers l'interieur :
											if(circuit[(kont+1)%(circuit.size())]==numSuc)
											{
												//cout<<"si1 "<<k%circuit.size()<<" "<<numSuc<<endl;
												if(reste<aFaireCirculer[j][(kont)%circuit.size()][(kont+1)%circuit.size()])
												{
													aFaireCirculer[j][(kont)%circuit.size()][(kont+1)%circuit.size()]-=reste;
												}
												else
												{
													//cout<<"sinon"<<endl;
													for(int m=0;m<circuit.size()-1;m++)
													{
														if(m!=kont%circuit.size())
														{
															aFaireCirculer[j][m][m+1]+=reste-aFaireCirculer[j][(kont)%circuit.size()][(kont+1)%circuit.size()];
														}
													}
													if(circuit.size()-1!=kont%circuit.size())
													{
														aFaireCirculer[j][circuit.size()-1][0]+=reste-aFaireCirculer[j][(kont)%circuit.size()][(kont+1)%circuit.size()];
													}
													aFaireCirculer[j][(kont)%circuit.size()][(kont+1)%circuit.size()]=0;
												}
												if(longL>0)resteADeverser-=q;
											}
											//si vers l'exterieur :
											else{
												//cout<<"sinon l 1974"<<endl;
												//si vers un autre circuit :
												/*if(numSuc<nbReservoirs )
												{
													//cout<<"si vers dehors"<<endl;
													//trouver cet autre circuit :
													int autre;
													bool cond2=false;
													int cont=j;
													while(cont<nbCircuits&& cond2==false)
													{
														vector<int> c=C.getCircuit(cont);
														for(int b=0;b<c.size();b++)
														{
															if(c[b]==liaison.getReservoirFin())
															{
																cond2=true;
																autre=b;

															}
														}
														cont++;
													}
													//cout<<"fin tq"<<endl;
													//modifier aFaireCirculer de autre:
													for(int b=0;b<C.getCircuit(autre).size();b++)
													{
														//cout<<C.getCircuit(j).size()<<endl;
														
														for(int d=0;d<C.getCircuit(j).size();d++)
															{//cout<<aFaireCirculer[autre][b][d]<<endl;
															aFaireCirculer[autre][b][d]+=reste;}
													}

												}*/
												resteADeverser-=q;
											}
											//cout<<"fin sinon"<<endl;
												
											//sinon rien
										}//fin sinon 



									}//fin si à traiter
									//cout<<" contenu av der Liaison "<<_genotype.getContenu(h,numR)<<" "<<numR<<endl;
									
								}//fin pour chaque liaison
									//cout<<"der liaison"<<endl;
								//gestion derniere liaison :
								//liaison : 
									
								kont++;
								//if(numR==5)cout<<"Vinit(5)="<<R->getVinit()<<endl;
							}//fin tant que(resteADeverser>0) 
							//	cout<<"fin tq"<<endl;
								
						}//fin si dernier reservior
							//cout<<"nbSorties "<<nbSorties<<endl;
							
					}//cout<<"fin pour k"<<endl;
							
					//	if(h>8757)cout<<"h="<<h<<" aDebiter 5/8:"<<aDebiter[5][8]<<" aDebiter 4/5:"<<aDebiter[4][5]<<" aDebiter 5/4:"<<aDebiter[5][4]<<" cont(5)="<<_genotype.getContenu(h,5)<<" cont(4)="<<_genotype.getContenu(h,4)<<" rDeb="<<rDep<<endl;
						//if(h>8757)cout<<"aFaire Circuler 5=>4 :"<<aFaireCirculer[0][1][0]<<" 4=>5 :"<<aFaireCirculer[0][0][1]<<endl;
				}//cout<<"fin h"<<endl;
				//cout<<"tempMax ="<<tpsMax[j]<<endl;
			}//cout<<"fin j i="<<i<<endl;
			
			  
		}//fin sin
	//cout<<"fin sinon"<<endl;



	


		}//fin i
	//	cout<<"fin i"<<endl;
		//reprise des minTempsConstant:
		/*for(i=0;i<nbLiaisons;i++)
		{
			int tpsMin=vecL[i].getMinTpsConstant();
			for(h=0;h<nbHeures;h++)
			{
				if(_genotype.getTempsMmPuissance(h,i)>tpsMin)
					_genotype.setTempsMemePuissance(h,i,tpsMin);
			}
		}*/
		cout<<"fin ini"<<endl;

		eoBooleanGenerator boost_init(biais);
		bool choixBoost = boost_init();

		if (choixBoost)
		{
			cout << "enhanceInit" << endl;
			enhanceInit(_genotype);
		}
}//fin operateur//fin j

double creerGraphe(Graphe & G, GenotypeT & _genotype1, int t_initial, int t_final, boost::property_map<Graphe,boost::edge_weight_t>::type & weight, boost::property_map<Graphe,boost::edge_capacity_t>::type & capacity, boost::property_map<Graphe,boost::edge_reverse_t>::type & rev, boost::property_map<Graphe,boost::edge_residual_capacity_t>::type & residual_capacity, boost::property_map<Graphe,boost::vertex_name_t>::type & id, boost::property_map<Graphe,boost::edge_name_t>::type & e_liaison)
 {
  int nbLiaisons = systeme->getNbLiaisons();
  int nbReservoirs = systeme->getNbReservoirs();
    // int t_final = G.getTFinal();
    // int t_initial = G.getTInitial();

  vector<vector<int> > liaisonsSupp;
  vector<int> liaisonKeep;
  for (int i=0; i<nbLiaisons;i++)
  {
    Liaison & L = systeme->getLiaison(i);
    int longueurL = L.getLongueur();
    vector<int> liaisonSupp;
    if (L.palier())
      liaisonKeep.push_back(i);
    else if (L.contrainteDincompatibilite())
    {
      bool find = false;
      int numLIncomp;

      for (int k=0; find==false && k<L.getNbLiens(); k++)
      {
        Lien & l = L.getLien(k);
        for (int j=0; find==false && j<l.getNbTurbines(); j++)
        {
          Turbine* T = l.getTurbine(j);
          int incomp = T->getIncompatibilites();
          if (incomp != -1)
          {
            find = true;
            numLIncomp = systeme->getPompe(incomp).getNumLiaison();
          }
        }

        for (int j=0; find==false && j<l.getNbPompes(); j++)
        {
          Pompe* P = l.getPompe(j);
          int incomp = P->getIncompatibilites();
          if (incomp != -1)
          {
            find = true;
            numLIncomp = systeme->getTurbine(incomp).getNumLiaison();
          }
        }
      }

      for (int h=t_initial; h<t_final; h++)
      {
        double debit_g1 = _genotype1.getDebitEnCours(h,i,longueurL);

        if (debit_g1 == 0.0 || isInVectInt(liaisonKeep,numLIncomp))
          liaisonSupp.push_back(h);
        
      }
    }
    liaisonsSupp.push_back(liaisonSupp);
  }


  int id_cpt = 0;    

  Sommet source = boost::add_vertex(G);
  id[source] = id_cpt;
  id_cpt++;
    //(-1, t_initial-1);
    //sommets.push_back(s);

  for (int i=0; i<nbReservoirs-1; i++)
  {
    for (int t=t_initial; t<t_final; t++)
    {
      Sommet s_tmp = boost::add_vertex(G);
      id[s_tmp] = id_cpt;
      id_cpt++;
        // Sommet s_tmp(i,t);
        // sommets.push_back(s_tmp);
    }
  }

  Sommet puits = boost::add_vertex(G);
  id[puits] = id_cpt;
  id_cpt++;
    // (-1, t_final);
    // sommets.push_back(puits);

  int nbSommets = id_cpt;

  boost::graph_traits<Graphe>::vertex_iterator u_first, u_end;

  boost::tie(u_first,u_end) = vertices(G);

  for (int i=0; i<nbReservoirs-1; i++)
  {
    Reservoir & R = systeme->getReservoir(i);
    vector<int> entrees = R.getEntrees();
      // On lie les apports
    for (int t=t_initial; t<t_final; t++)
    {
      int s = getIdSommet(t_initial,t_final,i,t);
      double cap = R.getApport(t);

      ajoutArc(G,e_liaison,-2,weight,capacity,residual_capacity,rev,cap,0.0,*u_first,*(u_first+s));
        //G.addArete(-1,0,s,cap,0.0,-1);
    }
    double cap_vol = R.getVinit();
    if (t_initial>0)
    	cap_vol = _genotype1.getContenu(t_initial-1,i);
    int s_if = getIdSommet(t_initial,t_final,i,t_initial);

    ajoutArc(G,e_liaison,-1,weight,capacity,residual_capacity,rev,cap_vol,0.0,*u_first,*(u_first+s_if));

      // On lie les liaisons déjà connues avant la recherche du flot
    for (int j=0; j<entrees.size(); j++)
    {
      Liaison & L = systeme->getLiaison(entrees[j]);
      int longueurL = L.getLongueur();
      double cap;
      int s_f;
      for (int t=t_initial; t<t_initial+longueurL; t++)
      {
        cap = _genotype1.getDebitEnCours(t,entrees[j],0);
        s_f = getIdSommet(t_initial,t_final,i,t);
        ajoutArc(G,e_liaison,entrees[j],weight,capacity,residual_capacity,rev,cap,0.0,*u_first,*(u_first+s_f));

          //G.addArete(entrees[j],0,s_f,cap,0.0,-1);
      }
    }
  }

  double flow_wanted = 0.0;
  boost::graph_traits<Graphe>::out_edge_iterator o_i, o_end;
  for(boost::tie(o_i,o_end)=boost::out_edges(*u_first,G); o_i != o_end; o_i++)
    flow_wanted += capacity[*o_i];


  vector<bool> volRes(nbReservoirs,false);
  for (int i=0; i<nbLiaisons; i++)
  {
    Liaison & L = systeme->getLiaison(i);
    int longueurL = L.getLongueur();
    int resIni = L.getReservoirIni();

    if (resIni < nbReservoirs-1)
    {
      Reservoir & R = systeme->getReservoir(resIni);
      double vmax = R.getVmax(); 
      int resFin = L.getReservoirFin();
      double qmax = L.getMaxQmax();
      double qmin = L.getQmin();

      for (int t=t_initial; t<t_final; t++)
      {
          // On regarde si on peut débiter sur la liaison
        bool enArret = false;
        for (int h=t; !enArret && h<t+longueurL; h++)
          enArret = L.enArret(h);

        int s_i = getIdSommet(t_initial,t_final,resIni,t);

          // lien entre deux reservoirs identiques à temps consécutifs
        if (!volRes[resIni])
        {
          int s_i_suiv;
          double cap_vol;
          int numLf = -1;
          double w = 10.0;

          if (t<t_final-1)
          {
            s_i_suiv = getIdSommet(t_initial,t_final,resIni,t+1);
            ajoutArc(G,e_liaison,-1,weight,capacity,residual_capacity,rev,R.getRealVmin(t),-1.0,*(u_first+s_i),*(u_first+s_i_suiv));
            cap_vol = vmax-R.getRealVmin(t);
          }
          else
          {
            s_i_suiv = nbSommets-1;
            cap_vol = _genotype1.getContenu(t,resIni);
            w=0.0;
          }

          ajoutArc(G,e_liaison,numLf,weight,capacity,residual_capacity,rev,cap_vol,w,*(u_first+s_i),*(u_first+s_i_suiv));
        }
          //G.addArete(-1,s_i,s_i_suiv,cap_vol,0.0,-1);

          // lien de la liaison
        int s_f;
        double cap;
        if (t+longueurL < t_final)
        {
          if (!enArret && !isInVectInt(liaisonsSupp[i],t))
          {
            if (resFin != nbReservoirs-1)
              s_f = getIdSommet(t_initial,t_final,resFin,t+longueurL);
            else
              s_f = nbSommets-1;

            if (t==t_initial)
            {
              ajoutArc(G,e_liaison,i,weight,capacity,residual_capacity,rev, qmin,0.0,*(u_first+s_i),*(u_first+s_f));

              cap = qmax-qmin;
              if (t_initial>0)
	              cap = min(qmax,_genotype1.getDebitEnCours(t_initial-1,i,longueurL)+L.getDeltaUp())-qmin;
              if (cap<flow_wanted)
                ajoutArc(G,e_liaison,i,weight,capacity,residual_capacity,rev,cap,10.0,*(u_first+s_i),*(u_first+s_f));
              // cout << cap << "yolo" << endl;
                //G.addArete(i,s_i,s_f,cap,0.0,-1);

              if (t_initial >0)
              {
	              cap = max(qmax,_genotype1.getDebitEnCours(t_initial-1,i,longueurL)+L.getDeltaUp())-min(qmax,_genotype1.getDebitEnCours(t_initial-1,i,longueurL)+L.getDeltaUp())-qmin;
	              if (cap<flow_wanted)
	                ajoutArc(G,e_liaison,i,weight,capacity,residual_capacity,rev, cap,100.0,*(u_first+s_i),*(u_first+s_f));
	            }
              // cout << cap << "yolo" << endl;

                //G.addArete(i,s_i,s_f,cap,100.0,-1);

              cap = flow_wanted;
              ajoutArc(G,e_liaison,i,weight,capacity,residual_capacity,rev,cap,10000.0,*(u_first+s_i),*(u_first+s_f));

                //G.addArete(i,s_i,s_f,cap,10000.0,-1);
            }
            else if (t+longueurL == t_final-1)
            {
              ajoutArc(G,e_liaison,i,weight,capacity,residual_capacity,rev, qmin,0.0,*(u_first+s_i),*(u_first+s_f));
              cap = min(qmax,_genotype1.getDebitEnCours(t_final,i,longueurL)+L.getDeltaLo())-qmin;
              if(cap>flow_wanted)
                ajoutArc(G,e_liaison,i,weight,capacity,residual_capacity,rev, cap,10.0,*(u_first+s_i),*(u_first+s_f));                

              //   //G.addArete(i,s_i,s_f,cap,0.0,-1);

              cap = max(qmax,_genotype1.getDebitEnCours(t_final,i,longueurL)+L.getDeltaLo())-min(qmax,_genotype1.getDebitEnCours(t_final,i,longueurL)+L.getDeltaLo())-qmin;
              if (cap>flow_wanted)
                ajoutArc(G,e_liaison,i,weight,capacity,residual_capacity,rev, cap,100.0,*(u_first+s_i),*(u_first+s_f));

                // G.addArete(i,s_i,s_f,cap,100.0,-1);

              cap = flow_wanted;
              ajoutArc(G,e_liaison,i,weight,capacity,residual_capacity,rev,cap,10000.0,*(u_first+s_i),*(u_first+s_f));

                // G.addArete(i,s_i,s_f,cap,10000.0,-1);
            }
            else
            {
              ajoutArc(G,e_liaison,i,weight,capacity,residual_capacity,rev, qmin,10.0,*(u_first+s_i),*(u_first+s_f));
              ajoutArc(G,e_liaison,i,weight,capacity,residual_capacity,rev, qmax-qmin,100.0,*(u_first+s_i),*(u_first+s_f));

                // G.addArete(i,s_i,s_f,qmax,0.0,-1);
              ajoutArc(G,e_liaison,i,weight,capacity,residual_capacity,rev,flow_wanted,10000.0,*(u_first+s_i),*(u_first+s_f));

                // G.addArete(i,s_i,s_f,FLT_MAX,10000.0,-1);
            }
          }
        }
        else
        {
          s_f = nbSommets-1;
          cap = _genotype1.getDebitEnCours(t,i,longueurL);
          ajoutArc(G,e_liaison,i,weight,capacity,residual_capacity,rev,cap,0.0,*(u_first+s_i),*(u_first+s_f));

            // G.addArete(i,s_i,s_f,cap,0.0,-1);
        }
      }
    }
    volRes[resIni] = true;
  }

  int cpt = 0;
  boost::graph_traits<Graphe>::edge_iterator e_i, e_end;
  double flot_possible = 0.0;
  for (boost::tie(e_i,e_end)=boost::edges(G); e_i != e_end; e_i++)
  {
    if (boost::target(*e_i,G) == *(u_end-1))
    {
      if (capacity[*e_i]==flow_wanted)
        cpt++;
      else if (capacity[*e_i]==0.0)
        flot_possible += capacity[*e_i];
    }
  }
  if (flow_wanted < flot_possible)
    flow_wanted = -1;
  // if (flow_wanted > flot_possible)
  // {
  //   for (boost::tie(e_i,e_end)=boost::edges(G); e_i != e_end; e_i++)
  //   {
  //     if (boost::target(*e_i,G) == *(u_end-1))
  //     {
  //       if (capacity[*e_i]==flow_wanted)
  //       {
  //         capacity[*e_i]=(flow_wanted-flot_possible)/(double (cpt));
  //         residual_capacity[*e_i]=capacity[*e_i];
  //       }
  //     }
  //   }
  // }
  // else
  //   flow_wanted = -1;
  // cout << flow_wanted << endl;
  return flow_wanted;
}

void transformationFlot(Graphe & G, GenotypeT & _genotype1, int t_i, int t, boost::property_map<Graphe,boost::edge_weight_t>::type & weight, boost::property_map<Graphe,boost::edge_capacity_t>::type & capacity, boost::property_map<Graphe,boost::edge_reverse_t>::type & rev, boost::property_map<Graphe,boost::edge_residual_capacity_t>::type & residual_capacity, boost::property_map<Graphe,boost::vertex_name_t>::type & id, boost::property_map<Graphe,boost::edge_name_t>::type & e_liaison)
{
    // cout << "Transformation Flot" << endl;
  int nbSommets = boost::num_vertices(G);
  boost::graph_traits<Graphe>::vertex_iterator u_i, u_end;
  for (boost::tie(u_i,u_end)=boost::vertices(G); u_i != u_end; u_i++)
  {
    Sommet s = *u_i;
    int id_s = id[s];
    if (id_s>0 && id_s<nbSommets-1)
    {
      int numR = (id_s-1)/(t-t_i);
        // cout << numR << endl;
      int tps = id_s - (numR * (t-t_i)) + t_i -1;
        // cout << tps << endl;
      Reservoir & R = systeme->getReservoir(numR);
      vector<int> sorties = R.getSorties();
      vector<double> flots(R.getCardSorties(),0.0);
      double vol_tmp = 0.0;
      boost::graph_traits<Graphe>::out_edge_iterator e_i, e_end;
      for (boost::tie(e_i,e_end)=boost::out_edges(s,G); e_i != e_end; e_i++)
      {
        int numL = e_liaison[*e_i];
        if (numL == -1)
        {
          // cout << "NUML = -1 volume a " << tps << " pour " << numR << " : " << R.getRealVmin(tps) + capacity[*e_i] - residual_capacity[*e_i] << endl;
          // _genotype1.setContenu(tps,numR,/* R.getRealVmin(tps) +*/ capacity[*e_i] - residual_capacity[*e_i]);
          // _genotype1.setModif(tps,true);
          vol_tmp += capacity[*e_i] - residual_capacity[*e_i];
        }
        // else if (numL == -2)
        // {
        //   cout << " NUML = -2 volume a " << tps << " pour " << numR << " : " << capacity[*e_i] << endl;
        //   _genotype1.setContenu(tps,numR,capacity[*e_i]);
        //   _genotype1.setModif(tps,true);
        // }
        else if (capacity[*e_i] > 0.0)
        {
          int j=0;
          while(j<R.getCardSorties() && sorties[j] != numL)
            j++;
          if (j<R.getCardSorties())
            flots[j] += capacity[*e_i] - residual_capacity[*e_i];
        }
      }
      for (int j=0; j<R.getCardSorties(); j++)
      {
        int numL = sorties[j];
        // cout << numR << " && " << tps << " && " << numL << " && " << flots[j] << endl;
        Liaison & L = systeme->getLiaison(numL);
        int longueurL = L.getLongueur();
        for (int h=0; h<=longueurL; h++)
        {
          _genotype1.setDebitEnCours(tps+h,numL,longueurL-h,flots[j]);
          _genotype1.setModif(tps+h,true);
        }
      }
      _genotype1.setContenu(tps,numR,vol_tmp);
    }
  }
}

bool make_crossover(GenotypeT & _genotype1, int t, int t_i)
{
    // cout << "make_crossover" << endl;
  int nbReservoirs = systeme->getNbReservoirs();
    // cout << "t_i : " << t_i << " && t : " << t << endl;
  double flow_wanted = 0.0;
  double flow = -1.0;
  Graphe G;

  boost::property_map<Graphe,boost::vertex_name_t>::type id;
  boost::property_map<Graphe,boost::edge_name_t>::type e_liaison;
  boost::property_map<Graphe,boost::edge_weight_t>::type weight;
  boost::property_map<Graphe,boost::edge_capacity_t>::type capacity;
  boost::property_map<Graphe,boost::edge_reverse_t>::type rev;
  boost::property_map<Graphe,boost::edge_residual_capacity_t>::type residual_capacity;

  bool find = true;
  boost::graph_traits<Graphe>::edge_iterator e_i,e_end;
  bool sature = true;
  while (find && sature && (t_i > 0 || flow != flow_wanted))
  {
    t_i--;
    G.clear();
    id = boost::get(boost::vertex_name,G);
    e_liaison = boost::get(boost::edge_name,G);
    weight = boost::get(boost::edge_weight,G);
    capacity = boost::get(boost::edge_capacity,G);
    rev = boost::get(boost::edge_reverse,G);
    residual_capacity = boost::get(boost::edge_residual_capacity,G);

    flow_wanted = creerGraphe(G,_genotype1,t_i,t,weight,capacity,rev, residual_capacity,id,e_liaison);
    // cout << flow_wanted << endl;

    boost::graph_traits<Graphe>::vertex_iterator u_first, u_end;
    boost::tie(u_first,u_end)=boost::vertices(G);

    Sommet source = *(u_first);

    bool findFlow = (flow_wanted!=-1);
    while (findFlow && flow_wanted != flow)
    {
      Graphe_e g_e;
      boost::property_map<Graphe_e,boost::vertex_name_t>::type id_e = boost::get(boost::vertex_name,g_e);
      boost::property_map<Graphe_e,boost::vertex_distance_t>::type distance_e = boost::get(boost::vertex_distance,g_e);
      boost::property_map<Graphe_e,boost::edge_weight_t>::type weight_e = boost::get(boost::edge_weight,g_e);
      boost::property_map<Graphe_e,boost::edge_name_t>::type name_e = boost::get(boost::edge_name,g_e);

      makeGrapheEcart(G,weight,capacity,residual_capacity,id,e_liaison, g_e,id_e,distance_e,weight_e,name_e);

      int N = boost::num_vertices(g_e);
      int M = boost::num_edges(g_e);
      // cout << N << " & " << M << endl;
      // cout << "calcul flot" << endl;
      vector<int> parents(N,0);
      for (int i=0; i<N; i++)
        parents[i]=i;
      vector<int> aretes(N,-1);

      bool findPath = BellManFord(g_e,id_e,distance_e,weight_e,name_e, N,parents,aretes);

      if (findPath)
      {
        modificationFlot(G,weight,capacity,residual_capacity,id,e_liaison, g_e,id_e,distance_e,weight_e,name_e,N,parents,aretes);
        flow = 0.0;
        boost::graph_traits<Graphe>::out_edge_iterator o_i,o_end;
        for (boost::tie(o_i,o_end)=boost::out_edges(source,G); o_i != o_end; o_i++)
          flow += capacity[*o_i] - residual_capacity[*o_i];

        // cout << flow_wanted << " && " << flow << endl;
      }
      else
        findFlow = false;
      
    }
    if (findFlow)
    {
      sature = true;
      for (boost::tie(e_i,e_end)=boost::edges(G); sature && e_i != e_end; e_i++)
      {
        if (boost::target(*e_i,G)==*(u_end-1) && capacity[*e_i] != flow_wanted)
          sature = (residual_capacity[*e_i]==0.0);
      }
      if (sature) 
        find = false;
    }
  }

  if(sature && (t_i > 0 || flow == flow_wanted))
  {
    // cout << "FLOT ===========>" << endl;
    
    // for (boost::tie(e_i,e_end)=boost::edges(G); e_i != e_end; e_i++)
    // {
    //   Arete e_tmp = *e_i;
    //   cout << boost::source(e_tmp,G) << " " << boost::target(e_tmp,G) << " " << capacity[e_tmp]-residual_capacity[e_tmp] << " " << capacity[e_tmp] << endl;
    // }
    // cout << endl;
    transformationFlot(G, _genotype1, t_i, t, weight, capacity, rev, residual_capacity, id, e_liaison);
    
    // cout << t_i << " " << t << endl;
      //check minTpsConstant
    for (int i=0; i<systeme->getNbLiaisons(); i++)
    { 
      Liaison & L = systeme->getLiaison(i);
      if (L.getMinTpsConstant() > 1)
        checkMinTpsConstant(_genotype1, L);
      if (L.palier())
        checkPalier(_genotype1,L);
    }
  }
  // cout << boolalpha << sature << endl;
  return sature;
}

void enhanceInit(GenotypeT & _genotype)
{
	int nbReservoirs = systeme->getNbReservoirs();

   vector<bool> P(nbReservoirs,false);
   vector<int> maxDist(nbReservoirs,0);
   vector<bool> dejaCalc(nbReservoirs,false);
   int minT = systeme->plusLongChemin(nbReservoirs-1,P,maxDist,dejaCalc)+2;

	int t_i = 0;
	int t_f;

	int duree = 200/(nbReservoirs-1);
	int cpt = 0;
	while (t_i < nbHeures - 1)
	{
		if (cpt != 0 && cpt < 6)
		{
			int t_min = nbHeures*cpt/6;
			int t_max = nbHeures*(cpt+1)/6;
			eoUniformGenerator<int> ti_init(t_min,t_max);
			t_i = ti_init();
		}
		else if (cpt == 6)
		{
			eoUniformGenerator<int> ti_init(nbHeures-((int) 1.25*duree), nbHeures-((int) 0.75*duree));
			t_i = ti_init();
		}
		eoUniformGenerator<int> temps(min(nbHeures-1,t_i+((int) 0.75*duree)),min(t_i+((int) 1.25*duree),nbHeures-1));
		t_f = temps();
		if (nbHeures-t_f < ((int) 0.75*duree))
			t_f = nbHeures-1;
		cout << t_i << " && " << t_f << endl;
		GenotypeT _genotype_copie = _genotype;
		if (!make_crossover(_genotype,t_f,t_i))
			_genotype = _genotype_copie;

		if (t_f<nbHeures-((int) 0.75*duree))
		{
			eoUniformGenerator<int> new_ti(t_f,nbHeures-((int) 0.75*duree));
			t_i = new_ti();
		}
		else
			t_i = t_f;
		cpt++;
	}
	for (int i=0; i<systeme->getNbLiaisons(); i++)
	{
		Liaison & L = systeme->getLiaison(i);
		if (L.getMinTpsConstant() > 1)
        checkMinTpsConstant(_genotype, L);
      if (L.palier())
        checkPalier(_genotype,L);
	}
}

void checkMinTpsConstant(GenotypeT & _genotype, Liaison & L)
{
  // cout << "minTpsConstant" << endl;
    int num = L.getNumero();
    int & longueur = L.getLongueur();
    int & minTpsConstant = L.getMinTpsConstant();

    vector<int> & candidats = systeme->getReservoir(L.getReservoirIni()).getSorties();
    bool find = false;
    int numC = 0;
    while (find==false && numC < candidats.size())
    {
      //cout << " find "<< boolalpha << find << endl;
      Liaison & L_tmp = systeme->getLiaison(candidats[numC]);
      int longueurC = L_tmp.getLongueur();
      if (L_tmp.getReservoirFin() == L.getReservoirFin() && candidats[numC] != num && longueurC == longueur)
        find = true;
      else
        numC++;
    }
    double debit_en_cours;
    vector<double> sequenceDebit;
    vector<int> sequenceDuree;
    vector<int> tempsInitial;

    for (int h=0; h<nbHeures;)
    {
      int t_prec=h+1;
      debit_en_cours = _genotype.getDebitEnCours(h,num,longueur);
      while (t_prec < nbHeures && _genotype.getDebitEnCours(t_prec,num,longueur)==debit_en_cours)
        t_prec++;
      sequenceDuree.push_back(t_prec-h);
      sequenceDebit.push_back(debit_en_cours);
      tempsInitial.push_back(h);
      // cout << h << " && " << debit_en_cours << " && " << t_prec-1 << endl;
      h = t_prec;
    }

    // cout << "sequenceDuree.size() = " << sequenceDuree.size() << endl;
    
    for (int card=0; card<sequenceDebit.size()-1;card++)
    {
      int duree_en_cours = sequenceDuree[card];
      if (duree_en_cours < minTpsConstant)
      {
        if (tempsInitial[card]==0)
        {
          int cardFin = card + 1;
          int sommeDuree = sequenceDuree[cardFin] + duree_en_cours;
          while (cardFin < sequenceDuree.size()-1 && sommeDuree < minTpsConstant)
          {
            cardFin++;
            sommeDuree += sequenceDuree[cardFin];
          }
          
          double qmin = sequenceDebit[card] + _genotype.getDebitEnCours(0,candidats[numC],longueur);
          for (int h=1; h<sommeDuree; h++)
            qmin = min(qmin,_genotype.getDebitEnCours(h,num,longueur) + _genotype.getDebitEnCours(h,candidats[numC],longueur));
          
          for (int h=0; h<sommeDuree; h++)
          {
            double somme = _genotype.getDebitEnCours(h,num,longueur) + _genotype.getDebitEnCours(h,candidats[numC],longueur);
            for (int t_tmp=0; t_tmp<=longueur; t_tmp++)
            {
              _genotype.setDebitEnCours(h+t_tmp,candidats[numC],longueur-t_tmp,somme-qmin);
              _genotype.setDebitEnCours(h+t_tmp,num,longueur-h,qmin);
              _genotype.setModif(h+t_tmp,true);
            }
          }
          card = cardFin;
        }
        else
        {
          double somme = sequenceDebit[card] + _genotype.getDebitEnCours(tempsInitial[card],candidats[numC],longueur);
          double q_prec = _genotype.getDebitEnCours(tempsInitial[card]-1,num,longueur);
          if (somme < q_prec)
          {
            double diff = q_prec - sequenceDebit[card];
            int t_init = tempsInitial[card] - 1;
            while(t_init>0 && _genotype.getDebitEnCours(t_init-1,num,longueur)==q_prec)
              t_init--;
            for (int h=t_init; h<tempsInitial[card]; h++)
            {
              double q_C = _genotype.getDebitEnCours(h,candidats[numC],longueur);
              for (int t_tmp=0; t_tmp<=longueur; t_tmp++)
              {
                _genotype.setDebitEnCours(h+t_tmp,candidats[numC],longueur-t_tmp,q_C+diff);
                _genotype.setDebitEnCours(h+t_tmp,num,longueur-t_tmp,q_prec-diff);
                _genotype.setModif(h+t_tmp,true);
              }
            }
          }
          else
          {
            for (int h=tempsInitial[card]; h<tempsInitial[card]+sequenceDuree[card]; h++)
            {
              for (int t_tmp=0; t_tmp<=longueur; t_tmp++)
              {
                _genotype.setDebitEnCours(h+t_tmp,candidats[numC],longueur-t_tmp,somme-q_prec);
                _genotype.setDebitEnCours(h+t_tmp,num,longueur-t_tmp,q_prec);
                _genotype.setModif(h+t_tmp,true);
              }
            }
          }
        }
      }
    }
}

void checkPalier(GenotypeT & _genotype, Liaison & L)
{
  // cout << "palier" << endl;
  int num = L.getNumero();
  int & longueur = L.getLongueur();
  vector<int> & candidats = systeme->getReservoir(L.getReservoirIni()).getSorties();
  bool find = false;
  int numC = 0;
  int longueurC;
  while (find==false && numC < candidats.size())
  {
    //cout << " find "<< boolalpha << find << endl;
    Liaison & L_tmp = systeme->getLiaison(candidats[numC]);
    longueurC = L_tmp.getLongueur();
    if (L_tmp.getReservoirFin() == L.getReservoirFin() && candidats[numC] != num && longueurC == longueur)
      find = true;
    else
      numC++;
  }

  bool probPalier = false;
  double qPalier = 0.0;
  bool findTurb = false;
  bool isTurb;
  Turbine* T;
  Pompe* P;
  for (int nbL=0; !findTurb && nbL<L.getNbLiens(); nbL++)
  {
    Lien & l_tmp = L.getLien(nbL);
    for (int card=0; !findTurb && card<l_tmp.getNbTurbines(); card++)
    {
      T = l_tmp.getTurbine(card);
      if (T->getPalier() > 0.0)
      {
        findTurb = true;
        isTurb = true;
      }
    }
    for (int card=0; !findTurb && card<l_tmp.getNbPompes(); card++)
    {
      P = l_tmp.getPompe(card);
      if (P->getPalier() > 0.0)
      {
        findTurb = true;
        isTurb = false;
      }
    }
  }

  double debit_en_cours;
  vector<double> sequenceDebit;
  vector<int> sequenceDuree;
  vector<int> tempsInitial;

  for (int h=0; h<nbHeures;)
  {
    int t_prec=h+1;
    debit_en_cours = _genotype.getDebitEnCours(h,num,longueur);
    while (t_prec < nbHeures && _genotype.getDebitEnCours(t_prec,num,longueur)==debit_en_cours)
      t_prec++;
    sequenceDuree.push_back(t_prec-h);
    sequenceDebit.push_back(debit_en_cours);
    tempsInitial.push_back(h);
    h = t_prec;
  }

      // cout << "sequenceDuree.size() = " << sequenceDuree.size() << endl;

      // cout << "tempsFinal : " << tempsInitial[tempsInitial.size()-1] << endl;
      // cout << "DebitFinal : " << sequenceDebit[sequenceDebit.size()-1] << endl;

  for (int card=1; card<sequenceDebit.size(); card++)  
  {
        // cout << endl;
        // cout << tempsInitial[card] << endl;
    if (sequenceDebit[card]==0.0)
    {
      int cardPrec = card-1;
      bool palierOK = false;
      double qPalier;
          // cout << "pa" << endl;
      if (isTurb)
        qPalier = T->getQPalier(_genotype.getContenu(tempsInitial[card]-2,T->getReservoirParent()));
      else
        qPalier = P->getQPalier(_genotype.getContenu(tempsInitial[card]-2,P->getReservoirParent()));
          // cout << qPalier << " && " << tempsInitial[card] << endl;
      while(cardPrec>=0 && !palierOK)
      {
        if (sequenceDebit[cardPrec]==0.0)
        {
          palierOK = true;
              // cout << "STOP a 0 pour h " << tempsInitial[cardPrec] << endl;
        }
        bool probInt = false;
        for (int h=tempsInitial[cardPrec]; !palierOK && !probInt && h<tempsInitial[cardPrec]+sequenceDuree[cardPrec]; h++)
        {
              // cout << h << " & ";
          double somme = _genotype.getDebitEnCours(h,candidats[numC],longueur) + sequenceDebit[cardPrec];
              // cout << somme << endl;
          probInt = (somme<qPalier);
        }
        if(!probInt)
        {
              // if (!palierOK)
              //   cout << "STOP a palier pour h " << tempsInitial[cardPrec] << endl;
          palierOK = true;
        }
        else
          cardPrec--;
      }
      if (cardPrec>=0 && sequenceDebit[cardPrec]!=0.0)
      {
            // cout << tempsInitial[cardPrec] << "    " << tempsInitial[cardPrec+1] << endl;
        for (int h=tempsInitial[cardPrec]; h<tempsInitial[cardPrec+1]; h++)
        {
              // cout << h << " &&&&& ";
          double somme = _genotype.getDebitEnCours(h,num,longueur) + _genotype.getDebitEnCours(h,candidats[numC],longueur);
          // cout << somme << endl;
          for (int t_tmp=0; t_tmp<=longueur; t_tmp++)
          {
            _genotype.setDebitEnCours(h+t_tmp,candidats[numC],longueur-t_tmp,somme-qPalier);
            _genotype.setDebitEnCours(h+t_tmp,num,longueur-h,qPalier);
            _genotype.setModif(h+t_tmp,true);
          }
        }
      }
          // cout << tempsInitial[cardPrec+1] << "    " << tempsInitial[card] << endl;
      for (int h=tempsInitial[cardPrec+1]; h<tempsInitial[card]; h++)
      {
            // cout << h << " &&& ";
        double somme = _genotype.getDebitEnCours(h,num,longueur) + _genotype.getDebitEnCours(h,candidats[numC],longueur);
            // cout << somme << endl;
        for (int t_tmp=0; t_tmp<=longueur; t_tmp++)
        {
          _genotype.setDebitEnCours(h+t_tmp,candidats[numC],longueur-t_tmp,somme);
          _genotype.setDebitEnCours(h+t_tmp,num,longueur-h,0.0);
          _genotype.setModif(h+t_tmp,true);
        }
      }
    }
  } 
}

private:
// START Private data of an eocasGeneralInit object
//  varType anyVariable;		   // for example ...
int nbHeures;
Systeme* systeme;
int nbLiaisons;
int nbReservoirs;
int nbComposantesConnexes;
double biais;
// END   Private data of an eocasGeneralInit object
};

#endif
