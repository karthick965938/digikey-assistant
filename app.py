import gradio as gr
from transformers import pipeline
from gtts import gTTS
import numpy as np
from index import ask

transcriber = pipeline("automatic-speech-recognition", model="openai/whisper-small")

def text_to_speech(text, file_path):
    language = 'en'

    audioobj = gTTS(text = text,
                    lang = language,
                    slow = False)

    audioobj.save(file_path)

    return file_path

def process_inputs(audio):
    sr, y = audio
    y = y.astype(np.float32)
    y /= np.max(np.abs(y))

    try:
        transcript = transcriber({"sampling_rate": sr, "raw": y})["text"]
        print('Speaking text:', transcript)
        response = transcript
    except Exception as e:
        print(f"Error during transcription: {e}")
        response = 'Error during transcription'
    
    user_query = response
    final_answer = ask(response)
    print(final_answer)
    processed_audio_path = text_to_speech(final_answer, "Temp3.mp3")

    return user_query, final_answer, processed_audio_path

      

demo = gr.Interface(
    fn=process_inputs,
    inputs=[
        gr.Audio(sources=["microphone"]),
    ],
    outputs=[
        gr.Textbox(label="User Question"),
        gr.Textbox(label="Assistant Answer"),
        gr.Audio("Temp3.mp3")
    ],
    title="DigiKey Voice-Activated Office Assistant",
    description="This assistant will help users interact with DigiKey’s site more efficiently by answering their questions and providing useful information"
)

demo.launch()
