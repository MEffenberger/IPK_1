//
// Created by marek on 17.03.2024.
//

#ifndef IPK_1_FSM_VALIDATE_H
#define IPK_1_FSM_VALIDATE_H

#include <map>

class FSMValidate {
public:

    FSMValidate() : currentState(State::START) {} // constructor

private:

    enum class State {
        START,
        AUTH,
        OPEN,
        ERROR,
        END
    };

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

    enum class Source {
        USER,
        SERVER
    };

    State currentState;
    State previousState;

    std::vector<Action> expectedAction;

    bool check_expected_action(Action action);

    Source get_source(Action action);
    Action validate_action();

//    Action pendingUserInputAction;
//    Action pendingServerMessageAction;
//    bool pendingUserInput;
//    bool pendingServerMessage;
//
//    void check_pending_states();
//    void error_message();

};


#endif //IPK_1_FSM_VALIDATE_H
