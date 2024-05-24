"""
    Copyright 2022 Continental Corporation

    :file: controlbridge_Variables.py
    :platform: Windows, Linux
    :synopsis:
        Script containing only the variables for the software-tests from the controlbridge

    :author:
        - Fabian Janker (uif41320) <fabian.janker@continental-corporation.com>
"""
import json
from enum import Enum

# custom import area
from helper_files.General import global_Variables

# Address of the ControlBridge
CONTROLBRIDGE_IP_ADDRESS = "127.0.0.1"
CONTROLBRIDGE_PORT = "8200"


# Messages Controlbridge
# ---------------------------------------------------------------------------------------------------
# Message to load a z-recording
def get_load_recording_msg(path):
    return (
        '{"channel": "player","event": "UserRequestedInputOpen","source": "NextGUI","payload": {"fileName": "'
        + path
        + '", "mode": "OLS"}}'
    )


class TimestampType(Enum):
    ABSOLUTE = "Absolute"
    RELATIVE = "Relative"


def get_jump_to_timestamp_msg(
    timestamp,
    timestamp_type=TimestampType.ABSOLUTE.value,
    stream_frames_in_between="false",
):
    message = {
        "channel": "player",
        "event": "UserRequestedJumpToTimestamp",
        "source": "NextGUI",
        "payload": {
            "requestedTimeStamp": timestamp,
            "type": timestamp_type,
            "streamFramesInBetween": stream_frames_in_between,
        },
    }
    return json.dumps(message, indent=4)


CONTROLBRIDGE_LOAD_RECORDING = get_load_recording_msg(global_Variables.recording_dir())
CONTROLBRIDGE_LOAD_ZRECORDING = get_load_recording_msg(global_Variables.recording_dir(is_zrec=True))

# Message to close a recording
CONTROLBRIDGE_CLOSE_RECORDING = (
    '{"channel": "player",' '"event": "UserPressedInputClearButton",' '"source": "NextGUI",' '"payload": {}}'
)

# Message to start a recording
CONTROLBRIDGE_PLAY_RECORDING = (
    '{"channel": "player",' '"event": "UserPressedPlayButton",' '"source": "NextGUI",' '"payload": {"forward": true}}'
)
# Message to pause a recording
CONTROLBRIDGE_PAUSE_RECORDING = (
    '{"channel": "player",' '"event": "UserPressedPauseButton",' '"source": "NextGUI",' '"payload": {}}'
)
# Message to jump to beginning
CONTROLBRIDGE_JUMP_TO_BEGINNING = (
    '{"channel": "player",' '"event": "UserPressedJumpToBeginButton",' '"source": "NextGUI",' '"payload": {}}'
)


# Message to make n Steps
def controlbridge_step_n(steps):
    return (
        '{"channel": "player",'
        '"event": "UserPressedStepButton",'
        '"source": "NextGUI",'
        '"payload": {"requestedSteps": ' + steps + "}}"
    )


# Channel Controlbridge
# ---------------------------------------------------------------------------------------------------
# Channel: Player
RECEIVE_CHANNEL_PLAYER = "player"
# Channel: Logger
RECEIVE_CHANNEL_LOGGER = "logger"

# Events Controlbridge
# ---------------------------------------------------------------------------------------------------
# Event: loaded a Recording
EXPECTED_EVENT_RECORDING_LOADED = "RecordingIsLoaded"
# Event: playing a Recording
EXPECTED_EVENT_PLAYBACK_PLAYING = "PlaybackIsPlaying"
# Event: pausing a Recording
EXPECTED_EVENT_PLAYBACK_PAUSED = "PlaybackIsPaused"
# Event: no recording is loaded
EXPECTED_EVENT_RECORDING_CLOSED = "SimulationStateIsIdle"
# Event: end of the file
EXPECTED_EVENT_END_OF_FILE = "PlaybackIsPaused"
# Event: jump to beginning
EXPECTED_EVENT_JUMP_TO_BEGINNING = "PlaybackIsPaused"
# Event: jump to timestmap
EXPECTED_EVENT_JUMP_TO_TIMESTAMP = "PlaybackJumped"

# Source Controlbridge
# ---------------------------------------------------------------------------------------------------
# Source: loaded a Recording
EXPECTED_SOURCE_RECORDING_LOADED = "NextBridge"

# Payload Controlbridge
# ---------------------------------------------------------------------------------------------------
# Payload: toleration of end of file (50 ms)
TOLERANCE_PAYLOAD_END_OF_FILE = 50000
TOLERANCE_PAYLOAD_PAUSE_OF_FILE = 50000
