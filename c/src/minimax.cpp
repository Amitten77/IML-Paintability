#include <stack>
#include "minimax.h"

struct ProgressTracker {
    size_t depth;
    size_t idx;
    size_t total;
};

size_t getFreq(size_t total) {
    (void)total;
    return 1;
//    if (total > 1000) {
//        return 100;
//    } else if (total > 100) {
//        return 10;
//    } else if (total > 20) {
//        return 5;
//    } else {
//        return 1;
//    }
}

void log(const ProgressTracker& pt, const std::string& msg) {
    // Skip if too deep
    if (pt.depth > 5) {
        return;
    }

    if (pt.idx % getFreq(pt.total) == 0 || pt.idx == pt.total) {
        for (size_t i = 0; i < pt.depth; i++) {
            printf("  ");
        }
        printf("[%zu: %lu/%lu] %s\n", pt.depth, pt.idx, pt.total, msg.c_str());
    }
}

Player minimax(const GameState& initialState, Archive& archive, size_t& count) {
    struct GameSnapShot {
        GameState gameState;  // Current game state
        GameSnapShot* parent;  // GameSnapShot that leads to this state
        Player winner;  // Winner of this state (set to Player::None if unknown)
        bool preventAddingToArchive;
        ProgressTracker pt;
    };

    // Use stack for DFS
    // IMPORTANT: Stacks using deque as the underlying container does not move existing items on the stack. Therefore,
    //            we can safely store pointers to the GameSnapShot objects.
    std::stack<GameSnapShot> s;
    s.push({ initialState, nullptr, Player::NONE, false, { 0, 1, 1 } });
    count = 0;

    // Final winner of the initial state
    Player result = Player::NONE;

    // Main loop
    while (!s.empty()) {
        GameSnapShot& curr = s.top();
        GameState& gameState = curr.gameState;
        GameSnapShot* parent = curr.parent;

        // 1. First check if we already know the winner
        if (curr.winner != Player::NONE) {
            log(curr.pt, "Completed");

            // If current player is Pusher, add it to the archive
            if (!curr.preventAddingToArchive && gameState.getCurrentPlayer() == Player::PUSHER) {
                if (curr.winner == Player::PUSHER) {
                    archive.addWinning(gameState.getBoard());
                } else {
                    archive.addLosing(gameState.getBoard());
                }
            }

            // If this is the initial state, then we have the final result.
            if (!parent) {
                result = curr.winner;
                s.pop();
                continue;
            }

            // If this is an intermediate state, pass on the result to the parent.
            if (parent->gameState.getCurrentPlayer() == Player::PUSHER && curr.winner == Player::PUSHER) {
                // Then the Pusher should make this move to win
                parent->winner = Player::PUSHER;
            } else if (parent->gameState.getCurrentPlayer() == Player::REMOVER && curr.winner == Player::REMOVER) {
                // Then the Remover should make this move to win
                parent->winner = Player::REMOVER;
            } else {
                // Do nothing. The parent's winner is set to the opposite player by default.
            }
            s.pop();
            continue;
        }

        // 2. Then, see if its parent already know its winner. If so, we can skip this case.
        // Since we set the parent's winner to the opposite player by default, we only need to check if the parent's
        // winner is the same as its current player.
        if (parent && parent->winner == parent->gameState.getCurrentPlayer()) {
            // No need to increment the count here, as we are not actually visiting this state.
            s.pop();
            continue;
        }

        // 3. After that, try to predict the winner using the archive.
        // Starting from this step we consider this state as visited, thus increment the count.
        count++;
        curr.winner = archive.predictWinner(gameState);
        log(curr.pt, "Begin");

        // 4. If we still don't have the state, we have to expand all possible next states.
        if (curr.winner == Player::NONE) {
            // If winner cannot be predicted, then generate all possible next states and push them onto the stack.
            std::vector<GameState> nextStates = gameState.stepPruned();
            size_t total = nextStates.size();
            for (size_t i = 0; i < total; i++) {
                s.push({
                    std::move(nextStates[total - i - 1]),
                    &curr,
                    Player::NONE,
                    false,
                    { curr.pt.depth + 1, total - i, total }
                });
            }
            // Set winner to the opposite player as the default value
            curr.winner = (gameState.getCurrentPlayer() == Player::PUSHER) ? Player::REMOVER : Player::PUSHER;
        } else {
            // If winner can be predicted using existing archive data, then we don't need to record it.
            curr.preventAddingToArchive = true;
        }
    }

    return result;
}
