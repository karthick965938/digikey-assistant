import os
import uuid
import chromadb
import chromadb.utils.embedding_functions as embedding_functions

# Initialize embedding function
sentence_transformer_ef = embedding_functions.SentenceTransformerEmbeddingFunction(model_name="all-MiniLM-L6-v2")

# Set persistent directory and create collection
persist_directory = "./digi_key"
db = chromadb.PersistentClient(path=persist_directory)
collection = db.get_or_create_collection(name="digi_key", embedding_function=sentence_transformer_ef)

# Directory containing .txt files
directory = "data"
action = "new"

# Pre-processing function to read .txt files and prepare data
def pre_processing(directory):
    metadatas = []
    documents = []
    ids = []
    
    # Iterate over all .txt files in the specified directory
    for filename in os.listdir(directory):
        if filename.endswith(".txt"):
            file_path = os.path.join(directory, filename)
            
            # Read the content of the file
            with open(file_path, 'r') as file:
                content = file.read()
                print('content', content)
            
            # Generate a unique ID for each document
            doc_id = str(uuid.uuid4())
            
            # Append data to lists
            documents.append(content)
            metadatas.append({"filename": filename, "content": content})
            ids.append(doc_id)
    
    return metadatas, documents, ids

# Function to perform collection actions
def collection_action(collection, metadatas, documents, ids, action):
    if action == 'new':
        print(f"*****Adding new data, current count: {collection.count()}****************************")
        collection.add(
            documents=documents,
            metadatas=metadatas,
            ids=ids
        )
        print(f"******Data added, new count: {collection.count()}****************************")
    elif action == 'update':
        print(f"*****Updating data, current count: {collection.count()}****************************")
        collection.upsert(
            documents=documents,
            metadatas=metadatas,
            ids=ids
        )
        print(f"******Data updated, new count: {collection.count()}****************************")
    elif action == 'delete':
        print(f"*****Deleting data, current count: {collection.count()}****************************")
        collection.delete(ids=ids)
        print(f"******Data deleted, new count: {collection.count()}****************************")

# Execute the pre-processing and collection action
metadatas, documents, ids = pre_processing(directory)
collection_action(collection, metadatas, documents, ids, action)


# print(f"**************************** completed*****************ids****************** ")
print(ids)
print(metadatas)
print(documents)  # Optionally print formatted documents for debugging


# import sys
# results = collection.query(
#     query_texts=["How do I build a BOM with myLists?"],
#     n_results=3,
#     include=['distances', 'metadatas']
# )
# print('results', results)