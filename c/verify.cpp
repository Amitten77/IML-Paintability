#include <queue>
#include "Board.h"
#include "board_operation.h"
#include "compare.h"
#include "helper.h"

// Verify the correctness of Pusher strategy
// 1. We provide the starting state and a list of "winning states" to the system.
// 2. The system will generate all possible moves of the Pusher and Remover and check if the Pusher can guarantee the
//    next game state is a winning state.
// 3. If not, then our results are incorrect (does not necessarily mean that the Pusher will lose though).
// 4. If yes, then let the Pusher make that move and continue to verify the resulting states (6 in total).
// 5. The check terminates when all paths lead to the Pusher winning (actually winning, not by comparing to
//    the list of "winning states").
void verifyPusherWin(const Board& board, const std::vector<Board>& winningStates) {
    std::queue<Board> boards;
    boards.push(board);

//    int pusherWinTarget = board.goal;
//    int removerWinTarget = board.num_tokens;
//    int pusherWinProgress = -1;
//    int removerWinProgress = -1;

    while (!boards.empty()) {
        Board curr = boards.front();
        boards.pop();
        // printf("%s\n", curr.serialize().c_str());

        // If already won, no need to continue
        if (checkWinner(curr) == Player::PUSHER) break;

        bool foundWinningMove = false;
        // Find all possible moves of the pusher
        std::vector<PusherMove> pusherMoves;
        getAllPusherMovesPruned(curr, pusherMoves);
        for (const PusherMove& pusherMove : pusherMoves) {
            // Apply pusher's move
            Board afterPusher = curr;
            applyPusherMove(afterPusher, pusherMove);

            foundWinningMove = true;
            std::vector<Board> nextStates;

            // Find all possible moves of the remover
            std::vector<int> removerMoves;
            getAllRemoverMovesPruned(curr, removerMoves);
            for (int removerMove : removerMoves) {
                Board afterRemover = afterPusher;
                applyRemoverMove(afterRemover, removerMove);

                // If pusher will win, remover should not take this move
                if (checkWinner(afterRemover) == Player::PUSHER) continue;

                // Add to list of potential next states
                nextStates.push_back(afterRemover);

                // If this board is not a "winning state", Pusher should not take this move.
                bool isWinning = boardIsWinning(afterRemover, winningStates);
                if (!isWinning) {
                    foundWinningMove = false;
                    break;
                }
            }

            // If pusher's move guarantees reaching a winning state, then no need to search other moves.
            if (foundWinningMove) {
                // Insert all generated state to the back of the queue
                for (const Board& next : nextStates) {
                    boards.push(next);
                }
                break;
            }
        }

        // If all moves lead to non-winning state, then the pusher fails
        if (!foundWinningMove) {
            printf("Failed to find a winning move for:\n%s\n", curr.serialize().c_str());
            printf("Verification aborted.\n");
            // No winning move for `curr`
            return;
        }
    }

    printf("Verified Pusher strategy against all possible Remover strategies.\n");
    printf("Verification successful.\n");
}

// Not implemented
void verifyRemoverWin(Board board, int goal);

int main() {
    int N = 4;
    int K = 3;
    int GOAL = 4;

    // Load the winning states
    std::stringstream winning_ss;
    winning_ss << "winning/N" << N << "_K" << K << "_goal" << GOAL << "_board.txt";
    std::string WINNING_FILE = winning_ss.str();

    std::vector<Board> winningBoard;
    loadBoardsFromFile(WINNING_FILE, winningBoard);

    // Verify
    Board board(N, K, GOAL, {
            {{0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}},
            {{0, 0}, {0, 0}, {0, 0}},
//            {{3, 0}, {3, 0}, {3, 0}},
//            {{3, 0}, {3, 0}, {3, 0}}
    });

    verifyPusherWin(board, winningBoard);
}
