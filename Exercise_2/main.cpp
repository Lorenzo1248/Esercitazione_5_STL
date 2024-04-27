#include <iostream>
#include "PolygonalMesh.hpp"
#include "Utils.hpp"

using namespace std;
using namespace Eigen;
using namespace PolygonalLibrary;

int main()
{
    cout<<"prova";
    PolygonalMesh mesh;

    if(!ImportMesh(mesh))
    {
        cout<<"errore";
        return 1;
    }
    cout<<"corretto";
    return 0;
}
