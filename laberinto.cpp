#include <iostream> //se utiliza para la entrada y la salida estandar (cin y cout)
#include <vector> //se utiliza para la matiz del del laberinto, la matriz de celdas utilizadas y almacenar el camino
#include <ctime> //se utiliza para el generador de nuemeros aleatorios, srand(time(0))
#include <cstdlib> // se utiliza para las funciones "rand" y "srand" para la generacion de numeros aleatorios
#include <algorithm> // se utiliza para "shuffle" que se emplea para barajar las direcciones en "carvePassage"
#include <random> // se utiliza para generar numeros aleatorios con "random_device" y "mt19937" en el metodo "carvePassage"

using namespace std; // se utiliza  para hacer que todas las funciones y clases estándar de C++ definidas en el espacio de nombres std estén disponibles globalmente sin necesidad de calificarlas con std::

// Direcciones para moverse en el laberinto (arriba, derecha, abajo, izquierda)
const int dx[] = { -1, 0, 1, 0 }; //Define cómo cambia la coordenada x cuando te mueves en una dirección específica. (<----->)
const int dy[] = { 0, 1, 0, -1 }; // Define cómo cambia la coordenada y cuando te mueves en la misma dirección correspondiente en dx

// Clase para manejar el laberinto
class Maze {
public:
    Maze(int width, int height);
    void generate();
    void display();
    bool solve();
    void displaySolution();
    void displayOnlyPath();

private:
    int width, height;
    vector<vector<char>> maze;
    vector<vector<bool>> visited;
    vector<pair<int, int>> path; // Para almacenar la solución

    bool isInside(int x, int y);
    void carvePassage(int x, int y);
    bool solveUtil(int x, int y);
    void createPathToExit(int x, int y);
};

// Constructor de la clase Maze
Maze::Maze(int width, int height) : width(width), height(height), maze(height, vector<char>(width, '|')), visited(height, vector<bool>(width, false)) {}

// Método para generar el laberinto
void Maze::generate() {
    srand(time(0));
    createPathToExit(0, 0); // Crea un camino directo a la salida
    visited = vector<vector<bool>>(height, vector<bool>(width, false)); // Resetear visitados
    carvePassage(0, 0); // Tallar caminos adicionales
}

// Método para verificar si las coordenadas están dentro del laberinto
bool Maze::isInside(int x, int y) {
    return x >= 0 && x < height && y >= 0 && y < width;
}

// Método para crear un camino directo a la salida
void Maze::createPathToExit(int x, int y) {
    while (x < height - 1) {
        maze[x][y] = ' ';
        x++;
    }
    while (y < width - 1) {
        maze[x][y] = ' ';
        y++;
    }
    maze[x][y] = ' ';
}

// Método para tallar un camino en el laberinto
void Maze::carvePassage(int x, int y) {
    visited[x][y] = true;
    maze[x][y] = ' ';

    // Barajar direcciones
    vector<int> dirs = { 0, 1, 2, 3 };
    random_device rd;
    mt19937 g(rd());
    shuffle(dirs.begin(), dirs.end(), g);

    for (int i = 0; i < 4; ++i) {
        int nx = x + dx[dirs[i]], ny = y + dy[dirs[i]];

        int nx2 = x + 2 * dx[dirs[i]], ny2 = y + 2 * dy[dirs[i]];
        if (isInside(nx2, ny2) && !visited[nx2][ny2]) {
            maze[nx][ny] = ' ';
            carvePassage(nx2, ny2);
        }
    }
}

// Método para visualizar el laberinto
void Maze::display() {
    for (const auto& row : maze) {
        for (char cell : row) {
            cout << cell;
        }
        cout << '\n';
    }
}

// Método para resolver el laberinto utilizando backtracking
bool Maze::solve() {
    path.clear();
    visited = vector<vector<bool>>(height, vector<bool>(width, false));
    return solveUtil(0, 0);
}

// Método auxiliar para resolver el laberinto
bool Maze::solveUtil(int x, int y) {
    if (x == height - 1 && y == width - 1) {
        path.push_back({ x, y });
        return true;
    }

    if (isInside(x, y) && maze[x][y] == ' ' && !visited[x][y]) {
        visited[x][y] = true;
        path.push_back({ x, y });

        for (int i = 0; i < 4; ++i) {
            int nx = x + dx[i], ny = y + dy[i];
            if (solveUtil(nx, ny)) {
                return true;
            }
        }

        path.pop_back();
    }
    return false;
}

// Método para visualizar la solución del laberinto
void Maze::displaySolution() {
    for (const auto& [x, y] : path) {
        maze[x][y] = '*';
    }
    maze[0][0] = 'S'; // Entrada
    maze[height - 1][width - 1] = 'E'; // Salida

    for (const auto& row : maze) {
        for (char cell : row) {
            cout << cell;
        }
        cout << '\n';
    }
}

// Función principal
int main() {
    int width, height;

    // Leer el tamaño del laberinto
    do {
        cout << "Ingrese el ancho del laberinto (10-50): ";
        cin >> width;
    } while (width < 10 || width > 50);

    do {
        cout << "Ingrese el alto del laberinto (10-50): ";
        cin >> height;
    } while (height < 10 || height > 50);

    Maze maze(width, height);

    cout << "Generando laberinto...\n";
    maze.generate();
    maze.display();

    cout << "\nResolviendo laberinto...\n";
    if (maze.solve()) {
        cout << "\nSolucion completa del laberinto:\n";
        maze.displaySolution();
    } else {
        cout << "No se encontro solucion para el laberinto.\n";
    }

    // Esperar una entrada del usuario antes de cerrar
    cout << "\nPresione Enter para salir...";
    cin.ignore();
    cin.get();

    return 0;
}
