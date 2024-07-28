#include "minimax.h"

#include <stack>

Player minimax(const GameState& initialState, const Archive& archive, size_t& count) {
    struct GameSnapShot {
        GameState gameState;  // Current game state
        GameSnapShot* parent;  // GameSnapShot that leads to this state
        Player winner;  // Winner of this state (set to Player::None if unknown)
    };

    // Use stack for DFS
    // IMPORTANT: Stacks using deque as the underlying container does not move existing items on the stack. Therefore,
    //            we can safely store pointers to the GameSnapShot objects.
    std::stack<GameSnapShot> s;
    s.push({ initialState, nullptr, Player::NONE });
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
            // If this is the initial state, then we have the final result.
            if (!parent) {
                result = curr.winner;
                count++;
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
            count++;
            s.pop();
            continue;
        }

        // 2. Then, try to predict the winner using the archive.
        curr.winner = archive.predictWinner(gameState);

        // 3. If we still don't have the state, we have to expand all possible next states.
        if (curr.winner == Player::NONE) {
            // If winner cannot be predicted, then generate all possible next states and push them onto the stack.
            std::vector<GameState> nextStates = gameState.step();
            for (const GameState& nextState: nextStates) {
                s.push({ nextState, &curr, Player::NONE });
            }
            // Set winner to the opposite player as the default value
            curr.winner = (gameState.getCurrentPlayer() == Player::PUSHER) ? Player::REMOVER : Player::PUSHER;
        }
    }

    return result;
}
