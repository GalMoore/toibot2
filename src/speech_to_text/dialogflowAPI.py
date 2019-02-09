#!/home/gal/ToiBotEnv/bin/python
from sys import byteorder
from array import array
from struct import pack
import dialogflow_v2 as dialogflow
import pyaudio
import wave
# from ohbot import ohbot
import time
import sys
# import rospy

''' PYTHON 3 CODE THAT CONVERTS WAV 
TO STRING AND QUERIES DIALOGFLOW FOR INTENT & RESULT '''

def detect_intent_audio():
    """Returns the result of detect intent with an audio file as input.
    Using the same `session_id` between requests allows continuation
    of the conversation."""
    audio_file_path = "/home/gal/toibot_ws/src/ToiBot1/src/speech_to_text/speech_wavs/filename.wav"
    session_client = dialogflow.SessionsClient()

    # Note: hard coding audio_encoding and sample_rate_hertz for simplicity.
    audio_encoding = dialogflow.enums.AudioEncoding.AUDIO_ENCODING_LINEAR_16
    sample_rate_hertz = 16000

    session = session_client.session_path("toibot-1549026967633", "gal1")
    print('Session path: {}\n'.format(session))

    with open(audio_file_path, 'rb') as audio_file:
        input_audio = audio_file.read()

    audio_config = dialogflow.types.InputAudioConfig(
        audio_encoding=audio_encoding, language_code="en",
        sample_rate_hertz=sample_rate_hertz)

    query_input = dialogflow.types.QueryInput(audio_config=audio_config)

    response = session_client.detect_intent(
        session=session, query_input=query_input,
        input_audio=input_audio)

    print('=' * 20)
    print('Query text: {}'.format(response.query_result.query_text))
    print('Detected intent: {} (confidence: {})\n'.format(
        response.query_result.intent.display_name,
        response.query_result.intent_detection_confidence))
    print('Fulfillment text: {}\n'.format(
        response.query_result.fulfillment_text))
    # return response
    return(response.query_result.fulfillment_text)




if __name__ == '__main__':

	returned = detect_intent_audio()
	print("")
	print("returned here")
	print(returned)
	print("")
	# print("please speak a word into the microphone")
 #    play_wav("start")
 #    record_to_file('filename.wav')
 #    print("done - result written to filename.wav")
 #    play_wav("end")
    # stringResponse = detect_intent_audio()
    # print (stringResponse)
    # time.sleep(1)
    # ohbot.say(stringResponse)
