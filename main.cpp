#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <dirent.h>
#include <png.h>
#include <unordered_map>
#include "redCounter.h"

/**
 * Given the pokemon data file, readAttack goes line by line, and loads each Pokemon's name and attack value in attackMap.
 */
void readAttack(const std::string& filename, std::unordered_map<std::string, int>& attackMap) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Failed to open file: " << filename << std::endl;
        return;
    }

    std::string line;
    
    while (std::getline(file, line)) {
        std::vector<std::string> values;
        std::stringstream ss(line);
        std::string value;

        //The values in the text file are separated by commas, split it up.
        while (std::getline(ss, value, ',')) {
            values.push_back(value);
        }

        if (values.size() != 9) {
            std::cout << "Invalid line: " << line << std::endl;
            continue;
        }

        std::string name = values[0];

        int attack = std::stoi(values[4]);

        // Store the attack value in the hashmap
        attackMap[name] = attack;
    }

    file.close();
    return;
}

/**
 * Main method. Finds the three pokemon with the highest "red attack" value, which is the number of red pixels
 * in the pokemons' sprite multiplied by its base attack value.
 */
int main() {
    //Create an unordered map that links each pokemons name to it's base attack value
    std::string filename = "Pokemondata.txt";
    std::unordered_map<std::string, int> attackMap;
    readAttack(filename, attackMap);

    //create an array of <string, int> pairs to keep track of the best pokemon.
    int bestCount = 6;
    std::pair<std::string, int> bestPokemon[bestCount];

    std::string folderPath = "data"; 

    //Open the data folder and iterate through each image.
    DIR* dir;
    struct dirent* entry;
    if ((dir = opendir(folderPath.c_str())) != nullptr) {
        while ((entry = readdir(dir)) != nullptr) {
            std::string photoName = entry->d_name;
            std::string fullPath = folderPath + "/" + photoName;

            // Check if it's a regular file
            if (entry->d_type == DT_REG) {
                //treats every form (mega, normal, types) as the same since they occupy the same pokedex entry
                std::string pokemonName = photoName.substr(0, photoName.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ "));

                // Capitalize the first character of pokemonName to match attackMap.
                pokemonName[0] = std::toupper(pokemonName[0]);

                //Get the number of red pixels
                int redCount = readPNGforRed(fullPath.c_str());

                //Calculate the red attack.
                int redAttack = attackMap[pokemonName] * redCount;

                //See if the current pokemon belongs in the top 3. 
                for (int i = 0; i < bestCount; i++) {
                    if (bestPokemon[i].second < redAttack) {
                        bestPokemon[i].first = photoName;
                        bestPokemon[i].second = redAttack;
                        break;
                    }
                }
            }
        }
        closedir(dir);
    } else {
        std::cout << "Failed to open directory: " << folderPath << std::endl;
    }

    //Print out the 3 best pokemon.
    for (int i = 0; i < bestCount; i++) { 
        std::cout << "Best Pokemon " << i+1 << ": " << bestPokemon[i].first << " with attack value " << bestPokemon[i].second << std::endl;
    }

    return 0;
}
