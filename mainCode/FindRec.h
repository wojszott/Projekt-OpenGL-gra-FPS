#ifndef FINDREC_H
#define FINDREC_H

#include <vector>
#include <iostream>

struct Rectangle {
    float x1, y1, x2, y2;
    uint8_t value;
};

void markVisited(std::vector<std::vector<bool>>& visited, int startRow, int startCol, int endRow, int endCol) {
    for (int i = startRow; i <= endRow; ++i) {
        for (int j = startCol; j <= endCol; ++j) {
            visited[i][j] = true;
        }
    }
}

std::vector<Rectangle> detectRectangles(const std::vector<std::vector<uint8_t>>& matrix) {
    int rows = matrix.size();
    int cols = matrix[0].size();
    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
    std::vector<Rectangle> rectangles;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (!visited[i][j]) {
                uint8_t val = matrix[i][j];
                int endRow = i;
                int endCol = j;

                // Expand rectangle right
                while (endCol + 1 < cols && matrix[i][endCol + 1] == val) {
                    ++endCol;
                }

                // Expand rectangle down
                bool validRectangle = true;
                while (validRectangle && endRow + 1 < rows) {
                    for (int col = j; col <= endCol; ++col) {
                        if (matrix[endRow + 1][col] != val) {
                            validRectangle = false;
                            break;
                        }
                    }
                    if (validRectangle) {
                        ++endRow;
                    }
                }

                // Mark the found rectangle as visited
                markVisited(visited, i, j, endRow, endCol);

                // Store the rectangle
                rectangles.push_back(Rectangle{ static_cast<float>(j), static_cast<float>(i), static_cast<float>(endCol), static_cast<float>(endRow), val });
            }
        }
    }
    return rectangles;
}



#endif