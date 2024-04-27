#include "Utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include "Eigen/Eigen"

using namespace std;
using namespace Eigen;

namespace PolygonalLibrary {
bool ImportMesh(PolygonalMesh& mesh)
{

    if(!ImportCell0Ds(mesh))      //Se questa restituisce un False, anche l'importo dell'intera mesh fallisce.
    {
        return false;
    }
    else
    {
        cout << "Cell0D marker:" << endl;
        for(auto it = mesh.Cell0DMarkers.begin(); it != mesh.Cell0DMarkers.end(); it++)
        {         //Per ogni marker mi stampo la coppia chiave-valore.
            cout << "key:\t" << it -> first << "\t values:";     //Con first accedo alla chiave
            for(const unsigned int id : it -> second)     //Con second accedo al valore
                cout << "\t" << id;

            cout << endl;
        }
    }

    if(!ImportCell1Ds(mesh))
    {
        return false;
    }
    else
    {
        cout << "Cell1D marker:" << endl;
        for(auto it = mesh.Cell1DMarkers.begin(); it != mesh.Cell1DMarkers.end(); it++)
        {
            cout << "key:\t" << it -> first << "\t values:";
            for(const unsigned int id : it -> second)
                cout << "\t" << id;

            cout << endl;
        }
    }

    if(!ImportCell2Ds(mesh))
    {
        return false;
    }
    else
    {
        // Test:
        for(unsigned int c = 0; c < mesh.NumberCell2D; c++)     //Considero tutti i poligoni in Cell2D
        {
            vector<unsigned int> edges = mesh.Cell2DEdges[c];     //Considero tutti i poligoni (insiemi di lati) in Cell2D

            for(unsigned int e = 0; e < edges.size(); e++)
            {
                const unsigned int origin = mesh.Cell1DVertices[edges[e]][0];      //id punto in cui ha origine il primo lato secondo 1D  //edges[e] è l'indice globale di mesh
                const unsigned int end = mesh.Cell1DVertices[edges[e]][1];     //id punto in cui termina il primo lato secondo 1D

                auto findOrigin = find(mesh.Cell2DVertices[c].begin(), mesh.Cell2DVertices[c].end(), origin);      //cerca l'origine, se lo trova mi restituisce un iterator a quell'elemento, altrimenti end. (?)
                if(findOrigin == mesh.Cell2DVertices[c].end())
                {
                    cerr << "Wrong mesh" << endl;
                    return 2;
                }

                auto findEnd = find(mesh.Cell2DVertices[c].begin(), mesh.Cell2DVertices[c].end(), end);
                if(findEnd == mesh.Cell2DVertices[c].end())
                {
                    cerr << "Wrong mesh" << endl;
                    return 3;
                }

            }
        }
    }

    return true;

}
// ***************************************************************************
bool ImportCell0Ds(PolygonalMesh& mesh)
{

    ifstream file;
    file.open("./Cell0Ds.csv");   //Nome del file standard, per qullo lo scrivo solo qui

    if(file.fail())
        return false;

    list<string> listLines;       //Salvo le righe del file in una lista di stringhe
    string line;
    while (getline(file, line))
        listLines.push_back(line);      //Uso un push_back per aggiunger ein coda alla lista

    file.close();

    listLines.pop_front();

    mesh.NumberCell0D = listLines.size();     //Ottengo il numero di celle 0D che sono andata a leggere

    if (mesh.NumberCell0D == 0)
    {
        cerr << "There is no cell 0D" << endl;
        return false;
    }

    mesh.Cell0DId.resize(mesh.NumberCell0D);
    mesh.Cell0DCoordinates.resize(mesh.NumberCell0D);

    for (const string& line : listLines)
    {
        istringstream converter(line);

        unsigned int id;
        unsigned int marker;
        Vector2d coord;

        converter >>  id >> marker >> coord(0) >> coord(1);     //Mi salvo le due cordinate. Prima la coordinata x e poi la coordinata y.

        mesh.Cell0DId.push_back(id);    //Con un push_back inserisco il mio id
        mesh.Cell0DCoordinates.push_back(coord);    //Trovo le mie coordinate e le inserisco

        //I marker mi identificano la posizione dei vertici. ES. 0:Punti interni, 1:Angolo in basso a dx,...
        if( marker != 0)
        {
            if (mesh.Cell0DMarkers.find(marker) == mesh.Cell0DMarkers.end())
                mesh.Cell0DMarkers.insert({marker, {id}});
            else      //Se la chiave era stata aggiunta in precedenza
                mesh.Cell0DMarkers[marker].push_back(id);
        }

    }
    file.close();
    return true;
}

// ***************************************************************************
bool ImportCell1Ds(PolygonalMesh& mesh)
{

    ifstream file;
    file.open("./Cell1Ds.csv");

    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);

    listLines.pop_front();

    mesh.NumberCell1D = listLines.size();

    if (mesh.NumberCell1D == 0)
    {
        cerr << "There is no cell 1D" << endl;
        return false;
    }

    mesh.Cell1DId.resize(mesh.NumberCell1D);
    mesh.Cell1DVertices.resize(mesh.NumberCell1D);

    for (const string& line : listLines)
    {
        istringstream converter(line);

        unsigned int id;
        unsigned int marker;
        Vector2i vertices;

        converter >>  id >> marker >> vertices(0) >> vertices(1);

        mesh.Cell1DId.push_back(id);
        mesh.Cell1DVertices.push_back(vertices);

        if( marker != 0)
        {
            if (mesh.Cell1DMarkers.find(marker) == mesh.Cell1DMarkers.end())
                mesh.Cell1DMarkers.insert({marker, {id}});
            else
                mesh.Cell1DMarkers[marker].push_back(id);
        }
    }

    file.close();

    return true;
}

// ***************************************************************************
bool ImportCell2Ds(PolygonalMesh& mesh)
{

    ifstream file;
    file.open("./Cell2Ds.csv");

    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);

    listLines.pop_front();

    mesh.NumberCell2D = listLines.size();

    if (mesh.NumberCell2D == 0)
    {
        cerr << "There is no cell 2D" << endl;
        return false;
    }

    mesh.Cell2DId.resize(mesh.NumberCell2D);
    mesh.Cell2DVertices.resize(mesh.NumberCell2D);
    mesh.Cell2DEdges.resize(mesh.NumberCell2D);

    string idString;
    string numString;

    for (const string& line : listLines)
    {
        istringstream ss(line);
        getline(ss,idString, ' ');

        unsigned int id = stoi(idString);
        vector<unsigned int> vertices = {};
        vector<unsigned int> edges = {};

        while(getline(ss, numString, ' '))
        {
            int num = stoi(numString);
            vertices.push_back(num);
            edges.push_back(num);
        }

        vertices.erase(vertices.end()-(vertices.size()/2), vertices.end());
        edges.erase(edges.begin(),edges.begin()+(edges.size()/2));

        mesh.Cell2DId.push_back(id);
        mesh.Cell2DVertices.push_back(vertices);
        mesh.Cell2DEdges.push_back(edges);

    }
    file.close();
    return true;
}

}
