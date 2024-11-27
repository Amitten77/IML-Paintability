#include <stack>
#include "helper.h"
#include "minimax.h"

struct ProgressTracker {
    size_t depth;
    size_t idx;
    size_t total;
};

std::tuple<short, short, short> getColor(size_t idx, size_t total) {
    // idx = 1: Red
    // idx = total: Green
    if (total <= 1) {
        return std::make_tuple<short, short, short>(0, 255, 0);
    }

    // Ratio
    float t = (float)(idx - 1) / (float)(total - 1);

    // Scale color to have maximum lightness
    if (t < 0.5) {
        return std::make_tuple<short, short, short>(255, (short)(255.0f * t / (1 - t)), 0);
    } else {
        return std::make_tuple<short, short, short>((short)(255.0f * (1 - t) / t), 255, 0);
    }
}

void log(const ProgressTracker& pt, const std::string& msg) {
    static size_t currentDepth = 0;
    static constexpr size_t DEPTH_THRESHOLD = 7;

    // Skip if too deep
    if (pt.depth > DEPTH_THRESHOLD) {
        return;
    }

    // Adjust cursor position
    if (pt.depth > currentDepth) {
        for (size_t i = 0; i < pt.depth - currentDepth; i++) {
            printf("\n");
        }
    } else if (pt.depth < currentDepth) {
        for (size_t i = 0; i < currentDepth - pt.depth; i++) {
            printf("\033[2K\033[F");
        }
        printf("\033[2K\033[G");
    } else {
        printf("\033[2K\033[G");
    }
    currentDepth = pt.depth;

    // Print actual message
    auto [r, g, b] = getColor(pt.idx, pt.total);
    printf("\033[38;2;%d;%d;%dm", r, g, b);
    printf("Depth %zu [%zu/%zu]: %s", pt.depth, pt.idx, pt.total, msg.c_str());
    printf("\033[0m");

    // Flush
    fflush(stdout);
}

Player minimax(
    const GameState& state, Archive& archive, double hoursPerSave, size_t threads, size_t& count,
    const ProgressTracker& pt, std::chrono::time_point<std::chrono::steady_clock>& lastSaveTime) {

    // Save the partial result
    // In case there is an exception, we can still continue from where we left off
    if (hoursPerSave > 0) {
        auto currentTime = std::chrono::steady_clock::now();
        double duration = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - lastSaveTime).count();
        if (duration >= hoursPerSave * 3600) {
            std::filesystem::path filename = getFilename(
                state.getBoard().getN(), state.getBoard().getK(), state.getGoal(), state.isSymmetric(),
                "_" + getCurrentTime());
            archive.saveWinning("winning" / ("temp" / filename));
            archive.saveLosing("losing" / ("temp" / filename));
            lastSaveTime = currentTime;
        }
    }

    // Log current progress
    log(pt, "In progress");
    count++;

    Player currPlayer = state.getCurrentPlayer();

    // 1. If we already know the winner, no need to expand further.
    Player winner = archive.predictWinner(state, threads);
    if (winner != Player::NONE) {
        return winner;
    }

    // 2. Otherwise, expand all possible next states.
    std::vector<GameState> nextStates = state.stepPruned();
    size_t total = nextStates.size();

    // Set winner to the opponent as the default value (because the opponent will win unless we find a winning move for
    // the current player)
    winner = (currPlayer == Player::PUSHER) ? Player::REMOVER : Player::PUSHER;

    for (size_t i = 0; i < total; i++) {
        Player nextWinner = minimax(
            nextStates[i], archive, hoursPerSave, threads, count,
            { pt.depth + 1, i + 1, total }, lastSaveTime);

        // If the current player will win by making this move, then the move is optimal
        // In that case, we know the current player will win since they have the optimal strategy
        if (currPlayer == nextWinner) {
            winner = currPlayer;
            break;
        }

        // Otherwise the current player will lose by making this move, so we continue searching
    }

    // 3. Record the result to the archive if the current player is Pusher
    if (currPlayer == Player::PUSHER) {
        winner == Player::PUSHER ? archive.addWinning(state.getBoard()) : archive.addLosing(state.getBoard());
    }

    return winner;
}

Player minimax(const GameState& startingState, Archive& archive, double hoursPerSave, size_t threads, size_t& count) {
    auto lastSaveTime = std::chrono::steady_clock::now();

    // Find the winner recursively
    Player winner = minimax(startingState, archive, hoursPerSave, threads, count, { 0, 1, 1 }, lastSaveTime);

    // Log end
    printf("\n");

    return winner;
}

// // Original implementation using stack (DFS) instead of recursion
// Player minimax(const GameState& initialState, Archive& archive, double hoursPerSave, size_t threads, size_t& count) {
//     struct GameSnapShot {
//         GameState gameState;  // Current game state
//         GameSnapShot* parent;  // GameSnapShot that leads to this state
//         Player winner;  // Winner of this state (set to Player::None if unknown)
//         bool preventAddingToArchive;
//         ProgressTracker pt;
//     };
//
//     // Use stack for DFS
//     // IMPORTANT: Stacks using deque as the underlying container does not move existing items on the stack. Therefore,
//     //            we can safely store pointers to the GameSnapShot objects.
//     std::stack<GameSnapShot> s;
//     s.push({ initialState, nullptr, Player::NONE, false, { 0, 1, 1 } });
//     count = 0;
//     auto lastSaveTime = std::chrono::steady_clock::now();
//
//     // Final winner of the initial state
//     Player result = Player::NONE;
//
//     // Main loop
//     while (!s.empty()) {
//         // Save the partial result
//         // In case there is an exception, we can still continue from where we left off
//         if (hoursPerSave > 0) {
//             auto currentTime = std::chrono::steady_clock::now();
//             double duration = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - lastSaveTime).count();
//             if (duration >= hoursPerSave * 3600) {
//                 std::filesystem::path filename = getFilename(
//                     initialState.getBoard().getN(), initialState.getBoard().getK(), initialState.getGoal(),
//                     "_" + getCurrentTime());
//                 archive.saveWinning("winning" / ("temp" / filename));
//                 archive.saveLosing("losing" / ("temp" / filename));
//                 lastSaveTime = currentTime;
//             }
//         }
//
//         // Retrieve the current state
//         GameSnapShot& curr = s.top();
//         GameState& gameState = curr.gameState;
//         GameSnapShot* parent = curr.parent;
//
//         // 1. First check if we already know the winner
//         if (curr.winner != Player::NONE) {
//             // If current player is Pusher, add it to the archive
//             if (!curr.preventAddingToArchive && gameState.getCurrentPlayer() == Player::PUSHER) {
//                 if (curr.winner == Player::PUSHER) {
//                     archive.addWinning(gameState.getBoard());
//                 } else {
//                     archive.addLosing(gameState.getBoard());
//                 }
//             }
//
//             // If this is the initial state, then we have the final result.
//             if (!parent) {
//                 result = curr.winner;
//                 s.pop();
//                 continue;
//             }
//
//             // If this is an intermediate state, pass on the result to the parent.
//             if (parent->gameState.getCurrentPlayer() == Player::PUSHER && curr.winner == Player::PUSHER) {
//                 // Then the Pusher should make this move to win
//                 parent->winner = Player::PUSHER;
//             } else if (parent->gameState.getCurrentPlayer() == Player::REMOVER && curr.winner == Player::REMOVER) {
//                 // Then the Remover should make this move to win
//                 parent->winner = Player::REMOVER;
//             } else {
//                 // Do nothing. The parent's winner is set to the opposite player by default.
//             }
//             s.pop();
//             continue;
//         }
//
//         // 2. Then, see if its parent already know its winner. If so, we can skip this case.
//         // Since we set the parent's winner to the opposite player by default, we only need to check if the parent's
//         // winner is the same as its current player.
//         if (parent && parent->winner == parent->gameState.getCurrentPlayer()) {
//             // No need to increment the count here, as we are not actually visiting this state.
//             s.pop();
//             continue;
//         }
//
//         // 3. After that, try to predict the winner using the archive.
//         // Starting from this step we consider this state as visited, thus increment the count.
//         count++;
//         curr.winner = archive.predictWinner(gameState, threads);
//         log(curr.pt, "In progress");
//
//         // 4. If we still don't have the winner, we have to expand all possible next states.
//         if (curr.winner == Player::NONE) {
//             // If winner cannot be predicted, then generate all possible next states and push them onto the stack.
//             std::vector<GameState> nextStates = gameState.stepPruned();
//             size_t total = nextStates.size();
//             for (size_t i = 0; i < total; i++) {
//                 s.push({
//                     std::move(nextStates[total - i - 1]),
//                     &curr,
//                     Player::NONE,
//                     false,
//                     { curr.pt.depth + 1, total - i, total }
//                 });
//             }
//             // Set winner to the opposite player as the default value
//             curr.winner = (gameState.getCurrentPlayer() == Player::PUSHER) ? Player::REMOVER : Player::PUSHER;
//         } else {
//             // If winner can be predicted using existing archive data, then we don't need to record it.
//             curr.preventAddingToArchive = true;
//         }
//     }
//
//     // Log end
//     printf("\n");
//
//     return result;
// }
