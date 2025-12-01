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
// Removed ActorMap enum. Now using unsigned int for actor type.

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

// Removed GetActorMap. Now using csvEntry directly as unsigned int.
} // namespace CSVHelper

// TODO: Rename this class
class MapReader {
public:
  explicit MapReader(const std::string &filePath, bool merge = true);

  std::vector<std::tuple<Vector2, int, int, unsigned int>> &GetMapActors() {
    return mapActors;
  }

  void PartitionGrid();

  std::vector<std::vector<int>> grid;
  std::vector<std::tuple<Vector2, int, int, unsigned int>> mapActors;
  unsigned width;
  unsigned height;
};
