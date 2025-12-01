#include "CSV.h"
#include <SDL_log.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stack>
#include <utility>

unsigned numberOfLines(const std::string &filePath) {
  std::ifstream inFile(filePath);
  return std::count(std::istreambuf_iterator<char>(inFile),
                    std::istreambuf_iterator<char>(), '\n');
}

// Returns (cx, cy, scaleX, scaleY)
std::tuple<Vector2, int, int, int>
getRectangle(const std::vector<std::vector<int>> &grid,
             std::vector<std::vector<int>> &vis, int sx, int sy) {
  int minX = sx;
  int minY = sy;
  int maxX = sx;
  int maxY = sy;

  std::random_device rd;

  std::mt19937 gen(rd());

  const int n = grid.size();
  const int m = grid[0].size();

  bool grown = true;
  while (grown) {
    grown = false;
    // check grow directions in random order
    std::vector<int> directions{0, 1, 2, 3};
    std::shuffle(directions.begin(), directions.end(), gen);

    int choosen = -1;
    for (const int dir : directions) {
      if (dir == 0) {
        // grown minX:
        if (minX == 0)
          continue;
        bool ok = true;
        for (int i = minY; i <= maxY; i++) {
          if (i < 0 or i >= m)
            ok = false;
          else if (grid[minX - 1][i] != grid[sx][sy])
            ok = false;
          else if (vis[minX - 1][i])
            ok = false;
        }
        if (!ok)
          continue;
        choosen = dir;
        grown = true;
        minX--;
        break;
      }
      if (dir == 1) {
        // grown maxX:
        if (maxX == n - 1)
          continue;
        bool ok = true;
        for (int i = minY; i <= maxY; i++) {
          if (i < 0 or i >= m)
            ok = false;
          else if (grid[maxX + 1][i] != grid[sx][sy])
            ok = false;
          else if (vis[maxX + 1][i])
            ok = false;
        }
        if (!ok)
          continue;
        grown = true;
        maxX++;
        break;
      }
      if (dir == 2) {
        // grow minY
        if (minY == 0)
          continue;
        bool ok = true;
        for (int i = minX; i <= maxX; i++) {
          if (i < 0 or i >= m)
            ok = false;
          else if (grid[i][minY - 1] != grid[sx][sy])
            ok = false;
          else if (vis[i][minY - 1])
            ok = false;
        }
        if (!ok)
          continue;
        grown = true;
        minY--;
        break;
      }
      if (dir == 3) {
        // grow maxY
        if (maxY == m - 1)
          continue;
        bool ok = true;
        for (int i = minX; i <= maxX; i++) {
          if (i < 0 or i >= m)
            ok = false;
          else if (grid[i][maxY + 1] != grid[sx][sy])
            ok = false;
          else if (vis[i][maxY + 1])
            ok = false;
        }
        if (!ok)
          continue;
        grown = true;
        maxY++;
        break;
      }
    }
  }

  // mark visited cells:
  for (int i = minX; i <= maxX; i++) {
    for (int j = minY; j <= maxY; j++) {
      vis[i][j] = sx * m + sy;
    }
  }

  // compute center of rectangle and scale (in tile size scale)
  float cx = (maxX - minX) / 2.0f + minX;
  float cy = (maxY - minY) / 2.0f + minY;
  return std::make_tuple(Vector2(cx, cy), maxX - minX + 1, maxY - minY + 1,
                         grid[sx][sy]);
}

long computePartitionScore(
    const std::vector<std::tuple<Vector2, int, int, int>> &partition) {
  long perimeter = 0;
  long numberOfRectangles = partition.size();
  for (auto [pos, sx, sy, _t] : partition) {
    // compute perimeter:
    perimeter += 2 * sx + 2 * sy;
  }

  // score of partition: try to penalize # rectangles, but focus on perimeter
  return numberOfRectangles + 3 * perimeter;
}

void MapReader::PartitionGrid() {

  // run x partitions, get best
  long best = 1e9 + 10;
  std::vector<std::tuple<Vector2, int, int, int>> bestPartition;
  std::vector<std::vector<int>> bestVis;
  for (int t = 0; t < PARTITION_TRIES; t++) {
    std::vector<std::tuple<Vector2, int, int, int>> partition;
    std::vector<std::vector<int>> vis(grid.size(),
                                      std::vector<int>(grid[0].size(), 0));
    for (int i = 0; i < grid.size(); i++) {
      for (int j = 0; j < grid[i].size(); j++) {
        if (vis[i][j])
          continue;
        auto rect = getRectangle(grid, vis, i, j);
        partition.push_back(rect);
      }
    }
    long cur = computePartitionScore(partition);
    if (best > cur) {
      best = cur;
      bestPartition = partition;
      bestVis = vis;
    }
  }
  SDL_Log("[MapReader] Best partition score: %ld", best);

  // TODO: Remove this:
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      std::cout << bestVis[i][j];
      if (j < width - 1)
        std::cout << " ";
    }
    std::cout << std::endl;
  }

  // convert format
  for (auto [pos, sx, sy, csvEntry] : bestPartition) {
    mapActors.emplace_back(pos, sx, sy, static_cast<unsigned int>(csvEntry));
  }
}

MapReader::MapReader(const std::string &filePath, bool merge) {
  std::ifstream file(filePath);
  if (!file) {
    SDL_Log("[MapReader] Could not open file %s", filePath.c_str());
    exit(1);
  }
  std::vector<std::string> lines;
  std::string line;
  while (std::getline(file, line)) {
    if (!line.empty()) {
      lines.push_back(line);
    }
  }
  height = lines.size();
  grid.assign(height, std::vector<int>());
  SDL_Log("[MapReader] Loading level %s\n", filePath.c_str());
  width = 0;
  for (unsigned i = 0; i < height; i++) {
    auto entries = CSVHelper::Split(lines[i]);
    if (width == 0)
      width = entries.size();
    grid[i] = entries;
  }

  SDL_Log("[MapReader] Loaded level with dimensions %d, %d", width, height);

  if (merge) {
    SDL_Log("[MapReader] Getting partition...");
    PartitionGrid();
    std::cout << "Read " << mapActors.size() << " actors from map."
              << std::endl;
  } else {
    for (unsigned i = 0; i < height; i++) {
      for (unsigned j = 0; j < grid[i].size(); j++) {
        mapActors.emplace_back(Vector2((float)i, (float)j), 1, 1,
                               static_cast<unsigned int>(grid[i][j]));
      }
    }

    std::cout << "Read " << mapActors.size() << " actors from map."
              << std::endl;
  }
}