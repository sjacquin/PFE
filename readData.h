#ifndef ReadDataH
#define ReadDataH

/*!
 * \file readData.h
 * \author Sophie Jacquin & Guillaume Pataut
 */

#include <string>
#include <fstream>
#include <iostream>
#include <climits>
#include <cfloat>
#include <cstdlib>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/strong_components.hpp>
#include <boost/graph/graphviz.hpp>

#include "utils.h"
#include "Systeme.h"

using namespace std;

/*!
 * \class ReadData
 *
 * \brief Classe qui gère l'initialisation du système par la lecture des données
 */

class ReadData {

private: 
    Systeme sys;/*!< Système hydroélectrique */
    string directory;/*!< Répertoire des fichiers des données */
    int apportTest;/*!< Catégorie des apports à fournir */
    vector<string> data;/*!< Vecteur des données récupérées lors de la lecture des fichiers */
    graph_t g;/*!< Graphe associé au système */
    int coefProd;
    
public:
    // Constructeur :
    /*!
     * \brief Constructeur
     *
     * \param _directory : string, répertoire des fichiers de données
     * \param _prixTest : vecteur d'entier, vecteur des tests sur les prix
     * \param _apportTest : entier, catégorie des apports
     */
    ReadData(string _directory, int _apportTest,int _coefProd=1000)
    {
        directory = _directory;
        apportTest = _apportTest;
        coefProd=_coefProd;
    }

    //Accesseurs :
    /*!
     * \brief Donne le système hydroélectrique
     *
     * \return par référence un Systeme : sys
     */
    Systeme& getSysteme()
    {
        return sys;
    }

    /*!
     * \brief Donne le répertoire des fichiers de données
     *
     * \return une string : directory
     */
    string getDirectory()
    {
        return directory;
    }

    // Autres :
    /*!
     * \brief Lit un fichier donné en récupérant les informations dans \a data
     *
     * \param fileName : string, chemin d'accés au fichier de données depuis le répertoire courant
     *
     * \return une erreur si le chemin d'accès est invalide ou que le fichier n'existe pas
     */
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

    /*!
     * \brief Initialise les arêtes du graphes ainsi que les composantes connexes et les circuits
     */
    void initConduites() 
    {
        // cout << "init conduite" << endl;
        vertex_t v_dep, v_arr;
        std::pair<vertex_iterator_t, vertex_iterator_t> it_vertices;
        for (std::vector<std::string>::iterator it = data.begin(); it != data.end(); it = it + 8)
        {
            // On parcourt les sommets du graphes
            for (it_vertices = boost::vertices(g); it_vertices.first != it_vertices.second; it_vertices.first++)
            {
                // Si le sommet courant a le bon nom, on affecte le sommet correspondant
                if (get(boost::vertex_bundle,g)[*it_vertices.first].name == *(it+1))
                    v_dep = *it_vertices.first;
                if (get(boost::vertex_bundle,g)[*it_vertices.first].name == *(it+2))
                    v_arr = *it_vertices.first;
            }
            // On ajoute l'arc correspondant avec les propriétés
            float qmax = FLT_MAX, qmin = 0, deltaUp = FLT_MAX, deltaLo = FLT_MAX, distance = 0;

            string qmax_s = *(it+4);
            if (!qmax_s.empty())
                qmax = atof(qmax_s.c_str())*3600;
            
            string qmin_s = *(it+3);
            if (!qmin_s.empty())
                qmin = atof(qmin_s.c_str())*3600;
            
            string deltaLo_s = *(it+5);
            if (!deltaLo_s.empty())
                deltaLo = atof(deltaLo_s.c_str())*3600;
            
            string deltaUp_s = *(it+6);
            if (!deltaUp_s.empty())
                deltaUp = atof(deltaUp_s.c_str())*3600;
            
            string distance_s = *(it+7);
            if (!distance_s.empty())
                distance = atoi(distance_s.c_str());

            boost::add_edge(v_dep, v_arr, EdgeProperties(qmin, qmax, deltaLo, deltaUp, distance), g);

        }

        // On récupère les composantes fortement connexes du graphe
        vector<int> component(boost::num_vertices(g));
        int num = boost::strong_components(g, boost::make_iterator_property_map(component.begin(), get(boost::vertex_index, g)));
        // cout << num << endl;

        // On crée les composantes connexes du problème
        vertex_iterator_t it_v, it_end;
        tie(it_v, it_end) = boost::vertices(g);
        for (int i = 0; i<num; i++)
        {
            vector<int> listRes;
            for (int j=0; j<sys.getNbReservoirs(); j++)
            {
                if (component[j]==i)
                    listRes.push_back(j);    
            }
            if (listRes.size() > 0)
            {
                vector<vector<int> > circuits;
                ComposanteConnexe comp(listRes, circuits);
                sys.adComposanteConnexe(comp);
            }
        }
        
        string filename = directory + "Lien/circuits.csv";
        bool isCircuit = readDataFile(filename);
        if (isCircuit)
        {
            vector<string> nameCircuit;
            vector<int> nbResCircuits;

            for (vector<string>::iterator it = data.begin(); it != data.end(); it = it + 2)
            {
                nameCircuit.push_back(*it);
                nbResCircuits.push_back(atoi((*(it+1)).c_str()));
            }

            for (int i=0; i<nameCircuit.size(); i++)
            {
                vector<int> list_Res;
                string nameC = directory + "Lien/" + nameCircuit[i] + ".csv";
                readDataFile(nameC);
                for(vector<string>::iterator it = data.begin(); it != data.end(); it++)
                {
                        int num = atoi((*it).c_str());
                        list_Res.push_back(num);
                }
                for (int j=0; j<sys.getNbComposantesConnexes(); j++)
                {
                    ComposanteConnexe & C = sys.getComposanteConnexe(j);
                    
                    bool allResInC = true;
                    for (int k=0;k<list_Res.size(); k++)
                    {
                        if (!isInVectInt(C.getReservoirs(), list_Res[k]))
                            allResInC = false;
                    }
                    if (allResInC)
                    {
                        vector<int> newCircuit;
                        for (int l=0; l<list_Res.size(); l++)
                        {
                            newCircuit.push_back(sys.getReservoir(list_Res[l]).getNumero());
                        }
                        C.adCircuit(newCircuit);
                    }
                }
            }
        }
    }

    /*!
     * \brief Ordonne les réservoirs du plus en amont au plus en aval
     *
     * \param _adjacents : vecteur de vecteur d'entier, matrice d'adjacence créée à partir des données
     *
     * \return un vecteur d'entier : nouvel ordre des réservoirs créés initialement par lecture des données
     */
    vector<int> ordonnerRes(const vector<vector<int> > & _adjacents)
    {
        // cout << "ordonner res" << endl;
        vector<int> dejaTrier;
        vector<ComposanteConnexe> comps;
        int* newPos = (int*) malloc(sys.getNbReservoirs()*sizeof(int));
        for (int i=0; i<_adjacents.size(); i++)
        {
            if (_adjacents[i].size() == 0)
            {
                dejaTrier.push_back(i);
                ajoutComp(sys,comps,i);
            }
        }

        while (dejaTrier.size() < sys.getNbReservoirs())
        {
            vector<int> amonts;
            for (int i=0; i<_adjacents.size(); i++)
            {
                if (!isInVectInt(dejaTrier,i))
                {
                    vector<int> aAjouter;
                    for (int j=0; j<comps.size(); j++)
                    {
                        if (comps[j].in(i))
                            aAjouter.push_back(i);
                    }
                    for (int j=0; j< aAjouter.size(); j++)
                    {
                        dejaTrier.push_back(aAjouter[j]);
                        ajoutComp(sys,comps,aAjouter[j]);
                    }
                    for (int k=0; k<dejaTrier.size(); k++)
                    {
                        if (isInVectInt(_adjacents[i],dejaTrier[k]))
                            amonts.push_back(i);
                    }
                }
            }

            for (int l=0; l<amonts.size(); l++)
            {
                int ajout = 0;
                for (int j=0; j<amonts.size(); j++)
                {
                    if (j!=l && isInVectInt(_adjacents[amonts[l]],amonts[j]))
                        ajout++;
                }

                if (ajout==0 && !isInVectInt(dejaTrier,amonts[l]))
                {
                    dejaTrier.push_back(amonts[l]);
                    ajoutComp(sys,comps,amonts[l]);
                }
            }
        }

        vector<Reservoir> tmp;
        vector<int> toReturn;
        vertex_iterator_t it = boost::vertices(g).first;
        for (int i=0; i<dejaTrier.size(); i++)
        {
            tmp.push_back(sys.getReservoir(dejaTrier[dejaTrier.size()-1-i]));
            newPos[dejaTrier[dejaTrier.size()-1-i]] = i;
            tmp[i].setNumero(i);
            get(boost::vertex_bundle,g)[*(it+i)].id = dejaTrier[dejaTrier.size()-1-i];          
        }

        for (int i=0; i<sys.getNbReservoirs(); i++)
        {
            toReturn.push_back(newPos[i]);
            Reservoir & R = sys.getReservoir(i);
            R = tmp[i];
            vector<int> lastAdjacents = R.getAdjacents();
            for (int j=0; j<lastAdjacents.size(); j++)
            {
                lastAdjacents[j] = newPos[lastAdjacents[j]];
            }
            R.setAdjacents(lastAdjacents);
        }

        return toReturn;
    }

    /*!
     * \brief Crée toutes les liaisons entre les différents réservoirs
     *
     * \param _liens : vecteur de vecteur de Lien, matrice des liens
     * \param _adjacents : vecteur de vecteur d'entier, matrice d'adjacence
     * \param _props : vecteur de vecteur d'EdgeProperties, matrice des arêtes
     */
    void creerLiaison(const vector<vector<Lien> > & _liens, const vector<vector<int> > & _adjacents, const vector<vector<EdgeProperties> > & _props)
    {
        // cout << " creer liaisons " << endl;
        vector<int> newOrdre = ordonnerRes(_adjacents);
        vector<int> lastOrdre;
        for (int i=0; i<sys.getNbReservoirs(); i++)
        {
            int j=0;
            while (newOrdre[j]!=i)
                j++;
            lastOrdre.push_back(j);
        }

        for (int i=0; i<sys.getNbTurbines(); i++)
        {
            Turbine & T = sys.getTurbine(i);
            // cout << i << " " << T.getReservoirParent() << endl;
            T.setReservoirParent(newOrdre[T.getReservoirParent()]);
        }

        for (int i=0; i<sys.getNbPompes(); i++)
        {
            Pompe & P = sys.getPompe(i);
            P.setReservoirParent(newOrdre[P.getReservoirParent()]);
        }

        for (int i=0; i<sys.getNbComposantesConnexes(); i++)
        {
            ComposanteConnexe & C = sys.getComposanteConnexe(i);
            for (int j=0; j<C.getNbReservoirs(); j++)
            {
                int num_R = C.getReservoir(j);
                C.setReservoir(j,newOrdre[num_R]); 
            }
            for (int j=0; j<C.getNbElementaryCircuits(); j++)
            {
                vector<int> list_tmp = C.getCircuit(j);
                vector<int> new_list;
                for (int k=0; k<list_tmp.size(); k++)
                {
                    new_list.push_back(newOrdre[list_tmp[k]]);
                }
                C.setCircuit(j,new_list);
                list_tmp = C.getCircuit(j);
            }
        }

        ordonnerComposanteConnexe(sys);
        
        vector<vector<Liaison> > liaisons;

        for (int i=0; i<sys.getNbReservoirs(); i++)
        {
            vector<Liaison> l_tmp;
            Reservoir & R_deb = sys.getReservoir(i); 
            vector<int> res_deja_lie;
            vector<Lien> toDo = _liens[lastOrdre[i]];
            vector<int> toLink = R_deb.getAdjacents();
            vector<EdgeProperties> toEdge = _props[lastOrdre[i]];
            for (int j=0; j<toLink.size(); j++)
            {
                vector<Lien> liens_tmp;
                Reservoir & R_fin = sys.getReservoir(toLink[j]);
                int minTpsConstant = toDo[j].getMinTpsConstant();
                EdgeProperties prop_tmp = toEdge[j];
                int ajoutLiaison = 0;
                if (isInVectInt(res_deja_lie, toLink[j]))
                {
                    int k=0;
                    while (k<l_tmp.size() && l_tmp[k].getReservoirFin() != toLink[j] )
                    {
                        k++;
                    }
                    Liaison & L_tmp = l_tmp[k];
                    
                    if (L_tmp.getLongueur() == prop_tmp.distance && prop_tmp.deltaUp == FLT_MAX && prop_tmp.deltaUp == L_tmp.getDeltaUp() && (prop_tmp.deltaLo == 0  || prop_tmp.deltaLo==FLT_MAX)
&& prop_tmp.deltaLo == L_tmp.getDeltaLo() && toDo[j].getMinTpsConstant() == 0 && toDo[j].getMinTpsConstant() == L_tmp.getMinTpsConstant() && !L_tmp.contrainteDincompatibilite() && !L_tmp.palier() && !toDo[j].contrainteDincompatibilite() && !toDo[j].palier() && !L_tmp.arretProgramme() && !toDo[j].arretProgramme() )
                    {
			if(L_tmp.get_nbPlant()<1||toDo[j].getNbTurbines()==0)
			{
				//cout<<"ici nbPlant="<<L_tmp.get_nbPlant()<<" "<<toDo[j].getNbTurbines()<<endl;
                    		Lien & l_to_change = toDo[j];
                    		for (int l=0; l<l_to_change.getNbTurbines(); l++)
                    			l_to_change.getTurbine(l)->setNumLiaison(L_tmp.getNumero());
                    		for (int l=0; l<l_to_change.getNbPompes(); l++)
                    			l_to_change.getPompe(l)->setNumLiaison(L_tmp.getNumero());
                        	L_tmp.adLien(toDo[j]);
				//if(toDo[j].getNbTurbines()>0)
					//L_tmp.addPlant();
                        	ajoutLiaison++;
			}
                    }
                }
                if (ajoutLiaison==0)
                {
                    res_deja_lie.push_back(toLink[j]);
                    liens_tmp.clear();
                    liens_tmp.push_back(toDo[j]);
                    Liaison L(i,toLink[j],1,liens_tmp, prop_tmp.distance, prop_tmp.deltaUp, prop_tmp.deltaLo, minTpsConstant);
                    Lien & l_to_change = toDo[j];
                    for (int l=0; l<l_to_change.getNbTurbines(); l++)
                    	l_to_change.getTurbine(l)->setNumLiaison(L.getNumero());
                     for (int l=0; l<l_to_change.getNbPompes(); l++)
                    	 l_to_change.getPompe(l)->setNumLiaison(L.getNumero());
                    l_tmp.push_back(L);
                    R_deb.adSortie(L.getNumero());
                    R_fin.adEntree(L.getNumero());
                }
            }
            liaisons.push_back(l_tmp);
        }
        for (int i=0; i<sys.getNbReservoirs(); i++)
        {
            for (int j=0; j<liaisons[i].size(); j++)
                sys.adLiaison(liaisons[i][j]);
        }
        // for (int i=0; i<sys.getNbLiaisons(); i++)
        // {
        //     Liaison & L = sys.getLiaison(i); 
        //     cout << L.getNumero() << " " << L.getMaxQmax()<< endl;
        // }
    }

    /*!
     * \brief Crée les liens entre les différents réservoirs
     */
    void creerLiens()
    {   
        // cout << " creer liens " << endl;
        vector<vector<int> > adjacents;
        vector<vector<Lien> > liens;
        vertex_iterator_t it_vertices = boost::vertices(g).first;
        vector<vector<EdgeProperties> > props;
        for (int i=0; i< sys.getNbReservoirs(); i++)
        {
            vector<int> adjacent;
            vector<Lien> list_liens;
            vector<EdgeProperties> prop;
            vertex_t v = *(it_vertices + i);
            out_edge_iterator_t it_out, it_end;
            tie(it_out, it_end) = boost::out_edges(v,g);
            vector<vertex_t> adj_v;
            vector<edge_t> adj_edge_v;
            for (; it_out != it_end; it_out++)
            {
                edge_t e = *(it_out);
                adj_edge_v.push_back(e);
                adj_v.push_back(boost::target(e,g));
            }
            for (int j=0; j<adj_v.size(); j++)
            { 
                vector<Pompe*> pompes;
                vector<Turbine*> turbines;
                
                
                vertex_t v_tmp = adj_v[j];
                edge_t e_tmp = adj_edge_v[j];
                EdgeProperties e_prop = g[e_tmp];
                
                int ind_tmp = get(boost::vertex_bundle,g)[v_tmp].id;
                
                while (ind_tmp>= sys.getNbReservoirs())
                {
                    if (ind_tmp < sys.getNbReservoirs()+sys.getNbTurbines())
                    {
                        Turbine & T = sys.getTurbine(ind_tmp-(sys.getNbReservoirs()));
                        turbines.push_back(&T);
                        T.setReservoirParent(i);
                    }
                    else
                    {
                        Pompe & P = sys.getPompe(ind_tmp-(sys.getNbReservoirs()+sys.getNbTurbines()));
                        pompes.push_back(&P);
                        P.setReservoirParent(i);
                    }
                    cout << ind_tmp << " " << i << endl;
                    // on suppose qu'il n'y a qu'une seule sortie pour les turbines ou les pompes
                    e_tmp = *(boost::out_edges(v_tmp,g).first);
                    v_tmp = boost::target(e_tmp,g);
                    ind_tmp = get(boost::vertex_bundle,g)[v_tmp].id;
                    miseAjourEdge(e_prop,g[e_tmp]);
                }
                prop.push_back(e_prop);
                adjacent.push_back(ind_tmp);
                cout << e_prop.qmax << endl;
                Lien l(pompes.size(),turbines.size(),pompes,turbines,e_prop.qmin, e_prop.qmax);
                list_liens.push_back(l);
            }
            props.push_back(prop);
            Reservoir & R = sys.getReservoir(i);
            R.setAdjacents(adjacent);
            adjacents.push_back(adjacent);
            liens.push_back(list_liens);
        }
        creerLiaison(liens, adjacents, props);
    }

    /*!
     * \brief Initialise les réservoirs à partir des données
     *
     * \param _compt : entier, à partir duquel on commence la numérotation des sommets correspondants aux réservoirs dans le graphe
     */
    void initReservoirs(int & _compt)
    {
        // cout << " init res " << endl;
        for (std::vector<std::string>::iterator it = data.begin(); it != data.end(); it = it + 5)
        {
            int nbIntVmin = atof((*(it+2)).c_str());
            double Vmax;
            string Vmax_s = *(it+1);
            if (Vmax_s.empty())
                Vmax = FLT_MAX;
            else
                Vmax = atof(Vmax_s.c_str());
            int* intVmin = (int*) malloc(nbIntVmin * sizeof(int));
            double* Vmin = (double*) malloc(nbIntVmin * sizeof(double));
            string intVmin_s = *(it+3);
            string Vmin_s = *(it+4);
            if (nbIntVmin>1)
            {
                std::size_t pos_intVmin_prec = 0;
                std::size_t pos_Vmin_prec = 0;
                for (int i=0; i<nbIntVmin; i++)
                {
                    size_t pos_intVmin = intVmin_s.find('+',pos_intVmin_prec);
                    intVmin[i] = atoi((intVmin_s.substr(pos_intVmin_prec,pos_intVmin-pos_intVmin_prec)).c_str());
                    size_t pos_Vmin = Vmin_s.find('+',pos_Vmin_prec);
                    Vmin[i] = atof((Vmin_s.substr(pos_Vmin_prec,pos_Vmin-pos_Vmin_prec)).c_str());
                    pos_intVmin_prec = pos_intVmin+1;
                    pos_Vmin_prec = pos_Vmin+1;
                }
            }
            else
            {
                intVmin[0] = atoi(intVmin_s.c_str());
                Vmin[0] = atof(Vmin_s.c_str());
            }

            vector<int> adjacents;
            Reservoir R(*it, Vmax/2,Vmax,nbIntVmin,intVmin,Vmin,adjacents);
            sys.adReservoir(R);   

            // Ajout du réservoir dans le graphe
            vertex_t res = boost::add_vertex(VertexProperties(*it, _compt),g);
            _compt++;
        }
        // Reste à mettre les apports
        ostringstream oss;
        oss << apportTest;
        string filename = directory+"Apport/apports_serie"+oss.str()+".csv";
        readDataFile(filename);
        int nbReservoirs = sys.getNbReservoirs();
        
        double **apports = (double**) malloc(nbReservoirs * sizeof(double *));
        for (int i=0; i<nbReservoirs; i++)
            apports[i] = (double*) malloc(8760 * sizeof(double));

        int j=0;
        for (std::vector<std::string>::iterator it = data.begin(); it != data.end(); it = it + nbReservoirs)
        {
            for (int i=0; i<nbReservoirs; i++)   
                apports[i][j] = atof((*(it+i)).c_str());
            j++;
        }

        for (int i=0; i<nbReservoirs; i++)
        {
            Reservoir & R = sys.getReservoir(i);
            R.adApports(apports[i]);
        }
    }

    /*!
     * \brief Initialise les turbines à partir des données
     *
     * \param _compt : entier, à partir duquel on commence la numérotation des sommets correspondants aux turbines dans le graphe
     */
    void initTurbines(int & _compt)
    {
        // cout << "init turbines" << endl;
        for (std::vector<std::string>::iterator it = data.begin(); it != data.end(); it = it + 12)
        {
            int prodMin = atoi((*(it+6)).c_str());
            int distance = atoi((*(it+1)).c_str());
            int resParent = -1;
            int catPrix = atoi((*(it+10)).c_str());

            int minT = atoi((*(it+2)).c_str());


            int incompatibilites;
            if ((*(it+3)).empty())
                incompatibilites = -1;
            else 
                incompatibilites = atoi(((*(it+3)).substr(5)).c_str()) - 1;

            int deb = atoi((*(it+4)).c_str());
            int fin = atoi((*(it+5)).c_str());
            int minTpsConstant = atoi((*(it+9)).c_str());
            double palier = atof((*(it+11)).c_str());

            Turbine T(*it, prodMin, distance, resParent, catPrix, minT, incompatibilites, deb, fin, minTpsConstant, palier);

            int nbPieces = atoi((*(it+7)).c_str());
            int nbInt = atoi((*(it+8)).c_str());

            T.setNbInt(nbInt);
            T.setNbPieces(nbPieces);
            T.setPrix(sys.getPrix(catPrix));
            // Ajout de la turbine dans le système
            sys.adTurbine(T);

            // Ajout de la turbine dans le graphe
            vertex_t turb = boost::add_vertex(VertexProperties(*it, _compt),g);
            _compt++;
        }
        // Reste à mettre à jour la production et les prix

        // Production
        int nbTurbines = sys.getNbTurbines();
        cout << nbTurbines << endl;
        for (int t=0; t<nbTurbines; t++)
        {
            ostringstream oss;
            oss << t+1;
            string filename = directory+"Turbine/Turb"+oss.str()+".csv";
            readDataFile(filename);


            Turbine & T = sys.getTurbine(t);
            int nbInt = T.getNbInt();
            int nbPieces = T.getNbPieces();
            double* intReservoirs = (double *) malloc(nbInt*sizeof(double));
            double* pieces = (double*) malloc(nbPieces*sizeof(double));
            double* qmax = (double*) malloc(nbInt*sizeof(double));
            double** production = (double**) malloc(nbPieces*sizeof(double*));

            std::vector<std::string>::iterator it = data.begin();
            for (int i=0; i<nbPieces; i++)
                production[i] = (double*) malloc(nbInt*sizeof(double));

            for (int j=0; j<nbInt; j++)
            {
                if (nbInt>1)
                {
                   // cout<<"atof((*(it+j+1)).c_str());="<<atof((*(it+j+1)).c_str())<< " " << j << endl;;
                    intReservoirs[j] = atof((*(it+j+1)).c_str());
                }
                else
                    intReservoirs[j] = 0.0;
               // cout<<"j="<<j<<" "<<intReservoirs[j]<<endl;
                qmax[j] = 0.0;
            }

            if (nbInt>1)
                it += nbInt + 1;

            int i=0;
            // cout << "est-ce là ? " << endl;
            for ( ; it != data.end(); it = it + nbInt + 1)
            {
                double tmp_d =atof((*it).c_str());
                pieces[i] = tmp_d*3600;
                // cout<<"i="<<i<<" piece="<<pieces[i]<<endl;
                for (int j=0; j<nbInt; j++)
                {
                    // cout << *(it+j+1) << endl;
                    string tmp_s = *(it+j+1);
                    production[i][j] = atof(tmp_s.c_str())*coefProd;
                    // cout<< " i="<<i<<" j="<<j<< " prod="<<production[i][j]<<endl;
                    if (tmp_s != "")
                        qmax[j] = tmp_d;
                }
                i++;
            }
            double** coeff = (double**) malloc((nbPieces-1)*sizeof(double*));
            for (int i=0; i<nbPieces-1; i++)
                coeff[i] = (double*) malloc(nbInt*sizeof(double));
            for (int i=0; i<nbPieces; i++)
            {
                for (int j=0; j<nbInt; j++)
                {
                    if (i==0)
                        coeff[i][j] = production[i][j];
                    else
                        coeff[i-1][j] = (production[i][j]-production[i-1][j]) / (pieces[i]-pieces[i-1]);
                   // if(i==0)cout<<production[i][j]<<" coeff="<<coeff[i][j]<<endl;
                }
            }

            T.setProd(nbInt, nbPieces, intReservoirs, pieces, qmax, coeff);
            // cout << "fin turbines" << endl;
            // cout << "turbine " << t << " max qmax() " << T.getMaxQmax() << endl;
        }
        cout << "fin init turbines" << endl;
    }
    
    /*!
     * \brief Initialise les pompes à partir des données
     *
     * \param _compt : entier, à partir duquel on commence la numérotation des sommets correspondants aux pompes dans le graphe
     */
    void initPompes(int & _compt)
    {
    	cout<<"init Pompe"<<endl;
        for (std::vector<std::string>::iterator it = data.begin(); it != data.end(); it = it + 15)
        {
            int prodMin = atoi((*(it+6)).c_str());
            int distance = atoi((*(it+1)).c_str());
            int resParent = -1;
            int catPrix = atoi((*(it+12)).c_str());
            int minT = atoi((*(it+2)).c_str());

            int incompatibilites;
            if ((*(it+3)).empty())
                incompatibilites = -1;
            else 
                incompatibilites = atoi(((*(it+3)).substr(4)).c_str()) - 1;
            // cout<<715<<endl;
            int deb = atoi((*(it+4)).c_str());
            int fin = atoi((*(it+5)).c_str());
            int nbAchats = atoi((*(it+9)).c_str());
            int minTpsConstant = atoi((*(it+11)).c_str());
            double palier = atof((*(it+13)).c_str());
            bool turp = (atof((*(it+14)).c_str())==1);
            
            vector<int> achats;
            string achats_s = *(it+10);
            string achats_tmp_s;
            if (!achats_s.empty())
            {
                std::size_t pos_prec_ach = 0;
                std::size_t pos_ach = 0;
                while (pos_ach < achats_s.size())
                {
                    pos_prec_ach = pos_ach;
                    pos_ach = achats_s.find('+',pos_ach+1);
                    if (pos_ach != std::string::npos)
                    {
                        achats_tmp_s = achats_s.substr(pos_prec_ach+4,pos_ach-(pos_prec_ach+4));
                        pos_ach++;
                    }
                    else
                        achats_tmp_s = achats_s.substr(pos_prec_ach+4, achats_s.size()-(pos_prec_ach+4));
                    achats.push_back(atoi(achats_tmp_s.c_str())-1);

                }
            }


            Pompe P(*it, prodMin, distance, resParent, catPrix, minT, incompatibilites, deb, fin, nbAchats, achats, minTpsConstant, palier,turp);

            P.setNbInt(atoi((*(it+7)).c_str()));
            P.setNbPieces(atoi((*(it+8)).c_str()));
            P.setPrix(sys.getPrix(catPrix));

            //Ajout de la pompe dans le système
            sys.adPompe(P);
            
            // Ajout de la pompe dans le graphe
            vertex_t pomp = boost::add_vertex(VertexProperties(*it, _compt),g);
            _compt++;
        }

        // Production
        cout<<"prod pompe"<<endl;
        int nbPompes = sys.getNbPompes();
        for (int p=0; p<nbPompes; p++)
        {
        	cout<<"deb for"<<endl;
            ostringstream oss;
            oss << p+1;
            string filename = directory+"Pompe/Pompe"+oss.str()+".csv";
            readDataFile(filename);
            
            Pompe & P = sys.getPompe(p);
            int nbInt = P.getNbInt();
            int nbPieces = P.getNbPieces();

            double* intReservoirs = (double *) malloc(nbInt*sizeof(double));
            double* pieces = (double*) malloc(nbPieces*sizeof(double));
            double* qmax = (double*) malloc(nbInt*sizeof(double));
            double** production = (double**) malloc(nbInt*sizeof(double*));

            for (int j=0; j<nbInt; j++)
            {
                production[j] = (double*) malloc(nbPieces*sizeof(double));
                intReservoirs[j] = 0.0;
                qmax[j] = 0.0;
            }

            int i=0;
            for (std::vector<std::string>::iterator it = data.begin(); it != data.end(); it = it + nbInt + 1)
            {
            	cout<<i<<endl;
                double tmp_d =atof((*it).c_str());
                pieces[i] = tmp_d*3600;
                // cout<<"pieces[i]="<<pieces[i]<<endl;
                for (int j=0; j<nbInt; j++)
                {
                    string tmp_s = *(it+j+1);
                    // cout<<"i="<<i<<" j="<<j<<endl;
                    production[j][i] = atof(tmp_s.c_str());
                    if (tmp_s != "")
                        qmax[j] = tmp_d;
                }
                i++;
            }
            // cout<<"là"<<endl;
            double** coeff = (double**) malloc((nbPieces-1)*sizeof(double*));
            for (int i=0; i<nbPieces-1; i++)
                coeff[i] = (double*) malloc(nbInt*sizeof(double));
            for (int i=1; i<nbPieces; i++)
            {
                for (int j=0; j<nbInt; j++)
                {

                        coeff[i-1][j] = (production[j][i]-production[j][i-1]) / (pieces[i]-pieces[i-1]);
                        // cout<<" coeff["<<i-1<<"]["<<j<<"]="<<coeff[i-1][j] ;
                }
            }

            P.setProd(nbInt, nbPieces, intReservoirs, pieces, qmax, coeff);
            // cout << "pompe " << p << " msax qmax() " << P.getMaxQmax() << endl;

            //cout<<"sorti"<<endl;

        }
        cout<<"fin"<<endl;
    }

    /*!
     * \brief Initialise les prix horaires des turbines et des pompes à partir des données
     */
    void initPrix()
    {
        cout << "init prix" << endl;
        vector<string> namePrix;
        vector<int> nbParam;
        for (std::vector<std::string>::iterator it = data.begin(); it != data.end(); it = it + 2)
        {
            namePrix.push_back(*it);
            nbParam.push_back(atoi((*(it+1)).c_str()));
            sys.adNbParamPrix(atoi((*(it+1)).c_str()));
        }

        for (int i=0; i<namePrix.size(); i++)
        {
            string filename = directory + "Prix/" + namePrix[i] + ".csv";
            readDataFile(filename);
            double * prix = (double*) malloc(8760*sizeof(double));
            int j=0;
            
            for (std::vector<std::string>::iterator it = data.begin(); it != data.end(); it = it + nbParam[i] + 1)
            {
                if (nbParam[i] == 1)
                    prix[j] = atof((*(it+1)).c_str());
                else
                    prix[j] = atof((*(it+apportTest)).c_str());
                j++;
            }
            sys.adPrix(prix);
        }
    }

    void creerAdjacents()
    {
        for (int i=0; i< sys.getNbReservoirs(); i++)
        {
            Reservoir & R = sys.getReservoir(i);
            vector<int> adj;
            for (int card=0; card<R.getCardSorties(); card++)
            {
                Liaison & L = sys.getLiaison(R.getSorties()[card]);
                int numRFin = L.getReservoirFin();
                if (!isInVectInt(adj,numRFin))
                    adj.push_back(numRFin);
            }
            R.setAdjacents(adj);
            cout << "Reservoir " << i << " a " << R.getAdjacents().size() << " réservoirs adjacents " << endl;
        }
    }

    /*!
     * \brief Lit le fichier "conduites.csv" et initalise les conduites
     */
    void readConduites() {
        string filename = directory+"Lien/conduites.csv";
        // cout << "readConduites" << endl;
        bool isCond = readDataFile(filename);
        // cout << boolalpha << isCond << endl;
        initConduites();
    }
    
    /*!
     * \brief Lit le fichier "turbines.csv" et initalise les tubines
     */
    void readTurbines(int & _compt) {
        string filename = directory+"Turbine/turbines.csv";
        bool isTurb = readDataFile(filename);
        if (isTurb)
            initTurbines(_compt);  
    }
    
    /*!
     * \brief Lit le fichier "pompes.csv" et initalise les pompes
     */
    void readPompes(int & _compt) {
        string filename = directory+"Pompe/pompes.csv";
        // cout << "readPompes" << endl;
        bool isPomp = readDataFile(filename);
        // cout << boolalpha << isPomp << endl;
        if (isPomp)
        {
            // cout<< "prob"<<endl;
            initPompes(_compt);
        }
    }

    /*!
     * \brief Lit le fichier "reservoirs.csv" et initalise les réservoirs
     */
    void readReservoirs(int & _compt) {
        string filename = directory+"Reservoir/reservoirs.csv";
        readDataFile(filename);
        initReservoirs(_compt);  
    }

    /*!
     * \brief Lit le fichier "dataPrix.csv" et initalise les prix des turbines et des pompes
     */
    void readPrix()
    {
        string filename = directory+"Prix/dataPrix.csv";
        readDataFile(filename);
        initPrix();

    }

    /*!
     * \brief Lit toutes les données et initialise le système
     */
    void readAllFiles() {
        int compt = 0;
        readPrix();
        readReservoirs(compt);
        readTurbines(compt);
        readPompes(compt);
        readConduites();

        creerLiens();
        creerAdjacents();
        ajoutLiaisonFictive(sys);
        sortLiaison(sys);
    }
};

#endif
