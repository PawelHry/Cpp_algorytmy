#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

struct punkt{
    int x;
    int y;
    int dojsce;
};

struct dziedzic{
    punkt rodzic;
    double h;
    int kolejnosc;
    int dojsce;
};

punkt znajdzPunkt(dziedzic tablica[][20]) {
    punkt najmniejszy = {0,0};  // Początkowa wartość dla najmniejszej liczby dodatniej

    for (int i = 0; i < 20; ++i) {
        for (int j = 0; j < 20; ++j) {
            if (tablica[i][j].h > 0){
                if (tablica[i][j].h == tablica[najmniejszy.x][najmniejszy.y].h){
                    if (tablica[i][j].kolejnosc < tablica[najmniejszy.x][najmniejszy.y].kolejnosc) {
                        najmniejszy = {i, j};
                    }
                }
                if (tablica[i][j].h < tablica[najmniejszy.x][najmniejszy.y].h){
                    najmniejszy = {i,j};
                }
            }
        }
    }
    return najmniejszy;
}

void dodajDoOtwartej(dziedzic otwarta[][20], dziedzic zamknieta[][20], int G[][20],
                     const punkt &teraz, punkt zmiana, const punkt koniec, int &kolejnosc) {
    int x = teraz.x + zmiana.x;
    int y = teraz.y + zmiana.y;

    if (x >= 0 && x < 20 && y >= 0 && y < 20 && G[x][y] == 0) {
        dziedzic temp = {{teraz}, sqrt(pow(x - koniec.x, 2) + pow(y - koniec.y, 2))};
        if (otwarta[x][y].h > temp.h || otwarta[x][y].h == 0) {
            otwarta[x][y] = temp;
            otwarta[x][y].dojsce = otwarta[teraz.x][teraz.y].dojsce + 1; // Zwiększamy `dojsce` o 1 od poprzednika
            otwarta[x][y].kolejnosc = kolejnosc;
            kolejnosc++;
        }
    }
}

int main() {
    const punkt koniec = {20, 20};

    int G[20][20];

    // Wczytywanie z pliku

    ifstream plik("grid.txt");

    if (plik.is_open()) {
        for (auto & i : G) {
            for (int & j : i) {
                if (!(plik >> j)) {
                    cout << "Błąd podczas wczytywania danych z pliku." << endl;
                    return 1;
                }
            }
        }
        plik.close();

        // tworzenie obu list

        dziedzic otwarta[20][20];
        dziedzic zamknieta[20][20];
        punkt teraz = {0,0};
        int kolejnosc = 0;

        for (int i=0;i<koniec.x;i++)
        {
            for (int j=0;j<koniec.y;j++)
            {
                zamknieta[i][j] = {{-1,-1,0},0,0,0};
            }
        }

        for (int i=0;i<koniec.x;i++)
        {
            for (int j=0;j<koniec.y;j++)
            {
                otwarta[i][j] = {{-1,-1,0},0,-1,0};
            }
        }

        // pierwsza wartość do zamknietej
        if (G[0][0] == 0){
            zamknieta[0][0] = {teraz,sqrt(pow((teraz.x)-koniec.x,2)
                                          +(pow((teraz.y-koniec.y),2))),0,0};
        }
        // pierwsze wartości do otwartej
        otwarta[0][0] = {{0,0,0},zamknieta[0][0].h,kolejnosc,0};
        kolejnosc++;
        dziedzic temp{};
        dodajDoOtwartej(otwarta,zamknieta,G,teraz,{0,1},koniec,kolejnosc);
        dodajDoOtwartej(otwarta,zamknieta,G,teraz,{0,-1},koniec,kolejnosc);
        dodajDoOtwartej(otwarta,zamknieta,G,teraz,{1,0},koniec,kolejnosc);
        dodajDoOtwartej(otwarta,zamknieta,G,teraz,{-1,0},koniec,kolejnosc);

        // magia
        while(true){
            // znajdywanie najmniejszej w otwartej
            teraz = znajdzPunkt(otwarta);
            if (teraz.x == 0 && teraz.y == 0){
                cout << "Brak trasy" << "\n";
                return 0;
            }

            // usuwanie z otwartej i dodawanie do zamknietej
            zamknieta[teraz.x][teraz.y] = otwarta[teraz.x][teraz.y];
            otwarta[teraz.x][teraz.y].h = -10;

            // stop jak doszliśmy do końca
            if (teraz.x == koniec.x-1 && teraz.y == koniec.y-1){
                break;
            }
            dodajDoOtwartej(otwarta,zamknieta,G,teraz,{0,1},koniec,kolejnosc);
            dodajDoOtwartej(otwarta,zamknieta,G,teraz,{0,-1},koniec,kolejnosc);
            dodajDoOtwartej(otwarta,zamknieta,G,teraz,{1,0},koniec,kolejnosc);
            dodajDoOtwartej(otwarta,zamknieta,G,teraz,{-1,0},koniec,kolejnosc);
        }

        // przepisywanie drogi

        teraz.x = koniec.x-1;
        teraz.y = koniec.y-1;

        while (!(teraz.x == 0 && teraz.y == 0)){
            cout << teraz.x << ", " << teraz.y << "\n";
            G[teraz.x][teraz.y] = 3;
            if (teraz.x+1 < koniec.x-1 && otwarta[teraz.x][teraz.y].dojsce > otwarta[teraz.x+1][teraz.y].dojsce+2 &&
            otwarta[teraz.x+1][teraz.y].dojsce > 0){
                teraz = {teraz.x+1,teraz.y};
                G[teraz.x][teraz.y] = 3;
                cout << "x+1" << "\n";
            }
            if (teraz.x-1 >= 0 && otwarta[teraz.x][teraz.y].dojsce > otwarta[teraz.x-1][teraz.y].dojsce+2 &&
            otwarta[teraz.x-1][teraz.y].dojsce > 0){
                teraz = {teraz.x-1,teraz.y};
                G[teraz.x][teraz.y] = 3;
                cout << "x-1" << "\n";
            }
            if (teraz.y+1 < koniec.y-1 && otwarta[teraz.x][teraz.y].dojsce > otwarta[teraz.x][teraz.y+1].dojsce+2 &&
            otwarta[teraz.x][teraz.y+1].dojsce > 0){
                teraz = {teraz.x,teraz.y+1};
                G[teraz.x][teraz.y] = 3;
                cout << "y+1" << "\n";
            }
            if (teraz.y-1 >= 0 && otwarta[teraz.x][teraz.y].dojsce > otwarta[teraz.x][teraz.y-1].dojsce+2 &&
            otwarta[teraz.x][teraz.y-1].dojsce > 0){
                teraz = {teraz.x,teraz.y-1};
                G[teraz.x][teraz.y] = 3;
                cout << "y-1" << "\n";
            }
            punkt rodzic = otwarta[teraz.x][teraz.y].rodzic;
            teraz.x = rodzic.x;
            teraz.y = rodzic.y;
        }
        G[teraz.x][teraz.y] = 3;

        // wyświetlanie

        for (int i = 0; i < koniec.x; i++) {
            for (int j = 0; j < koniec.y; j++) {
                cout << G[i][j] << " ";
            }
            cout << endl;
        }
    } else {
        cout << "Nie można otworzyć pliku." << endl;
        return 1;
    }
        return 0;
}
