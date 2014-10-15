#include "eo"
#include <iostream>
#include<string>
using namespace std;
#include"Systeme.h"
#include"utils.h"
#include"eocasGeneral.h"
#include"eocasGeneralInit.h"
#include"eocasGeneralEvalFunc.h"
#include"readData.h"


typedef eocasGeneral<double> Indi; 
main()
{
	ReadData r("/home/jacquin/Bureau/PFE/Cas_2_bis/", 1,1);
    	r.readAllFiles();
	Systeme & sys = r.getSysteme();
	
	vector<int> numPTurpe;
        vector<vector<double> > turpe;
        vector<double> coefs;
        coefs.push_back(10.2);coefs.push_back(23.86);coefs.push_back(0.717);coefs.push_back(5000);
        turpe.push_back(coefs);

        for (int i=0; i<sys.getNbPompes(); i++)
        {
        	Pompe & P = sys.getPompe(i);
         	if (P.getTurp())
          		numPTurpe.push_back(i);
       	}
	eocasGeneralEvalFunc<Indi> plainEval (&sys,8760,numPTurpe,turpe,100000) ;
	 eocasGeneralInit<Indi> init(8760,&sys,0);
	Indi individu;
	init(individu);
	plainEval(individu);
	individu.printOn(cout,&sys);
	//cout<<"nb reservoirs"<<sys.getNbReservoirs();
	

}
