#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <array>
#include <random>
#include <cstdlib>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> distrib(0, 2);

// Classe TicTacToe
class TicTacToe
{
private:
    std::array<std::array<char, 3>, 3> board; // Tabuleiro do jogo
    std::mutex board_mutex;                   // Mutex para controle de acesso ao tabuleiro
    std::condition_variable turn_cv;          // Variável de condição para alternância de turnos
    char current_player;                      // Jogador atual ('X' ou 'O')
    bool game_over;                           // Estado do jogo
    char winner;                              // Vencedor do jogo

public:
    TicTacToe()
    {
        // Inicializar o tabuleiro e as variáveis do jogo
        this->game_over = false;
        this->winner = ' ';
        this->current_player = 'X';
        for (int i = 0; i <= 2; i++)
        {
            for (int j = 0; j <= 2; j++)
            {
                this->board[i][j] = ' ';
            }
        }
    }

    void display_board()
    {
        system("clear");
        std::cout << "Jogador atual: " << this->current_player << "\n" << std::endl;
        for (int i = 0; i <= 2; i++)
        {
            for (int j = 0; j <= 2; j++)
            {
                std::cout << ' ' << this->board[i][j] << ' ';
                if (j < 2)
                {
                    std::cout << '|';
                }
            }
            std::cout << "\n";
            if (i < 2)
            {
                std::cout << "———————————" << std::endl;
            }
        }
        std::cout << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        return;
    }

    bool make_move(char player, int row, int col)
    {
        // Implementar a lógica para realizar uma jogada no tabuleiro
        // Utilizar mutex para controle de acesso
        // Utilizar variável de condição para alternância de turnos
        if(this->game_over){
            return false;
        }
        std::unique_lock<std::mutex> lock(this->board_mutex);
        if (!turn_cv.wait_for(lock, std::chrono::seconds(1), [this, player] { return current_player == player || game_over; })){
            return false;
        }
        if (this->board[row][col] != ' ' || game_over)
            return false;
        this->board[row][col] = player;
        this->display_board();
        if(this->check_win(player))
            this->current_player = ' ';
        else
            this->current_player = (this->current_player == 'X') ? 'O' : 'X';
        this->turn_cv.notify_all();
        return true;
    }

    bool check_win(char player){
        // Verificar se o jogador atual venceu o jogo
        for (int i = 0; i <= 2; i++){
            if (this->board[i][0] == player && this->board[i][1] == player && this->board[i][2] == player || this->board[0][i] == player && this->board[1][i] == player && this->board[2][i] == player){
                this->winner = player;
                this->game_over = true;
                return true;
            }
        }
        if (this->board[0][0] == player && this->board[1][1] == player && this->board[2][2] == player || this->board[0][2] == player && this->board[1][1] == player && this->board[2][0] == player){
            this->winner = player;
            this->game_over = true;
            return true;
        }
        if(this->check_draw())
            return false;
        return false;
    }

    bool check_draw(){
        // Verificar se houve um empate
        if (this->game_over || this->winner == 'D')
            return true;
        for (int i = 0; i <= 2; i++){
            for (int j = 0; j <= 2; j++){
                if (this->board[i][j] == ' '){
                    return false;
                }
            }
        }
        this->winner = 'D';
        this->game_over = true;
        return true;
    }

    bool is_game_over(){
        return this->game_over;
    }

    char get_winner(){
        // Retornar o vencedor do jogo ('X', 'O', ou 'D' para empate)
        return this->winner;
    }

};

// Classe Player
class Player
{
private:
    TicTacToe &game;      // Referência para o jogo
    char symbol;          // Símbolo do jogador ('X' ou 'O')
    std::string strategy; // Estratégia do jogador

    int row = -1;
    int col = -1;

public:
    Player(TicTacToe &g, char s, std::string strat)
        : game(g), symbol(s), strategy(strat) {}

    void play()
    {

        // Executar jogadas de acordo com a estratégia escolhida

        while(!this->game.is_game_over()){
            if (this->strategy == "sequential" || this->strategy == "Sequential" || this->strategy == "sequencial" || this->strategy == "Sequencial" || this->strategy == "s" || this->strategy == "S")
                this->play_sequential();
            else
                this->play_random();
        }
        return;
    }

private:
    void play_sequential(){
        // Implementar a estratégia sequencial de jogadas
        if (row == -1 || col == -1){
            row = 0;
            col = 0;
        }
        while (!this->game.make_move(this->symbol, this->row, this->col) && !this->game.is_game_over()){
            this->col++;
            if (this->col >= 3) {
                this->col = 0;
                this->row = (this->row + 1) % 3;
            }
        }
        return;
    }

    void play_random(){
        // Implementar a estratégia aleatória de jogadas
        while (!this->game.make_move(this->symbol, this->row, this->col) && !this->game.is_game_over()){
            row = distrib(gen);
            col = distrib(gen);
        }
        return;
    }
};

// Função principal
int main()
{
    // Inicializar o jogo e os jogadores
    TicTacToe Game;

    //Random ou Sequential
    Player Player_X(Game, 'X', "random");
    Player Player_O(Game, 'O', "random");

    // Criar as threads para os jogadores

    std::thread Thread_X(&Player::play, &Player_X);
    std::thread Thread_O(&Player::play, &Player_O);

    // Aguardar o término das threads
    Thread_X.join();
    Thread_O.join();

    // Exibir o resultado final do jogo

    if (Game.get_winner() == 'D')
        std::cout << "O resultado foi empate!" << std::endl;
    else
        std::cout << "O vendedor foi " << Game.get_winner() << '!' << std::endl;

    return 0;
}
