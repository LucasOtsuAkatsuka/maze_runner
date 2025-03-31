#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <thread>
#include <chrono>

// Representação do labirinto
using Maze = std::vector<std::vector<char>>;

// Estrutura para representar uma posição no labirinto
struct Position {
    int row;
    int col;
};

// Variáveis globais
Maze maze;
int num_rows;
int num_cols;
int total_threads;
bool exit_found = false;

// Função para carregar o labirinto de um arquivo
Position load_maze(const std::string& file_name) {
    // TODO: Implemente esta função seguindo estes passos:
    // 1. Abra o arquivo especificado por file_name usando std::ifstream
    // 2. Leia o número de linhas e colunas do labirinto
    // 3. Redimensione a matriz 'maze' de acordo (use maze.resize())
    // 4. Leia o conteúdo do labirinto do arquivo, caractere por caractere
    // 5. Encontre e retorne a posição inicial ('e')
    // 6. Trate possíveis erros (arquivo não encontrado, formato inválido, etc.)
    // 7. Feche o arquivo após a leitura

    std::ifstream file(file_name);
    if (!file) {
        std::cerr << "arquivo nao encontrado\n";
        return {-1, -1};
    }

    file >> num_rows >> num_cols;
    maze.resize(num_rows, std::vector<char>(num_cols));

    Position inicio = {-1, -1};

    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            file >> maze[i][j];
            if (maze[i][j] == 'e') {
                inicio = {i, j};
            }
        }
    }

    file.close();
    
    return inicio; // Placeholder - substitua pelo valor correto
}

// Função para imprimir o labirinto
void print_maze() {
    // TODO: Implemente esta função
    // 1. Percorra a matriz 'maze' usando um loop aninhado
    // 2. Imprima cada caractere usando std::cout
    // 3. Adicione uma quebra de linha (std::cout << '\n') ao final de cada linha do labirinto

    std::system("clear");

    for (const auto& row : maze) {
        for (char cell : row) {
            std::cout << cell << " ";
        }
        std::cout << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

// Função para verificar se uma posição é válida
bool is_valid_position(int row, int col) {
    // TODO: Implemente esta função
    // 1. Verifique se a posição está dentro dos limites do labirinto
    //    (row >= 0 && row < num_rows && col >= 0 && col < num_cols)
    // 2. Verifique se a posição é um caminho válido (maze[row][col] == 'x')
    // 3. Retorne true se ambas as condições forem verdadeiras, false caso contrário

    return row >= 0 && row < num_rows && col >= 0 && col < num_cols && (maze[row][col] == 'x' || maze[row][col] == 's');
}

// Função principal para navegar pelo labirinto
bool walk(Position position) {
    // TODO: Implemente a lógica de navegação aqui
    // 1. Marque a posição atual como visitada (maze[pos.row][pos.col] = '.')
    // 2. Chame print_maze() para mostrar o estado atual do labirinto
    // 3. Adicione um pequeno atraso para visualização:
    //    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    // 4. Verifique se a posição atual é a saída (maze[pos.row][pos.col] == 's')
    //    Se for, retorne true
    // 5. Verifique as posições adjacentes (cima, baixo, esquerda, direita)
    //    Para cada posição adjacente:
    //    a. Se for uma posição válida (use is_valid_position()), adicione-a à pilha valid_positions
    // 6. Enquanto houver posições válidas na pilha (!valid_positions.empty()):
    //    a. Remova a próxima posição da pilha (valid_positions.top() e valid_positions.pop())
    //    b. Chame walk recursivamente para esta posição
    //    c. Se walk retornar true, propague o retorno (retorne true)
    // 7. Se todas as posições foram exploradas sem encontrar a saída, retorne false

    std::vector<Position> movimentos;
    Position prox;
    prox.row = position.row;
    prox.col = position.col;
    int caminhos;
    int invalido = 0;

    while (!exit_found) {
        Position atual;
        atual.col = prox.col;
        atual.row = prox.row;
    
        if (maze[atual.row][atual.col] == 's') {
            exit_found = true;
            return 0;
        }

        
        if (maze[atual.row][atual.col] != 'e')
            maze[atual.row][atual.col] = 'o';
        
        movimentos = {
            {atual.row - 1, atual.col}, 
            {atual.row + 1, atual.col}, 
            {atual.row, atual.col - 1}, 
            {atual.row, atual.col + 1}  
        };

        invalido = 0;
        caminhos = 0;
        
        for (const auto& movimento : movimentos) {
            if (is_valid_position(movimento.row, movimento.col) && caminhos == 0) {
                prox.col = movimento.col;
                prox.row = movimento.row;
                caminhos++;
            }else if(is_valid_position(movimento.row, movimento.col) && caminhos == 1){
                std::thread t(walk,movimento);
                total_threads++;
                t.detach();
            }else if(!is_valid_position(movimento.row, movimento.col) ){
                invalido++;
            }
        }

        if (invalido >= 4)
        {
            maze[atual.row][atual.col] = '.';
            total_threads--;
            return 0;
        }
        

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (maze[atual.row][atual.col] != 'e')
            maze[atual.row][atual.col] = '.';
    }
    total_threads--;
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_labirinto>" << std::endl;
        return 1;
    }

    Position initial_pos = load_maze(argv[1]);
    if (initial_pos.row == -1) {
        std::cerr << "Posição inicial não encontrada no labirinto." << std::endl;
        return 1;
    }

    total_threads = 1;
    std::thread w (walk,initial_pos);
    w.detach();

    while (true)
    {
        print_maze();
        if (exit_found) {
            print_maze();
            std::cout << "Saída encontrada!" << std::endl;
            return 0;
        } else if(exit_found == false && total_threads == 0) {
            print_maze();
            std::cout << "Não foi possível encontrar a saída." << std::endl;
            return 0;
        }
    }
    
}

// Nota sobre o uso de std::this_thread::sleep_for:
// 
// A função std::this_thread::sleep_for é parte da biblioteca <thread> do C++11 e posteriores.
// Ela permite que você pause a execução do thread atual por um período especificado.
// 
// Para usar std::this_thread::sleep_for, você precisa:
// 1. Incluir as bibliotecas <thread> e <chrono>
// 2. Usar o namespace std::chrono para as unidades de tempo
// 
// Exemplo de uso:
// std::this_thread::sleep_for(std::chrono::milliseconds(50));
// 
// Isso pausará a execução por 50 milissegundos.
// 
// Você pode ajustar o tempo de pausa conforme necessário para uma melhor visualização
// do processo de exploração do labirinto.
