#pragma once
#include <SDL_log.h>
#include <random>
#include <string>
#include <vector>

#include "Math.hpp"

// TODO: fine tunar isso aqui nos csvs reais depois
// how many random partitions are tried
#define PARTITION_TRIES 10

// TODO: Colocar as coisas de verdade aqui
// Holds actors that appears on Map files
enum class ActorMap {
  FLOOR,
  HOUSE,
};

namespace CSVHelper {
inline std::vector<int> Split(const std::string &str, char delim = ',') {
  std::vector<int> retVal;

  size_t start = 0;
  size_t delimLoc = str.find_first_of(delim, start);
  while (delimLoc != std::string::npos) {
    retVal.emplace_back(std::stoi(str.substr(start, delimLoc - start)));

    start = delimLoc + 1;
    delimLoc = str.find_first_of(delim, start);
  }

  retVal.emplace_back(std::stoi(str.substr(start, delimLoc - start)));
  return retVal;
}

inline ActorMap GetActorMap(const int csvEntry) {
  switch (csvEntry) {
  case 0:
    return ActorMap::FLOOR;
  case 1:
    return ActorMap::HOUSE;
  default:
    SDL_Log("Invalid csv entry: %d", csvEntry);
    exit(-1);
  }
}
} // namespace CSVHelper

// TODO: Rename this class
class MapReader {
public:
  explicit MapReader(const std::string &filePath);

  std::vector<std::tuple<Vector2, int, int, ActorMap>> &GetMapActors() {
    return mapActors;
  }

private:
  void PartitionGrid();

  std::vector<std::vector<int>> grid;
  std::vector<std::tuple<Vector2, int, int, ActorMap>> mapActors;
  unsigned width;
  unsigned height;
};
