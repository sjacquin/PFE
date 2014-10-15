#ifndef UTILSH
#define UTILSH


#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/strong_components.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>

#include "Systeme.h"
//#include "ComposanteConnexe.h"

/*!
 * \struct VertexProperties
 *
 * \brief Structure gérant les sommets d'un graphe
 */
struct VertexProperties
{
   std::string name;/*!< Nom du sommet*/
   unsigned id;/*!< Identifiant du sommet*/

   /*!
    * \brief Constructeur à partir de données
    *
    * \param _n : nom du sommet
    * \param _i : identifiant du sommet
    */ 
   VertexProperties(std::string const& _n, unsigned _i) : name(_n), id(_i) {}

   /*!
    * \brief Construteur par défaut
    */
   VertexProperties() : name(""), id(0) {}
};

/*!
 * \struct EdgeProperties
 *
 * \brief Structure qui gère les arêtes d'un graphe
 */
struct EdgeProperties
{
    float qmin;/*!< Débit minimum */
    float qmax;/*!< Débit maximum */
    float deltaLo;/*!< Variation minimum du débit */
    float deltaUp;/*!< Variation maximum du débit */
    int distance;/*!< Temps de parcours entre les extrémités */

    /*!
     * \brief Constructeur par défaut
     */
    EdgeProperties() : qmin(0.0), qmax(0.0), deltaLo(0.0), deltaUp(0.0), distance(0) {}

    /*!
     * \brief Constructeur par données
     *
     * \param _qmin : float, débit minimum
     * \param _qmax : float, débit maximum
     * \param _deltaLo : float, variation minimum du débit
     * \param _deltaUp : float, variation maximum du débit
     * \param _distance : int, temps de parcours de l'arête
     */
    EdgeProperties(float _qmin, float _qmax, float _deltaLo, float _deltaUp, float _dist) : qmin(_qmin), qmax(_qmax), deltaLo(_deltaLo), deltaUp(_deltaUp), distance(_dist) {}
};

/*! 
 * \struct graphProperties
 *
 * \brief Structure qui gère les graphes
 */
struct graphProperties {};

/*!
 * \typedef graph_t
 *
 * \brief Graphe orienté associé aux structures VertexProperties, EdgeProperties et graphProperties dans lequel les sommets et les arêtes sont représentés par des vecteurs
 */
typedef boost::adjacency_list<
   boost::vecS, boost::vecS, boost::directedS,
   boost::property<boost::vertex_bundle_t, VertexProperties>,
   boost::property<boost::edge_bundle_t, EdgeProperties>,
   boost::property<boost::graph_bundle_t, graphProperties>
> graph_t;
   
/*!
 * \typedef vertex_t
 *
 * \brief Sommets associés au graphe graph_t
 */ 
typedef boost::graph_traits<graph_t>::vertex_descriptor vertex_t;

/*!
 * \typedef vertex_iterator_t
 *
 * \brief Itérateur sur les sommets du graphe
 */
typedef boost::graph_traits<graph_t>::vertex_iterator vertex_iterator_t;

/*!
 * \typedef edge_t
 *
 * \brief Arêtes associées au graphe graph_t
 */
typedef boost::graph_traits<graph_t>::edge_descriptor edge_t;

/*!
 * \typedef edge_iterator_t
 *
 * \brief Itérateur sur les arêtes du graphe
 */
typedef boost::graph_traits<graph_t>::edge_iterator edge_iterator_t;

/*!
 * \typedef out_edge_iterator_t
 *
 * \brief Itérateur sur les arêtes sortantes du graphe
 */
typedef boost::graph_traits<graph_t>::out_edge_iterator out_edge_iterator_t;



// Graphe du Xover
typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> Traits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS,
    boost::property<boost::vertex_name_t, int>,
    boost::property<boost::edge_name_t, int,
    boost::property<boost::edge_weight_t, double,
    boost::property<boost::edge_capacity_t, double,
    boost::property<boost::edge_residual_capacity_t, double,
    boost::property<boost::edge_reverse_t, Traits::edge_descriptor > > > > > >Graphe;

typedef boost::graph_traits<Graphe>::vertex_descriptor Sommet;
typedef boost::graph_traits<Graphe>::edge_descriptor Arete;

// Graphe d'ecart
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS,
    boost::property<boost::vertex_name_t, int,
    boost::property<boost::vertex_distance_t, double> >,
    boost::property<boost::edge_name_t, int,
    boost::property<boost::edge_weight_t, double > > >Graphe_e;

typedef boost::graph_traits<Graphe_e>::vertex_descriptor Sommet_e;
typedef boost::graph_traits<Graphe_e>::edge_descriptor Arete_e;


void makeGrapheEcart(Graphe & G,
    boost::property_map<Graphe,boost::edge_weight_t>::type & weight,
    boost::property_map<Graphe,boost::edge_capacity_t>::type & capacity,
    boost::property_map<Graphe,boost::edge_residual_capacity_t>::type & residual_capacity,
    boost::property_map<Graphe,boost::vertex_name_t>::type & id,
    boost::property_map<Graphe,boost::edge_name_t>::type & e_liaison,
    Graphe_e & g_e,
    boost::property_map<Graphe_e,boost::vertex_name_t>::type & id_e,
    boost::property_map<Graphe_e,boost::vertex_distance_t>::type & distance_e,
    boost::property_map<Graphe_e,boost::edge_weight_t>::type & weight_e,
    boost::property_map<Graphe_e,boost::edge_name_t>::type & name_e)
{
    // cout << "entrée makeGrapheEcart" << endl;
    boost::graph_traits<Graphe>::vertex_iterator u_i, u_end;
    boost::graph_traits<Graphe>::vertex_iterator v_i, v_end;
    for (boost::tie(u_i,u_end)=boost::vertices(G); u_i != u_end; u_i++)
    {
        Sommet_e s_tmp = boost::add_vertex(g_e);
        id_e[s_tmp] = id[*u_i];
        if (id[*u_i]==0)
            distance_e[s_tmp] = 0;
        else
            distance_e[s_tmp] = FLT_MAX;
    }
    // cout << "sommets créés : " << boost::num_vertices(g_e) << endl;
    boost::graph_traits<Graphe>::edge_iterator e_i, e_end;
    int compt = 0;
    for (boost::tie(e_i,e_end)=boost::edges(G); e_i != e_end; e_i++, compt++)
    {
        if (residual_capacity[*e_i]>0)
        {
           //  cout << "arc non saturé" << endl;
           // cout << boost::source(*e_i,G) << " -> " << boost::target(*e_i,G) << " " << residual_capacity[*e_i] << " " << capacity[*e_i] << endl;
            bool b;
            Arete_e e_tmp;
            Sommet u = boost::source(*e_i,G);
            Sommet v = boost::target(*e_i,G);

            boost::tie(u_i,u_end)=boost::vertices(G);
            while (u_i != u_end && id_e[*u_i] != id[u])
                u_i++;
            if (u_i==u_end)
            {
                cerr << "Problème graphe auxiliaire" << endl;
                throw "error";
            }
            Sommet_e u_e = *u_i;

            boost::tie(v_i,v_end)=boost::vertices(G);
            while (v_i != v_end && id_e[*v_i] != id[v])
                v_i++;
            if (v_i==v_end)
            {
                cerr << "Problème graphe auxiliaire" << endl;
                throw "error";
            }
            Sommet_e v_e = *v_i;

            boost::tie(e_tmp,b) = boost::add_edge(u_e, v_e, g_e);
            name_e[e_tmp] = 2*compt;
            weight_e[e_tmp] = weight[*e_i];
            // cout << boost::source(e_tmp,g_e) << " => " << boost::target(e_tmp,g_e) << endl;
        }

        if (residual_capacity[*e_i]<capacity[*e_i])
        {
            // cout << "flot positif" << endl;
            // cout << boost::source(*e_i,G) << " -> " << boost::target(*e_i,G) << " " << residual_capacity[*e_i] << " " << capacity[*e_i] << endl;
            bool b;
            Arete_e e_tmp;
            Sommet v = boost::source(*e_i,G);
            Sommet u = boost::target(*e_i,G);

            boost::tie(u_i,u_end)=boost::vertices(G);
            while (u_i != u_end && id_e[*u_i] != id[u])
                u_i++;
            if (u_i==u_end)
            {
                cerr << "Problème graphe auxiliaire" << endl;
                throw "error";
            }
            Sommet_e u_e = *u_i;

            boost::tie(v_i,v_end)=boost::vertices(G);
            while (v_i != v_end && id_e[*v_i] != id[v])
                v_i++;
            if (v_i==v_end)
            {
                cerr << "Problème graphe auxiliaire" << endl;
                throw "error";
            }
            Sommet_e v_e = *v_i;

            boost::tie(e_tmp,b) = boost::add_edge(u_e, v_e, g_e);
            name_e[e_tmp] = 2*compt+1;
            weight_e[e_tmp] = -weight[*e_i];
            // cout << boost::source(e_tmp,g_e) << " => " << boost::target(e_tmp,g_e) << endl;
        }
    }
    // cout << "arcs créés : " << boost::num_edges(g_e) << endl;
    // cout << "sortie makeGrapheEcart" << endl;
}

bool BellManFord(Graphe_e & g_e,
    boost::property_map<Graphe_e,boost::vertex_name_t>::type & id_e,
    boost::property_map<Graphe_e,boost::vertex_distance_t>::type & distance_e,
    boost::property_map<Graphe_e,boost::edge_weight_t>::type & weight_e,
    boost::property_map<Graphe_e,boost::edge_name_t>::type & name_e,
    int & N, vector<int> & parent, vector<int> & aretes)
{
    // cout << "entrée BellManFord" << endl;
    boost::graph_traits<Graphe_e>::edge_iterator e_i, e_end;
    boost::graph_traits<Graphe_e>::edge_iterator* e_ret=NULL;
    for (int i=1; i<N; i++)
    {
        for (boost::tie(e_i,e_end)=boost::edges(g_e); e_i != e_end; e_i++)
        {
            Sommet_e u = boost::source(*e_i,g_e);
            Sommet_e v = boost::target(*e_i,g_e);
            if (weight_e[*e_i] + distance_e[u] < distance_e[v])
            {
                distance_e[v] = weight_e[*e_i] + distance_e[u];
                parent[id_e[v]] = id_e[u];
                aretes[id_e[v]] = name_e[*e_i];
            }
        }
    }
    boost::graph_traits<Graphe>::vertex_iterator u_i,u_end;
    boost::tie(u_i,u_end)=boost::vertices(g_e);
    bool find = (distance_e[*(u_end-1)]<FLT_MAX);
    // for (boost::tie(e_i,e_end)=boost::edges(g_e); !find && e_i != e_end; e_i++)
    // {
    //     Sommet_e u = boost::source(*e_i,g_e);
    //     Sommet_e v = boost::target(*e_i,g_e);
    //     if (weight_e[*e_i] + distance_e[u] < distance_e[v])
    //         find = false;
    // }
    // cout << "sortie BellManFord " << boolalpha << find << endl;
    return find;
}

void modificationFlot(Graphe & G,
    boost::property_map<Graphe,boost::edge_weight_t>::type & weight,
    boost::property_map<Graphe,boost::edge_capacity_t>::type & capacity,
    boost::property_map<Graphe,boost::edge_residual_capacity_t>::type & residual_capacity,
    boost::property_map<Graphe,boost::vertex_name_t>::type & id,
    boost::property_map<Graphe,boost::edge_name_t>::type & e_liaison,
    Graphe_e & g_e,
    boost::property_map<Graphe_e,boost::vertex_name_t>::type & id_e,
    boost::property_map<Graphe_e,boost::vertex_distance_t>::type & distance_e,
    boost::property_map<Graphe_e,boost::edge_weight_t>::type & weight_e,
    boost::property_map<Graphe_e,boost::edge_name_t>::type & name_e,
    int & N, vector<int> & parent, vector<int> & aretes)
{
    // cout << "entrée modification flot" << endl;
    boost::graph_traits<Graphe>::edge_iterator e_i,e_end,e_i_tmp;

    boost::graph_traits<Graphe_e>::edge_iterator e_e_i,e_e_end;
    
    boost::graph_traits<Graphe_e>::vertex_iterator u_i,u_end;
    boost::tie(u_i,u_end)=boost::vertices(g_e);
    int id_parent =id_e[*(u_end-1)];

    vector<int> aretes_to_modif;
    vector<bool> addOrSub;
    double flot = 0.0;
    double flow_wanted = 0.0;
    boost::graph_traits<Graphe>::out_edge_iterator o_i,o_end;
    for (boost::tie(o_i,o_end)=boost::out_edges(*u_i,G); o_i != o_end; o_i++)
    {
        flot += capacity[*o_i] - residual_capacity[*o_i];
        flow_wanted += capacity[*o_i];
    }
    double max_variation = flow_wanted-flot;
    while(id_parent != id_e[*u_i])
    {  
        // cout << id_parent << " " << aretes[id_parent] << endl;
        for(boost::tie(e_e_i,e_e_end)=boost::edges(g_e);e_e_i != e_e_end && name_e[*e_e_i] != aretes[id_parent]; e_e_i++);
        // cout << boost::source(*e_e_i,g_e) << " == > " << boost::target(*e_e_i,g_e) << endl;
        int a = aretes[id_parent]/2;
        aretes_to_modif.push_back(a); 
        boost::tie(e_i,e_end)=boost::edges(G);
        e_i_tmp = e_i;
        for (int i=0; i<a; i++,e_i_tmp++);
        Arete e_tmp = *e_i_tmp;
        if (aretes[id_parent] % 2 == 0)
        {
            max_variation = min(max_variation,residual_capacity[e_tmp]);
            addOrSub.push_back(true);
        }
        else
        {
            max_variation = min(max_variation,capacity[e_tmp]-residual_capacity[e_tmp]);
            addOrSub.push_back(false);
        }
        id_parent = parent[id_parent];

    }
    // cout << "max_variation : " << max_variation << endl;

    for (int i=0; i<aretes_to_modif.size(); i++) 
    {
        boost::tie(e_i,e_end)=boost::edges(G);
        e_i_tmp = e_i;
        for (int j=0; j<aretes_to_modif[i]; j++, e_i_tmp++);
        Arete e_tmp = *(e_i_tmp);
        // cout << boost::source(e_tmp,G) << " -> " << boost::target(e_tmp,G) << endl;
        if (addOrSub[i])
            residual_capacity[e_tmp] -= max_variation;
        else
            residual_capacity[e_tmp] += max_variation;
    }
    // cout << "sortie modification flot" << endl;
}

/*!
 * \brief Détermine si un entier donné est ddans un vecteur d'entier donné
 *
 * \param _list : vecteur d'entier
 * \param _toSerach : entier à tester
 *
 * \return true si _toSearch appartient à _list, false sinon
 */
bool isInVectInt(const vector<int> & _list, const int & _toSearch)
{
	int test = 0;
	for (int i=0; i<_list.size(); i++)
	{
		if (_list[i]==_toSearch)
			test++;
	}

	return (test!=0);
} 

/*!
 * \brief Met à jour les propriétés d'une arête en fonction d'une autre
 *
 * \param _e_to_change : EdgeProperties, arête à modifier
 * \param _e_necessary : EdgeProperties, arête de référence
 */
void miseAjourEdge(EdgeProperties & _e_to_change, const EdgeProperties & _e_neccesary)
{
    // MAJ qmin
    if (_e_to_change.qmin < _e_neccesary.qmin)
        _e_to_change.qmin = _e_neccesary.qmin;
    
    //MAJ qmax
    if (_e_to_change.qmax > _e_neccesary.qmax)
        _e_to_change.qmax = _e_neccesary.qmax;

    //MAJ deltaLO
    if (_e_to_change.deltaLo < _e_neccesary.deltaLo)
        _e_to_change.deltaLo = _e_neccesary.deltaLo;
    
    //MAJ deltaUP
    if (_e_to_change.deltaUp > _e_neccesary.deltaUp)
        _e_to_change.deltaUp = _e_neccesary.deltaUp;

    //MAJ distance
    _e_to_change.distance += _e_neccesary.distance;

}

/*!
 * \brief Ajoute une composante connexe d'un système contenant un certain réservoir dans un vecteur de composantes connexes donnés
 *
 * \param sys : Systeme
 * \param _comps : vecteur de ComoosanteConnexe
 * \param r : entier, identifiant du réservoir
 */
void ajoutComp(Systeme & sys, vector<ComposanteConnexe> & _comps, const int & r)
{
    for (int i=0; i<sys.getNbComposantesConnexes(); i++)
    {
        ComposanteConnexe & C = sys.getComposanteConnexe(i);
        if (C.in(r))
            _comps.push_back(C);
    }
}

/*!
 * \brief Ordonne les composantes connexes d'un système de la plus en amont à la plus en aval
 *
 * \param sys : Systeme
 */
void ordonnerComposanteConnexe(Systeme & sys)
{
    vector<ComposanteConnexe> vec_comp_tmp;
    
    for (int i=0; i<sys.getNbComposantesConnexes(); i++)
        vec_comp_tmp.push_back(sys.getComposanteConnexe(i));

    
    int j=0;
    for (int i=0; i< sys.getNbReservoirs(); i++)
    {
        int comp = 0;
        ComposanteConnexe C = vec_comp_tmp[comp];
        while(!C.in(i))
        {
            comp++;
            C = vec_comp_tmp[comp];
        }
        for (int k=1; k<C.getNbReservoirs(); k++)
            i++;

        sys.setComposanteConnexe(j,C);
        j++;
    }
}

int getIdSommet(int t_initial, int t_final, int res, int t)
{
    return (t_final-t_initial)*res + 1 + t - t_initial;
}

void ajoutArc(Graphe & G, boost::property_map<Graphe,boost::edge_name_t>::type & e_liaison, int numL, boost::property_map<Graphe,boost::edge_weight_t>::type & weight, boost::property_map<Graphe,boost::edge_capacity_t>::type & capacity, boost::property_map<Graphe,boost::edge_residual_capacity_t>::type & residual_capacity, boost::property_map<Graphe,boost::edge_reverse_t>::type & rev, double cap, double w, Sommet source, Sommet dest)
{
    bool b1, b2;
    Arete e1, e2;
    boost::tie(e1,b1) = boost::add_edge(source,dest,G);
    
    e_liaison[e1] = numL;
    capacity[e1] = cap;
    weight[e1] = w;
    residual_capacity[e1] = cap;
}


/*!
 * \brief Ajoute une liaison fictive dans un système
 *
 * \param sys : Systeme
 */
void ajoutLiaisonFictive(Systeme & sys)
{
    vector<Liaison> LtoAdd;
    for (int i=0; i<sys.getNbLiaisons(); i++)
    {
        Liaison & L_tmp = sys.getLiaison(i);
        if (L_tmp.getMinTpsConstant() != 0)
        {
            cout << "Ajout liaison minTpsConstant "<< endl;
            Lien l_tmp(0.0,0.0);
            vector<Lien> l_v;
            l_v.push_back(l_tmp);
            Liaison addL(L_tmp.getReservoirIni(),L_tmp.getReservoirFin(),1,l_v,L_tmp.getLongueur(),0.0,0.0,0);
            sys.getReservoir(L_tmp.getReservoirIni()).adSortie(addL.getNumero());
            sys.getReservoir(L_tmp.getReservoirFin()).adEntree(addL.getNumero());
            LtoAdd.push_back(addL);
        }
        else if (L_tmp.contrainteDincompatibilite())
        {
            bool ajoute = false;
            for (int j=0; j<L_tmp.getNbLiens(); j++)
            {
                Lien & l_tmp = L_tmp.getLien(j);
                for (int k=0; k<l_tmp.getNbPompes(); k++)
                {
                    Pompe* P = l_tmp.getPompe(k);
                    if (P->getIncompatibilites()!=-1)
                    {
                        ajoute = true;
                    }
                }
            }
            if (ajoute == true)
            {
                cout << "Ajout liaison coté pompe "<< endl;
                Lien l_tmp(0.0,0.0);
                vector<Lien> l_v;
                l_v.push_back(l_tmp);
                Liaison addL(L_tmp.getReservoirIni(),L_tmp.getReservoirFin(),1,l_v,L_tmp.getLongueur(),0.0,0.0,0);
                sys.getReservoir(L_tmp.getReservoirIni()).adSortie(addL.getNumero());
                sys.getReservoir(L_tmp.getReservoirFin()).adEntree(addL.getNumero());
                LtoAdd.push_back(addL);
            }
        }
    }
    for (int i=0; i<LtoAdd.size(); i++)
        sys.adLiaison(LtoAdd[i]);
}

/*!
 * \brief Donne le vecetur de toutes les liaisons sortantes d'une composante connnexe donnée
 *
 * \param C : ComposanteConnexe
 *
 * \return un vecteur de pointeurs sur Liaison
 */
vector<Liaison*> getListeLiaisonsSortantes(ComposanteConnexe & C, Systeme & sys)
{
    vector <Liaison*> result;
    int i;
    int nbReservoirs= C.getNbReservoirs();
    vector<int> R=C.getReservoirs();
    for(i=0;i<nbReservoirs;i++)
    {
        int nbSorties=sys.getReservoir(R[i]).getCardSorties();
        int j;
        for(j=0;j<nbSorties;j++)
        {
            int l= sys.getReservoir(R[i]).getSorties()[j];
            if(C.in(sys.getLiaison(l).getReservoirFin())==false)
                result.push_back(&sys.getLiaison(l));
        }
    }

    return result;
}


void sortLiaison(Systeme & sys)
{
    for (int i=0; i<sys.getNbReservoirs(); i++)
    {
        Reservoir & R = sys.getReservoir(i);
        int cardSorties = R.getCardSorties();
        vector<int> & sorties = R.getSorties();
        vector<int> contraintesPuissance(cardSorties,0);
        for (int j=0; j<cardSorties; j++)
        {
            Liaison & L = sys.getLiaison(sorties[j]);
            if (L.getMinTpsConstant())
                contraintesPuissance[j] += 1000;
            if (L.arretProgramme())
                contraintesPuissance[j] += 100;
            if (L.palier())
                contraintesPuissance[j] += 10;
            if (L.contrainteDincompatibilite())
                contraintesPuissance[j] += 1;
        }
        vector<int> copie(contraintesPuissance);
        sort(contraintesPuissance.begin(), contraintesPuissance.end());
        vector<int> newSorties;
        for (int j=cardSorties-1; j>=0; j--)
        {
            int k=0;
            while (k<cardSorties && (isInVectInt(newSorties,sorties[k]) || contraintesPuissance[j]!=copie[k]))
                k++;
            newSorties.push_back(sorties[k]); 
        }
        sorties = newSorties;
    }
}

vector<double> getTotalDebit(vector<vector<vector<double> > > debits, int t, Systeme & sys)
{
    int nbLiaisons = sys.getNbLiaisons();
    vector<double> res(nbLiaisons,0.0);

    for (int i=0; i<nbLiaisons; i++)
    {
        Liaison & L = sys.getLiaison(i);
        int longueurL = L.getLongueur();

        for (int h=0; h<=t; h++)
        {
            res[i] += debits[h][i][longueurL];

        }
    }

    return res;
}

vector<double> getQtotRes(vector<vector<vector<double> > > & debits, int t, int numR, Systeme & sys)
{
    Reservoir & R = sys.getReservoir(numR);
    vector<int> & adjacents = R.getAdjacents();
    int nbAdjacents = adjacents.size();
    cout << "nbAdjacents : " << nbAdjacents << endl;
    vector<double> res(nbAdjacents,0.0);
    for (int i=0; i<R.getCardSorties(); i++)
    {
        Liaison & L = sys.getLiaison(R.getSorties()[i]);
        int place = 0;
        while (adjacents[place]!=L.getReservoirFin())
        {
            place++;
            cout << place << endl;
        }
        for (int h=0; h<t; h++)
            res[place] += debits[h][L.getNumero()][L.getLongueur()];
    }
    return res;
}

vector<int> getNbLiaisonsPourAdj(int numR, Systeme & sys)
{
    Reservoir & R = sys.getReservoir(numR);
    vector<int> adjacents = R.getAdjacents();
    vector<int> res(adjacents.size(),0);
    for (int i=0; i<R.getCardSorties(); i++)
    {
        Liaison & L = sys.getLiaison(R.getSorties()[i]);
        int place=0;
        while (adjacents[place]!=L.getReservoirFin())
            place++;
        res[place]++;
    }
    return res;
}

vector<vector<int> > getLiaisonsPourAdj(int numR, Systeme & sys)
{
    Reservoir & R = sys.getReservoir(numR);
    vector<int> adjacents = R.getAdjacents();
    vector<bool> dejaVu(adjacents.size(),false);

    vector<vector<int> > res;
    for (int card=R.getCardSorties()-1; card>=0; card--)
    {
        Liaison & L = sys.getLiaison(R.getSorties()[card]);
        int place=0;
        while(adjacents[place]!=L.getReservoirFin())
            place++;
        if (dejaVu[place])
        {
            int pos=0;
            while (sys.getLiaison(res[pos][0]).getReservoirFin() != adjacents[place])
                pos++;
            res[pos].push_back(L.getNumero());
        }
        else
        {
            vector<int> res_n(1,L.getNumero());
            res.insert(res.begin(),res_n);
            dejaVu[place]=true;
        }
    }
    return res;
}
/*!
 * \brief Donne le volume d'un reservoir en fonction des débits des liaisons
 *
 * \param debits : debits[i][k][t] donne le debit sortant de la liaison i au temps t+k
 * \param i : identifiant du réservoir
 * \param sys : Système hydroélectrique
 * 
 * \return le vecteur des volumes horaire du réservoir i du systeme
 */
 vector<double> getVolumeHoraire(vector<vector<vector<double> > > & debits, int & i, Systeme & sys)
 {
    Reservoir & R = sys.getReservoir(i);
    vector<int> entrees = R.getEntrees();
    vector<int> sorties = R.getSorties();
    vector<double> ret;
    double Vprec = R.getVinit();
    ret.push_back(Vprec);
    for (int t=1; t<debits.size(); t++)
    {
        double toPush = Vprec;
        for (int k=0; k<entrees.size(); k++)
        {
            toPush += debits[k][0][t];
        }
        for (int k=0; k<sorties.size(); k++)
        {
            toPush -= debits[k][sys.getLiaison(k).getLongueur()][t];
        }
        ret.push_back(toPush);
        Vprec = toPush;
    }    
 }

/*!
 * \brief Détermine si la contrainte des volumes est vérifiée
 *
 * \param volume matrices des volumes des réservoirs dans le temps : volume[t][i] donne le volume du réservoir i à l'instant t
 * \param systeme Système hydroelectrique
 *
 * \return true si pour tous les réservoirs, et à chaque instant, le volume présent est compris entre le volume minimum et le volume maximum, sinon false avec message d'erreur pour savoir quel réservoir a violé la condition et à quel instant
 */
bool checkConstraintOne(const vector<vector<double> > & volume, Systeme & sys,ostream& os=cerr)
{
    int constraintNotRespected = 0;

    // On ne tient pas compte du réservoir final
    for (int i=0; i<sys.getNbReservoirs()-1; i++)
    {
    	
        Reservoir & R = sys.getReservoir(i);
        double Vmax = R.getVmax();
        for (int t=0; t<volume.size(); t++)
        {
            if (volume[t][i] > Vmax+0.00005 || volume[t][i] < R.getRealVmin(t)-0.0001)
            {
                constraintNotRespected++;
               os << "Contraintes des volumes non respectée pour le réservoir " << i << " au temps " << t << " : " << endl;
                os << "Vmin : " << R.getRealVmin(t) << "; volume : " << volume[t][i] << "; Vmax : " << Vmax << endl;
                return false;
            }
        }
    }

    return (constraintNotRespected==0);
}

bool checkConstraintTwo(vector<vector<double> > & volume, Systeme & sys,ostream& os=cerr)
{
    int constraintNotRespected = 0;
    for (int i=0; i<sys.getNbReservoirs()-1; i++)
    {
        Reservoir & R = sys.getReservoir(i);
        if (abs(volume[volume.size()-1][i]-R.getVinit())>0.001)
        {
            constraintNotRespected++;
            os << "Contrainte de volume final non respectée pour le réservoir " << i << " : " << endl;
            os << "Volume final : " << volume[volume.size()-1][i] << "; Volume attendu : " << R.getVinit() << endl;
        }
    }
    return (constraintNotRespected==0);
}

bool checkConstraintFour(vector<vector<vector<double> > > & debits, Systeme & sys,ostream& os=cerr)
{
    int constraintNotRespected = 0;
    for (int i=0; i<sys.getNbLiaisons(); i++)
    {
        Liaison & L = sys.getLiaison(i);
        int longueur = L.getLongueur();
        for (int t=1; t<debits.size(); t++)
        {
            if (debits[t][i][longueur]-debits[t-1][i][longueur]<-L.getDeltaLo()-0.00001)
            {
                constraintNotRespected++;
                os << "Contrainte de variation minimum de débit non respectée pour la liaison " << i << " : " << endl;
                os << "Variation obtenue : " << debits[t][i][longueur]-debits[t-1][i][longueur] << "; Variation minimum attendue : " << -L.getDeltaLo() << endl;
            }
            if (debits[t][i][longueur]-debits[t-1][i][longueur]>L.getDeltaUp()+0.00001)
            {
                constraintNotRespected++;
                os << "Contrainte de variation maximum de débit non respectée pour la liaison " << i << " : " << endl;
                os << "Variation obtenue : " << debits[t][i][longueur]-debits[t-1][i][longueur] << "; Variation maximum attendue : " << L.getDeltaUp() << endl;
            }
        }
    }
    return (constraintNotRespected==0);
}
bool checkDebitMinMax(vector<vector<vector<double> > > & debits, Systeme & sys,vector<vector<double> > & volume,ostream& os=cerr)
{
	bool reponse=true;
	for(int t=0; t<debits.size();t++)
	{
		for(int i=0;i<sys.getNbLiaisons();i++)
		{
	        Liaison & L = sys.getLiaison(i);
	        int longueur = L.getLongueur();
	        if(debits[t][i][longueur]<L.getQmin()-0.00001)
	        {
	        	reponse=false;
	        	os<<"contrainte de débit minimal non respecté pour la liaison : "<<i<<" au temps "<< t<<endl;
	        	os<<" le débit est de :"<<debits[t][i][longueur]<<" alors que le débit minimal est de : "<<L.getQmin()<<endl;
	        }
	        else
	        {
	        	double qTot=debits[t][i][longueur]-L.getQmin();
	        	for(int l=0;l<L.getNbLiens();l++)
	        	{
	        		Lien lien=L.getLien(l);
	        		if(lien.getNbTurbines()>0)
	        		{
	        			double qte=qTot+lien.getQmin();
	        			int dist=lien.getTurbine(0)->getDistance();
	        			if (t-dist>=0 && qte>lien.getQminVol(volume[t-dist][L.getReservoirIni()]))
	        			{
	        				if(qte<lien.getQmax(volume[t-dist][L.getReservoirIni()]))
	        				{
	        					qTot=0;
	        				}
	        				else
	        				{
	        					qTot+=-lien.getQmax(volume[t-dist][L.getReservoirIni()])+lien.getQmin();
	        				}
	        			}

	        		}
	        		else if(lien.getNbPompes()>0)
	        		{
	        			double qte=qTot+lien.getQmin();
	        			int dist=lien.getPompe(0)->getDistance();
	        			if (t-dist>=0&& qte>lien.getQminVol(volume[t-dist][L.getReservoirIni()]))
	        			{
	        				if(qte<lien.getQmax(volume[t-dist][L.getReservoirIni()]))
	        				{
	        					qTot=0;
	        				}
	        				else
	        				{
	        					qTot+=-lien.getQmax(volume[t-dist][L.getReservoirIni()])+lien.getQmin();
	        				}
	        			}

	        		}
        			else
        			{
        				if(qTot<lien.getQmaxBasique()-lien.getQmin())
        					qTot=0;
        				else
        				{
        					qTot=qTot-(lien.getQmaxBasique()-lien.getQmin());
        				}
        			}
	        	}
	        	if(qTot>0.00001)
	        	{
	        		reponse=false;
	        		os<<"contrainte de débit max non respectée pour la liaison "<<i<<" au temps t="<<t<<endl;
	        		os<<"diff=" <<qTot<<endl;
	        	}
	        }
		}
	}
	return reponse;
}

bool checkConstraintSix(vector<vector<vector<double> > > & debits, Systeme & sys,ostream& os=cerr)
{
    int constraintNotRespected = 0;
    for (int i=0; i<sys.getNbLiaisons(); i++)
    {
        Liaison & L = sys.getLiaison(i);
        int longueurL = L.getLongueur();
        for (int t=0; t<debits.size(); t++)
        {
            if (L.enArret(t))
            {
                for (int j=0; j<L.getNbLiens(); j++)
                {
                    Lien & l = L.getLien(j);
                    for (int k=0; k<l.getNbTurbines(); k++)
                    {
                        Turbine* T = l.getTurbine(k);
                        int longueur = T->getDistance();
                        if (T->estEnArret(t) && debits[t][i][longueurL-longueur])
                        {
                            constraintNotRespected++;
                            os << "Contrainte d'arrêt programmé non respectée pour la turbine " << T->getName() << " à l'instant " << t <<" dist="<<longueur<< endl;
                        }
                    }

                    for (int k=0; k<l.getNbPompes(); k++)
                    {
                        Pompe* P = l.getPompe(k);
                        int longueur = P->getDistance();
                        if (P->estEnArret(t) && debits[t][i][longueurL-longueur])
                        {
                            constraintNotRespected++;
                            os << "Contrainte d'arrêt programmé non respectée pour la Pompe " << P->getName() << " à l'instant " << t << " dist="<<longueur<< " deb="<<debits[t][i][longueurL-longueur]<<endl;
                        }
                    }
                }
            }
        }
    }
    return (constraintNotRespected==0);
}

bool checkConstraintNine(vector<vector<vector<double> > > & debits, Systeme & sys,ostream& os=cerr)
{
    int constraintNotRespected = 0;
    for (int j=0; j< sys.getNbLiaisons(); j++)
    {
        Liaison & L = sys.getLiaison(j);
        int longueurL = L.getLongueur();
        int minTpsConstant = L.getMinTpsConstant();
        if (minTpsConstant!=0)
        {
            int t=1;
            int t_prec=0;
            while(t<debits.size())
            {
                while (t<debits.size() && debits[t][j][longueurL]<=debits[t_prec][j][longueurL]+0.000001 &&debits[t][j][longueurL]>=debits[t_prec][j][longueurL]-0.000001)
                {
                    // cout << debits[t][j][longueurL] << endl;
                    t++;
                }
                if (t<debits.size() &&(t_prec !=0 || (t_prec==0 && debits[t_prec][j][longueurL]!=0))&& t-t_prec < minTpsConstant)
                {
                    constraintNotRespected++;
                    os << "Contrainte de temps minimal de fonctionnement constant non respectée : " << endl;
                    os << "La liaison " << j << " est en arret de " << t_prec << " à " << t-1 << " alors qu'elle devrait attendre au moins " << minTpsConstant << endl;
                    os << "Débit à " << t_prec << " : " << debits[t_prec][j][longueurL] << " et " << "Débit à " << t << " : " << debits[t][j][longueurL] << " et à " <<t_prec -1 << " : " << debits[t_prec-1][j][longueurL]<<endl;
                    os << endl;
                }
                t_prec = t;
            }
        }
    }
    return (constraintNotRespected==0);
}

bool checkCoherenceTpsMin(vector<vector<vector<double> > > & debits, vector<vector<int> > & tps, Systeme & sys,ostream& os=cerr)
{
    int constraintNotRespected = 0;
    for (int j=0; j< sys.getNbLiaisons(); j++)
    {
        Liaison & L = sys.getLiaison(j);
        int longueurL = L.getLongueur();
        int minTpsConstant = L.getMinTpsConstant();

            int t=1;
            int t_prec=0;
            while(t<debits.size())
            {
                while (t<debits.size() && debits[t][j][longueurL]<=debits[t_prec][j][longueurL]+0.000001 &&debits[t][j][longueurL]>=debits[t_prec][j][longueurL]-0.000001)
                {
                    // cout << debits[t][j][longueurL] << endl;
                    t++;
                }
                if ( t<debits.size() && t-t_prec < tps[t-1][j])
                {
                    constraintNotRespected++;
                    os << "tps constant indiqué incoherent " << endl;
                    os << "La liaison " << j << " est econstante de " << t_prec << " à " << t-1 << " alors qu'elle indique " << tps[t-1][j] << endl;
                    os << "Débit à " << t_prec-1 << " : " << debits[t_prec-1][j][longueurL] << " et "<<"Débit à " << t_prec << " : " << debits[t_prec][j][longueurL] << " et " << "Débit à " << t << " : " << debits[t][j][longueurL]<<" et " << "Débit à " << t+1 << " : " << debits[t+1][j][longueurL]<<endl;
                    os << endl;
                    return false;
                }
                t_prec = t;
            }

    }
    return (constraintNotRespected==0);
}


bool checkConstraintTen(vector<vector<vector<double> > > & debits, Systeme & sys,ostream& os=cerr)
{
    int constraintNotRespected = 0;
    for (int j=0; j<sys.getNbLiaisons(); j++)
    {
        Liaison & L = sys.getLiaison(j);
        int longueurL = L.getLongueur();
        if (L.contrainteDincompatibilite()==true)
        {
            for (int i=0; i<L.getNbLiens(); i++)
            {
                Lien & l = L.getLien(i);
                if (l.contrainteDincompatibilite()==true)
                {
                    for (int k=0; k<l.getNbTurbines(); k++)
                    {
                        Turbine* T = l.getTurbine(k);
                        int longueurT = T->getDistance();
                        int incomp = T->getIncompatibilites();
                        if(incomp!=-1)
                        {
                            Pompe & P = sys.getPompe(incomp);
                            int longueurP = P.getDistance();
                            int numLiaisonIncomp = P.getNumLiaison();
                            int longueurLb = sys.getLiaison(numLiaisonIncomp).getLongueur();
                            for (int t=0; t<debits.size(); t++)
                            {
                                if (debits[t][j][longueurL-longueurT]*debits[t][numLiaisonIncomp][longueurLb-longueurP]>0.0001)
                                {
                                    constraintNotRespected++;
                                    os << "Contrainte d'incompatibilités non respectée : " << endl;
                                    os << "La turbine " << T->getName() << " fonctionne avec une valeur " << debits[t][j][longueurL-longueurT] << " à l'instant " << t << " alors que la pompe " << P.getName() << " fonctionne avec une valeur " << debits[t][numLiaisonIncomp][longueurLb-longueurP] << endl;
					//exit(2);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return (constraintNotRespected==0);
}

bool checkConstraintEleven(vector<vector<vector<double> > > & debits, vector<vector<double> > & volume, Systeme & sys,ostream& os=cerr)
{
    int constraintNotRespected=0;
    for (int j=0; j<sys.getNbTurbines(); j++)
    {
        Turbine & T = sys.getTurbine(j);
        int longueurT = T.getDistance();
        double palier = T.getPalier();
        if (palier!=0)
        {
            //cout << palier << endl;
            int numL = T.getNumLiaison();
            Liaison & L = sys.getLiaison(numL);
            int longueurL = L.getLongueur();
            int resP = T.getReservoirParent();
            int t = 0;
            while(t<debits.size())
            {
                while(t<debits.size() && debits[t][numL][longueurL]<0.00000001)
                    t++;
                //cout << t << endl;
                while(t<debits.size() && debits[t][numL][longueurL]!=0)
                    t++;
                //cout << t << endl;
                if (t!=debits.size() && (T.getProdV(volume[t-2][resP],debits[t-1][numL][longueurL])<palier-0.000001 || T.getProdV(volume[t-2][resP],debits[t-1][numL][longueurL])>palier+0.000001) )
                {
                    constraintNotRespected++;
                    os << "Contrainte de palier non respectée : " << endl;
                    os << "La turbine " << T.getName() << " produit " << T.getProdV(volume[t-2][resP],debits[t-1][numL][longueurL]) << " au lieu de " << palier << " à l'instant " << t-1 <<" débite "<< debits[t-1][numL][longueurL]<<" au lieu de "<<T.getQPalier(volume[t-2][resP])<<" débit suivant: "<<debits[t][numL][longueurL]<<" débit précédent: "<<debits[t-2][numL][longueurL]<<endl;
                }
                //cout<<" t de palier ="<<t<<endl;
            }
        }
    }
    for (int j=0; j<sys.getNbPompes(); j++)
    {
        Pompe & P = sys.getPompe(j);
        int longueurP = P.getDistance();
        double palier = P.getPalier();
        if (palier!=0)
        {
            int numL = P.getNumLiaison();
            Liaison & L = sys.getLiaison(numL);
            int longueurL = L.getLongueur();
            int resP = P.getReservoirParent();
            int t = 0, t_prec = 0;
            while(t<debits.size())
            {
                while(t<debits.size() && debits[t][numL][longueurL-longueurP]==0)
                    t++;
                while(t<debits.size() && debits[t][numL][longueurL-longueurP]!=0)
                    t++;
                if (t!=debits.size() && P.getProdV(volume[t-2][resP],debits[t-1][j][longueurL-longueurP])!=palier)
                {
                    constraintNotRespected++;
                    os << "Contrainte de palier non respectée : " << endl;
                    os << "La pompe " << P.getName() << " produit " << P.getProdV(volume[t-2][resP],debits[t-1][j][longueurL-longueurP]) << " au lieu de " << palier << " à l'instant " << t-1 << endl;
                }
            }
        }
    }
    return (constraintNotRespected==0);
}
bool checkCoherenceVolumes (vector<vector<vector<double> > > & debits, vector<vector<double> > & volume, Systeme & sys,ostream& os=cerr)
{
	int h,i,j;
	int nbReservoirs= sys.getNbReservoirs();
	bool rep=true;
	for(i=0;i<nbReservoirs-1;i++)
	{
		Reservoir R=sys.getReservoir(i);
		//premiere heure:
		double Vinit=R.getVinit();
		double vol= Vinit+ R.getApport(0);
		vector<int> entrees=R.getEntrees();
		int nbE=R.getCardEntrees();
		for(j=0;j<nbE;j++)
		{
			vol+=debits[0][entrees[j]][0];
		}
		int nbS=R.getCardSorties();
		vector<int> sorties= R.getSorties();
		for(j=0;j<nbS;j++)
		{
			int longL=sys.getLiaison(sorties[j]).getLongueur();
			vol-=debits[0][sorties[j]][longL];
		}
		if(vol< volume[0][i]-0.0001 || vol>volume[0][i]+0.0001)
		{
			os<<" volume incoherent à l'heure 0 pour le reservoir "<<i<<" vol="<<volume[0][i]<<" alors que par calcul on a :"<<vol<<" dif ="<<vol-volume[0][i]<<endl;
			//if(vol-volume[0][i]>1)exit(1);
			rep=false;
			//return rep;
		}
		//autres heures :
		for(h=1;h<debits.size();h++)
		{
			double vol= volume[h-1][i]+ R.getApport(h);
					vector<int> entrees=R.getEntrees();
					int nbE=R.getCardEntrees();
					for(j=0;j<nbE;j++)
					{
						vol+=debits[h][entrees[j]][0];
					}
					int nbS=R.getCardSorties();
					vector<int> sorties= R.getSorties();
					for(j=0;j<nbS;j++)
					{
						int longL=sys.getLiaison(sorties[j]).getLongueur();
						vol-=debits[h][sorties[j]][longL];
					}
					if(vol< volume[h][i]-0.00001 || vol>volume[h][i]+0.00001)
					{
						os<<" volume incoherent à l'heure "<<h<<" pour le reservoir "<<i<<" vol="<<volume[h][i]<<" alors que par calcul on a :"<<vol<<" dif ="<<vol-volume[h][i]<<endl;
						//if(vol-volume[h][i]>1)exit(1);
						rep=false;
						return rep;
					}
		}
		
	}
	return rep;
}

bool checkAllConstraintsHaveToMeet(vector<vector<vector<double> > > & debits, vector<vector<double> > & volume, Systeme & sys)
{
    bool constraintOne = checkConstraintOne(volume,sys);

    bool constraintTwo = checkConstraintTwo(volume,sys);

    //bool constraintFour = checkConstraintFour(debits,sys);
    bool constraintSix = checkConstraintSix(debits,sys);

    bool constraintNine = checkConstraintNine(debits,sys);

    bool constraintTen = checkConstraintTen(debits,sys);

    bool constraintEleven = checkConstraintEleven(debits,volume,sys);

    bool coherence=checkCoherenceVolumes (debits, volume, sys);

    //return false;
    return (constraintOne && constraintTwo /*&& constraintFour */ && constraintSix && constraintNine && constraintTen && constraintEleven && coherence);
}

bool checkAllConstraints(vector<vector<vector<double> > > & debits, vector<vector<double> > & volume, Systeme & sys,ostream& os=cerr)
{
    bool constraintOne = checkConstraintOne(volume,sys,os);

    bool constraintTwo = checkConstraintTwo(volume,sys,os);

    bool constraintFour = checkConstraintFour(debits,sys,os);
    bool constraintSix = checkConstraintSix(debits,sys,os);

    bool constraintNine = checkConstraintNine(debits,sys,os);

    bool constraintTen = checkConstraintTen(debits,sys,os);

    bool constraintEleven = checkConstraintEleven(debits,volume,sys,os);

    bool coherence=checkCoherenceVolumes (debits, volume, sys,os);
    bool minMax=checkDebitMinMax(debits,  sys, volume,os);


    //return false;
    return (constraintOne && constraintTwo && constraintFour  && constraintSix && constraintNine && constraintTen && constraintEleven && coherence && minMax);
}

#endif
