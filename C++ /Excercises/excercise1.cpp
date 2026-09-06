#include<iostream>
#include<vector>


struct Player{
    std::string name;
    int score;
};

int main(){

    std::vector<std::string> players = {"Alice,85", "Bob,95", "Charlie,90", "David,80"};

    std::vector<Player> playerList;
    for(const auto& player : players){
        size_t commaPos = player.find(',');
        std::string name = player.substr(0, commaPos);
        int score = std::stoi(player.substr(commaPos + 1));
        playerList.push_back({name, score});
    }
    
    for(const auto& p : playerList){
            std::cout << "Name: " << p.name << ", Score: " << p.score << std::endl;
        }

}

