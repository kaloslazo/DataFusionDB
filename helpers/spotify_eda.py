import numpy as np
import pandas as pd

# general
delimiter = "\n" + "-" * 50

# data analysis for spotify
df = pd.read_csv("./data/tracks_features.csv", encoding="ISO-8859-1")

print(delimiter)
print("Dataframe information:")
print(df.info())

# delete unnecessary columns
columns_to_maintain = [
    "id",
    "name",
    "album",
    "album_id",
    "artists"
]

# remove columns and improve format
df = df.filter(items=columns_to_maintain)
df.columns = [col.replace(" ", "_").lower() for col in df.columns]

# remove null values2.
df = df.dropna(how="any", axis=0)

# remove if any column has a "," in it
df = df.replace(",", "", regex=True)

print(delimiter)
print("Dataframe information after cleanup:")
print(df.info())

print(delimiter)
print("Dataframe rows non-null:", df.shape[0])
print("Dataframe cols non-null:", df.shape[1])

# ver el tamaño minimo y maximo de columnas de texto
print(delimiter)
print("Text columns min length:")
print(df.select_dtypes(include=["object"]).apply(lambda x: x.str.len().min()))

print(delimiter)
print("Text columns max length:")
print(df.select_dtypes(include=["object"]).apply(lambda x: x.str.len().max()))

# mostrar ejemplos de filas
print(delimiter)
print("Example rows:")
print(df.sample(20))

# guardar el dataframe limpio
sample_df = df.sample(100000) #like the other dataset
sample_df.to_csv("./data/spotify_data_clean.csv", index=False)
