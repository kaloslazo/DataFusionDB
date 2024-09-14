import numpy as np
import pandas as pd

# general
delimiter = "\n" + "-" * 50;

# data analysis for spotify
df = pd.read_csv('./data/spotify_data.csv', encoding = "ISO-8859-1");

print(delimiter);
print("Dataframe information:");
print(df.info());

# delete unnecessary columns
columns_to_drop = [
    "Spotify Playlist Count",
    "Spotify Playlist Reach",
    "YouTube Likes",
    "YouTube Playlist Reach",
    "Apple Music Playlist Count",
    "Deezer Playlist Count",
    "Deezer Playlist Reach",
    "Amazon Playlist Count",
    "Pandora Track Stations",
    "TikTok Posts",
    "TikTok Likes",
    "TIDAL Popularity",
];

# remove columns and improve format
df = df.drop(columns_to_drop, axis = 1);
df.columns = [col.replace(" ", "_").lower() for col in df.columns];

# remove null values2. 
df = df.dropna(how = "any", axis = 0);

print(delimiter);
print("Dataframe information after cleanup:");
print(df.info());

print(delimiter);
print("Dataframe rows non-null:", df.shape[0]);
print("Dataframe cols non-null:", df.shape[1]);

# data type conversion
int_columns = [
    'all_time_rank', 'track_score', 'spotify_streams', 'spotify_popularity',
    'youtube_views', 'tiktok_views', 'airplay_spins', 'siriusxm_spins',
    'pandora_streams', 'soundcloud_streams', 'shazam_counts', 'explicit_track'
];

df['release_date'] = pd.to_datetime(df['release_date'], format = '%m/%d/%Y')
df[int_columns] = df[int_columns].replace({',': ''}, regex = True).astype(int)

# ver el tamaño minimo y maximo de columnas de texto
print(delimiter);
print("Text columns min length:");
print(df.select_dtypes(include = ["object"]).apply(lambda x: x.str.len().min()));

print(delimiter);
print("Text columns max length:");
print(df.select_dtypes(include = ["object"]).apply(lambda x: x.str.len().max()));

# mostrar ejemplos de filas
print(delimiter);
print("Example rows:");
print(df.sample(5));
