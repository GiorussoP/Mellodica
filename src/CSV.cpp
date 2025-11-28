#include "CSV.h"
#include <fstream>
#include <SDL_log.h>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <stack>
#include <utility>

unsigned numberOfLines(const std::string& filePath) {
    std::ifstream inFile(filePath);
    return std::count(std::istreambuf_iterator<char>(inFile),
               std::istreambuf_iterator<char>(), '\n');
}

// Returns (cx, cy, scaleX, scaleY)
std::tuple<Vector2, int, int, int> getRectangle(const std::vector<std::vector<int>>&grid, std::vector<std::vector<int>>& vis, int sx, int sy) {
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
                if (minX == 0) continue;
                bool ok = true;
                for (int i = minY; i <= maxY; i++) {
                    if (i < 0 or i >= m) ok = false;
                    else if (grid[minX-1][i] != grid[sx][sy]) ok = false;
                    else if (vis[minX-1][i]) ok = false;
                }
                if (!ok) continue;
                choosen = dir;
                grown = true;
                minX--;
                break;
            }
            if (dir == 1) {
                // grown maxX:
                if (maxX == n - 1) continue;
                bool ok = true;
                for (int i = minY; i <= maxY; i++) {
                    if (i < 0 or i >= m) ok = false;
                    else if (grid[maxX+1][i] != grid[sx][sy]) ok = false;
                    else if (vis[maxX+1][i]) ok = false;
                }
                if (!ok) continue;
                grown = true;
                maxX++;
                break;
            } if (dir == 2) {
                // grow minY
                if (minY == 0) continue;
                bool ok = true;
                for (int i = minX; i <= maxX; i++) {
                    if (i < 0 or i >= m) ok = false;
                    else if (grid[i][minY - 1] != grid[sx][sy]) ok = false;
                    else if (vis[i][minY - 1]) ok = false;
                }
                if (!ok) continue;
                grown = true;
                minY--;
                break;
            } if (dir == 3) {
                // grow maxY
                if (maxY == m - 1) continue;
                bool ok = true;
                for (int i = minX; i <= maxX; i++) {
                    if (i < 0 or i >= m) ok = false;
                    else if (grid[i][maxY + 1] != grid[sx][sy]) ok = false;
                    else if (vis[i][maxY + 1]) ok = false;
                }
                if (!ok) continue;
                grown = true;
                maxY++;
                break;
            }
        }
    }

    // mark visited cells:
    for (int i = minX; i <= maxX; i++) {
        for (int j = minY; j <= maxY; j++) {
            vis[i][j] = sx*m + sy;
        }
    }

    // compute center of rectangle and scale (in tile size scale)
    float cx = (maxX - minX) / 2.0f;
    float cy = (maxY - minY) / 2.0f;
    return std::make_tuple(Vector2(cx, cy), maxX - minX + 1, maxY - minY + 1, grid[sx][sy]);
}

long computePartitionScore(const std::vector<std::tuple<Vector2, int, int, int>>& partition) {
    long perimeter = 0;
    long numberOfRectangles = partition.size();
    for (auto [pos, sx, sy, _t] : partition) {
        // compute perimeter:
        perimeter += 2*sx + 2*sy;
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
        std::vector<std::vector<int>> vis(grid.size(), std::vector<int>(grid[0].size(), 0));
        for (int i = 0; i < grid.size(); i++) {
            for (int j = 0; j < grid[i].size(); j++) {
                if (vis[i][j]) continue;
                auto rect = getRectangle(grid, vis, i, j);
                partition.push_back(rect);
            }
        }
        long cur = computePartitionScore(partition);
        if (best > cur){
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
            if (j < width - 1) std::cout << " ";
        }
        std::cout << std::endl;
    }

    // convert format
    for (auto [pos, sx, sy, csvEntry] : bestPartition) {
        const auto actorMap = CSVHelper::GetActorMap(csvEntry);
        mapActors.emplace_back(pos, sx, sy, actorMap);
    }
}

MapReader::MapReader(const std::string& filePath) {
    // TODO: fix this function
    height = numberOfLines(filePath) + 1;
    grid.assign(height, {});
    std::ifstream file(filePath);
    if (!file) {
        SDL_Log("[MapReader] Could not open file %s", filePath.c_str());
        exit(1);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    SDL_Log("[MapReader] Loading level %s\n", filePath.c_str());
    width = 0;
    for (unsigned i = 0; i < height; i++) {
        std::string line; buffer >> line;
        auto entries = CSVHelper::Split(line);
        if (width == 0) width = entries.size();
        for (int entry : entries) {
            grid[i].push_back(entry);
        }
    }

    SDL_Log("[MapReader] Loaded level with dimensions %d, %d", width, height);

    SDL_Log("[MapReader] Getting partition...");
    PartitionGrid();
}
