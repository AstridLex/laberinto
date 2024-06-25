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
    void generate(); //genera el laberinto
    void display(); //imprime el laberinto en la consola
    bool solve(); //metodo que intenta resolver el laberinto con algun algoritmo
    void displaySolution(); //metodo que muestra la solucion del laberinto

private:
    int width, height; //ancho y altura
    vector<vector<char>> maze; //es la matriz que representa el laberinto (es una matriz bidimensional)
    vector<vector<bool>> visited; // es una matriz en la que busca cuales celdas fueron visitadas, ya sea por la creacion del laberinto o si ya se paso por el camino vacio
    vector<pair<int, int>> path; // Para almacenar las coordenadas del camino correcto

    bool isInside(int x, int y); // verifica si las coordenadas estan dentro de los limites
    void carvePassage(int x, int y); //es el que crea caminos adicionales dentro del laberinto utilizando recursion y verificacion de visias para evitar ciclos infinitos
    bool solveUtil(int x, int y); //metodo que utiliza internamente backtracking para intentar resolver el laberinto
    void createPathToExit(int x, int y); // es el metodo que se utiliza para crear un camino directo de la entrada a la salida, se usa para que el laberinto siempre se pueda resolver
};

// Constructor de la clase Maze
Maze::Maze(int width, int height) : width(width), height(height), maze(height, vector<char>(width, '|')), visited(height, vector<bool>(width, false)) {}

// Método para generar el laberinto, primero crea el camino directo a la salida y despues crea los caminos aleatorios
void Maze::generate() {
    srand(time(0)); // devuelve el número actual de segundos desde la época (inicio del tiempo), lo que ayuda a generar números aleatorios diferentes cada vez que se ejecuta el programa
    createPathToExit(0, 0); // Crea un camino directo a la salida
    visited = vector<vector<bool>>(height, vector<bool>(width, false)); // Resetear visitados
    carvePassage(0, 0); // Tallar caminos adicionales
}

// Método para verificar si las coordenadas están dentro del laberinto
bool Maze::isInside(int x, int y) {
    return x >= 0 && x < height && y >= 0 && y < width; //Comprueba si x está dentro del rango [0, height-1] y si y está dentro del rango [0, width-1]
}

// Método para crear un camino directo a la salida, en cada iteración, establece el carácter en la posición maze[x][y] como ' ', lo cual indica un pasillo abierto en el laberinto
void Maze::createPathToExit(int x, int y) {
    while (x < height - 1) { //Se ejecuta mientras x sea menor que height - 1. Esto significa que itera desde la posición inicial (x, y) hacia abajo hasta alcanzar la fila height - 1
        maze[x][y] = ' ';
        x++;
    }
    while (y < width - 1) { //Se ejecuta mientras y sea menor que width - 1. Itera desde la posición donde se detuvo el primer bucle hacia la derecha hasta alcanzar la columna width - 1
        maze[x][y] = ' ';
        y++;
    }
    maze[x][y] = ' '; //Después de ejecutar los bucles, x y y habrán alcanzado los valores height - 1 y width - 1, osea la entrada, lo que representa la esquina inferior derecha del laberinto.
}

// Método para tallar un camino en el laberinto
void Maze::carvePassage(int x, int y) {
    visited[x][y] = true; //marca la celda actual de (x,y) como visitados para evitar ciclos infinitos y que no vuelva a pasar por un camino ya visitado
    maze[x][y] = ' '; //Modifica la celda actual (x,y) en el laberinto para representar un pasillo abierto, indicado por el carácter ' '

    // Barajar direcciones, para que el vector busque de forma aleatoria las direcciones 
    vector<int> dirs = { 0, 1, 2, 3 };
    random_device rd;
    mt19937 g(rd());
    shuffle(dirs.begin(), dirs.end(), g);

    for (int i = 0; i < 4; ++i) { //Se itera sobre cada dirección barajada en el vector dirs. 
        int nx = x + dx[dirs[i]], ny = y + dy[dirs[i]];  // Calcula las coordenadas (nx, ny) que representan la siguiente celda en la dirección actual dirs[i]

        int nx2 = x + 2 * dx[dirs[i]], ny2 = y + 2 * dy[dirs[i]]; //Calcula las coordenadas (nx2, ny2) que representan la celda dos posiciones más allá en la misma dirección dirs[i]. Esto es crucial para determinar si hay espacio suficiente para tallar un camino adicional
        if (isInside(nx2, ny2) && !visited[nx2][ny2]) {
            maze[nx][ny] = ' ';
            carvePassage(nx2, ny2);
        }
    }
}

// Método para visualizar el laberinto, 
void Maze::display() {
    for (const auto & row : maze) { //const auto& row: row es una referencia constante a cada fila en la matriz maze.
        for (char cell : row) { //char cell: cell representa el carácter que representa el contenido de cada celda en el laberinto (' ', '|', '*',)
            cout << cell;
        }
        cout << '\n';
    }
}

// Método para resolver el laberinto utilizando backtracking
bool Maze::solve() {
    path.clear(); // Limpia el vector "path", que se utiliza para encontrar el camino correcto
    visited = vector<vector<bool>>(height, vector<bool>(width, false)); //reinicia la matriz visited para marcar que celdas han sido visitados durante el proceso de resolver el laberinto
    return solveUtil(0, 0); //Llama al metodo desde la posición inicial (0, 0) que seria la salida para comenzar la búsqueda de la solución al laberinto utilizando backtracking
}


bool Maze::solveUtil(int x, int y) {
    if (x == height - 1 && y == width - 1) { // Caso base: Se llega a la posición de salida
        path.push_back({ x, y }); // Agrega la posicion actual al camino solucion
        return true; // Indica que se encontró una solución
    }

    if (isInside(x, y) && maze[x][y] == ' ' && !visited[x][y]) {  // Verificar si la posicion (x, y) es valida para avanzar
        visited[x][y] = true; // Marca la posicion como visitada
        path.push_back({ x, y }); // Agrega la posición actual al camino solucion

        for (int i = 0; i < 4; ++i) {  // Explorar en las cuatro direcciones posibles
            int nx = x + dx[i], ny = y + dy[i]; // Calcula las nuevas coordenadas (nx, ny)
            if (solveUtil(nx, ny)) { // Llama recursivamente para intentar encontrar una solución desde (nx, ny)
                return true; // Si se encontro una solución desde (nx, ny), retorna true
            }
        }
        // Si ninguna dirección lleva a una solución, retrocede y desmarca la posición actual
        path.pop_back();
    }
    return false; // Retorna false si no se encontró una solución desde la posición (x, y)
}

// Método para visualizar la solución del laberinto
void Maze::displaySolution() { // Marcar el camino correcto en el laberinto con *
    for (const auto& [x, y] : path) {
        maze[x][y] = '*';
    }
    maze[0][0] = 'S'; //posicion salida
    maze[height - 1][width - 1] = 'E'; //posicion entrada

    for (const auto& row : maze) { // Mostrar el laberinto con la solucion marcada
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

    Maze maze(width, height); //Se crea un objeto maze de la clase Maze utilizando los valores de ancho (width) y alto (height) ingresados por el usuario

    cout << "Generando laberinto...\n";
    maze.generate();
    maze.display();

    cout << "\nResolviendo laberinto...\n";
    if (maze.solve()) {
        cout << "\nCamino enconrado...\n";
        maze.displaySolution();
    } else {
        cout << "No se encontro solucion para el laberinto.\n";
    }
    cout << "\nPresione Enter para cerrar la ventana...";
    cin.ignore();
    cin.get();

    return 0;
}
