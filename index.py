from langchain.prompts import PromptTemplate
from langchain.chains import LLMChain
import chromadb
import os
from langchain_openai import ChatOpenAI
from dotenv import load_dotenv
# Load environment variables from .env file
load_dotenv()

api_key = os.getenv("OPENAI_API_KEY")

llm = ChatOpenAI(model_name="gpt-3.5-turbo", openai_api_key=api_key, temperature=0)

# Directory where the ChromaDB data is persisted
persist_directory = "./digi_key"

# Initialize ChromaDB PersistentClient
chroma_client = chromadb.PersistentClient(path=persist_directory)

# Specify your collection name
collection_name = 'digi_key'

# Define the updated prompt template
PROMPT_TEMPLATE = """
You are an AI-powered Digikey Voice Assistant. Your task is to provide informative and accurate responses based on the user's query and relevant contents. If there is no relevant information within the context, just say "We don't have such information". Don't try to make up an answer. Never break character

User Query:
'{query}'

Contextual Information:
{context}

Conversation History:
{history}

Instructions:
1. Review the user query and the contextual information provided.
2. Based on this information, generate a detailed and precise response to the user's query.
3. If there is no relevant information within the context, just say "We don't have such information". Don't try to make up an answer.

Response:

REMEMBER: If there is no relevant information within the context, just say "We don't have such information". Don't try to make up an answer. Never break character.
"""

# Initialize LangChain's LLMChain with the new prompt template
llm_chain = LLMChain(
    llm=llm,
    prompt=PromptTemplate(
        input_variables=["context", "query", "history"],
        template=PROMPT_TEMPLATE
    )
)

def query_chromadb(query_text):
    """Query ChromaDB for relevant documents based on the query."""
    try:
        collection = chroma_client.get_collection(name=collection_name)
        results = collection.query(
            query_texts=query_text,
            n_results=1,
        )
    except Exception as e:
        print(f"Error querying collection: {e}")
        results = []
    
    return results

def generate_answer(query_text, history=""):
    # Query ChromaDB
    results = query_chromadb(query_text)
    
    # Extract and construct context from results
    if results.get("metadatas"):
        context = results["metadatas"]
    else:
        context = "No relevant information found."
    
    # Generate response using LangChain
    response = llm_chain.run({"context": context, "query": query_text, "history": history})
    
    return response

def ask(user_query):
    result = generate_answer(user_query)
    return result
# Example usage
# question = "How is VAT calculated?"
# answer = generate_answer(question)
# print(answer)
