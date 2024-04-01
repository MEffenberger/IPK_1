/**
 * @file FSMValidate.cpp
 *
 * Implementation of the FSMValidate class
 * By a set of enums validating the actions of the client
 *
 * @Author Marek Effenberger
 */

#include "FSMValidate.h"


bool FSMValidate::check_expected_action(Action action) {
    // Searching for the action in the expected actions
    for (Action a : expectedAction) {
        if (a == action) {
            return true;
        }
    }
    // Shouldn't be needed
    if (expectedAction[0] == Action::ANY) {
        return true;
    }
    return false;
}

FSMValidate::Source FSMValidate::get_source(Action action) {
    // Enums concerning server
    if (action == Action::EMPTY_SERVER || action == Action::MESSAGE_SERVER || action == Action::BYE_SERVER || action == Action::ERROR_SERVER || action == Action::REPLY_SERVER || action == Action::NREPLY_SERVER) {
        return Source::SERVER;
    }
    // Enums concerning user
    if (action == Action::EMPTY_USER || action == Action::AUTHORIZE_USER || action == Action::JOIN_USER || action == Action::MESSAGE_USER || action == Action::BYE_USER || action == Action::ERROR_USER) {
        return Source::USER;
    }
}


FSMValidate::Action FSMValidate::validate_action(Action action) {


    // Returning action based on the expected action
    if (!expectedAction.empty() && expectedAction[0] != Action::ANY) {

        if (!check_expected_action(action)) {
            if (get_source(action) == Source::USER) {
                expectedAction = {Action::ANY};
                currentState = previousState;
                return Action::WARN_CLIENT;

            } else {
                expectedAction = {Action::BYE_USER};
                currentState = State::ERROR;
                return Action::ERROR_USER;
            }

        } else {
            expectedAction = {Action::ANY};
            return Action::ANY;
        }
    }

    // Switch statement for the current state
    switch (currentState) {
        case State::START:
            previousState = State::START;

            if (action == Action::AUTHORIZE_USER) {
                expectedAction = {Action::ANY};
                currentState = State::AUTH;
                return Action::ANY;

            } else {

                if (get_source(action) == Source::USER) {
                    expectedAction = {Action::ANY};
                    currentState = State::START;
                    return Action::WARN_CLIENT;
                } else {
                    expectedAction = {Action::BYE_USER};
                    currentState = State::ERROR;
                    return Action::ERROR_USER;
                }
            }
            break;
        case State::AUTH:
            previousState = State::AUTH;

            if (action == Action::AUTHORIZE_USER) {
                expectedAction = {Action::REPLY_SERVER, Action::NREPLY_SERVER, Action::ERROR_SERVER};
                currentState = State::AUTH;
                return Action::ANY;

            } else if (action ==  Action::BYE_USER) {
                currentState = State::END;
                expectedAction = {Action::EMPTY_SERVER, Action::ERROR_SERVER};
                return Action::OVER;

            } else if (action == Action::ERROR_SERVER) {
                currentState = State::END;
                expectedAction = {Action::BYE_USER};
                return Action::ANY;

            } else if (action == Action::REPLY_SERVER) {
                currentState = State::OPEN;
                expectedAction = {Action::ANY};
                return Action::ANY;

            } else if (action == Action::NREPLY_SERVER) {
                currentState = State::AUTH;
                expectedAction = {Action::AUTHORIZE_USER};
                return Action::ANY;

            } else {

                if (get_source(action) == Source::USER) {
                    expectedAction = {Action::ANY};
                    currentState = State::AUTH;
                    return Action::WARN_CLIENT;

                } else {
                    expectedAction = {Action::BYE_USER};
                    currentState = State::ERROR;
                    return Action::ERROR_USER;
                }
            }

            break;

        case State::OPEN:
            previousState = State::OPEN;

            if (action == Action::MESSAGE_SERVER || action == Action::REPLY_SERVER || action == Action::NREPLY_SERVER){
                expectedAction = {Action::ANY};
                currentState = State::OPEN;
                return Action::ANY;

            } else if (action == Action::JOIN_USER || action == Action::MESSAGE_USER){
                expectedAction = {Action::ANY};
                currentState = State::OPEN;
                return Action::ANY;
            }

            else if (action == Action::ERROR_SERVER) {
                currentState = State::END;
                expectedAction = {Action::BYE_USER};
                return Action::ANY;
            }

            else if (action == Action::BYE_SERVER) {
                currentState = State::END;
                expectedAction = {Action::ANY};
                return Action::ANY;
            }

            else if (action == Action::BYE_USER) {
                currentState = State::END;
                expectedAction = {Action::ERROR_SERVER};
                return Action::ANY;
            }

            else if (action == Action::ERROR_USER) {
                currentState = State::END;
                expectedAction = {Action::BYE_SERVER};
                return Action::ANY;
            }

            else {
                if (get_source(action) == Source::USER) {
                    expectedAction = {Action::ANY};
                    return Action::WARN_CLIENT;
                } else {
                    expectedAction = {Action::BYE_USER};
                    currentState = State::ERROR;
                    return Action::ERROR_USER;
                }
            }
            break;

        case State::ERROR:
            previousState = State::ERROR;

            if (action == Action::BYE_USER) {
                currentState = State::END;
                return Action::ANY;
            }
            break;

        case State::END:
            previousState = State::END;

            return Action::OVER;
    }
}

