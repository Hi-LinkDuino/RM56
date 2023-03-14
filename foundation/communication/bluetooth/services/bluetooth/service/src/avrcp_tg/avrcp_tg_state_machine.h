/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AVRCP_TG_STATE_MACHINE_H
#define AVRCP_TG_STATE_MACHINE_H

#include "avrcp_tg_connection.h"
#include "message.h"
#include "raw_address.h"
#include "state_machine.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief This enumeration declares the events for sending to the state machine.
 */
enum AvrcTgSmEvent {
    // Invalid message.
    AVRC_TG_SM_EVENT_INVALID = 0x00,
    // Transits to the "CONNECTED" state.
    AVRC_TG_SM_EVENT_TO_CONNECTED_STATE,
    // Transits to the "DISCONNECTING" state.
    AVRC_TG_SM_EVENT_TO_DISCONNECTING_STATE,
    // Transits to the "DISABLE" state.
    AVRC_TG_SM_EVENT_TO_DISABLE_STATE,
    // Send the response of the <b>PASS THROUGH</b>.
    AVRC_TG_SM_EVENT_PASS_THROUGH,
    // Send the response of the <b>UNIT INFO"</b>.
    AVRC_TG_SM_EVENT_UNIT_INFO,
    // Send the response of the <b>SUB UNIT INFO</b>.
    AVRC_TG_SM_EVENT_SUB_UNIT_INFO,
    // Send the response of the <b>GetCapabilities</b>.
    AVRC_TG_SM_EVENT_GET_CAPABILITIES,
    // Send the response of the <b>ListPlayerApplicationSettingAttributes</b>.
    AVRC_TG_SM_EVENT_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES,
    // Send the response of the <b>ListPlayerApplicationSettingValues</b>.
    AVRC_TG_SM_EVENT_LIST_PLAYER_APPLICATION_SETTING_VALUES = AVRC_TG_PDU_ID_LIST_PLAYER_APPLICATION_SETTING_VALUES,
    // Send the response of the <b>GetCurrentPlayerApplicationSettingValue</b>.
    AVRC_TG_SM_EVENT_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE =
        AVRC_TG_PDU_ID_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE,
    // Send the response of the <b>SetPlayerApplicationSettingValue</b>.
    AVRC_TG_SM_EVENT_SET_PLAYER_APPLICATION_SETTING_VALUE = AVRC_TG_PDU_ID_SET_PLAYER_APPLICATION_SETTING_VALUE,
    // Send the command of the <b>GetPlayerApplicationSettingAttributeText"</b>.
    AVRC_TG_SM_EVENT_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT,
    // Send the command of the <b>GetPlayerApplicationSettingValueText"</b>.
    AVRC_TG_SM_EVENT_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT,
    // Send the response of the <b>GetElementAttributes"</b>.
    AVRC_TG_SM_EVENT_GET_ELEMENT_ATTRIBTUES,
    // Send the response of the <b>GetPlayStatus"</b>.
    AVRC_TG_SM_EVENT_GET_PLAY_STATUS = AVRC_TG_PDU_ID_GET_PLAY_STATUS,
    // Send the response of the <b>RequestContinuingResponse</b>.
    AVRC_TG_SM_EVENT_REQUEST_CONTINUING_RESPONSE = AVRC_TG_PDU_ID_REQUEST_CONTINUING_RESPONSE,
    // Send the response of the <b>AbortContinuingResponse</b>.
    AVRC_TG_SM_EVENT_ABORT_CONTINUING_RESPONSE = AVRC_TG_PDU_ID_ABORT_CONTINUING_RESPONSE,
    // Send the response of the <b>SetAddressedPlayer</b>.
    AVRC_TG_SM_EVENT_SET_ADDRESSED_PLAYER = AVRC_TG_PDU_ID_SET_ADDRESSED_PLAYER,
    // Send the response of the <b>SetBrowsedPlayer</b>.
    AVRC_TG_SM_EVENT_SET_BROWSED_PLAYER = AVRC_TG_PDU_ID_SET_BROWSED_PLAYER,
    // Send the response of the <b>ChangePath</b>.
    AVRC_TG_SM_EVENT_CHANGE_PATH = AVRC_TG_PDU_ID_CHANGE_PATH,
    // Send the response of the <b>GetFolderItems</b>.
    AVRC_TG_SM_EVENT_GET_FOLDER_ITEMS = AVRC_TG_PDU_ID_GET_FOLDER_ITEMS,
    // Send the response of the <b>GetItemAttributes</b>.
    AVRC_TG_SM_EVENT_GET_ITEM_ATTRIBUTES = AVRC_TG_PDU_ID_GET_ITEM_ATTRIBUTES,
    // Send the response of the <b>GetTotalNumberOfItems</b>.
    AVRC_TG_SM_EVENT_GET_TOTAL_NUMBER_OF_ITEMS = AVRC_TG_PDU_ID_GET_TOTAL_NUMBER_OF_ITEMS,
    // Send the response of the <b>PlayItem</b>.
    AVRC_TG_SM_EVENT_PLAY_ITEM = AVRC_TG_PDU_ID_PLAY_ITEM,
    // Send the response of the <b>AddToNowPlaying</b>.
    AVRC_TG_SM_EVENT_ADD_TO_NOW_PLAYING = AVRC_TG_PDU_ID_ADD_TO_NOW_PLAYING,
    // Send the response of the <b>SetAbsoluteVolume</b>.
    AVRC_TG_SM_EVENT_SET_ABSOLUTE_VOLUME = AVRC_TG_PDU_ID_SET_ABSOLUTE_VOLUME,
    // Send the response of the <b>RegisterNotification"</b>.
    AVRC_TG_SM_EVENT_REGISTER_NOTIFICATION = AVRC_TG_PDU_ID_REGISTER_NOTIFICATION,
    // Send the response of the <b>GENERAL REJECT"</b>.
    AVRC_TG_SM_EVENT_GENERAL_REJECT = AVRC_TG_PDU_ID_GENERAL_REJECT,
};

/**
 * @brief This class provides a set of methods for adding/deleting the state machine and forwarding the
 * messages to the state machine.
 *
 * @detail There are two link(the control link and the browse link) of each bluetooth device. Each link has a state
 * machine for processing the state.
 *             1. The control state machine of the control link.
 *             2. The browse state machine of the browse link.
 *
 * The state in the control state machine:
 *                              CONNECTING
 *                                  |
 *                              CONNECTED
 *                   /           /     \        \                \
 *       DISCONNECTING     PENDING     BUSY     CONTINUATION     SHUTDOWN
 * The state transaction in the control state machine:
 *                  CONNECTING    CONNECTED    DISCONNECTING    PENDING    BUSY    CONTINUATION    SHUTDOWN
 * 1. CONNECTING        -             O              X             X        X           X             O
 * 2. CONNECTED         X             -              O             O        O           O             O
 * 3. DISCONNECTING     X             X              -             X        X           X             O
 * 4. PENDING           X             O              O             -        O           X             O
 * 5. BUSY              X             O              O             X        -           X             O
 * 6. CONTINUATION      X             O              O             X        O           -             O
 * 7. DISABLE           X             X              X             X        X           X             -
 *
 * The state in the browse state machine:
 *                              CONNECTING
 *                                  |
 *                              CONNECTED
 *                        /         |       \
 *            DISCONNECTING      PENDING     BUSY
 * The state transaction in the browse state machine:
 *                  CONNECTING    CONNECTED    DISCONNECTING    PENDING    BUSY
 * 1. CONNECTING        -             O              X             X        X
 * 2. CONNECTED         X             -              O             O        O
 * 3. DISCONNECTING     X             X              -             X        X
 * 4. PENDING           X             O              O             -        O
 * 5. BUSY              X             O              O             X        -
 */
class AvrcTgStateMachineManager {
public:
    /**
     * @brief A destructor used to delete the <b>AvrcTgStateMachineManager</b> instance.
     */
    ~AvrcTgStateMachineManager();

    /**
     * @brief Gets the instance of the <b>AvrcTgStateMachineManager</b> class.
     *
     * @return The instance of the <b>AvrcTgStateMachineManager</b> class.
     */
    static AvrcTgStateMachineManager *GetInstance(void);

    /**
     * @brief Shut down the all statmachine
     */
    void ShutDown(void);

    /**
     * @brief Adds the control state machine of the specified bluetooth address.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR  Execute success.
     * @retval RET_BAD_PARAM Execute failure.
     */
    int AddControlStateMachine(const RawAddress &rawAddr);

    /**
     * @brief Adds the browse state machine of the specified bluetooth address.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The result of the method execution.
     * @retval RET_NO_ERROR  Execute success.
     * @retval RET_BAD_PARAM Execute failure.
     */
    int AddBrowseStateMachine(const RawAddress &rawAddr);

    /**
     * @brief Deletes the pair of state machines of the specified bluetooth address.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void DeletePairOfStateMachine(const RawAddress &rawAddr);

    /**
     * @brief Deletes the browse state machine of the specified bluetooth address.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     */
    void DeleteBrowseStateMachine(const RawAddress &rawAddr);

    /**
     * @brief Sends the message to the control state machine of the specified bluetooth address.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] msg     The message that need to be sent.
     * @return The result of the method execution.
     * @retval true  The message is processed.
     * @retval false The message is not processed.
     */
    bool SendMessageToControlStateMachine(const RawAddress &rawAddr, const utility::Message &msg);

    /**
     * @brief Sends messages to all of the control state machine.
     *
     * @param[in] msg The message that need to be sent.
     */
    void SendMessageToAllControlStateMachine(const utility::Message &msg);

    /**
     * @brief Sends messages to the browse state machine.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @param[in] msg     The message that need to be sent.
     * @return The result of the method execution.
     * @retval true  The message is processed.
     * @retval false The message is not processed.
     */
    bool SendMessageToBrowseStateMachine(const RawAddress &rawAddr, const utility::Message &msg);

    /**
     * @brief Sends messages to all of the browse state machine.
     *
     * @param[in] msg The message that need to be sent.
     */
    void SendMessageToAllBrowseStateMachine(const utility::Message &msg);

    /**
     * @brief Checks the current state is "CONNECTING" or not.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The result of the method execution.
     * @retval true  Current state is "CONNECTING".
     * @retval false Current state is not "CONNECTING".
     */
    bool IsControlConnectingState(const RawAddress &rawAddr);

    /**
     * @brief Checks the current state is "CONNECTED" or not.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The result of the method execution.
     * @retval true  Current state is "CONNECTED".
     * @retval false Current state is not "CONNECTED".
     */
    bool IsControlConnectedState(const RawAddress &rawAddr);

    /**
     * @brief Checks the current state is "DISCONNECTING" or not.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The result of the method execution.
     * @retval true  Current state is "DISCONNECTING".
     * @retval false Current state is not "DISCONNECTING".
     */
    bool IsControlDisconnectingState(const RawAddress &rawAddr);

    /**
     * @brief Checks the current state is "CONTINUATION" or not.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The result of the method execution.
     * @retval true  Current state is "CONTINUATION".
     * @retval false Current state is not "CONTINUATION".
     */
    bool IsControlContinuationState(const RawAddress &rawAddr);

    /**
     * @brief Checks the current state is "DISABLE" or not.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The result of the method execution.
     * @retval true  Current state is "DISABLE".
     * @retval false Current state is not "DISABLE".
     */
    bool IsControlDisableState(const RawAddress &rawAddr);

private:
    /**
     * @brief This class provides a set of methods for adding/deleting/transiting the state and processing the
     * messages that are sent to the state machine.
     */
    class StateMachine : public utility::StateMachine {
    public:
        /**
         * @brief This enumeration declares types that are used to distinguish between the control state machine and the
         * browse state machine.
         */
        enum class Type {
            AVRC_TG_SM_TYPE_CONTROL,  // The control state machine.
            AVRC_TG_SM_TYPE_BROWSE,   // The browse state machine.
        };

        /**
         * @brief A constructor used to create an <b>StateMachine</b> instance.
         */
        StateMachine(Type type, const RawAddress &rawAddr);

        /**
         * @brief A destructor used to delete the <b>StateMachine</b> instance.
         */
        ~StateMachine() = default;

        /**
         * @brief Initializes the states.
         */
        void AddStates(void);

    private:
        // The type of the state machine.
        Type type_;
        // The address of the bluetooth device.
        RawAddress rawAddr_;

        /**
         * @brief A constructor used to create an <b>StateMachine</b> instance.
         */
        StateMachine() = delete;

        class State : public utility::StateMachine::State {
        public:
            explicit State(const std::string &name, utility::StateMachine &stateMachine, const RawAddress &rawAddr);
            explicit State(const std::string &name, utility::StateMachine &stateMachine, const RawAddress &rawAddr,
                utility::StateMachine::State &parent);
            virtual ~State();

        protected:
            // The address of the bluetooth device.
            RawAddress rawAddr_;

        private:
            State() = delete;
        };

        /**
         * @brief This class provides a set of methods for entry/exit the "CONNECTING" state of the control state
         * machine and processing messages that are sent to the state.
         */
        class CtConnecting : public State {
        public:
            /**
             * @brief A constructor used to create an <b>CtConnecting</b> instance.
             */
            CtConnecting(const std::string &name, utility::StateMachine &stateMachine, const RawAddress &rawAddr);

            /**
             * @brief A destructor used to delete the <b>CtConnecting</b> instance.
             */
            ~CtConnecting(void) override = default;

            /**
             * @brief Enters the state.
             */
            void Entry(void) override;

            /**
             * @brief Exits the state.
             */
            void Exit(void) override;

            /**
             * @brief Processes the required messages and send the unwanted to the parent state.
             *
             * @param[in] msg The message that need to be sent.
             * @return The result of the method execution.
             * @retval true  The message is processed.
             * @retval false The message is not processed.
             */
            bool Dispatch(const utility::Message &msg) override;

        private:
            /**
             * @brief A deleted default constructor.
             */
            CtConnecting(void) = delete;
        };

        /**
         * @brief This class provides a set of methods for entry/exit the "CONNECTED" state of the control state
         * machine and processing messages that are sent to the state.
         */
        class CtConnected : public State {
        public:
            /**
             * @brief A constructor used to create an <b>CtConnected</b> instance.
             */
            CtConnected(const std::string &name, utility::StateMachine &stateMachine, const RawAddress &rawAddr,
                utility::StateMachine::State &parent);

            /**
             * @brief A destructor used to delete the <b>CtConnected</b> instance.
             */
            ~CtConnected(void) override = default;

            /**
             * @brief Enters the state.
             */
            void Entry(void) override;

            /**
             * @brief Exits the state.
             */
            void Exit(void) override;

            /**
             * @brief Processes required messages and send unwanted messages to the parent state.
             *
             * @param[in] msg The message that need to be sent.
             * @return The result of the method execution.
             * @retval true  The message is processed.
             * @retval false The message is not processed.
             */
            bool Dispatch(const utility::Message &msg) override;

        private:
            /**
             * @brief A deleted default constructor.
             */
            CtConnected(void) = delete;

            /**
             * @brief Processes events sent by profile.
             */
            void ToPassThroughEvent();
            void ToUnitInfoEvent();
            void ToCommonEvent();
            void ToAbortContinuingEvent();
        };

        /**
         * @brief This class provides a set of methods for entry/exit the "DISCONNECTING" state of the control state
         * machine and processing messages that are sent to the state.
         */
        class CtDisconnecting : public State {
        public:
            /**
             * @brief A constructor used to create an <b>CtDisconnecting</b> instance.
             */
            CtDisconnecting(const std::string &name, utility::StateMachine &stateMachine, const RawAddress &rawAddr,
                utility::StateMachine::State &parent);

            /**
             * @brief A destructor used to delete the <b>CtDisconnecting</b> instance.
             */
            ~CtDisconnecting(void) override = default;

            /**
             * @brief Enters the state.
             */
            void Entry(void) override;

            /**
             * @brief Exits the state.
             */
            void Exit(void) override;

            /**
             * @brief Processes required messages and send unwanted messages to the parent state.
             *
             * @param[in] msg The message from the control state machine.
             * @return The result of the method execution.
             * @retval true  The message is processed.
             * @retval false The message is not processed.
             */
            bool Dispatch(const utility::Message &msg) override;

        private:
            /**
             * @brief A deleted default constructor.
             */
            CtDisconnecting(void) = delete;
        };

        /**
         * @brief This class provides a set of methods for entry/exit the "CONTINUATION" state of the control state
         * machine and processing messages that are sent to the state.
         */
        class CtContinuation : public State {
        public:
            /**
             * @brief A constructor used to create an <b>CtContinuation</b> instance.
             */
            CtContinuation(const std::string &name, utility::StateMachine &stateMachine, const RawAddress &rawAddr,
                utility::StateMachine::State &parent);

            /**
             * @brief A destructor used to delete the <b>CtContinuation</b> instance.
             */
            ~CtContinuation(void) override = default;

            /**
             * @brief Enters the state.
             */
            void Entry(void) override;

            /**
             * @brief Exits the state.
             */
            void Exit(void) override;

            /**
             * @brief Processes required messages and send unwanted messages to the parent state.
             *
             * @param[in] msg The message from the control state machine.
             * @return The result of the method execution.
             * @retval true  The message is processed.
             * @retval false The message is not processed.
             */
            bool Dispatch(const utility::Message &msg) override;

        private:
            /**
             * @brief A deleted default constructor.
             */
            CtContinuation(void) = delete;
        };

        /**
         * @brief This class provides a set of methods for entry/exit the "DISABLE" state of the control state
         * machine and processing messages that are sent to the state.
         */
        class CtDisable : public State {
        public:
            /**
             * @brief A constructor used to create an <b>CtDisable</b> instance.
             */
            CtDisable(const std::string &name, utility::StateMachine &stateMachine, const RawAddress &rawAddr,
                utility::StateMachine::State &parent);

            /**
             * @brief A destructor used to delete the <b>CtDisable</b> instance.
             */
            ~CtDisable(void) override = default;

            /**
             * @brief Enters the state.
             */
            void Entry(void) override;

            /**
             * @brief Exits the state.
             */
            void Exit(void) override;

            /**
             * @brief Processes required messages and send unwanted messages to the parent state.
             *
             * @param[in] msg The message from the control state machine.
             * @return The result of the method execution.
             * @retval true  The message is processed.
             * @retval false The message is not processed.
             */
            bool Dispatch(const utility::Message &msg) override;

        private:
            /**
             * @brief A deleted default constructor.
             */
            CtDisable(void) = delete;
        };

        /**
         * @brief This class provides a set of methods for entry/exit the "CONNECTING" state of the browse state
         * machine and processing messages that are sent to the state.
         */
        class BrConnecting : public State {
        public:
            /**
             * @brief A constructor used to create an <b>BrConnecting</b> instance.
             */
            BrConnecting(const std::string &name, utility::StateMachine &stateMachine, const RawAddress &rawAddr);

            /**
             * @brief A destructor used to delete the <b>BrConnecting</b> instance.
             */
            ~BrConnecting(void) override = default;

            /**
             * @brief Enters the state.
             */
            void Entry(void) override;

            /**
             * @brief Exits the state.
             */
            void Exit(void) override;

            /**
             * @brief Processes required messages and send unwanted messages to the parent state.
             *
             * @param[in] msg The message from the browse state machine.
             * @return The result of the method execution.
             * @retval true  The message is processed.
             * @retval false The message is not processed.
             */
            bool Dispatch(const utility::Message &msg) override;

        private:
            /**
             * @brief A deleted default constructor.
             */
            BrConnecting(void) = delete;
        };

        /**
         * @brief This class provides a set of methods for entry/exit the "CONNECTED" state of the browse state
         * machine and processing messages that are sent to the state.
         */
        class BrConnected : public State {
        public:
            /**
             * @brief A constructor used to create an <b>BrConnected</b> instance.
             */
            BrConnected(const std::string &name, utility::StateMachine &stateMachine, const RawAddress &rawAddr,
                utility::StateMachine::State &parent);

            /**
             * @brief A destructor used to delete the <b>BrConnected</b> instance.
             */
            ~BrConnected(void) override = default;

            /**
             * @brief Enters the state.
             */
            void Entry(void) override;

            /**
             * @brief Exits the state.
             */
            void Exit(void) override;

            /**
             * @brief Processes required messages and send unwanted messages to the parent state.
             *
             * @param[in] msg The message from the browse state machine.
             * @return The result of the method execution.
             * @retval true  The message is processed.
             * @retval false The message is not processed.
             */
            bool Dispatch(const utility::Message &msg) override;

        private:
            /**
             * @brief A deleted default constructor.
             */
            BrConnected(void) = delete;
        };

        /**
         * @brief This class provides a set of methods for entry/exit the "DISCONNECTING" state of the browse state
         * machine and processing messages that are sent to the state.
         */
        class BrDisconnecting : public State {
        public:
            /**
             * @brief A constructor used to create an <b>BrDisconnecting</b> instance.
             */
            BrDisconnecting(const std::string &name, utility::StateMachine &stateMachine, const RawAddress &rawAddr,
                utility::StateMachine::State &parent);

            /**
             * @brief A destructor used to delete the <b>BrDisconnecting</b> instance.
             */
            ~BrDisconnecting(void) override = default;

            /**
             * @brief Enters the state.
             */
            void Entry(void) override;

            /**
             * @brief Exits the state.
             */
            void Exit(void) override;

            /**
             * @brief Processes required messages and send unwanted messages to the parent state.
             *
             * @param[in] msg The message from the browse state machine.
             * @return The result of the method execution.
             * @retval true  The message is processed.
             * @retval false The message is not processed.
             */
            bool Dispatch(const utility::Message &msg) override;

        private:
            /**
             * @brief A deleted default constructor.
             */
            BrDisconnecting(void) = delete;
        };
    };

    // This type indicates the pair of the state machine.
    using StateMachinePair = std::pair<std::unique_ptr<StateMachine>, std::unique_ptr<StateMachine>>;

    // Saves the state machines of the different devices.
    std::map<std::string, StateMachinePair> stateMachines_ {};
    // using a local lock_guard to lock mutex guarantees unlocking on destruction / exception:
    std::mutex mutex_ {};

    /**
     * @brief A constructor used to create an <b>AvrcTgStateMachineManager</b> instance.
     */
    AvrcTgStateMachineManager();

    /**
     * @brief Gets the pair of state machines of the specified bluetooth address.
     *
     * @param[in] addr The address of the bluetooth device.
     * @return The pair of state machines of the specified bluetooth address..
     */
    StateMachinePair *GetPairOfStateMachine(const std::string &addr);

    /**
     * @brief Checks the current state is specified state or not.
     *
     * @param[in] rawAddr The address of the bluetooth device.
     * @return The result of the method execution.
     * @retval true  Current state is the specified state.
     * @retval false Current state is not the specified state.
     */
    bool IsControlSpecifiedState(const std::string &addr, std::string stateName);
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // !AVRCP_TG_STATE_MACHINE_H
