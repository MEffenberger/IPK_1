/**
 * @file FSMValidate.h
 *
 * Declarations of the FSMValidate class
 * Enums representing the actions, states and methods for validating the actions of the client
 *
 * @Author Marek Effenberger
 */

#ifndef IPK_1_FSM_VALIDATE_H
#define IPK_1_FSM_VALIDATE_H

#include <map>
#include <vector>

/**
 * FSMValidate class
 *
 * This class is used to validate the actions of the client
 */
class FSMValidate {
public:
    /// Constructor, default state is START
    FSMValidate() : currentState(State::START) {} // constructor
    ~FSMValidate() = default;

    /// Enums representing the actions
    enum class Action {
        EMPTY_SERVER,
        EMPTY_USER,
        AUTHORIZE_USER,
        JOIN_USER,
        MESSAGE_USER,
        MESSAGE_SERVER,
        BYE_USER,
        BYE_SERVER,
        ERROR_USER,
        ERROR_SERVER,
        REPLY_SERVER,
        NREPLY_SERVER,
        ANY_USER,
        ANY_SERVER,
        ANY,
        WARN_CLIENT,
        OVER
    };

    /**
     * Validate the action
     * @param action action to validate
     * @return feasible action
     */
    Action validate_action(Action action);



private:

    /// Enums representing the states
    enum class State {
        START,
        AUTH,
        OPEN,
        ERROR,
        END
    };

    /// Enums representing the source of the action
    enum class Source {
        USER,
        SERVER
    };

    /// State variables
    State currentState;
    State previousState;

    /// Map of the expected actions based on the previous action
    std::vector<Action> expectedAction;

    /**
     * Check if the action is expected
     * @param action action to check
     * @return true if the action is expected, false otherwise
     */
    bool check_expected_action(Action action);

    /**
     * Get the source of the action
     * @param action action to check
     * @return source of the action
     */
    Source get_source(Action action);

};


#endif //IPK_1_FSM_VALIDATE_H
